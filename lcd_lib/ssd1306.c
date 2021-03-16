/*
 * ssd1306.cpp
 *
 *  Created on: 07.09.2018
 *		Author: Rudenko Roman (aka Rem Norton)
 *
 */
#include <string.h>
#include "ssd1306.h"

#define _SSD1306_ID_                (uint16_t)0x1306
#define _SSD1306_SCREEN_WIDTH_      (uint16_t)128
#define _SSD1306_SCREEN_HEIGHT_     (uint16_t)64
#define _SSD1306_BPP_               (uint32_t)0x01

static uint8_t* screen_buffer = 0;
static uint16_t buf_size = 0;
static uint8_t ref_count = 0;

static uint8_t init_sequence[] = {
                            0xAE,       //display off,
                            0x20,       //Set Memory Addressing Mode
                            0x00,       //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
                            0x00,       //set low column address
                            0x10,       //set high column address
                            0x40,       //set start line address
                            0xC8,       //Set COM Output Scan Direction
                            0xB0,		//Set page 0
                            0x81,0x7F,  //set contrast control register
                            0xA1,       //set segment re-map 0 to 127
                            0xA6,       //set normal display
                            0xA8,0x3F,  //set multiplex ratio(1 to 64)
                            0xA4,       //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
                            0xD3,0x00,  //set display offset
                            0xD5,0x80,  //set display clock divide ratio/oscillator frequency (set divide ratio)
                            0xD9,0x22,  //set pre-charge period
                            0xDB,0x20,  //set vcomh
                            0x8D,0x14,  //set DC-DC enable
};
//
//
//

void ssd1306_send_commands(void* i, uint8_t* commands, uint8_t len)
{
    SSD1306_INIT_t* instance = (SSD1306_INIT_t*)i;
    if (!instance) return;
    HAL_I2C_Mem_Write(instance->i2c, instance->dsp_addr, 0x00, 1, commands, len, 10);
}

void ssd1306_sendData(void* i, uint8_t* data, uint16_t len)
{
    SSD1306_INIT_t* instance = (SSD1306_INIT_t*)i;
    if (!instance) return;
    HAL_I2C_Mem_Write(instance->i2c, instance->dsp_addr, 0x40, 1, data, sizeof(len), 100);
}

uint16_t ssd1306_get_screen_width(void* i)
{
    UNUSED(i);
    return _SSD1306_SCREEN_WIDTH_;
}

uint16_t ssd1306_get_screen_height(void* i)
{
    SSD1306_INIT_t* instance = (SSD1306_INIT_t*)i;
    if (!instance) return 0;
    return instance->height;
}

void ssd1306_push_screen(void* i)
{
    SSD1306_INIT_t* instance = (SSD1306_INIT_t*)i;
    if (!instance) return;
    HAL_I2C_Mem_Write(instance->i2c, instance->dsp_addr,0x40,1,screen_buffer,buf_size,100);
}

void ssd1306_switch_on(void* i, uint8_t active)
{
    SSD1306_INIT_t* instance = (SSD1306_INIT_t*)i;
    if (!instance) return;
    uint8_t cmd = active?0xAF:0xAE;//--turn on SSD1306 panel
    ssd1306_send_commands(i,&cmd, 1);
}

uint8_t ssd1306_init(void* init)
{
    if (!init) return 0;
    SSD1306_INIT_t* instance = (SSD1306_INIT_t*)init;
    if ((instance->height != 32) && (instance->height != 64)) return 0;
    if (!screen_buffer)
    {
        buf_size = _SSD1306_SCREEN_WIDTH_*instance->height/8;
        screen_buffer = (uint8_t*)malloc(buf_size);
    }

    if (!screen_buffer) return 0;

    uint8_t cmd[3] = {0xDA,0x22,0xAF};
    if (instance->height == 64) cmd[1] = 0x12;
    ssd1306_send_commands(init, init_sequence, sizeof(init_sequence));
    ssd1306_send_commands(init, cmd, sizeof(cmd));
    ref_count++;
    return 1;
}

void ssd1306_deInit(void* i)
{
    SSD1306_INIT_t* instance = (SSD1306_INIT_t*)i;
    if (!instance) return;
    ssd1306_switch_on(i, 0);
    ref_count--;
    if (screen_buffer && !ref_count)
    {
        free(screen_buffer);
        screen_buffer = 0;
        buf_size = 0;
    }
}

uint16_t ssd1306_get_id(void* i )
{
    UNUSED(i);
    return _SSD1306_ID_;
}

void ssd1306_clear_screen(void* i, uint32_t color)
{
    if (!i) return;
    memset(screen_buffer, (color == DSP_COLOR_Black)?0x00:0xff, buf_size);
}

void ssd1306_set_inverse(void* i, uint8_t inverse)
{
    if (!i) return;
    uint8_t cmd = inverse?0xA7:0xA6;
    ssd1306_send_commands(i,&cmd,1);
}

void ssd1306_set_brightnes(void* i, uint32_t level)
{
    if (!i) return;
    if (level > 255) level = 255;
    uint8_t cmd[2] = {0x81,(uint8_t)(level & 0xff)};
    ssd1306_send_commands(i,cmd, sizeof(cmd));
}

