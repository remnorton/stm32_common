/*
 * lcdlib.cpp
 *
 *  Created on: 08.09.2018
 *		Author: Rudenko Roman (aka Rem Norton)
 *
 */
#include <string.h>
#include "lcdlib.h"
#include "intmath.h"

#define DSP_LOW_MEM

//
//
//

typedef struct
{
    DSPInitStruct_t* init;
    uint32_t        bkColor;
    uint32_t        foreColor;
    uint8_t			textDirection;
}DSP_InfoStruct_t;

#pragma pack(1)

typedef struct
{
    uint8_t prefix[3];          //! file prefix (FNT)
    uint8_t name[16];           //! table of bitmaps of symbols
    uint8_t width;              //! width of symbol. Ignored if table of descriptors are defined
    uint8_t height;             //! untrimmed symbols height
    uint8_t start;              //! code of first symbol in table
    uint8_t stop;               //! code of last symbol in table
    int8_t spacing;             //! spacing between symbols in line
}FontFile_Header_t;

#pragma pack()

#define _MAX_DSP_   5

static DSP_InfoStruct_t dsp[_MAX_DSP_];
static uint8_t dd_idx = 0;
static uint8_t active_handle = 0;

//
//Private forwards
//

uint8_t bmp_get_pixel(DSP_Bitmap_t* bmp, uint16_t x, uint16_t y);
void bmp_set_pixel(DSP_Bitmap_t* bmp, uint16_t x, uint16_t y, uint8_t pixel);

//
//
//

void dspInit()
{
    for (int i = 0; i < _MAX_DSP_; i++)
    {
        dsp[i].bkColor = DSP_COLOR_Black;
        dsp[i].foreColor = DSP_COLOR_White;
        dsp[i].init = 0;
    }
}

uint8_t dspOpen(DSPInitStruct_t* init)
{
    if (!init) return 0;
    if (!init->driver) return 0;
    if (!init->driver->init) return 0;
    if (dd_idx > _MAX_DSP_) return 0;
    if (init->driver->init(init->drvSettings))
    {
    	dsp[dd_idx++] = (DSP_InfoStruct_t){init, dspRgbColor(STD_COLOR_BLACK), dspRgbColor(STD_COLOR_WHITE), DSP_TextDir_LeftRight};
    	return dd_idx;
    }
    return 0;
}

void dspSetActiveHandle(uint8_t handle)
{
	active_handle = handle;
}

void dspClose()
{
    if (!active_handle) return;
    if (!dsp[active_handle-1].init) return;
    if (dsp[active_handle-1].init->driver->deInit) dsp[active_handle-1].init->driver->deInit(dsp[active_handle-1].init->drvSettings);
    dsp[active_handle-1] = (DSP_InfoStruct_t){0,0,0};
}


uint16_t dspGetId()
{
    if (!active_handle) return 0;
    if (!dsp[active_handle-1].init) return 0;
    return dsp[active_handle-1].init->driver->getId(dsp[active_handle-1].init->drvSettings);
}

uint16_t dspGetScreenWidth()
{
    if (!active_handle) return 0;
    if (!dsp[active_handle-1].init) return 0;
    return dsp[active_handle-1].init->driver->screenWidth(dsp[active_handle-1].init->drvSettings);
}

uint16_t dspGetScreenHeight()
{
    if (!active_handle) return 0;
    if (!dsp[active_handle-1].init) return 0;
    return dsp[active_handle-1].init->driver->screenHeight(dsp[active_handle-1].init->drvSettings);
}

void dspSetTextDirection(uint8_t direction)
{
	if (!active_handle) return;
	dsp[active_handle-1].textDirection = direction;
}

void dspSetColor(uint32_t color)
{
    if (!active_handle) return;
    if (!dsp[active_handle-1].init) return;
    dsp[active_handle-1].foreColor = color;
}

void dspSetBkColor(uint32_t color)
{
    if (!active_handle) return;
    if (!dsp[active_handle-1].init) return;
    dsp[active_handle-1].bkColor = color;
}

void dspClearScreen()
{
    if (!active_handle) return;
    if (!dsp[active_handle-1].init) return;
    if (!dsp[active_handle-1].init->driver->clearScreen) return;
    dsp[active_handle-1].init->driver->clearScreen(dsp[active_handle-1].init->drvSettings,dsp[active_handle-1].bkColor);
}

