/*
 * ili9341.c
 *
 *  Created on: 5 џэт. 2019 у.
 *      Author: Rem Norton
 */
#include <string.h>
#include <stdlib.h>
#include "ili9341.h"

//
//Supported commands
//
#define ILI9341_NOP					0x00
#define ILI9341_RESET				0x01
#define ILI9341_SLEEP_OUT			0x11
#define ILI9341_GAMMA				0x26
#define ILI9341_DISPLAY_OFF			0x28
#define ILI9341_DISPLAY_ON			0x29
#define ILI9341_COLUMN_ADDR			0x2A
#define ILI9341_PAGE_ADDR			0x2B
#define ILI9341_GRAM				0x2C
#define ILI9341_GREAD				0x2E
#define ILI9341_MAC			        0x36
#define ILI9341_PIXEL_FORMAT        0x3A
#define ILI9341_WDB			    	0x51
#define ILI9341_WCD				    0x53
#define ILI9341_RGB_INTERFACE       0xB0
#define ILI9341_FRC				    0xB1
#define ILI9341_BPC				    0xB5
#define ILI9341_DFC				    0xB6
#define ILI9341_POWER1				0xC0
#define ILI9341_POWER2				0xC1
#define ILI9341_VCOM1				0xC5
#define ILI9341_VCOM2				0xC7
#define ILI9341_POWERA				0xCB
#define ILI9341_POWERB				0xCF
#define ILI9341_PGAMMA				0xE0
#define ILI9341_NGAMMA				0xE1
#define ILI9341_DTCA				0xE8
#define ILI9341_DTCB				0xEA
#define ILI9341_POWER_SEQ			0xED
#define ILI9341_3GAMMA_EN			0xF2
#define ILI9341_INTERFACE			0xF6
#define ILI9341_PRC				    0xF7


//#define ILI9341_LCD_WIDTH       240
//#define ILI9341_LCD_HEIGHT      320

#define ILI9341_LCD_WIDTH       320
#define ILI9341_LCD_HEIGHT      240
#ifndef ILI9341_LINES_FILL
#define ILI9341_LINES_FILL		10
#endif

#define _MAX_FILL_LEN_			(ILI9341_LCD_WIDTH*ILI9341_LINES_FILL)

//
//Startup sequence
//

static uint8_t startup[] =
{
		ILI9341_RESET, 0, 100,								//software reset
		ILI9341_POWER1, 1, 0, 0x23,							//
		ILI9341_POWER2, 1, 0, 0x10,							//
		ILI9341_MAC, 1, 0, 0x28,							// landscape, B-G-R color order
		//ILI9341_MAC, 1, 0, 0x20,							// landscape, R-G-B color order
		ILI9341_PIXEL_FORMAT, 1, 0, 0x55,					// RGB 565 format
		ILI9341_FRC, 2, 0, 0x00, 0x18,						//
		ILI9341_DFC, 3, 0, 0x08, 0x82, 0x27,				//
		ILI9341_3GAMMA_EN, 1, 0, 0x00,						//
		ILI9341_COLUMN_ADDR, 4, 0, 0x00, 0x00, 0x01, 0x3F,	// 0..320
		ILI9341_PAGE_ADDR, 4, 0, 0x00, 0x00, 0x00, 0xEF,	// 0..240
		ILI9341_GAMMA, 1, 0, 0x01,							//
		ILI9341_PGAMMA, 15, 0, 0x0F, 0x31, 0x2B, 0x0C, 0x0E,
				0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03,
				0x0E, 0x09, 0x00,							// positive gamma-correction curve
		ILI9341_NGAMMA, 15, 0, 0x00, 0x0E, 0x14, 0x03, 0x11,
				0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C,
				0x31, 0x36, 0x0F, 							// negative gamma-correction curve
		ILI9341_SLEEP_OUT, 0, 100,							//
		ILI9341_DISPLAY_ON, 0, 0,							//
		ILI9341_WCD, 1, 0, 0x20,							//
		ILI9341_WDB, 1, 0, 1,								//
		ILI9341_GRAM, 0, 0,									//
		ILI9341_NOP, 0, 0									// Termination marker
};

//
//Aux. functions
//

void ili9341_SendCmd(ILI9341_INIT_t* instance, uint8_t cmd)
{
	if (!instance) return;
	HAL_GPIO_WritePin(instance->dc.port, instance->dc.pin, !instance->dc.activeState);
	HAL_GPIO_WritePin(instance->cs.port, instance->cs.pin, instance->cs.activeState);
#ifdef HAL_SPI_MODULE_ENABLED
	HAL_SPI_Transmit(instance->spi, &cmd, 1, 10);
#endif
	HAL_GPIO_WritePin(instance->cs.port, instance->cs.pin, !instance->cs.activeState);

}

