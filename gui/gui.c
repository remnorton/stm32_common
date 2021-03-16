/*
 * gui.c
 *
 *  Created on: 11 дек. 2019 г.
 *      Author: User
 */

#include "gui.h"
#include "tp_kbd.h"

#define MIN_BT_WIDTH	100
#define MIN_BT_HEIGHT	40
#define HEADER_HEIGHT	28

//#define BUTTON_COLOR	0xD1,0xD1,0xD1
//#define WINDOW_COLOR	0x91,0x91,0x91

#define WINDOW_COLOR	0xE0,0xE0,0xE0
#define FRAME_COLOR		0xAB,0xAB,0xAB
#define BUTTON_COLOR	0xF8,0xF8,0xF8

static Font_type_t* hdr_fnt = 0;
static Font_type_t* btn_fnt = 0;
static Font_type_t* text_fnt = 0;
static TP_KBD_t* tpk = 0;
static uint8_t button_id;
static eventFunc prev_press_event = 0;

static uint16_t win_w = 0;
static uint16_t win_h = 0;
static uint16_t win_x = 0;
static uint16_t win_y = 0;

//
//Private forwards
//

void gui_draw_multiline_text(uint16_t x, uint16_t y, uint8_t* text, uint8_t max_len, uint8_t direction);
void gui_draw_window(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t* title, uint8_t direction);
void gui_draw_message_box(uint16_t cx, uint16_t cy, uint8_t* title, uint8_t* text, uint8_t buttons, uint8_t direction);
void gui_key_press_event(uint16_t id);

//
//Public members
//
void gui_init(Font_type_t* header_font, Font_type_t* button_font, Font_type_t* window_text_font, TP_KBD_t* kbd)
{
	hdr_fnt = header_font;
	btn_fnt = button_font;
	text_fnt = window_text_font;
	tpk = kbd;
}

void gui_draw_button(uint16_t cx, uint16_t cy, uint8_t* text, uint16_t id, uint8_t direction)
{
	if (!btn_fnt) return;
	uint16_t pix_len = calc_pix_len(btn_fnt, text, strlen((const char*)text)) + 10;
	uint16_t btn_h = MIN_BT_HEIGHT;
	uint16_t btn_w = (pix_len < MIN_BT_WIDTH)?MIN_BT_WIDTH:pix_len;
	uint16_t btn_x = cx-btn_w/2;
	uint16_t btn_y = cy-btn_h/2;

	if (direction == BD_VERT)
	{
		btn_h = (pix_len < MIN_BT_WIDTH)?MIN_BT_WIDTH:pix_len;
		btn_w = MIN_BT_HEIGHT;
		btn_x = cx-btn_w/2;
		btn_y = cy-btn_h/2;
	}

	dspSetColor(dspRgbColor(BUTTON_COLOR));
	dspFillRectangle(btn_x, btn_y, btn_w, btn_h);

	dspSetColor(dspRgbColor(FRAME_COLOR));
	dspDrawRectangle(btn_x, btn_y, btn_w, btn_h);

	dspSetColor(dspRgbColor(STD_COLOR_BLACK));
	dspSetBkColor(dspRgbColor(BUTTON_COLOR));
	dspDrawString(cx, cy, btn_fnt, text, DSP_TextAllignCenter);

	tp_kbd_register_key(id, btn_x, btn_y, btn_x+btn_w, btn_y+btn_h);
}