void dspSetInverse(uint8_t inverse)
{
    if (!active_handle) return;
    if (!dsp[active_handle-1].init) return;
    if (dsp[active_handle-1].init->driver->setInverse) dsp[active_handle-1].init->driver->setInverse(dsp[active_handle-1].init->drvSettings,inverse);
}

void dspSwitchOn(uint8_t active)
{
    if (!active_handle) return;
    if (!dsp[active_handle-1].init) return;
    if (dsp[active_handle-1].init->driver->switchOn) dsp[active_handle-1].init->driver->switchOn(dsp[active_handle-1].init->drvSettings,active);
}

void dspSetBrightnes(uint32_t level)
{
    if (!active_handle) return;
    if (!dsp[active_handle-1].init) return;
    if (dsp[active_handle-1].init->driver->setBrightnes) dsp[active_handle-1].init->driver->setBrightnes(dsp[active_handle-1].init->drvSettings,level);
}

void dspDrawPixel(uint32_t x, uint32_t y, uint32_t color)
{
    if (!active_handle) return;
    if (!dsp[active_handle-1].init) return;
    if (dsp[active_handle-1].init->driver->drawPixel) dsp[active_handle-1].init->driver->drawPixel(dsp[active_handle-1].init->drvSettings,x,y,color);
}

void dspDrawLine(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
    if (!active_handle) return;
    if (!dsp[active_handle-1].init) return;
    if (dsp[active_handle-1].init->driver->drawLine) dsp[active_handle-1].init->driver->drawLine(dsp[active_handle-1].init->drvSettings,x1,y1,x2,y2,dsp[active_handle-1].foreColor);
    else
    {
        int32_t dx = (x2-x1);
        int32_t dy = (y2-y1);

        int8_t  sx = (dx == 0)?0:((dx>0)?1:-1);
        int8_t  sy = (dy == 0)?0:((dy>0)?1:-1);

        int32_t x = x1;
        int32_t y = y1;


        if (dx && dy)
        {
            if (dx == dy)
            {
                while ((x != x2) || y != y2)
                {
                   dspDrawPixel(x,y, dsp[active_handle-1].foreColor);
                   x += sx;
                   y += sy;
                }
            }
            else
            {
                if (ABS(dy) < ABS(dx)) //f(x) = ((x-x1)*dy)/dx+y1;
                {
                    int32_t prev_y = y1;
                    while (x != x2)
                    {
                        y = ((x*10-(int32_t)(x1*10))*dy*10)/(dx*10)+(int32_t)y1*10;
                        uint8_t tail = ABS(y)%10;
                        y = y/10 + 1*((tail>5)?1:0);
                        if (ABS(prev_y - y) > 1)
                            dspDrawLine(x,prev_y,x,y);
                        else
                            dspDrawPixel(x,y, dsp[active_handle-1].foreColor);
                        prev_y = y;
                        x += sx;
                    }
                }
                else //f(y) = ((y-y1)*xy)/dy+x1;
                {
                    int32_t prev_x = x1;
                    while (y != y2)
                    {
                        x = (( y*10-(int32_t)(y1*10))*dx*10)/(dy*10)+(int32_t)x1*10;
                        uint8_t tail = ABS(x)%10;
                        x = x/10 + 1*((tail>5)?1:0);
                        if (ABS(prev_x - x) > 1)
                            dspDrawLine(prev_x,y,x,y);
                        else
                            dspDrawPixel(x,y, dsp[active_handle-1].foreColor);
                        prev_x = x;
                        y += sy;
                    }
                }
            }
        }
        else
        {
            dspDrawPixel(x,y, dsp[active_handle-1].foreColor);
            if (dx == 0) //vertical
            {
                while (y != y2)
                {
                    y += sy;
                    dspDrawPixel(x,y,dsp[active_handle-1].foreColor);
                }
            }
            else //horizontal
            {
                while (x != x2)
                {
                    x += sx;
                    dspDrawPixel(x,y,dsp[active_handle-1].foreColor);
                }
            }
        }

    }
}

void dspDrawRectangle(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    if (!active_handle) return;
    if (!dsp[active_handle-1].init) return;
    if (dsp[active_handle-1].init->driver->drawRectangle) dsp[active_handle-1].init->driver->drawRectangle(dsp[active_handle-1].init->drvSettings,x,y,width,height,dsp[active_handle-1].foreColor);
    else
    {
        dspDrawLine(x,y,x+width,y);
        dspDrawLine(x+width,y,x+width,y+height);
        dspDrawLine(x+width,y+height,x,y+height);
        dspDrawLine(x,y+height,x,y);
    }
}

