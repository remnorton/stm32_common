/*
 * st7789.cpp
 *
 *  Created on: 23.09.2018
 *		Author: Rudenko Roman (aka Rem Norton)
 *
 */
#include <stdlib.h>
#include <string.h>
#include "st7789.h"


//
//Private definitions
//

#define ST77XX_NOP     0x00
#define ST77XX_SWRESET 0x01
#define ST77XX_RDDID   0x04
#define ST77XX_RDDST   0x09

#define ST77XX_SLPIN   0x10
#define ST77XX_SLPOUT  0x11
#define ST77XX_PTLON   0x12
#define ST77XX_NORON   0x13

#define ST77XX_INVOFF  0x20
#define ST77XX_INVON   0x21
#define ST77XX_GAMMA   0x26
#define ST77XX_DISPOFF 0x28
#define ST77XX_DISPON  0x29
#define ST77XX_CASET   0x2A
#define ST77XX_RASET   0x2B
#define ST77XX_RAMWR   0x2C
#define ST77XX_RAMRD   0x2E

#define ST77XX_PTLAR   0x30
#define ST77XX_COLMOD  0x3A
#define ST77XX_MADCTL  0x36

#define ST77XX_DISBV    0x51
#define ST77XX_CTRLD    0x53


#define ST77XX_PGAMMA	0xE0
#define ST77XX_NGAMMA	0xE1


#define ST77XX_MADCTL_MY  0x80
#define ST77XX_MADCTL_MX  0x40
#define ST77XX_MADCTL_MV  0x20
#define ST77XX_MADCTL_ML  0x10
//#define ST77XX_MADCTL_RGB 0x00

#define ST77XX_RDID1   0xDA
#define ST77XX_RDID2   0xDB
#define ST77XX_RDID3   0xDC
#define ST77XX_RDID4   0xDD

#define ST7789_ID   0x7789
#define ST7789_BPP  16


static const uint8_t startup[] = {
            ST77XX_SWRESET,   0,  50,                     //    50 ms delay
            ST77XX_COLMOD , 1 ,  0, 0x55,                     //     1 ms delay
            ST77XX_MADCTL , 1 , 0, 0xC8, /*0x08*/                //     Row addr/col addr, bottom to top refresh, B-G-R color order
			ST77XX_GAMMA, 1, 0, 0x01,

			ST77XX_PGAMMA, 15, 0, 0x0F, 0x31, 0x2B, 0x0C, 0x0E,
					0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03,
					0x0E, 0x09, 0x00,							// positive gamma-correction curve
			ST77XX_NGAMMA, 15, 0, 0x00, 0x0E, 0x14, 0x03, 0x11,
					0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C,
					0x31, 0x36, 0x0F, 							// negative gamma-correction curve

			ST77XX_SLPOUT,	0, 100,
            ST77XX_DISPON , 0 , 10,
			ST77XX_NOP, 0, 0			//Termination marker
};

//
//Aux. functions
//

void st7789_reset(ST7789_Init_t* instance)
{
    if (!instance) return;
    HAL_GPIO_WritePin(instance->reset.port,instance->reset.pin, instance->reset.activeState);
    HAL_Delay(50);
    HAL_GPIO_WritePin(instance->reset.port,instance->reset.pin, !instance->reset.activeState);
    HAL_Delay(50);
}

void st7789_write_cmd(ST7789_Init_t* instance, uint8_t cmd)
{
    if (!instance) return;
    HAL_GPIO_WritePin(instance->dc.port, instance->dc.pin, !instance->dc.activeState);
    HAL_GPIO_WritePin(instance->cs.port, instance->cs.pin, instance->cs.activeState);
    HAL_SPI_Transmit(instance->spi, &cmd, sizeof(cmd), 1);
    HAL_GPIO_WritePin(instance->cs.port, instance->cs.pin, !instance->cs.activeState);
}

void st7789_write_data(ST7789_Init_t* instance, uint8_t* data, uint16_t len)
{
    if (!instance) return;
    HAL_GPIO_WritePin(instance->dc.port, instance->dc.pin, instance->dc.activeState);
    HAL_GPIO_WritePin(instance->cs.port, instance->cs.pin, instance->cs.activeState);
    HAL_SPI_Transmit(instance->spi, data, len, len*3/10);
    HAL_GPIO_WritePin(instance->cs.port, instance->cs.pin, !instance->cs.activeState);
}

void st7789_SetAddressWindow(ST7789_Init_t* instance, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    uint8_t xdata[4] = { 0x00, x0, 0x00, x1};
    uint8_t ydata[4] = { 0x00, y0, 0x00, y1};


    // column address set
    st7789_write_cmd(instance, ST77XX_CASET);
    st7789_write_data(instance, xdata, sizeof(xdata));

    // row address set
    st7789_write_cmd(instance, ST77XX_RASET);
    st7789_write_data(instance, ydata, sizeof(ydata));

}

