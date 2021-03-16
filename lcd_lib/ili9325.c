/*
 * ili9325.c
 *
 *  Created on: 10 но€б. 2018 г.
 *      Author: Rem Norton
 */

#include "lcd_gpio.h"
#include <string.h>
#include <stdlib.h>

//
// Registers definitions
//

#define LCD_REG_0             0x00
#define LCD_REG_1             0x01
#define LCD_REG_2             0x02
#define LCD_REG_3             0x03
#define LCD_REG_4             0x04
#define LCD_REG_5             0x05
#define LCD_REG_6             0x06
#define LCD_REG_7             0x07
#define LCD_REG_8             0x08
#define LCD_REG_9             0x09
#define LCD_REG_10            0x0A
#define LCD_REG_12            0x0C
#define LCD_REG_13            0x0D
#define LCD_REG_14            0x0E
#define LCD_REG_15            0x0F
#define LCD_REG_16            0x10
#define LCD_REG_17            0x11
#define LCD_REG_18            0x12
#define LCD_REG_19            0x13
#define LCD_REG_20            0x14
#define LCD_REG_21            0x15
#define LCD_REG_22            0x16
#define LCD_REG_23            0x17
#define LCD_REG_24            0x18
#define LCD_REG_25            0x19
#define LCD_REG_26            0x1A
#define LCD_REG_27            0x1B
#define LCD_REG_28            0x1C
#define LCD_REG_29            0x1D
#define LCD_REG_30            0x1E
#define LCD_REG_31            0x1F
#define LCD_REG_32            0x20
#define LCD_REG_33            0x21
#define LCD_REG_34            0x22
#define LCD_REG_36            0x24
#define LCD_REG_37            0x25
#define LCD_REG_40            0x28
#define LCD_REG_41            0x29
#define LCD_REG_43            0x2B
#define LCD_REG_45            0x2D
#define LCD_REG_48            0x30
#define LCD_REG_49            0x31
#define LCD_REG_50            0x32
#define LCD_REG_51            0x33
#define LCD_REG_52            0x34
#define LCD_REG_53            0x35
#define LCD_REG_54            0x36
#define LCD_REG_55            0x37
#define LCD_REG_56            0x38
#define LCD_REG_57            0x39
#define LCD_REG_58            0x3A
#define LCD_REG_59            0x3B
#define LCD_REG_60            0x3C
#define LCD_REG_61            0x3D
#define LCD_REG_62            0x3E
#define LCD_REG_63            0x3F
#define LCD_REG_64            0x40
#define LCD_REG_65            0x41
#define LCD_REG_66            0x42
#define LCD_REG_67            0x43
#define LCD_REG_68            0x44
#define LCD_REG_69            0x45
#define LCD_REG_70            0x46
#define LCD_REG_71            0x47
#define LCD_REG_72            0x48
#define LCD_REG_73            0x49
#define LCD_REG_74            0x4A
#define LCD_REG_75            0x4B
#define LCD_REG_76            0x4C
#define LCD_REG_77            0x4D
#define LCD_REG_78            0x4E
#define LCD_REG_79            0x4F
#define LCD_REG_80            0x50
#define LCD_REG_81            0x51
#define LCD_REG_82            0x52
#define LCD_REG_83            0x53
#define LCD_REG_96            0x60
#define LCD_REG_97            0x61
#define LCD_REG_106           0x6A
#define LCD_REG_118           0x76
#define LCD_REG_128           0x80
#define LCD_REG_129           0x81
#define LCD_REG_130           0x82
#define LCD_REG_131           0x83
#define LCD_REG_132           0x84
#define LCD_REG_133           0x85
#define LCD_REG_134           0x86
#define LCD_REG_135           0x87
#define LCD_REG_136           0x88
#define LCD_REG_137           0x89
#define LCD_REG_139           0x8B
#define LCD_REG_140           0x8C
#define LCD_REG_141           0x8D
#define LCD_REG_143           0x8F
#define LCD_REG_144           0x90
#define LCD_REG_145           0x91
#define LCD_REG_146           0x92
#define LCD_REG_147           0x93
#define LCD_REG_148           0x94
#define LCD_REG_149           0x95
#define LCD_REG_150           0x96
#define LCD_REG_151           0x97
#define LCD_REG_152           0x98
#define LCD_REG_153           0x99
#define LCD_REG_154           0x9A
#define LCD_REG_157           0x9D
#define LCD_REG_192           0xC0
#define LCD_REG_193           0xC1
#define LCD_REG_229           0xE5