void dspFillRectangle(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    if (!active_handle) return;
    if (!dsp[active_handle-1].init) return;
    if (dsp[active_handle-1].init->driver->fillRectangle) dsp[active_handle-1].init->driver->fillRectangle(dsp[active_handle-1].init->drvSettings,x,y,width,height,dsp[active_handle-1].foreColor);
    /*else
        for (uint16_t ly = y; ly <=(y+ height); ly++)
            dspDrawLine(x,ly,x+width,ly);*/
}

uint16_t dspDrawChar(uint32_t x, uint32_t y, Font_type_t* font, uint8_t sym, uint8_t direction)
{
    if (!active_handle) return 0;
    if (!dsp[active_handle-1].init->driver->drawBitmap) return 0;
    if (!font) return 0;
    if ((sym < font->start) || (sym > font->stop)) return 0;
    uint8_t vs = (font->descr)?1:0;
    Font_Char_info_t info;
    if (vs) info = font->descr[(sym-font->start)];

    uint8_t sym_width = vs?info.width:font->width;
    uint8_t sym_height = vs?info.height:font->height;
    uint8_t byte_width = sym_width/8+((sym_width % 8)?1:0);

    uint16_t offset = vs?info.offset:((sym-font->start)*byte_width*font->height);

    if ( ((x+sym_width) > dspGetScreenWidth(active_handle)) || ((y+font->height) > dspGetScreenHeight(active_handle) )) return 0;

    uint8_t vertical = ((direction == DSP_TextDir_BottomTop) || (direction == DSP_TextDir_TopBottom));
    uint8_t forward = ((direction == DSP_TextDir_BottomTop) || (direction == DSP_TextDir_RightLeft))?0:1;

    uint16_t startX = 0;
    uint16_t startY = vs?(forward?(font->height - info.trimed - sym_height):info.trimed):0;


    if (vertical)
    {
    	startX = vs?(forward?(font->height - info.trimed - sym_height):info.trimed):0;
    	startY = 0;
    }

    if (sym != 32) // not whitespace
    {
		DSP_Bitmap_t bmp;
		bmp.height = sym_height;
		bmp.width = sym_width;
		bmp.data = font->bitmaps+offset;
		dspDrawBitmap(x+startX,y+startY,&bmp,direction);
    }
    return sym_width+font->spacing;
}

void dspDrawString(uint32_t x, uint32_t y, Font_type_t* font, uint8_t* text, uint8_t allign)
{
	if (!text) return;
	if (!active_handle) return;
    uint16_t ch_len = strlen((const char*)text);
    uint8_t direction = dsp[active_handle-1].textDirection;
    uint8_t vertical = (direction%2)?1:0;
    uint8_t forward = ((direction == DSP_TextDir_TopBottom) || (direction == DSP_TextDir_RightLeft))?0:1;
    uint32_t pix_len = calc_pix_len(font, text, ch_len);
    uint32_t sx, sy;

    while (pix_len > (vertical?dspGetScreenHeight(active_handle):dspGetScreenWidth(active_handle)))
    {
        ch_len--;
        pix_len = calc_pix_len(font, text, ch_len);
    }

    if (vertical) sx = x;
    else sy = y;
    switch(allign)
    {
        case DSP_TextAllignLeft:
        {
            if (vertical) sy = y - pix_len;
            else sx = x;
            break;
        }
        case DSP_TextAllignRight:
        {
            if (vertical)
            {
                if (pix_len > y) sy = 0;
                else sy = y;
            }
            else
            {
                if (pix_len > x) sx = 0;
                else sx = x - pix_len;
            }
            break;
        }
        case DSP_TextAllignCenter:
        {
            if (vertical)
            {
                sy = y - (pix_len/2);
                sx -= (font->height/2);
            }
            else
            {
                sx = x - (pix_len/2);
                sy -= (font->height/2);
            }
            break;
        }
    }

    uint32_t pos = 0;
    for (int i = 0; i < ch_len; i++)
    {
        if (forward)
        	pos += dspDrawChar(vertical?sx:(sx+pos), vertical?(sy+pos):sy, font, text[i], (direction==DSP_TextDir_RightLeft)?DSP_TextDir_LeftRight:direction);
        else
        	pos += dspDrawChar(vertical?sx:(sx+pos), vertical?(sy+pos):sy, font, text[ch_len-i-1], (direction==DSP_TextDir_RightLeft)?DSP_TextDir_LeftRight:direction);

    }

}