void st7789_fill_data(ST7789_Init_t* instance, uint16_t data, uint32_t count, uint8_t gram)
{
	if (!instance) return;
	uint16_t len = (count > instance->bufSize)?instance->bufSize:count;
	uint16_t* buf = (uint16_t*)malloc(len*sizeof(len));
	if (!buf) return;

	//prepare data
	for (int i = 0; i < len; i++) buf[i] = data;

	if (gram) st7789_write_cmd(instance, ST77XX_RAMWR);
	HAL_GPIO_WritePin(instance->dc.port, instance->dc.pin, gram?instance->dc.activeState:!instance->dc.activeState);

	//send data
	HAL_GPIO_WritePin(instance->cs.port, instance->cs.pin, instance->cs.activeState);
	while(count)
	{
		HAL_SPI_Transmit(instance->spi, (uint8_t*)buf, len*sizeof(len), len*3/10);
		count -= len;
		len = (count > instance->bufSize)?instance->bufSize:count;
	}
	HAL_GPIO_WritePin(instance->cs.port, instance->cs.pin, !instance->cs.activeState);
	free(buf);

}

//
//Driver members
//
uint8_t st7789_init(void* init)
{
    if (!init) return 0;
    ST7789_Init_t* instance = (ST7789_Init_t*)init;

    st7789_reset(instance);

    Command8_t* cmd = (Command8_t*)startup;
	while (cmd->cmd || cmd->para_count || cmd->delay)
	{
		st7789_write_cmd(instance, cmd->cmd);
		if (cmd->para_count)
			st7789_write_data(instance, cmd->para, cmd->para_count);
		if (cmd->delay)
			HAL_Delay(cmd->delay);
		cmd = (Command8_t*)((uint8_t*)cmd + (cmd->para_count + 3));
	}


    st7789_SetAddressWindow(instance, 0, 0, instance->width, instance->height);
    return 1;
}

uint16_t st7789_id(void* i)
{
	if (!i) return 0;
    return ST7789_ID;
}

uint16_t st7789_width(void* i)
{
	if (!i) return 0;
	return ((ST7789_Init_t*)i)->width;
}

uint16_t st7789_height(void* i)
{
	if (!i) return 0;
    return ((ST7789_Init_t*)i)->height;
}

void st7789_FillRectangle(void* i, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color)
{
	ST7789_Init_t* instance = (ST7789_Init_t*)i;
	if (!instance) return;
    if (h > instance->height) h = instance->height;
    if (w > instance->width) w = instance->width;
    if ((x+w) > instance->width) w = instance->width - x;
    if ((y+h) > instance->height) h = instance->height - y;

    uint16_t cl = color & 0xFFFF;

    st7789_SetAddressWindow(instance, x, y, x+w, y+h);
    st7789_fill_data(instance, ((cl & 0xff) << 8) | (cl >> 8), w*h, 1);
}

uint32_t st7789_rgbColor(void* i, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	UNUSED(i);
	UNUSED(a);
	uint16_t max_r, max_g, max_b;
	max_r = max_b = 0x1f;
	max_g = 0x3f;

	uint16_t rs = r*100/255;
	uint16_t bs = b*100/255;
	uint16_t gs = g*100/255;

	RGB16_ColorStruct_t color_struct;
	color_struct.color = 0;

	color_struct.rgb.r = rs*max_r/100;
	color_struct.rgb.g = gs*max_g/100;
	color_struct.rgb.b = bs*max_b/100;

	//color_struct.rgb.b = bs*max_b/100;

	return color_struct.color;
}

void st7789_clear(void *i, uint32_t color)
{
	uint16_t cl = color & 0xFFFF;
	uint16_t w = st7789_width(i);
	uint16_t h = st7789_height(i);
	st7789_SetAddressWindow((ST7789_Init_t*)i, 2, 3, 2+w, 3+h);
	st7789_fill_data((ST7789_Init_t*)i, ((cl & 0xff) << 8) | (cl >> 8), w*h, 1);
}

void st7789_inverse(void* i, uint8_t inv)
{
	if (!i) return;
    st7789_write_cmd((ST7789_Init_t*)i, inv?ST77XX_INVOFF:ST77XX_INVON);
}

void st7789_swithOn(void* i, uint8_t on)
{
	if (!i) return;
    st7789_write_cmd((ST7789_Init_t*)i, on?ST77XX_DISPON:ST77XX_DISPOFF);
    //st7789_write_cmd(on?ST77XX_SLPOUT:ST77XX_SLPIN);
}

void st7789_drawPixel(void* i, uint32_t x, uint32_t y, uint32_t color)
{
	ST7789_Init_t* instance = (ST7789_Init_t*)i;
	if (!instance) return;
    if (x > st7789_width(instance)) x = st7789_width(instance);
    if (y > st7789_height(instance)) y = st7789_height(instance);
    st7789_SetAddressWindow(instance, x,y,x,y);
    uint16_t cl = color & 0xffff;
    cl = ((cl & 0xff) << 8) | (cl >> 8);
    st7789_write_cmd(instance, ST77XX_RAMWR);
    st7789_write_data(instance, (uint8_t*)&cl, sizeof(cl));
}

