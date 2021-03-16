/*
 * ssd1306.h
 *
 *  Created on: 07.09.2018
 *		Author: Rudenko Roman (aka Rem Norton)
 *
 */


#ifndef SSD1306_H
#define SSD1306_H
#include "dsp_driver.h"

typedef struct
{
#ifdef HAL_I2C_MODULE_ENABLED
    I2C_HandleTypeDef* i2c;
#else
    void*				i2c;
#endif
    uint8_t dsp_addr;
    uint8_t	height;
    uint8_t	yellow_lines;
}SSD1306_INIT_t;

#endif // SSD1306_H