//
//
//

#define _ILI9325_ID_		(uint16_t)0x9325

#ifndef ARRAY_LEN
#define ARRAY_LEN(x)	(sizeof(x)/sizeof(x[0]))
#endif

//
//Initial sequence
//
static DSP_LCDReg_t init_seq[] =
{
        /* Start Initial Sequence --------------------------------------------------*/
            {LCD_REG_0, 0x0001}, /* Start internal OSC. */
            {LCD_REG_1, 0x0100}, /* Set SS and SM bit */
            {LCD_REG_2, 0x0700}, /* Set 1 line inversion */
            {LCD_REG_3, 0x1018}, /* Set GRAM write direction and BGR=1. */
            {LCD_REG_4, 0x0000}, /* Resize register */
            {LCD_REG_8, 0x0202}, /* Set the back porch and front porch */
            {LCD_REG_9, 0x0000}, /* Set non-display area refresh cycle ISC[3:0] */
            {LCD_REG_10, 0x0000}, /* FMARK function */
            {LCD_REG_12, 0x0000}, /* RGB interface setting */
            {LCD_REG_13, 0x0000}, /* Frame marker Position */
            {LCD_REG_15, 0x0000}, /* RGB interface polarity */

            /* Power On sequence -------------------------------------------------------*/
            {LCD_REG_16, 0x0000}, /* SAP, BT[3:0], AP, DSTB, SLP, STB */
            {LCD_REG_17, 0x0000}, /* DC1[2:0], DC0[2:0], VC[2:0] */
            {LCD_REG_18, 0x0000}, /* VREG1OUT voltage */
            {LCD_REG_19, 0x0000}, /* VDV[4:0] for VCOM amplitude */

            {LCD_REG_16, 0x17B0}, /* SAP, BT[3:0], AP, DSTB, SLP, STB */
            {LCD_REG_17, 0x0137}, /* DC1[2:0], DC0[2:0], VC[2:0] */

            {LCD_REG_18, 0x0139}, /* VREG1OUT voltage */

            {LCD_REG_19, 0x1d00}, /* VDV[4:0] for VCOM amplitude */
            {LCD_REG_41, 0x0013}, /* VCM[4:0] for VCOMH */

            {LCD_REG_32, 0x0000}, /* GRAM horizontal Address */
            {LCD_REG_33, 0x0000}, /* GRAM Vertical Address */

            /* Adjust the Gamma Curve (ILI9325) ----------------------------------------*/
            {LCD_REG_48, 0x0007},
            {LCD_REG_49, 0x0302},
            {LCD_REG_50, 0x0105},
            {LCD_REG_53, 0x0206},
            {LCD_REG_54, 0x0808},
            {LCD_REG_55, 0x0206},
            {LCD_REG_56, 0x0504},
            {LCD_REG_57, 0x0007},
            {LCD_REG_60, 0x0105},
            {LCD_REG_61, 0x0808},

            /* Set GRAM area -----------------------------------------------------------*/
            {LCD_REG_80, 0x0000}, /* Horizontal GRAM Start Address */
            {LCD_REG_81, 0x00EF}, /* Horizontal GRAM End Address */
            {LCD_REG_82, 0x0000}, /* Vertical GRAM Start Address */
            {LCD_REG_83, 0x013F}, /* Vertical GRAM End Address */

            {LCD_REG_96,  0xA700}, /* Gate Scan Line(GS=1, scan direction is G320~G1) */
            {LCD_REG_97,  0x0001}, /* NDL,VLE, REV */
            {LCD_REG_106, 0x0000}, /* set scrolling line */

            /* Partial Display Control -------------------------------------------------*/
            {LCD_REG_128, 0x0000},
            {LCD_REG_129, 0x0000},
            {LCD_REG_130, 0x0000},
            {LCD_REG_131, 0x0000},
            {LCD_REG_132, 0x0000},
            {LCD_REG_133, 0x0000},

            /* Panel Control -----------------------------------------------------------*/
            {LCD_REG_144, 0x0010},
            {LCD_REG_146, 0x0000},
            {LCD_REG_147, 0x0003},
            {LCD_REG_149, 0x0110},
            {LCD_REG_151, 0x0000},
            {LCD_REG_152, 0x0000},

            /* set GRAM write direction and BGR = 1 */
            /* I/D=00 (Horizontal : increment, Vertical : decrement) */
            /* AM=1 (address is updated in vertical writing direction) */
            {LCD_REG_3, 0x1028},

            /* 262K color and display ON */
            {LCD_REG_7, 0x0173},

            /* Set the Cursor */
            {LCD_REG_32, 239},
            {LCD_REG_33, 0},
};

