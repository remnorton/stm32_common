/*
 * lora_driver.h
 *
 *  Created on: 20 сент. 2018 г.
 *      Author: User
 */

#ifndef APPLICATION_LORA_DRIVERS_LORA_DRIVER_H_
#define APPLICATION_LORA_DRIVERS_LORA_DRIVER_H_
#include <stdint.h>
#include <stdlib.h>
#include "lora_driver_types.h"

//
//Public common functions
//

LORA_Result_t lora_SPI_Read(SPI_Handle handle, uint8_t *buf, uint8_t length);
LORA_Result_t lora_SPI_Write(SPI_Handle handle, uint8_t *buf, uint8_t length);
LORA_Result_t lora_SPI_ReadWrite(SPI_Handle handle, uint8_t* txBuffer, uint8_t txCount, uint8_t* rxBuffer, uint8_t rxCount);
LORA_Result_t lora_SPI_Select(LORA_PinList_t* pins);
LORA_Result_t lora_SPI_Deselect(LORA_PinList_t* pins);
LORA_Result_t lora_Reset(LORA_PinList_t* pins);
uint32_t lora_calc_deadtime(uint32_t interval);
uint8_t lora_check_timeout(uint32_t deadtime);
void delayMs(uint16_t delay);

#endif /* APPLICATION_LORA_DRIVERS_LORA_DRIVER_H_ */
