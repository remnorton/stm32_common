/*
 * display.h
 *
 *  Created on: 08.09.2018
 *		Author: Rudenko Roman (aka Rem Norton)
 *
 */


#ifndef DISPLAY_H
#define DISPLAY_H
#include "dsp_driver.h"

//
//Standard colors
//

#define STD_COLOR_BLACK			0,0,0
#define STD_COLOR_GREEN			0,255,0
#define STD_COLOR_RED			255,0,0
#define STD_COLOR_BLUE			0,0,255
#define STD_COLOR_WHITE			255,255,255
#define STD_COLOR_YELLOW		255,255,0
#define STD_COLOR_CYAN			85,170,255
#define STD_COLOR_GRAY			125,125,125
#define STD_COLOR_AMBER			255,170,0
#define STD_COLOR_MAGENTA		170,85,255
#define STD_COLOR_PURPLE		85,0,127

#define STD_COLOR_DARK_CYAN		0,170,255
#define STD_COLOR_DARK_GREEN	0,85,0
#define STD_COLOR_DARK_BLUE		0,0,85
#define STD_COLOR_DARK_RED		85,0,0
#define STD_COLOR_DARK_YELLOW	85,85,0
#define STD_COLOR_DARK_GRAY		55,55,55
#define STD_COLOR_DARK_MAGENTA	170,85,127

#define STD_COLOR_LIGHT_BLUE	0,85,255
#define STD_COLOR_LIGHT_MAGENTA	255,85,255
#define STD_COLOR_LIGHT_GRAY	155,155,155



//
//Public functions
//

void dspInit();
uint8_t dspOpen(DSPInitStruct_t* init);
void dspSetActiveHandle(uint8_t handle);
uint16_t dspGetId();
uint16_t dspGetScreenWidth();
uint16_t dspGetScreenHeight();
void dspSetTextDirection(uint8_t direction);

void dspSetColor(uint32_t color);
void dspSetBkColor(uint32_t color);

void dspClearScreen();

void dspSetInverse(uint8_t inverse);
void dspSwitchOn(uint8_t active);
void dspSetBrightnes(uint32_t level);

void dspDrawPixel(uint32_t x, uint32_t y, uint32_t color);
void dspDrawLine(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);

void dspDrawRectangle(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
void dspFillRectangle(uint32_t x, uint32_t y, uint32_t width, uint32_t height);


uint16_t dspDrawChar(uint32_t x, uint32_t y, Font_type_t* font, uint8_t sym, uint8_t direction);
void dspDrawString(uint32_t x, uint32_t y, Font_type_t* font, uint8_t* text, uint8_t allign);

void dspSendCommands(uint8_t* commands, uint8_t len);
void dspSendData(uint8_t* data, uint16_t len);
void dspPushScreen();

uint32_t dspRgbaColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
uint32_t dspRgbColor(uint8_t red, uint8_t green, uint8_t blue);
uint32_t dspPixColor(uint32_t x, uint32_t y);

uint32_t dspBitPerPixel(uint8_t handle);
void dspDrawBitmap(uint32_t x, uint32_t y, DSP_Bitmap_t* bmp, uint8_t angle_code);
void dspDrawImage(uint32_t x, uint32_t y, DSP_Image_t* img);

void dspRotate(uint8_t angle_code);
uint8_t dspReadImage(uint32_t x, uint32_t y, DSP_Image_t* img);

//
//
//
void dspDrawElipse(uint32_t x, uint32_t y, uint32_t xrad, uint32_t yrad);
void dspDrawCyrcle(uint32_t x, uint32_t y, uint32_t rad);

void dspFillElipse(uint32_t x, uint32_t y, uint32_t xrad, uint32_t yrad);
void dspFillCyrcle(uint32_t x, uint32_t y, uint32_t rad);

void dspDrawArc(uint32_t x, uint32_t y, uint32_t xrad, uint32_t yrad, uint16_t start, uint32_t stop);
void dspFillArc(uint32_t x, uint32_t y, uint32_t xrad, uint32_t yrad, uint16_t start, uint32_t stop);

void dspFillRectangleGradient(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint8_t orientation, uint32_t startColor, uint32_t stopColor);

RGB_Color_t dspRGB16ColorDecode(RGB16_ColorStruct_t source);

void dspShowMemJPG(uint16_t x, uint16_t y, uint8_t* jpg_data, uint32_t jpg_size, uint16_t* line_buf, uint8_t* work_buf, uint8_t lines_per_pass);
void dspShowDiskJPG(uint16_t x, uint16_t y, uint8_t* jpg_file_name, uint16_t* line_buf, uint8_t* work_buf, uint8_t lines_per_pass);

uint8_t dspLoadFont(uint8_t* file_name, void* buffer, uint32_t* size);

#endif // DISPLAY_H
