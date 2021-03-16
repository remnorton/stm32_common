/*
 * mfrc522_spi_api.h
 *
 *  Created on: 22 џэт. 2020 у.
 *      Author: User
 */

#ifndef MFRC522_MFRC522_SPI_API_H_
#define MFRC522_MFRC522_SPI_API_H_

#include "board.h"


typedef struct
{
	GPIO_TypeDef*		port;
	uint16_t			pin;
	GPIO_PinState		active_state;
}MFRC522_Pin_t;


typedef SPI_HandleTypeDef SPI_t;


#define mfrc522_set_pin(x)		HAL_GPIO_WritePin((x)->port, (x)->pin, (x)->active_state)
#define mfrc522_reset_pin(x)	HAL_GPIO_WritePin((x)->port, (x)->pin, !((x)->active_state))
#define mfrc522_delay_ms(x)		HAL_Delay(x)

uint8_t mfrc522_spi_transmit(SPI_t* spi, void* data, uint16_t len, uint32_t timeout);
uint8_t mfrc522_spi_receive(SPI_t* spi, void* data, uint16_t len, uint32_t timeout);

#endif /* MFRC522_MFRC522_SPI_API_H_ */
