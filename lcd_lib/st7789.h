/*
 * st7789.h
 *
 *  Created on: 23.09.2018
 *		Author: Rudenko Roman (aka Rem Norton)
 *
 */


#ifndef ST7789_H
#define ST7789_H
#include "board.h"
#include "dsp_driver.h"

typedef struct
{
    SPI_HandleTypeDef*  spi;
    DSP_GPIO_PinDef_t 	cs;
	DSP_GPIO_PinDef_t 	dc;
	DSP_GPIO_PinDef_t 	reset;
    uint16_t		  	width;
    uint16_t		  	height;
    uint16_t			bufSize;
}ST7789_Init_t;


extern DSP_DriverDef_t st7789_driver;

#endif // ST7789_H