void dspSendCommands(uint8_t* commands, uint8_t len)
{
    if (!active_handle) return;
    if (!dsp[active_handle-1].init) return;
    if (dsp[active_handle-1].init->driver->sendCommands) dsp[active_handle-1].init->driver->sendCommands(dsp[active_handle-1].init->drvSettings,commands, len);
}

void dspSendData(uint8_t* data, uint16_t len)
{
    if (!active_handle) return;
    if (!dsp[active_handle-1].init) return;
    if (dsp[active_handle-1].init->driver->sendData) dsp[active_handle-1].init->driver->sendData(dsp[active_handle-1].init->drvSettings,data, len);
}

void dspPushScreen()
{
    if (!active_handle) return;
    if (!dsp[active_handle-1].init) return;
    if (dsp[active_handle-1].init->driver->pushData) dsp[active_handle-1].init->driver->pushData(dsp[active_handle-1].init->drvSettings);
}

uint32_t dspRgbaColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
    if (!active_handle) return 0;
    if (!dsp[active_handle-1].init) return 0;
    if (dsp[active_handle-1].init->driver->rgbColor) return dsp[active_handle-1].init->driver->rgbColor(dsp[active_handle-1].init->drvSettings,red, green, blue, alpha);
    return 0;
}

uint32_t dspRgbColor(uint8_t red, uint8_t green, uint8_t blue)
{
    return dspRgbaColor(red, green, blue, 0xff);
}

uint32_t dspPixColor(uint32_t x, uint32_t y)
{
    if (!active_handle) return 0;
    if (!dsp[active_handle-1].init) return 0;
    if (dsp[active_handle-1].init->driver->pixelColor) return dsp[active_handle-1].init->driver->pixelColor(dsp[active_handle-1].init->drvSettings,x,y);
    return 0;
}

uint32_t dspBitPerPixel(uint8_t active_handle)
{
    if (!active_handle) return 0;
    if (!dsp[active_handle-1].init) return 0;
    if (dsp[active_handle-1].init->driver->bitPerPixel) return dsp[active_handle-1].init->driver->bitPerPixel(dsp[active_handle-1].init->drvSettings);
    return 0;
}

void dspDrawBitmap(uint32_t x, uint32_t y, DSP_Bitmap_t* bmp, uint8_t angle_code)
{
    if (!active_handle) return;
    if (!dsp[active_handle-1].init) return;
    if (dsp[active_handle-1].init->driver->drawBitmap)
    	dsp[active_handle-1].init->driver->drawBitmap(dsp[active_handle-1].init->drvSettings,x,y,bmp,dsp[active_handle-1].foreColor, dsp[active_handle-1].bkColor, angle_code);
}

void dspDrawImage(uint32_t x, uint32_t y, DSP_Image_t* img)
{
	if (!active_handle) return;
	if (!dsp[active_handle-1].init) return;
	if (dsp[active_handle-1].init->driver->drawImage) dsp[active_handle-1].init->driver->drawImage(dsp[active_handle-1].init->drvSettings, x, y, img);
}


void dspRotate(uint8_t angle_code)
{
    if (!active_handle) return;
    if (!dsp[active_handle-1].init) return;
    if (dsp[active_handle-1].init->driver->rotate) dsp[active_handle-1].init->driver->rotate(dsp[active_handle-1].init->drvSettings,angle_code);
}

uint8_t dspReadImage(uint32_t x, uint32_t y, DSP_Image_t* img)
{
	if (!active_handle) return 0;
	if (!dsp[active_handle-1].init) return 0;
	if (!dsp[active_handle-1].init->driver->readImage) return 0;
	return dsp[active_handle-1].init->driver->readImage(dsp[active_handle-1].init->drvSettings, x, y, img);
}

//
//
//