static DSP_LCDReg_t disp_on_seq[] =
{
		{LCD_REG_16, 0x0000}, /* SAP, BT[3:0], AP, DSTB, SLP, STB */
		{LCD_REG_17, 0x0000}, /* DC1[2:0], DC0[2:0], VC[2:0] */
		{LCD_REG_18, 0x0000}, /* VREG1OUT voltage */
		{LCD_REG_19, 0x0000}, /* VDV[4:0] for VCOM amplitude*/

		{LCD_REG_16, 0x17B0}, /* SAP, BT[3:0], AP, DSTB, SLP, STB */
		{LCD_REG_17, 0x0137}, /* DC1[2:0], DC0[2:0], VC[2:0] */

		{LCD_REG_18, 0x0139}, /* VREG1OUT voltage */

		{LCD_REG_19, 0x1d00}, /* VDV[4:0] for VCOM amplitude */
		{LCD_REG_41, 0x0013}, /* VCM[4:0] for VCOMH */

		/* Display On */
		{LCD_REG_7, 0x0173}, /* 262K color and display ON */
};

static DSP_LCDReg_t disp_off_seq[] =
{
/* Power Off sequence ------------------------------------------------------*/
{LCD_REG_16, 0x0000}, /* SAP, BT[3:0], AP, DSTB, SLP, STB */
{LCD_REG_17, 0x0000}, /* DC1[2:0], DC0[2:0], VC[2:0] */
{LCD_REG_18, 0x0000}, /* VREG1OUT voltage */
{LCD_REG_19, 0x0000}, /* VDV[4:0] for VCOM amplitude*/

{LCD_REG_41, 0x0000}, /* VCM[4:0] for VCOMH */

/* Display Off */
{LCD_REG_7, 0x0},
};

//
//Private forwards
//

void ili9325_SetDisplayWindow(LCD_GPIO_Type_t* instance, uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);
void ili9325_SetCursor(LCD_GPIO_Type_t* instance, uint16_t Xpos, uint16_t Ypos);
uint16_t ili9325_ReadPixel(LCD_GPIO_Type_t* instance, uint16_t Xpos, uint16_t Ypos);

//
//Driver functions
//

uint8_t ili9325_init(void* init)
{
	if (!init) return 0;
	LCD_GPIO_Type_t* instance = (LCD_GPIO_Type_t*)init;
	if (!instance->dataPort) return 0;
	if (!instance->height || !instance->width) return 0;
	if (!instance->pins) return 0;
	if (!instance->bl && !(instance->pins->bl.port)) return 0;

	if (instance->bl)
	{
#ifdef HAL_TIM_MODULE_ENABLED
		__HAL_TIM_SET_AUTORELOAD(instance->bl->tmr, 5200);
		__HAL_TIM_SET_COMPARE(instance->bl->tmr, instance->bl->chan, 5200);
		HAL_TIM_PWM_Start(instance->bl->tmr, instance->bl->chan);
#endif
	}
	else LCD_SET_PIN(instance->pins->bl);

	lcd_reset(instance->pins);
	HAL_Delay(50);
	DSP_LCDReg_t reg = {0,0};
	lcd_read_reg(instance,&reg); //read ID

	if (reg.reg_data != _ILI9325_ID_) return 0;

	lcd_write_multi_reg(instance, init_seq, ARRAY_LEN(init_seq));
	return 1;
}

void ili9325_deInit(void* i)
{
	UNUSED(i);
}

uint16_t ili9325_getId(void* i)
{
	return _ILI9325_ID_;
}

