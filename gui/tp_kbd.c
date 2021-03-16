/*
 * tp_kbd.c
 *
 *  Created on: 9 џэт. 2020 у.
 *      Author: User
 */

#include "tp_kbd.h"
#include "list.h"

#define JABBER_INTERVAL		10

#pragma pack(1)

typedef struct
{
	uint16_t id;
	uint16_t x1;
	uint16_t y1;
	uint16_t x2;
	uint16_t y2;
	uint32_t press_time;
}TP_BTN_t;

#pragma pack()

TP_KBD_t* instance = 0;
List_t* buttons = 0;
uint16_t btn_pressed = 0;


void tp_kbd_init(TP_KBD_t* init)
{
	instance = init;
	buttons = list_create();
}

void tp_kbd_process()
{
	if (!instance) return;
	if (!instance->tp) return;
	if (!instance->press_event && !instance->release_event && !instance->click_event) return;
	if (list_is_empty(buttons)) return;

	TP_Point_t point = {0,0};

	tp_getPoint(&point);

	if (point.x * point.y) //check for pressed
	{
		if (!btn_pressed)
		{
			for (int i = 0; (i < list_item_count(buttons)) && !btn_pressed; i++)
			{
				TP_BTN_t* btn = (TP_BTN_t*)(list_get_item(buttons, i)->data);
				if ((point.x >= btn->x1) && (point.x <= btn->x2) && (point.y >= btn->y1) && (point.y <= btn->y2))
				{
					btn_pressed = btn->id;
					btn->press_time = HAL_GetTick();
				}
			}

			if (btn_pressed) instance->press_event(btn_pressed);
		}
	}
	else //check for released
	{
		if (btn_pressed)
		{
			for (int i = 0; (i < list_item_count(buttons)) && btn_pressed; i++)
			{
				TP_BTN_t* btn = (TP_BTN_t*)(list_get_item(buttons, i)->data);
				if ((btn->id == btn_pressed) && btn->press_time && ((btn->press_time+JABBER_INTERVAL) < HAL_GetTick()))
				{
					btn->press_time = 0;
					if (instance->release_event) instance->release_event(btn->id);
					if (instance->click_event) instance->click_event(btn->id);
				}
			}
			btn_pressed = 0;
		}
	}
}

uint8_t tp_kbd_register_key(uint16_t id, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	if (!buttons) return 0;
	if (!id) return 0;

	TP_BTN_t* btn = malloc(sizeof(TP_BTN_t));
	if (!btn) return 0;
	btn->id = id;
	btn->x1 = x1;
	btn->x2 = x2;
	btn->y1 = y1;
	btn->y2 = y2;
	btn->press_time = 0;

	if (list_append(buttons, btn)) return 1;
	return 0;
}

void tp_kbd_unregister_key(uint16_t id)
{
	if (!buttons) return;
	//if (btn_pressed == id) btn_pressed = 0;
	List_item_t* item = 0;
	for (int i = 0; (i < list_item_count(buttons)) && !item; i++)
	{
		item = list_get_item(buttons, i);
		TP_BTN_t* btn = (TP_BTN_t*)item->data;
		if (btn->id != id) item = 0;
	}

	if (!item) return;

	list_remove_item(buttons, item);
	list_release_item(item);
}

void tp_unregister_all()
{
	if (!buttons) return;
	while(!list_is_empty(buttons))
	{
		List_item_t* item = list_first(buttons);
		list_remove_item(buttons, item);
		list_release_item(item);
	}
	//btn_pressed = 0;
}