void ssd1306_draw_pixel(void* i, uint32_t x, uint32_t y, uint32_t color)
{
    SSD1306_INIT_t* instance = (SSD1306_INIT_t*)i;
    if (!instance) return;
    if ((x >= ssd1306_get_screen_width(i)) || (y >= ssd1306_get_screen_height(i))) return;
    if (color == DSP_COLOR_White)
        screen_buffer[x + (y / 8) * ssd1306_get_screen_width(i)] |= 1 << (y % 8);
    else
        screen_buffer[x + (y / 8) * ssd1306_get_screen_width(i)] &= ~(1 << (y % 8));
}

void ssd1306_draw_bitmap(void* i, uint32_t x, uint32_t y, DSP_Bitmap_t* bmp, uint32_t color, uint32_t bkColor)
{
	UNUSED(bkColor);
    SSD1306_INIT_t* instance = (SSD1306_INIT_t*)i;
    if (!instance) return;
    if (!bmp) return;
    uint32_t width = bmp->width;
    while (width % 8) width++;

    for (uint32_t by = 0; by < bmp->height; by++)
    {
        for (uint32_t bx = 0; bx < bmp->width; bx++)
        {
            if (bmp->data[bx/8+(by*width/8)] & (0x80 >> (bx%8))) ssd1306_draw_pixel(i,x+bx, y+by, color);
        }
    }
}

uint32_t ssd1306_rgb_color(void* i, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
    UNUSED(alpha);
    UNUSED(i);
    uint32_t res = (red+green+blue);
    return (res > 0)?0xffffffff:0;
}

uint32_t ssd1306_pixel_color(void* i, uint32_t x, uint32_t y)
{
    SSD1306_INIT_t* instance = (SSD1306_INIT_t*)i;
    if (!instance) return 0;
    if ((x > ssd1306_get_screen_width(i)) || (y > ssd1306_get_screen_height(i))) return DSP_COLOR_Black;
    if (screen_buffer[x + (y / 8) * ssd1306_get_screen_width(i)] & (1 << (y % 8)) ) return DSP_COLOR_White;
    return DSP_COLOR_Black;
}

uint32_t ssd1306_bit_per_pixel(void* i)
{
    UNUSED(i);
    return _SSD1306_BPP_;
}

RGB_Color_t ssd1306_decode_color(void* i, uint32_t color)
{
    UNUSED(i);
    RGB_Color_t res = {0,0,0,255};
    if (color)
        res.green = res.blue = res.red = 255;
    return res;
}

void ssd1306_rotate(void* i, uint8_t angle_code)
{
    SSD1306_INIT_t* instance = (SSD1306_INIT_t*)i;
    if (!instance) return;
    switch(angle_code)
    {
        case DSP_Rotate_0:
        {
            uint8_t cmd[] = {0xc8,0xa1,0xd3,0x00};
            ssd1306_send_commands(i,cmd, sizeof(cmd));
            break;
        }
        case DSP_Rotate_180:
        {
            uint8_t lines_to_scroll = 0;
            if (instance->height == 32) lines_to_scroll = 32;
            else lines_to_scroll = instance->yellow_lines;
            uint8_t cmd[] = {0xc0,0xa0,0xd3,lines_to_scroll};
            ssd1306_send_commands(i,cmd, sizeof(cmd));
            break;
        }
    }
}

//
//Driver definition
//

DSP_DriverDef_t ssd1306_driver = {
        ssd1306_init,                   //void        (*init)(void*);
        ssd1306_deInit,					//void		(*deInit)();
        ssd1306_get_id,                 //uint16_t    (*getId)();
        ssd1306_get_screen_width,       //uint16_t    (*screenWidth)();
        ssd1306_get_screen_height,      //uint16_t    (*screenHeight)();
        ssd1306_clear_screen,           //void        (*clearScreen)(uint32_t);
        ssd1306_set_inverse,            //void        (*setInverse)(uint8_t);
        ssd1306_switch_on,              //void        (*switchOn)(uint8_t);
        ssd1306_set_brightnes,          //void        (*setBrightnes)(uint32_t);
        ssd1306_draw_pixel,             //void        (*drawPixel)(uint32_t, uint32_t, uint32_t);
        0,                              //void        (*drawLine)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
        0,                              //void        (*drawRectangle)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
        0,                              //void        (*fillRectangle)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
        ssd1306_send_commands,          //void        (*sendCommands)(uint8_t*, uint8_t);
        ssd1306_sendData,				//void		  (*sendData)(uint8_t*, uint16_t);
        ssd1306_push_screen,            //void        (*pushData)();
        ssd1306_rgb_color,              //uint32_t    (*rgbColor)(uint8_t, uint8_t, uint8_t, uint8_t);
        ssd1306_pixel_color,            //uint32_t    (*pixelColor)(uint32_t, uint32_t);
        ssd1306_bit_per_pixel,          //uint32_t    (*bitPerPixel)();
        ssd1306_draw_bitmap,            //void        (*drawBitmap)(uint32_t, uint32_t, DSP_Bitmap_t*);
        0,								//void		  (*drawImage)(uint32_t, uint32_t, DSP_Image_t*);
        ssd1306_decode_color,           //RGB_Color_t (*decodeColor)(uint32_t);
        ssd1306_rotate,					//void		  (*rotate)(uint8_t);
};
