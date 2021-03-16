/*
 * ili9341.c
 *
 *  Created on: 5 џэт. 2019 у.
 *      Author: Rem Norton
 */
#include <string.h>
#include <stdlib.h>
#include "ili9844.h"
#include "lcdlib.h"

//
//Supported commands
//
#define ILI9844_NOP					0x00
#define ILI9844_RESET				0x01
#define ILI9844_SLEEP_IN			0x10
#define ILI9844_SLEEP_OUT			0x11
#define ILI9844_GAMMA				0x26
#define ILI9844_DISPLAY_OFF			0x28
#define ILI9844_DISPLAY_ON			0x29
#define ILI9844_COLUMN_ADDR			0x2A
#define ILI9844_PAGE_ADDR			0x2B
#define ILI9844_GRAM				0x2C
#define ILI9844_GREAD				0x2E
#define ILI9844_MAC			        0x36
#define ILI9844_PIXEL_FORMAT        0x3A
#define ILI9844_WDB			    	0x51
#define ILI9844_WCD				    0x53
#define ILI9844_RGB_INTERFACE       0xB0
#define ILI9844_FRC				    0xB1
#define ILI9844_BPC				    0xB5
#define ILI9844_DFC				    0xB6
#define ILI9844_POWER1				0xC0
#define ILI9844_POWER2				0xC1
#define ILI9844_VCOM1				0xC5
#define ILI9844_VCOM2				0xC7
#define ILI9844_POWERA				0xCB
#define ILI9844_POWERB				0xCF
#define ILI9844_PGAMMA				0xE0
#define ILI9844_NGAMMA				0xE1
#define ILI9844_DTCA				0xE8
#define ILI9844_DTCB				0xEA
#define ILI9844_POWER_SEQ			0xED
#define ILI9844_3GAMMA_EN			0xF2
#define ILI9844_INTERFACE			0xF6
#define ILI9844_PRC				    0xF7


#define ILI9844_LCD_WIDTH       480
#define ILI9844_LCD_HEIGHT      320
#define ILI9844_LINES_FILL		2

#define  ILI9844_BYTE_PER_PIXEL	3

#define _MAX_FILL_LEN_			(ILI9844_LCD_WIDTH*ILI9844_LINES_FILL)

//
//Startup sequence
//

static uint8_t ili9844_startup[] =
{
		ILI9844_RESET, 0, 100,								//software reset
		ILI9844_MAC, 1, 0, 0x28,							// landscape, B-G-R color order
		ILI9844_PIXEL_FORMAT, 1, 0, 0x66,					// RGB-666 format
		ILI9844_SLEEP_OUT, 0, 150,							//
		ILI9844_DISPLAY_ON, 0, 0,							//
		ILI9844_NOP, 0, 0									// Termination marker
};

//
//Aux. functions
//

void ili9844_SendCmd(ILI9844_INIT_t* instance, uint8_t cmd)
{
	if (!instance) return;
	HAL_GPIO_WritePin(instance->dc.port, instance->dc.pin, !instance->dc.activeState);
	HAL_GPIO_WritePin(instance->cs.port, instance->cs.pin, instance->cs.activeState);
#ifdef HAL_SPI_MODULE_ENABLED
	HAL_SPI_Transmit(instance->spi, &cmd, 1, 10);
#endif
	HAL_GPIO_WritePin(instance->cs.port, instance->cs.pin, !instance->cs.activeState);

}

void ili9844_SendData(ILI9844_INIT_t* instance, uint8_t data)
{
	if (!instance) return;
	HAL_GPIO_WritePin(instance->dc.port, instance->dc.pin, instance->dc.activeState);
	HAL_GPIO_WritePin(instance->cs.port, instance->cs.pin, instance->cs.activeState);
#ifdef HAL_SPI_MODULE_ENABLED
	HAL_SPI_Transmit(instance->spi, &data, 1, 10);
#endif
	HAL_GPIO_WritePin(instance->cs.port, instance->cs.pin, !instance->cs.activeState);
}

void ili9844_SendMultiData(ILI9844_INIT_t* instance, uint8_t* data, uint16_t len)
{
	if (!instance) return;
	if (!data) return;
	HAL_GPIO_WritePin(instance->dc.port, instance->dc.pin, instance->dc.activeState);
	HAL_GPIO_WritePin(instance->cs.port, instance->cs.pin, instance->cs.activeState);
#ifdef HAL_SPI_MODULE_ENABLED
	HAL_SPI_Transmit(instance->spi, data, len, len*10);
#endif
	HAL_GPIO_WritePin(instance->cs.port, instance->cs.pin, !instance->cs.activeState);
}