void dspDrawElipse(uint32_t x, uint32_t y, uint32_t xrad, uint32_t yrad)
{
    uint32_t ax = getArcX(x,1,xrad);
    uint32_t ay = getArcY(y,1,yrad);

    for (uint16_t t = 2; t < 360; t++)
    {
        if (t%90)
        {
            uint32_t bx = getArcX(x,t,xrad);
            uint32_t by = getArcY(y,t, yrad);
            dspDrawLine(ax, ay, bx, by);
            ax = bx;
            ay = by;
        }
    }
}

void dspDrawCyrcle(uint32_t x, uint32_t y, uint32_t rad)
{
    dspDrawElipse(x,y,rad,rad);
}

void dspFillElipse(uint32_t x, uint32_t y, uint32_t xrad, uint32_t yrad)
{
    for (int t = 1; t < 180; t++)
    {
        if (t%90)
        {
            uint32_t ax = getArcX(x,180+t,xrad);
            uint32_t ay = getArcY(y,180+t,yrad);

            uint32_t bx = getArcX(x,180-t,xrad);
            uint32_t by = getArcY(y,180-t,yrad);
            dspDrawLine(ax,ay,bx,by);
        }
    }
}

void dspFillCyrcle(uint32_t x, uint32_t y, uint32_t rad)
{
    dspFillElipse(x,y,rad,rad);
}

void dspDrawArc(uint32_t x, uint32_t y, uint32_t xrad, uint32_t yrad, uint16_t start, uint32_t stop)
{
    uint32_t ax = getArcX(x,start,xrad);
    uint32_t ay = getArcY(y,start,yrad);

    for (uint16_t t = start+1; t <= stop; t++)
    {
        if (t%90)
        {
            uint32_t bx = getArcX(x,t,xrad);
            uint32_t by = getArcY(y,t, yrad);
            dspDrawLine(ax, ay, bx, by);
            ax = bx;
            ay = by;
        }
    }
}

void dspFillArc(uint32_t x, uint32_t y, uint32_t xrad, uint32_t yrad, uint16_t start, uint32_t stop)
{
    uint16_t axis = (stop-start)/2;
    uint32_t ax,ay,bx,by;
    for (uint16_t t = 0; t < axis; t++)
    {
        ax = getArcX(x,start+t,xrad);
        ay = getArcY(y,start+t,yrad);

        bx = getArcX(x,stop-t,xrad);
        by = getArcY(y,stop-t,yrad);

        dspDrawLine(ax,ay,bx,by);
    }
}

void dspFillRectangleGradient(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint8_t orientation, uint32_t startColor, uint32_t stopColor)
{
    if (!active_handle) return;
    if (!dsp[active_handle-1].init) return;
    //if (!dsp[active_handle-1].init->driver->decodeColor) return;
    if (!dsp[active_handle-1].init->driver->bitPerPixel) return;
    if (!dsp[active_handle-1].init->driver->rgbColor) return;
    if (dsp[active_handle-1].init->driver->bitPerPixel(dsp[active_handle-1].init->drvSettings) < 12) return;


    int32_t dx = (x2-x1);
    int32_t dy = (y2-y1);

    int8_t  sx = (dx == 0)?0:((dx>0)?1:-1);
    int8_t  sy = (dy == 0)?0:((dy>0)?1:-1);

    int32_t x = x1;
    int32_t y = y1;

    //uint16_t line_len = (orientation == DSP_GRADIENT_HORIZONTAL)?ABS(y2-y1):ABS(x2-x1);
    uint16_t lines_count = (orientation == DSP_GRADIENT_HORIZONTAL)?ABS(x2-x1):ABS(y2-y1);
    RGB_Color_t start_struct = dsp[active_handle-1].init->driver->decodeColor(dsp[active_handle-1].init->drvSettings,startColor);
    RGB_Color_t stop_struct = dsp[active_handle-1].init->driver->decodeColor(dsp[active_handle-1].init->drvSettings,stopColor);

    uint16_t line_red = start_struct.red*100;
    uint16_t line_green = start_struct.green*100;
    uint16_t line_blue = start_struct.blue*100;

    int16_t dr = stop_struct.red - start_struct.red;
    int16_t dg = stop_struct.green - start_struct.green;
    int16_t db = stop_struct.blue - start_struct.blue;

    int16_t sr = dr*100/lines_count;
    int16_t sg = dg*100/lines_count;
    int16_t sb = db*100/lines_count;

    uint32_t color = dsp[active_handle-1].foreColor;
    uint8_t end = 0;

        while (!end)
        {
            dsp[active_handle-1].foreColor = dsp[active_handle-1].init->driver->rgbColor(dsp[active_handle-1].init->drvSettings,line_red/100, line_green/100, line_blue/100, 255);

            if (orientation == DSP_GRADIENT_HORIZONTAL)
            {
                end = (x == x2)?1:0;
                dspDrawLine(x, y1, x, y2);
                x += sx;
            }
            else
            {
                end = (y == y2)?1:0;
                dspDrawLine(x1, y, x2, y);
                y += sy;
            }

            line_red += sr;
            line_green += sg;
            line_blue += sb;
        }

    dsp[active_handle-1].foreColor = color;
}