void ili9341_SendData(ILI9341_INIT_t* instance, uint8_t data)
{
	if (!instance) return;
	HAL_GPIO_WritePin(instance->dc.port, instance->dc.pin, instance->dc.activeState);
	HAL_GPIO_WritePin(instance->cs.port, instance->cs.pin, instance->cs.activeState);
#ifdef HAL_SPI_MODULE_ENABLED
	HAL_SPI_Transmit(instance->spi, &data, 1, 10);
#endif
	HAL_GPIO_WritePin(instance->cs.port, instance->cs.pin, !instance->cs.activeState);
}

void ili9341_SendMultiData(ILI9341_INIT_t* instance, uint8_t* data, uint16_t len)
{
	if (!instance) return;
	HAL_GPIO_WritePin(instance->dc.port, instance->dc.pin, instance->dc.activeState);
	HAL_GPIO_WritePin(instance->cs.port, instance->cs.pin, instance->cs.activeState);
#ifdef HAL_SPI_MODULE_ENABLED
	HAL_SPI_Transmit(instance->spi, data, len, len*2/10+10);
#endif
	HAL_GPIO_WritePin(instance->cs.port, instance->cs.pin, !instance->cs.activeState);
}

void ili9341_SetDisplayWindow(ILI9341_INIT_t* instance, uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
	if (!instance) return;
	uint16_t x2 = Xpos+Width-1;
	uint16_t y2 = Ypos+Height-1;

	uint8_t xdata[4] = {(Xpos >> 8) & 0xff, Xpos & 0xff, (x2 >> 8) & 0xff, x2 & 0xff};
	uint8_t ydata[4] = {(Ypos >> 8) & 0xff, Ypos & 0xff, (y2 >> 8) & 0xff, y2 & 0xff};

	ili9341_SendCmd(instance, ILI9341_COLUMN_ADDR);
	ili9341_SendMultiData(instance, xdata, 4);

	ili9341_SendCmd(instance, ILI9341_PAGE_ADDR);
	ili9341_SendMultiData(instance, ydata, 4);

}

void ili9341_FillData(ILI9341_INIT_t* instance, uint16_t data, uint32_t count, uint8_t gram)
{
	if (!instance) return;
	uint16_t len = (count > _MAX_FILL_LEN_)?_MAX_FILL_LEN_:count;
	uint16_t* buf = (uint16_t*)malloc(len*sizeof(len));
	if (!buf) return;
	for (int i = 0; i < len; i++)
		buf[i] = data;

	if (gram) ili9341_SendCmd(instance,ILI9341_GRAM);

	while(count)
	{
		ili9341_SendMultiData(instance, (uint8_t*)buf, len*sizeof(len));
		count -= len;
		len = (count > _MAX_FILL_LEN_)?_MAX_FILL_LEN_:count;
	}
	free(buf);
}

void ili9341_SetCursor(ILI9341_INIT_t* instance, uint16_t Xpos, uint16_t Ypos)
{
	ili9341_SetDisplayWindow(instance, Xpos, Ypos, 1, 1);
}