void ili9844_SetDisplayWindow(ILI9844_INIT_t* instance, uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
	if (!instance) return;
	uint16_t x2 = Xpos+Width-1;
	uint16_t y2 = Ypos+Height-1;

	uint8_t xdata[4] = {(Xpos >> 8) & 0xff, Xpos & 0xff, (x2 >> 8) & 0xff, x2 & 0xff};
	uint8_t ydata[4] = {(Ypos >> 8) & 0xff, Ypos & 0xff, (y2 >> 8) & 0xff, y2 & 0xff};

	ili9844_SendCmd(instance, ILI9844_COLUMN_ADDR);
	ili9844_SendMultiData(instance, xdata, 4);

	ili9844_SendCmd(instance, ILI9844_PAGE_ADDR);
	ili9844_SendMultiData(instance, ydata, 4);

}

void ili9844_FillData(ILI9844_INIT_t* instance, uint32_t data, uint32_t count, uint8_t gram)
{
	if (!instance) return;
	uint16_t len = (count > _MAX_FILL_LEN_)?_MAX_FILL_LEN_:count;
	uint16_t buf_size = len*ILI9844_BYTE_PER_PIXEL;
	uint8_t* buf = (uint8_t*)malloc(buf_size);
	if (!buf) return;

	memset(buf, 0, buf_size);

	for (int i = 0; i < len; i++)
		memcpy(&buf[i*ILI9844_BYTE_PER_PIXEL], &data, ILI9844_BYTE_PER_PIXEL);

	if (gram) ili9844_SendCmd(instance,ILI9844_GRAM);

	while(count)
	{
		ili9844_SendMultiData(instance, buf, buf_size);
		count -= len;
		len = (count > _MAX_FILL_LEN_)?_MAX_FILL_LEN_:count;
	}
	free(buf);
}

void ili9844_SetCursor(ILI9844_INIT_t* instance, uint16_t Xpos, uint16_t Ypos)
{
	ili9844_SetDisplayWindow(instance, Xpos, Ypos, 1, 1);
}

uint8_t ili9844_ReadImage(ILI9844_INIT_t* instance, uint16_t x, uint16_t y, DSP_Image_t* img)
{
	if (!img) return 0;
	uint16_t count = img->height*img->width;
	if (!count) return 0;
	uint16_t* dta = (uint16_t*)img->data;
	ili9844_SetDisplayWindow(instance, x, y, img->width, img->height);
	HAL_GPIO_WritePin(instance->dc.port, instance->dc.pin, !instance->dc.activeState);
	HAL_GPIO_WritePin(instance->cs.port, instance->cs.pin, instance->cs.activeState);
	uint8_t buf[4] = {ILI9844_GREAD};
	HAL_SPI_Transmit(instance->spi, buf, 1, 10);
	HAL_GPIO_WritePin(instance->dc.port, instance->dc.pin, instance->dc.activeState);
	//dummy read
	HAL_SPI_Receive(instance->spi, buf, 1, 50);
	while(count)
	{
		HAL_SPI_Receive(instance->spi, buf, 3, 50);
		RGB16_ColorStruct_t color_struct;
		color_struct.color = 0;
		color_struct.rgb.r = buf[0]*0x1f/0xfc;
		color_struct.rgb.g = buf[1]*0x3f/0xfc;
		color_struct.rgb.b = buf[2]*0x1f/0xfc;
		*dta = ((color_struct.color & 0xff) << 8) | (color_struct.color >> 8);
		dta++;
		count--;
	}
	HAL_GPIO_WritePin(instance->cs.port, instance->cs.pin, !instance->cs.activeState);
	return 1;
}

//
//Driver functions
//
uint8_t ili9844_init(void* i)
{
	ILI9844_INIT_t* instance = (ILI9844_INIT_t*)i;
	if (!instance) return 0;

	HAL_GPIO_WritePin(instance->reset.port, instance->reset.pin, instance->reset.activeState);
	HAL_Delay(10);
	HAL_GPIO_WritePin(instance->reset.port, instance->reset.pin, !instance->reset.activeState);
	HAL_Delay(200);


	uint8_t cmd_count = 0;
	Command8_t* cmd = (Command8_t*)ili9844_startup;
	while (cmd->cmd || cmd->para_count || cmd->delay)
	{
		ili9844_SendCmd(instance, cmd->cmd);
		if (cmd->para_count)
			ili9844_SendMultiData(instance, cmd->para, cmd->para_count);
		if (cmd->delay)
			HAL_Delay(cmd->delay);
		cmd = (Command8_t*)((uint8_t*)cmd + (cmd->para_count + 3));
		cmd_count++;
	}

	return 1;
}

uint16_t ili9844_getId(void* i)
{
	UNUSED(i);
	return 0x9341;
}

uint16_t ili9844_screenWidth(void* i)
{
	UNUSED(i);
	return ILI9844_LCD_WIDTH;
}

