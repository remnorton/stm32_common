/*
 * fonts_lib.cpp
 *
 *  Created on: 10.09.2018
 *		Author: Rudenko Roman (aka Rem Norton)
 *
 */
#include <string.h>
#include "fonts_lib.h"

uint16_t calc_pix_len(Font_type_t* font, uint8_t* text, uint16_t ch_len)
{
    uint16_t res = 0;
    if (font)
    {
        if (font->descr)
        {
            for (int i = 0; i < ch_len; i++)
            {
            	if ((text[i] >= font->start) && (text[i] <= font->stop))
            		res += font->spacing + font->descr[(text[i])-font->start].width;
            }
            res -= font->spacing;
        }
        else
            res = ((font->width+font->spacing)*ch_len) - font->spacing;
    }
    return res;
}