void gui_draw_message(uint16_t x, uint16_t y, uint32_t text_color, uint32_t bk_color, uint8_t* header, uint8_t* text, uint16_t show_time, uint8_t direction)
{
	uint8_t lines = 1;
	uint8_t max_len = strlen(header);
	uint16_t hdr_len = calc_pix_len(hdr_fnt, header, max_len)+10;
	uint8_t llen = 0;
	uint16_t tl = 0;
	uint16_t text_len = calc_pix_len(hdr_fnt, header, max_len);
	for (int i = 0; i <= strlen(text); i++)
	{
		if ((text[i] == 0x0a) || (text[i] == 0))
		{
			if (text[i] != 0) lines++;
			if (max_len < llen)
			{
				max_len = llen;
				text_len = tl;
				tl = 0;
				llen = 0;
			}
		}
		else
		{
			tl += calc_pix_len(hdr_fnt, &text[i], 1) + 2*hdr_fnt->spacing;
			llen++;
		}
	}
	dspSetColor(bk_color);

	if (direction == BD_HOR)
	{
		win_w = text_len+30;
		win_h = lines*(hdr_fnt->height+5)+MIN_BT_HEIGHT+HEADER_HEIGHT+15;
	}
	else
	{
		win_w = lines*(hdr_fnt->height+5)+MIN_BT_HEIGHT+HEADER_HEIGHT+15;
		win_h = text_len+30;
	}

	win_x = x-win_w/2;
	win_y = y-win_h/2;

	dspFillRectangle(win_x, win_y, win_w, win_h);

	dspSetColor(text_color);
	dspSetBkColor(bk_color);

	uint16_t txt_x,txt_y;

	if (direction == BD_HOR)
	{
		txt_x = x;
		txt_y = win_y + 10 + 2*text_fnt->height;
		dspDrawRectangle(win_x+5, win_y+5+hdr_fnt->height/2, win_w-10, win_h-10-hdr_fnt->height/2);
		dspDrawString(x, win_y+5+hdr_fnt->height/2, hdr_fnt, header, DSP_TextAllignCenter);
	}
	else
	{
		txt_x = win_x + 10 + 2*text_fnt->height;
		txt_y = y;
		dspDrawRectangle(win_x+5+hdr_fnt->height/2, win_y+5, win_w-10-hdr_fnt->height/2, win_h-10);
		dspSetColor(bk_color);
		dspFillRectangle(win_x, win_y + win_h/2 - hdr_len/2, hdr_fnt->height, hdr_len);
		dspSetColor(text_color);
		dspDrawString(win_x+5+hdr_fnt->height/2, win_y+win_h/2, hdr_fnt, header, DSP_TextAllignCenter);
	}


	gui_draw_multiline_text(txt_x, txt_y, text, max_len, direction);

	HAL_Delay(show_time);
}

uint8_t gui_message_box(uint16_t cx, uint16_t cy, uint8_t* title, uint8_t* text, uint8_t buttons, uint8_t direction)
{

	button_id = 0;
	gui_draw_message_box(cx, cy, title, text, buttons, direction);
	prev_press_event = tpk->press_event;
	tpk->press_event = gui_key_press_event;
	while (!button_id)
	{
		tp_kbd_process();
		HAL_Delay(10);
	};
	tp_kbd_process();
	tpk->press_event = prev_press_event;
	tp_kbd_unregister_key(GUI_BTN_OK);
	tp_kbd_unregister_key(GUI_BTN_CANCEL);
	tp_kbd_unregister_key(GUI_BTN_CLOSE);
	tp_kbd_unregister_key(GUI_BTN_NO);
	tp_kbd_unregister_key(GUI_BTN_YES);

	return button_id;
}


//
//Private members
//

void gui_draw_multiline_text(uint16_t x, uint16_t y, uint8_t* text, uint8_t max_len, uint8_t direction)
{
	uint16_t txt_x = x;
	uint16_t txt_y = y;
	uint8_t* line = (uint8_t*)malloc(max_len+1);
	uint8_t pos = 0;
	memset(line, 0, max_len+1);

	while(*text != 0)
	{
		line[pos] = *text;
		text++;
		pos++;
		if ((*text == 0) || (*text == 0x0A))
		{
			dspDrawString(txt_x, txt_y, text_fnt, line, DSP_TextAllignCenter);
			if (direction == BD_HOR) txt_y += text_fnt->height + 5;
			else txt_x += text_fnt->height + 5;
			memset(line, 0, max_len+1);
			pos = 0;
			if (*text != 0) text++;
		}
	}
	free(line);
}

void gui_draw_window(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t* title, uint8_t direction)
{
	if (!hdr_fnt) return;

	uint16_t hdx, hdy, hdw, hdh, win_x, win_y, win_w, win_h;
	hdx = x+1;
	hdy = y+1;
	if (direction == BD_HOR)
	{
		hdw = width-2;
		hdh = HEADER_HEIGHT;
		win_w = hdw;
		win_h = height-hdh-2;
		win_x = hdx+1;
		win_y = y+hdh+2;
	}
	else
	{
		hdw = HEADER_HEIGHT;
		hdh = height-2;
		win_w = width-hdw-2;
		win_h = hdh+1;
		win_x = hdx+HEADER_HEIGHT+1;
		win_y = y+1;
	}

	//draw outline
	dspSetColor(dspRgbColor(STD_COLOR_GRAY));
	dspDrawRectangle(x, y, width, height);

	//draw body
	dspSetColor(dspRgbColor(WINDOW_COLOR));
	dspFillRectangle(win_x, win_y, win_w, win_h);

	//draw header
	dspSetColor(dspRgbColor(STD_COLOR_DARK_BLUE));
	dspFillRectangle(hdx, hdy, hdw, hdh);

	dspSetColor(dspRgbColor(STD_COLOR_LIGHT_BLUE));
	dspDrawRectangle(hdx, hdy, hdw, hdh);

	//draw tile
	dspSetColor(dspRgbColor(STD_COLOR_WHITE));
	dspSetBkColor(dspRgbColor(STD_COLOR_DARK_BLUE));
	dspDrawString(hdx+hdw/2, hdy+hdh/2, hdr_fnt, title, DSP_TextAllignCenter);

}