uint16_t ili9844_screenHeight(void* i)
{
	UNUSED(i);
	return ILI9844_LCD_HEIGHT;
}

void ili9844_switchOn(void* i, uint8_t on)
{
	ILI9844_INIT_t* instance = (ILI9844_INIT_t*)i;
	if (!instance) return;
	ili9844_SendCmd(instance, on?ILI9844_DISPLAY_ON:ILI9844_DISPLAY_OFF);
}

void ili9844_drawPixel(void* i,uint32_t x, uint32_t y, uint32_t color)
{
	ILI9844_INIT_t* instance = (ILI9844_INIT_t*)i;
	if (!instance) return;
	ili9844_SetCursor(instance, x, y);
	ili9844_SendCmd(instance,ILI9844_GRAM);
	ili9844_SendMultiData(instance, &color, 3);
}

void ili9844_drawLine(void* i, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t color)
{
	if (!i) return;
	ILI9844_INIT_t* instance = (ILI9844_INIT_t*)i;

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
            	ili9844_drawPixel(i, x,y, color);
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
                    	ili9844_drawLine(i, x,prev_y,x,y, color);
                    else
                    	ili9844_drawPixel(i,x,y, color);
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
                    	ili9844_drawLine(i, prev_x,y,x,y,color);
                    else
                    	ili9844_drawPixel(i,x,y, color);
                    prev_x = x;
                    y += sy;
                }
            }
        }
    }
    else
    {
    	ili9844_SetDisplayWindow(instance, (x1 < x2)?x1:x2, (y1 < y2)?y1:y2, abs(dx)?abs(dx):1, abs(dy)?abs(dy):1);
        ili9844_FillData(instance, color, dx*sx+dy*sy, 1);
    }

}

void ili9844_fillRectangle(void* i, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color)
{
	ILI9844_INIT_t* instance = (ILI9844_INIT_t*)i;
	if (!instance) return;
	ili9844_SetDisplayWindow(instance, x, y, width, height);
	ili9844_FillData(instance, color, width*height, 1);
}

void ili9844_clearScreen(void* i, uint32_t color)
{
	ILI9844_INIT_t* instance = (ILI9844_INIT_t*)i;
	if (!instance) return;
	ili9844_fillRectangle(i, 0, 0, ILI9844_LCD_WIDTH, ILI9844_LCD_HEIGHT, color);
}

uint32_t ili9844_rgbColor(void* i, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	UNUSED(i);
	UNUSED(a);

	RGB24_ColorStruct_t color_struct;
	color_struct.color = 0;

	color_struct.rgb.r = b;
	color_struct.rgb.g = g;
	color_struct.rgb.b = r;

	return color_struct.color;
}

uint32_t ili9844_pixelColor(void* i, uint32_t x, uint32_t y)
{
	ILI9844_INIT_t* instance = (ILI9844_INIT_t*)i;
	if (!instance) return 0;
	uint8_t buf[4] = {ILI9844_GREAD};
	ili9844_SetCursor(instance, x, y);

	HAL_GPIO_WritePin(instance->dc.port, instance->dc.pin, !instance->dc.activeState);
	HAL_GPIO_WritePin(instance->cs.port, instance->cs.pin, instance->cs.activeState);
	HAL_SPI_Transmit(instance->spi, buf, 1, 10);
	HAL_GPIO_WritePin(instance->dc.port, instance->dc.pin, instance->dc.activeState);
	HAL_SPI_Receive(instance->spi, buf, sizeof(buf), 10);
	HAL_GPIO_WritePin(instance->cs.port, instance->cs.pin, !instance->cs.activeState);

	RGB24_ColorStruct_t color_struct;
	color_struct.color = 0;
	color_struct.rgb.r = buf[1];
	color_struct.rgb.g = buf[2];
	color_struct.rgb.b = buf[3];

	return color_struct.color;
}

uint32_t ili9844_bitPerPixel(void* i)
{
	UNUSED(i);
	return 16;
}