uint16_t ili9325_screenWidth(void* i)
{
	if (!i) return 0;
	return ((LCD_GPIO_Type_t*)i)->width;
}

uint16_t ili9325_screenHeight(void* i)
{
	if (!i) return 0;
	return ((LCD_GPIO_Type_t*)i)->height;
}

void  ili9325_clearScreen(void* i, uint32_t color)
{
	if (!i) return;
	LCD_GPIO_Type_t* instance = (LCD_GPIO_Type_t*)i;
	ili9325_SetDisplayWindow(instance, 0, 0, instance->width, instance->height);
	ili9325_SetCursor(instance, 0, 0);
	lcd_write_io_reg(instance, LCD_REG_34);
	lcd_fill_data(instance, (uint16_t)(color&0xffff), instance->width * instance->height);

}

void ili9325_switchOn(void* i, uint8_t enable)
{
	if (!i) return;
	if (enable) lcd_write_multi_reg((LCD_GPIO_Type_t*)i, disp_on_seq, ARRAY_LEN(disp_on_seq));
	else lcd_write_multi_reg((LCD_GPIO_Type_t*)i, disp_off_seq, ARRAY_LEN(disp_off_seq));
}

void ili9325_setBrightnes(void* i, uint32_t level)
{
	if (!i) return;
	LCD_GPIO_Type_t* instance = (LCD_GPIO_Type_t*)i;
#ifdef HAL_TIM_MODULE_ENABLED
	if (instance->bl)
		__HAL_TIM_SET_COMPARE(instance->bl->tmr, instance->bl->chan, (level&0xFF)*20+100);
	else
#endif
		level?LCD_SET_PIN(instance->pins->bl):LCD_CLEAR_PIN(instance->pins->bl);
}

void ili9325_drawPixel(void* i, uint32_t x, uint32_t y, uint32_t color)
{
	if (!i) return;
	LCD_GPIO_Type_t* instance = (LCD_GPIO_Type_t*)i;
	ili9325_SetCursor(instance, x, y);
	lcd_write_io_reg(instance, LCD_REG_34);
	lcd_write_data(instance, (uint8_t*)&color, sizeof(uint16_t));
}

void ili9325_drawLine(void* i, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t color)
{
	if (!i) return;
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
            	ili9325_drawPixel(i, x,y, color);
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
                    	ili9325_drawLine(i, x,prev_y,x,y, color);
                    else
                    	ili9325_drawPixel(i,x,y, color);
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
                    	ili9325_drawLine(i, prev_x,y,x,y,color);
                    else
                    	ili9325_drawPixel(i,x,y, color);
                    prev_x = x;
                    y += sy;
                }
            }
        }
    }
    else
    {
        if (dx == 0) //vertical
        {
        	lcd_write_reg((LCD_GPIO_Type_t*)i, (DSP_LCDReg_t){LCD_REG_3, 0x1020});
        	ili9325_SetCursor((LCD_GPIO_Type_t*)i, x1, (y1 < y2)?y1:y2);
        }
        else //horizontal
        {
        	ili9325_SetCursor((LCD_GPIO_Type_t*)i, (x1 < x2)?x1:x2, y1);
        }

        lcd_write_io_reg((LCD_GPIO_Type_t*)i, LCD_REG_34);
        lcd_fill_data((LCD_GPIO_Type_t*)i, (uint16_t)(color&0xffff), dx*sx+dy*sy);

        if (dx == 0)
        	lcd_write_reg((LCD_GPIO_Type_t*)i, (DSP_LCDReg_t){LCD_REG_3, 0x1028});
    }

}