uint32_t st7789_pixelColor(uint32_t x, uint32_t y)
{
    UNUSED(x);
    UNUSED(y);
    //TODO: read from GRAM
    return 0;
}

uint32_t st7789_bitPerPixel(void* i)
{
	UNUSED(i);
    return ST7789_BPP;
}

void st7789_drawImage(void *i, uint32_t x, uint32_t y, DSP_Image_t* img)
{
	ST7789_Init_t* instance = (ST7789_Init_t*)i;
	if (!instance) return;
    if (img->bit_per_pixel < 16) return;

    uint16_t w = img->width;
    uint16_t h = img->height;

    if ((x+w) > st7789_width(instance)) w = st7789_width(instance) - x;
    if ((y+h) > st7789_height(instance)) h = st7789_height(instance) - y;

    st7789_SetAddressWindow(instance, x,y,x+w-1,y+h-1);
    st7789_write_cmd(instance, ST77XX_RAMWR);
    st7789_write_data(instance, img->data, w*h*sizeof(uint16_t));
}

void st7789_drawBitmap(void* i, uint32_t x, uint32_t y, DSP_Bitmap_t* bmp, uint32_t color, uint32_t bkColor)
{
	UNUSED(bkColor);
	ST7789_Init_t* instance = (ST7789_Init_t*)i;
	if (!instance) return;
	uint16_t size = bmp->height*bmp->width*2;
	if (!size) return;

	//prepare background
	DSP_Image_t bg;
	bg.bit_per_pixel = 16;
	bg.height = bmp->height;
	bg.width = bmp->width;
	bg.data = (uint8_t*)malloc(size);

	if (!bg.data) return;
	uint16_t* pixel = (uint16_t*) bg.data;
	uint32_t width = bmp->width;
	while (width % 8) width++;

	for (int h = 0; h < bmp->height; h++)
	{
		for (int w = 0; w < bmp->width; w++)
		{
			if (bmp->data[w/8+(h*width/8)] & (0x80 >> (w%8)))
				*pixel = (color >> 8) | ((color & 0xff) << 8);
			else
				*pixel = (bkColor >> 8) | ((bkColor & 0xff) << 8);
			pixel++;
		}
	}
	st7789_drawImage(i, x, y, &bg);
	free(bg.data);

}

RGB_Color_t st7789_getColor(void* i, uint32_t color)
{
    UNUSED(i);
    RGB_Color_t res = {(color >> 16) & 0xFF, (color >> 8) & 0xFF, (color & 0xFF), 0xFF};
    return res;
}

//
//Driver definition
//
DSP_DriverDef_t st7789_driver =
{
    st7789_init,            /*uint8_t        (*init)(void*);*/
	0,						/*void		(*deInit)(void*);*/
    st7789_id,              /*uint16_t    (*getId)(void*);*/
    st7789_width,           /*uint16_t    (*screenWidth)(void*);*/
    st7789_height,          /*uint16_t    (*screenHeight)(void*);*/
    st7789_clear,           /*void        (*clearScreen)(void*,uint32_t);*/
    st7789_inverse,         /*void        (*setInverse)(void*,uint8_t);*/
    st7789_swithOn,         /*void        (*switchOn)(void*,uint8_t);*/
    0,                      /*void        (*setBrightnes)(void*,uint32_t);*/
    st7789_drawPixel,       /*void        (*drawPixel)(void*,uint32_t, uint32_t, uint32_t);*/
    0,        				/*void        (*drawLine)(void*,uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);*/
    0,   					/*void        (*drawRectangle)(void*,uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);*/
    st7789_FillRectangle,   /*void        (*fillRectangle)(void*,uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);*/
    0,        				/*void        (*sendCommands)(void*,uint8_t*, uint8_t);*/
    0,                      /*void		  (*sendData)(void*,uint8_t*, uint16_t);*/
    0,                      /*void        (*pushData)(void*);*/
	st7789_rgbColor,        /*uint32_t    (*rgbColor)(void*,uint8_t, uint8_t, uint8_t, uint8_t);*/
    0,                      /*uint32_t    (*pixelColor)(void*,uint32_t, uint32_t);*/
	st7789_bitPerPixel,     /*uint32_t    (*bitPerPixel)(void*);*/
	st7789_drawBitmap,      /*void        (*drawBitmap)(void*,uint32_t, uint32_t, DSP_Bitmap_t*, uint32_t, uint32_t );*/
	st7789_drawImage,		/*void		  (*drawImage)(void*,uint32_t, uint32_t, DSP_Image_t*);*/
	st7789_getColor,        /*RGB_Color_t (*decodeColor)(void*,uint32_t);*/
    0,      				/*void		 (*rotate)(void*,uint8_t);*/
    0,     					/*void		 (*sleep)(void*,uint8_t);*/

};

