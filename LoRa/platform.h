/*
 * platform.h
 *
 *  Created on: 28 но€б. 2018 г.
 *      Author: User
 */

#ifndef PLATFORM_H_
#define PLATFORM_H_

#include <stdint.h>
#include "board.h"

typedef uint32_t PIN_Handle;
typedef SPI_HandleTypeDef* SPI_Handle;
//typedef void* SPI_Handle;

typedef struct
{
    //!Hardware depended pin identifier
	GPIO_TypeDef* port;
	uint16_t	  pin;
    //!Pin active state (0..1)
	GPIO_PinState     actState;
}LORA_PinDef_t;

typedef struct
{
    //! GPIO handle
    //PIN_Handle      pinHandle;
    //! Chip select pin
    LORA_PinDef_t   cs_pin;
    //! Reset pin
    LORA_PinDef_t   rst_pin;
}LORA_PinList_t;



#endif /* PLATFORM_H_ */