uint32_t ili9325_rgbColor(void* i, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
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

uint32_t ili9325_bitPerPixel(void* i)
{
	UNUSED(i);
	return 16;
}

RGB_Color_t ili9325_decodeColor(void* i, uint32_t color)
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

void ili9325_drawBitmap(void* i, uint32_t x, uint32_t y, DSP_Bitmap_t* bmp, uint32_t color, uint32_t bkColor)
{
	if (!i) return;
	if (!bmp) return;
	LCD_GPIO_Type_t* instance = (LCD_GPIO_Type_t*)i;

	uint32_t width = bmp->width;
	while (width % 8) width++;

	uint32_t size = bmp->width*sizeof(uint16_t);
	uint8_t* line = (uint8_t*)malloc(size);
	if (!line) return;

	for (int h = 0; h < bmp->height; h++)
	{
		for (int w = 0; w < bmp->width; w++)
		{
			uint16_t cl = bkColor;
			if (bmp->data[w/8+(h*width/8)] & (0x80 >> (w%8))) cl = color & 0xFFFF;

			*((uint16_t*)(line+2*w)) = cl;
		}
		ili9325_SetCursor(instance, x, y+h);
		lcd_write_io_reg(instance, LCD_REG_34);
		lcd_write_data(instance, line, size);
	}
	free(line);
}

//
//private members
//
void ili9325_SetDisplayWindow(LCD_GPIO_Type_t* instance, uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
	if (!instance) return;

	DSP_LCDReg_t t[] =
	{
			{LCD_REG_80, instance->height - Ypos - Height},
			{LCD_REG_81, (instance->height - Ypos - 1)},
			{LCD_REG_82,  Xpos},
			{LCD_REG_83, (Xpos + Width - 1)}
	};
	lcd_write_multi_reg(instance, t, ARRAY_LEN(t));
	lcd_write_io_reg(instance, LCD_REG_34);

}

void ili9325_SetCursor(LCD_GPIO_Type_t* instance, uint16_t Xpos, uint16_t Ypos)
{
	lcd_write_reg(instance, (DSP_LCDReg_t){LCD_REG_32, (instance->height- 1 - Ypos)});
	lcd_write_reg(instance, (DSP_LCDReg_t){LCD_REG_33, Xpos});
}

uint16_t ili9325_ReadPixel(LCD_GPIO_Type_t* instance, uint16_t Xpos, uint16_t Ypos)
{
	DSP_LCDReg_t reg = {LCD_REG_34, 0};
	ili9325_SetCursor(instance, Xpos, Ypos);
	lcd_read_reg(instance,&reg);//dummy read
	lcd_read_reg(instance,&reg);
	return reg.reg_data;
}

//
//Driver definition
//

DSP_DriverDef_t ili9325_driver =
{
		ili9325_init,					//    uint8_t     (*init)(void*);
		ili9325_deInit,					//    void		  (*deInit)(void*);
		ili9325_getId,					//    uint16_t    (*getId)(void*);
		ili9325_screenWidth,			//    uint16_t    (*screenWidth)(void*);
		ili9325_screenHeight,			//    uint16_t    (*screenHeight)(void*);
		ili9325_clearScreen,			//    void        (*clearScreen)(void*,uint32_t);
		0,								//    void        (*setInverse)(void*,uint8_t);
		ili9325_switchOn,				//    void        (*switchOn)(void*,uint8_t);
		ili9325_setBrightnes,			//    void        (*setBrightnes)(void*,uint32_t);
		ili9325_drawPixel,				//    void        (*drawPixel)(void*,uint32_t, uint32_t, uint32_t);
		ili9325_drawLine,				//    void        (*drawLine)(void*,uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
		0,								//    void        (*drawRectangle)(void*,uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
		0,								//    void        (*fillRectangle)(void*,uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
		0,								//    void        (*sendCommands)(void*,uint8_t*, uint8_t);
		0,								//    void		  (*sendData)(void*,uint8_t*, uint16_t);
		0,								//    void        (*pushData)(void*);
		ili9325_rgbColor,				//    uint32_t    (*rgbColor)(void*,uint8_t, uint8_t, uint8_t, uint8_t);
		0,								//    uint32_t    (*pixelColor)(void*,uint32_t, uint32_t);
		ili9325_bitPerPixel,			//    uint32_t    (*bitPerPixel)(void*);
		ili9325_drawBitmap,				//    void        (*drawBitmap)(void*,uint32_t, uint32_t, DSP_Bitmap_t*, uint32_t/*color*/);
		0,								//    void		  (*drawImage)(void*,uint32_t, uint32_t, DSP_Image_t*);
		ili9325_decodeColor,			//    RGB_Color_t (*decodeColor)(void*,uint32_t);
		0,								//    void		  (*rotate)(void*,uint8_t);
};

//typedef struct
//{

//
//}DSP_DriverDef_t;
