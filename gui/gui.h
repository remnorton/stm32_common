/*
 * gui.h
 *
 *  Created on: 11 дек. 2019 г.
 *      Author: User
 */

#ifndef GUI_GUI_H_
#define GUI_GUI_H_
#include "board.h"
#include "lcdlib.h"
#include "fonts_lib.h"
#include "tp_kbd.h"

//!Button orientation
enum
{
	BD_HOR = 0,
	BD_VERT
};


//!Buttons mask

#define GUI_BTN_OK			0x01
#define GUI_BTN_CANCEL		0x02
#define GUI_BTN_YES			0x04
#define GUI_BTN_NO			0x08
#define GUI_BTN_CLOSE		0x10


void gui_init(Font_type_t* header_font, Font_type_t* button_font, Font_type_t* window_text_font, TP_KBD_t* kbd);
void gui_draw_button(uint16_t cx, uint16_t cy, uint8_t* text, uint16_t id, uint8_t direction);
void gui_draw_message(uint16_t x, uint16_t y, uint32_t text_color, uint32_t bk_color, uint8_t* header, uint8_t* text, uint16_t show_time, uint8_t direction);

uint8_t gui_message_box(uint16_t cx, uint16_t cy, uint8_t* title, uint8_t* text, uint8_t buttons, uint8_t direction);

#endif /* GUI_GUI_H_ */
