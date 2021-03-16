/*
 * spi_platform.h
 *
 *  Created on: 3 ���. 2019 �.
 *      Author: User
 */

#ifndef SX1301_SPI_PLATFORM_H_
#define SX1301_SPI_PLATFORM_H_
#include <stdio.h>
#include "config.h"
#include "hal_includes.h"

typedef struct
{
	GPIO_TypeDef* 	port;
	uint16_t 		pin;
	GPIO_PinState 	active_state;
}GPIO_Pin_t;


typedef struct
{
	SPI_HandleTypeDef*	spi;
	GPIO_Pin_t			cs;
	GPIO_Pin_t			rst;
}SPI_t;

void platform_spi_init(SPI_t* instance);

#endif /* SX1301_SPI_PLATFORM_H_ */