uint8_t ili9341_ReadImage(ILI9341_INIT_t* instance, uint16_t x, uint16_t y, DSP_Image_t* img)
{
	if (!img) return 0;
	uint16_t count = img->height*img->width;
	if (!count) return 0;
	uint16_t* dta = (uint16_t*)img->data;
	ili9341_SetDisplayWindow(instance, x, y, img->width, img->height);
	HAL_GPIO_WritePin(instance->dc.port, instance->dc.pin, !instance->dc.activeState);
	HAL_GPIO_WritePin(instance->cs.port, instance->cs.pin, instance->cs.activeState);
	uint8_t buf[4] = {ILI9341_GREAD};
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
uint8_t ili9341_init(void* i)
{
	ILI9341_INIT_t* instance = (ILI9341_INIT_t*)i;
	if (!instance) return 0;

	HAL_GPIO_WritePin(instance->reset.port, instance->reset.pin, instance->reset.activeState);
	HAL_Delay(20);
	HAL_GPIO_WritePin(instance->reset.port, instance->reset.pin, !instance->reset.activeState);
	HAL_Delay(200);


	uint8_t cmd_count = 0;
	Command8_t* cmd = (Command8_t*)startup;
	while (cmd->cmd || cmd->para_count || cmd->delay)
	{
		ili9341_SendCmd(instance, cmd->cmd);
		if (cmd->para_count)
			ili9341_SendMultiData(instance, cmd->para, cmd->para_count);
		if (cmd->delay)
			HAL_Delay(cmd->delay);
		cmd = (Command8_t*)((uint8_t*)cmd + (cmd->para_count + 3));
		cmd_count++;
	}

	return 1;
}

uint16_t ili9341_getId(void* i)
{
	UNUSED(i);
	return 0x9341;
}

uint16_t ili9341_screenWidth(void* i)
{
	UNUSED(i);
	return ILI9341_LCD_WIDTH;
}

uint16_t ili9341_screenHeight(void* i)
{
	UNUSED(i);
	return ILI9341_LCD_HEIGHT;
}

void ili9341_switchOn(void* i, uint8_t on)
{
	ILI9341_INIT_t* instance = (ILI9341_INIT_t*)i;
	if (!instance) return;
	ili9341_SendCmd(instance, on?ILI9341_DISPLAY_ON:ILI9341_DISPLAY_OFF);
}

void ili9341_drawPixel(void* i,uint32_t x, uint32_t y, uint32_t color)
{
	ILI9341_INIT_t* instance = (ILI9341_INIT_t*)i;
	if (!instance) return;
	ili9341_SetCursor(instance, x, y);
	ili9341_SendCmd(instance,ILI9341_GRAM);
	uint8_t pix[2] = {(uint8_t)(color >> 8), (uint8_t)(color & 0xff)};
	ili9341_SendMultiData(instance, pix, 2);
}

void ili9341_drawLine(void* i, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t color)
{
	if (!i) return;
	ILI9341_INIT_t* instance = (ILI9341_INIT_t*)i;

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
            	ili9341_drawPixel(i, x,y, color);
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
                    	ili9341_drawLine(i, x,prev_y,x,y, color);
                    else
                    	ili9341_drawPixel(i,x,y, color);
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
                    	ili9341_drawLine(i, prev_x,y,x,y,color);
                    else
                    	ili9341_drawPixel(i,x,y, color);
                    prev_x = x;
                    y += sy;
                }
            }
        }
    }
    else
    {
    	ili9341_SetDisplayWindow(instance, (x1 < x2)?x1:x2, (y1 < y2)?y1:y2, abs(dx)?abs(dx):1, abs(dy)?abs(dy):1);
        uint16_t cl = color & 0xffff;
        ili9341_FillData(instance, ((cl & 0xff) << 8) | (cl >> 8), dx*sx+dy*sy, 1);
    }

}

void ili9341_fillRectangle(void* i, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color)
{
	ILI9341_INIT_t* instance = (ILI9341_INIT_t*)i;
	if (!instance) return;
	ili9341_SetDisplayWindow(instance, x, y, width, height);
	uint16_t cl = color & 0xffff;
	ili9341_FillData(instance, ((cl & 0xff) << 8) | (cl >> 8), width*height, 1);
}

void ili9341_clearScreen(void* i, uint32_t color)
{
	ILI9341_INIT_t* instance = (ILI9341_INIT_t*)i;
	if (!instance) return;
	ili9341_fillRectangle(i, 0, 0, ILI9341_LCD_WIDTH, ILI9341_LCD_HEIGHT, color);
}

uint32_t ili9341_rgbColor(void* i, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
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

	return color_struct.color;
}

uint32_t ili9341_pixelColor(void* i, uint32_t x, uint32_t y)
{
	ILI9341_INIT_t* instance = (ILI9341_INIT_t*)i;
	if (!instance) return 0;
	uint8_t buf[4] = {ILI9341_GREAD};
	ili9341_SetCursor(instance, x, y);

	HAL_GPIO_WritePin(instance->dc.port, instance->dc.pin, !instance->dc.activeState);
	HAL_GPIO_WritePin(instance->cs.port, instance->cs.pin, instance->cs.activeState);
	HAL_SPI_Transmit(instance->spi, buf, 1, 10);
	HAL_GPIO_WritePin(instance->dc.port, instance->dc.pin, instance->dc.activeState);
	HAL_SPI_Receive(instance->spi, buf, sizeof(buf), 10);
	HAL_GPIO_WritePin(instance->cs.port, instance->cs.pin, !instance->cs.activeState);

	RGB16_ColorStruct_t color_struct;
	color_struct.color = 0;
	color_struct.rgb.r = buf[1] >> 2;
	color_struct.rgb.g = buf[2] >> 2;
	color_struct.rgb.b = buf[3] >> 2;

	return color_struct.color;
}

