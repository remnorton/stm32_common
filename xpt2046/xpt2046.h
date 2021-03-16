/*
 * xpt2046.h
 *
 *  Created on: 26 мая 2019 г.
 *      Author: Rem Norton
 */

#ifndef _XPT2046_H_
#define _XPT2046_H_

#include "hal_includes.h"

typedef struct
{
	GPIO_TypeDef*	port;
	uint16_t		pin;
	GPIO_PinState	activeState;
}TP_GPIO_PinDef_t;

typedef struct
{
    uint16_t x;
    uint16_t y;
}TP_POINT_t;


typedef struct
{
#ifdef HAL_SPI_MODULE_ENABLED
	SPI_HandleTypeDef*	spi;
#else
	void*		spi;
#endif
	TP_GPIO_PinDef_t cs;
	TP_GPIO_PinDef_t irq;
}XP2048_Instance_t;


void tp_init(XP2048_Instance_t* init);
uint8_t tp_getPoint(TP_POINT_t* point);

#endif


