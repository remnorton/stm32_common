/*
 * tp_kbd.h
 *
 *  Created on: 9 џэт. 2020 у.
 *      Author: User
 */

#ifndef GUI_TP_KBD_H_
#define GUI_TP_KBD_H_

#include "list.h"
#include "xpt2046.h"


typedef void(*eventFunc)(uint16_t);

typedef struct
{
	TP_Init_t* 	tp;
	eventFunc	press_event;
	eventFunc	release_event;
	eventFunc	click_event;
}TP_KBD_t;


void tp_kbd_init(TP_KBD_t* init);
void tp_kbd_process();
uint8_t tp_kbd_register_key(uint16_t id, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void tp_kbd_unregister_key(uint16_t id);
void tp_unregister_all();

#endif /* GUI_TP_KBD_H_ */