void gui_draw_message_box(uint16_t cx, uint16_t cy, uint8_t* title, uint8_t* text, uint8_t buttons, uint8_t direction)
{
	if (!hdr_fnt || !btn_fnt || !text_fnt || !buttons || !title || !text) return;

	uint8_t bt_count = 0;
	uint8_t tmp = buttons;
	//calc buttons count
	for (int i = 0; i < 8; i++)
	{
		if (tmp & 0x01) bt_count++;
		tmp = tmp >> 1;
	}

	//calc lines count
	uint8_t len = strlen(text);
	uint8_t lines = 1;
	uint8_t max_len = strlen(title);
	uint8_t llen = 0;
	uint16_t tl = 0;
	uint16_t text_len = calc_pix_len(text_fnt, text, max_len);
	for (int i = 0; i <= strlen(text); i++)
	{
		if ((text[i] == 0x0a) || (text[i] == 0))
		{
			if (text[i] != 0) lines++;
			if (max_len < llen)
			{
				max_len = llen;
				text_len = tl;
				tl = 0;
				llen = 0;
			}
		}
		else
		{
			tl += calc_pix_len(hdr_fnt, &text[i], 1) + 2*hdr_fnt->spacing;
			llen++;
		}
	}
	uint16_t btn_len = bt_count*(MIN_BT_WIDTH+5)-5;
	uint16_t bt_y,bt_x,txt_x,txt_y;

	if (direction == BD_HOR)
	{
		win_w = ((text_len > btn_len)?text_len:btn_len)+30;
		win_h = lines*(text_fnt->height+5)+MIN_BT_HEIGHT+HEADER_HEIGHT+15;
	}
	else
	{
		win_w = lines*(text_fnt->height+5)+MIN_BT_HEIGHT+HEADER_HEIGHT+15;
		win_h = ((text_len > btn_len)?text_len:btn_len)+10;
	}

	win_x = cx-win_w/2;
	win_y = cy-win_h/2;
	gui_draw_window(win_x, win_y, win_w, win_h, title, direction);

	if (direction == BD_HOR)
	{
		bt_y = win_y + win_h - 5 - MIN_BT_HEIGHT/2;
		bt_x = cx - btn_len/2 + MIN_BT_WIDTH/2;
	}
	else
	{
		bt_y = cy - btn_len/2 + MIN_BT_WIDTH/2;
		bt_x = win_x + win_w - 5 - MIN_BT_HEIGHT/2;
	}

	if (buttons & GUI_BTN_OK)
	{
		gui_draw_button(bt_x, bt_y, (uint8_t*)"Ok", GUI_BTN_OK, direction);
		bt_x += MIN_BT_WIDTH+5;
	}

	if (buttons & GUI_BTN_CANCEL)
	{
		gui_draw_button(bt_x, bt_y, (uint8_t*)"Cancel", GUI_BTN_CANCEL, direction);
		bt_x += MIN_BT_WIDTH+5;
	}

	if (buttons & GUI_BTN_YES)
	{
		gui_draw_button(bt_x, bt_y, (uint8_t*)"Yes", GUI_BTN_YES, direction);
		bt_x += MIN_BT_WIDTH+5;
	}

	if (buttons & GUI_BTN_NO)
	{
		gui_draw_button(bt_x, bt_y, (uint8_t*)"No", GUI_BTN_NO, direction);
		bt_x += MIN_BT_WIDTH+5;
	}

	if (buttons & GUI_BTN_CLOSE)
	{
		gui_draw_button(bt_x, bt_y, (uint8_t*)"Close", GUI_BTN_CLOSE, direction);
		bt_x += MIN_BT_WIDTH+5;
	}

	dspSetBkColor(dspRgbColor(WINDOW_COLOR));
	dspSetColor(dspRgbColor(STD_COLOR_BLACK));
	if (direction == BD_HOR)
	{
		//txt_x = win_x + win_w/2;
		txt_x = cx;
		txt_y = win_y + HEADER_HEIGHT+10 + text_fnt->height/2;
	}
	else
	{
		txt_x = win_x + HEADER_HEIGHT+10 + text_fnt->height/2;
		txt_y = cy;
	}
	gui_draw_multiline_text(txt_x, txt_y, text, max_len, direction);

}

void gui_key_press_event(uint16_t id)
{
	if ((id == GUI_BTN_OK) || (id == GUI_BTN_CANCEL) || (id == GUI_BTN_CLOSE) || (id == GUI_BTN_NO) || (id == GUI_BTN_YES))
		button_id = id;
}
