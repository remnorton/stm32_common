/*
 * ili9341.h
 *
 *  Created on: 5 џэт. 2019 у.
 *      Author: Rem Norton
 */

#ifndef LCD_LIB_ILI9341_H_
#define LCD_LIB_ILI9341_H_
#include "dsp_driver.h"


typedef struct
{
#ifdef HAL_SPI_MODULE_ENABLED
	SPI_HandleTypeDef*	spi;
#else
	void*		spi;
#endif
	DSP_GPIO_PinDef_t cs;
	DSP_GPIO_PinDef_t dc;
	DSP_GPIO_PinDef_t reset;
}ILI9341_INIT_t;


//#define _DRAW_OVER_GRADIENT


#endif /* LCD_LIB_ILI9341_H_ */