uint32_t ili9341_bitPerPixel(void* i)
{
	UNUSED(i);
	return 16;
}

void ili9341_drawImage(void* i, uint32_t x, uint32_t y, DSP_Image_t* img)
{
	ILI9341_INIT_t* instance = (ILI9341_INIT_t*)i;
	if (!instance) return;
	if (!img) return;
	uint16_t size = img->height * img->width * 2;
	if (!size) return;
	ili9341_SetDisplayWindow(instance, x, y, img->width, img->height);
	ili9341_SendCmd(instance, ILI9341_GRAM);
	ili9341_SendMultiData(instance, img->data, size);
}

void ili9341_drawBitmap(void* i, uint32_t x, uint32_t y, DSP_Bitmap_t* bmp, uint32_t color, uint32_t bkColor)
{
	UNUSED(bkColor);
	ILI9341_INIT_t* instance = (ILI9341_INIT_t*)i;
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

#ifdef _DRAW_OVER_GRADIENT
	ili9341_ReadImage(instance, x, y, &bg);
#else
	uint16_t* dta = (uint16_t*)bg.data;
	uint16_t count = size/2;
	uint16_t bc = bkColor;
	while (count)
	{
		*dta = ((bc & 0xff) << 8) | (bc >> 8);
		dta++;
		count--;
	}
#endif

	uint16_t* pixel = (uint16_t*) bg.data;
	uint32_t width = bmp->width;
	while (width % 8) width++;

	for (int h = 0; h < bmp->height; h++)
	{
		for (int w = 0; w < bmp->width; w++)
		{
			if (bmp->data[w/8+(h*width/8)] & (0x80 >> (w%8)))
				*pixel = (color >> 8) | ((color & 0xff) << 8);
			pixel++;
		}
	}
	ili9341_drawImage(i, x, y, &bg);
	free(bg.data);

}

RGB_Color_t ili9341_decodeColor(void* i, uint32_t color)
{
	UNUSED(i);
    RGB16_ColorStruct_t cs;
    uint16_t max_r, max_g, max_b;
    max_r = max_b = 0x1f;
    max_g = 0x3f;
    cs.color = color;
    RGB_Color_t res = {0,0,0,255};

    uint16_t r = cs.rgb.r;
    uint16_t g = cs.rgb.g;
    uint16_t b = cs.rgb.b;

    res.red = (r*255/max_r);
    res.green = (g*255/max_g);
    res.blue = (b*255/max_b);
    return res;
}

//
//
//
DSP_DriverDef_t ili9341_driver = {
		ili9341_init,                   //void        (*init)(void*);
        0,//void		  (*deInit)(void*);
		ili9341_getId,					//uint16_t    (*getId)(void*);
		ili9341_screenWidth,			//uint16_t    (*screenWidth)(void*);
		ili9341_screenHeight,			//uint16_t    (*screenHeight)(void*);
		ili9341_clearScreen,           	//void        (*clearScreen)(void*,uint32_t);
        0,//void        (*setInverse)(void*,uint8_t);
		ili9341_switchOn,				//void        (*switchOn)(void*,uint8_t);
        0,//void        (*setBrightnes)(void*,uint32_t);
		ili9341_drawPixel,				//void        (*drawPixel)(void*,uint32_t, uint32_t, uint32_t);
		ili9341_drawLine,				//void        (*drawLine)(void*,uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
        0,//void        (*drawRectangle)(void*,uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
        ili9341_fillRectangle,			//void        (*fillRectangle)(void*,uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
        0,//void        (*sendCommands)(void*,uint8_t*, uint8_t);
        0,//void		  (*sendData)(void*,uint8_t*, uint16_t);
        0,//void        (*pushData)(void*);
		ili9341_rgbColor,				//uint32_t    (*rgbColor)(void*,uint8_t, uint8_t, uint8_t, uint8_t);
		ili9341_pixelColor,				//uint32_t    (*pixelColor)(void*,uint32_t, uint32_t);
		ili9341_bitPerPixel,			//uint32_t    (*bitPerPixel)(void*);
		ili9341_drawBitmap,				//void        (*drawBitmap)(void*,uint32_t, uint32_t, DSP_Bitmap_t*);
		ili9341_drawImage,				//void		  (*drawImage)(void*,uint32_t, uint32_t, DSP_Image_t*);
		ili9341_decodeColor,			//RGB_Color_t (*decodeColor)(void*,uint32_t);
        0,//void		  (*rotate)(void*,uint8_t);
};

