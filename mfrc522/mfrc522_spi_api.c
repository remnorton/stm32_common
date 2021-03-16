/*
 * mfrc522_spi_api.c
 *
 *  Created on: 22 џэт. 2020 у.
 *      Author: User
 */
#include "mfrc522_spi_api.h"


uint8_t mfrc522_spi_transmit(SPI_t* spi, void* data, uint16_t len, uint32_t timeout)
{
	return (HAL_SPI_Transmit(spi, (uint8_t*)data, len, timeout) == HAL_OK)?1:0;
}

uint8_t mfrc522_spi_receive(SPI_t* spi, void* data, uint16_t len, uint32_t timeout)
{
	return (HAL_SPI_Receive(spi, (uint8_t*)data, len, timeout) == HAL_OK)?1:0;
}