void ili9844_drawImage(void* i, uint32_t x, uint32_t y, DSP_Image_t* img)
{
	ILI9844_INIT_t* instance = (ILI9844_INIT_t*)i;
	if (!instance) return;
	if (!img) return;
	uint16_t size = img->height * img->width * ILI9844_BYTE_PER_PIXEL;
	if (!size) return;
	if (img->bit_per_pixel < 16) return;
	ili9844_SetDisplayWindow(instance, x, y, img->width, img->height);
	ili9844_SendCmd(instance, ILI9844_GRAM);
	if (img->bit_per_pixel == (ILI9844_BYTE_PER_PIXEL*8)) ili9844_SendMultiData(instance, img->data, size);
	else
	{
		//TODO: decode image
		uint8_t buf[_MAX_FILL_LEN_*2];
		uint16_t dpos = 0;
		size = img->height * img->width * 2;
		for (uint16_t spos = 0; spos < size; spos += 2)
		{

			RGB16_ColorStruct_t r16;
			memcpy(&r16.color, &img->data[spos], 2);
			RGB_Color_t full = dspRGB16ColorDecode(r16);
			uint32_t pixel = ili9844_rgbColor(i, full.red, full.green, full.blue, 255);
			memcpy(&buf[dpos], &pixel, 3);
			dpos += 3;

			if (dpos >= (_MAX_FILL_LEN_*2))
			{
				ili9844_SendMultiData(instance, buf, dpos);
				dpos = 0;
			}
		}
		if (dpos)
			ili9844_SendMultiData(instance, buf, dpos);
	}
}

void ili9844_drawBitmap(void* i, uint32_t x, uint32_t y, DSP_Bitmap_t* bmp, uint32_t color, uint32_t bkColor)
{
	UNUSED(bkColor);
	ILI9844_INIT_t* instance = (ILI9844_INIT_t*)i;
	if (!instance) return;
	uint16_t len = bmp->height*bmp->width;
	if (!len) return;

	uint16_t size = len*ILI9844_BYTE_PER_PIXEL;

	//prepare background
	DSP_Image_t bg;
	bg.bit_per_pixel = 24;
	bg.height = bmp->height;
	bg.width = bmp->width;
	bg.data = (uint8_t*)malloc(size);

	if (!bg.data) return;

#ifdef _DRAW_OVER_GRADIENT
	ili9844_ReadImage(instance, x, y, &bg);
#endif

	uint8_t* pixel = bg.data;
	uint32_t width = bmp->width;
	while (width % 8) width++;

	for (int h = 0; h < bmp->height; h++)
	{
		for (int w = 0; w < bmp->width; w++)
		{
			if (bmp->data[w/8+(h*width/8)] & (0x80 >> (w%8)))
				memcpy(pixel, &color, ILI9844_BYTE_PER_PIXEL);
#ifndef _DRAW_OVER_GRADIENT
			else
				memcpy(pixel, &bkColor, ILI9844_BYTE_PER_PIXEL);
#endif
			pixel += ILI9844_BYTE_PER_PIXEL;
		}
	}
	ili9844_drawImage(i, x, y, &bg);
	free(bg.data);

}

RGB_Color_t ili9844_decodeColor(void* i, uint32_t color)
{
	UNUSED(i);
	RGB24_ColorStruct_t cs;

	cs.color = color;
    RGB_Color_t res = {0,0,0,255};

    res.red = cs.rgb.b;
    res.green = cs.rgb.g;
    res.blue = cs.rgb.r;
    return res;
}

//
//
//
DSP_DriverDef_t ili9844_driver = {
		ili9844_init,                   //void        (*init)(void*);
        0,//void		  (*deInit)(void*);
		ili9844_getId,					//uint16_t    (*getId)(void*);
		ili9844_screenWidth,			//uint16_t    (*screenWidth)(void*);
		ili9844_screenHeight,			//uint16_t    (*screenHeight)(void*);
		ili9844_clearScreen,           	//void        (*clearScreen)(void*,uint32_t);
        0,//void        (*setInverse)(void*,uint8_t);
		ili9844_switchOn,				//void        (*switchOn)(void*,uint8_t);
        0,//void        (*setBrightnes)(void*,uint32_t);
		ili9844_drawPixel,				//void        (*drawPixel)(void*,uint32_t, uint32_t, uint32_t);
		ili9844_drawLine,				//void        (*drawLine)(void*,uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
        0,//void        (*drawRectangle)(void*,uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
        ili9844_fillRectangle,			//void        (*fillRectangle)(void*,uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
        0,//void        (*sendCommands)(void*,uint8_t*, uint8_t);
        0,//void		  (*sendData)(void*,uint8_t*, uint16_t);
        0,//void        (*pushData)(void*);
		ili9844_rgbColor,				//uint32_t    (*rgbColor)(void*,uint8_t, uint8_t, uint8_t, uint8_t);
		ili9844_pixelColor,				//uint32_t    (*pixelColor)(void*,uint32_t, uint32_t);
		ili9844_bitPerPixel,			//uint32_t    (*bitPerPixel)(void*);
		ili9844_drawBitmap,				//void        (*drawBitmap)(void*,uint32_t, uint32_t, DSP_Bitmap_t*);
		ili9844_drawImage,				//void		  (*drawImage)(void*,uint32_t, uint32_t, DSP_Image_t*);
		ili9844_decodeColor,			//RGB_Color_t (*decodeColor)(void*,uint32_t);
        0,//void		  (*rotate)(void*,uint8_t);
};