RGB_Color_t dspRGB16ColorDecode(RGB16_ColorStruct_t source)
{
	uint16_t max_r, max_g, max_b;
	max_r = max_b = 0x1f;
	max_g = 0x3f;
	RGB_Color_t res = {0,0,0,255};

	uint16_t r = source.rgb.r;
	uint16_t g = source.rgb.g;
	uint16_t b = source.rgb.b;

	res.red = (r*255/max_r);
	res.green = (g*255/max_g);
	res.blue = (b*255/max_b);
	return res;
}

void dspShowMemJPG(uint16_t x, uint16_t y, uint8_t* jpg_data, uint32_t jpg_size, uint16_t* line_buf, uint8_t* work_buf, uint8_t lines_per_pass)
{
	if (!active_handle) return;
	if (!dsp[active_handle-1].init) return;
	if (dsp[active_handle-1].init->driver->streamJPGmem)
		dsp[active_handle-1].init->driver->streamJPGmem(dsp[active_handle-1].init->drvSettings, x, y, jpg_data, jpg_size, line_buf, work_buf, lines_per_pass);
}

void dspShowDiskJPG(uint16_t x, uint16_t y, uint8_t* jpg_file_name, uint16_t* line_buf, uint8_t* work_buf, uint8_t lines_per_pass)
{
	if (!active_handle) return;
	if (!dsp[active_handle-1].init) return;
	if (dsp[active_handle-1].init->driver->streamJPGfile)
		dsp[active_handle-1].init->driver->streamJPGfile(dsp[active_handle-1].init->drvSettings, x, y, jpg_file_name, line_buf, work_buf, lines_per_pass);
}

uint8_t dspLoadFont(uint8_t* file_name, void* buffer, uint32_t* size)
{
	uint8_t res = 0;
#ifdef _FATFS

	FIL ffile;
	FRESULT r = f_open(&ffile, (const TCHAR*)file_name, FA_READ);
	if (r != FR_OK) return 0;

	FontFile_Header_t fhdr;

	Font_type_t* font = (Font_type_t*)buffer;
	uint32_t br = 0;
	f_read(&ffile, &fhdr, sizeof(FontFile_Header_t), &br);

	font->height = fhdr.height;
	font->spacing = fhdr.spacing;
	font->start = fhdr.start;
	font->stop = fhdr.stop;
	font->width = fhdr.width;
	font->descr = (Font_Char_info_t*)(buffer + sizeof(Font_type_t));
	font->bitmaps = ((uint8_t*)font->descr) + sizeof(Font_Char_info_t)*(fhdr.stop-fhdr.start+1);
	memset(font->name, 0, 16);
	memcpy(font->name, fhdr.name, 16);

	f_read(&ffile, font->descr, f_size(&ffile)-sizeof(FontFile_Header_t), &br);
	f_close(&ffile);

	if (size) *size = sizeof(Font_type_t)+f_size(&ffile)-sizeof(FontFile_Header_t);
	res = 1;
#endif
	return res;
}

//
//Private members
//

uint8_t bmp_get_pixel(DSP_Bitmap_t* bmp, uint16_t x, uint16_t y)
{
	uint16_t width = bmp->width;
	if (width%8) width = (width/8+1)*8;
	return (bmp->data[x/8 + y*(width/8)] & (0x80 >> x%8))?1:0;
}

void bmp_set_pixel(DSP_Bitmap_t* bmp, uint16_t x, uint16_t y, uint8_t pixel)
{
	uint16_t width = bmp->width;
	if (width%8) width = (width/8+1)*8;
	if (pixel) bmp->data[x/8 + y*(width/8)] |= (0x80 >> x%8);
	else bmp->data[x/8 + y*(width/8)] &= ~(0x80 >> x%8);
}
