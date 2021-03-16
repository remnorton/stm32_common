/*
 * lora_driver.c
 *
 *  Created on: 20 сент. 2018 г.
 *      Author: User
 */
#include "lora_driver.h"

//
//
//

LORA_Result_t lora_SPI_Read(SPI_Handle handle, uint8_t *buf, uint8_t length)
{
	HAL_StatusTypeDef res = HAL_SPI_Receive(handle, buf, length, 100);
	return (res == HAL_OK)?LRR_Ok:LRR_SPI_error;
}

LORA_Result_t lora_SPI_Write(SPI_Handle handle, uint8_t *buf, uint8_t length)
{
	HAL_StatusTypeDef res = HAL_SPI_Transmit(handle, buf, length, 100);
	return (res == HAL_OK)?LRR_Ok:LRR_SPI_error;
}

LORA_Result_t lora_SPI_ReadWrite(SPI_Handle handle, uint8_t* txBuffer, uint8_t txCount,uint8_t* rxBuffer, uint8_t rxCount)
{
	HAL_StatusTypeDef res = HAL_SPI_TransmitReceive(handle, txBuffer, txBuffer, txCount+rxCount, 100);
	return (res == HAL_OK)?LRR_Ok:LRR_SPI_error;
}

LORA_Result_t lora_SPI_Select(LORA_PinList_t* pins)
{
    if (!pins) return LRR_Invalid_handle;
    HAL_GPIO_WritePin(pins->cs_pin.port, pins->cs_pin.pin, pins->cs_pin.actState);
    return LRR_Ok;
}
LORA_Result_t lora_SPI_Deselect(LORA_PinList_t* pins)
{
    if (!pins) return LRR_Invalid_handle;
    HAL_GPIO_WritePin(pins->cs_pin.port, pins->cs_pin.pin, !pins->cs_pin.actState);
    return LRR_Ok;
}
LORA_Result_t lora_Reset(LORA_PinList_t* pins)
{
    if (!pins) return LRR_Invalid_handle;
    HAL_GPIO_WritePin(pins->rst_pin.port, pins->rst_pin.pin, pins->rst_pin.actState);
    delayMs(150);
    HAL_GPIO_WritePin(pins->rst_pin.port, pins->rst_pin.pin, !pins->rst_pin.actState);
    delayMs(50);
    return LRR_Ok;
}

uint32_t lora_calc_deadtime(uint32_t interval)
{
    return HAL_GetTick()+interval;
}

uint8_t lora_check_timeout(uint32_t deadtime)
{
    return (deadtime > HAL_GetTick())?1:0;
}

void delayMs(uint16_t delay)
{
    HAL_Delay(delay);
}
