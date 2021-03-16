/*
 * S2LP_CORE_SPI.c
 *
 *  Created on: 26 янв. 2020 г.
 *      Author: igor
 */

#include "S2LP_CORE_SPI.h"

/**
 * @file    S2LP_CORE_SPI.c
 * @author  LowPower RF BU - AMG
 * @version 2.0.0
 * @date    May, 2019
 * @brief   This file provides all the low level API to manage SPI interface for eval board.
 * @details
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * THIS SOURCE CODE IS PROTECTED BY A LICENSE.
 * FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
 * IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
 *
 * <h2><center>&copy; COPYRIGHT 2019 STMicroelectronics</center></h2>
 */


/* Includes ------------------------------------------------------------------*/
#include "hardware_config.h"
#include "string.h"



static uint8_t waiting_irq=0;
static volatile uint8_t spi_in_use=0;

static uint8_t tx_buff[128];
static uint8_t rx_buff[128];

static SPI_HandleTypeDef *SpiHandle;
static S2LPManagePin *_m_pin;

#define WAIT_FOR_SPI_TC()               {while(!__HAL_DMA_GET_FLAG(SpiHandle->hdmarx, __HAL_DMA_GET_TC_FLAG_INDEX(SpiHandle->hdmarx)));\
                                          do{HAL_DMA_IRQHandler(SpiHandle->hdmarx);\
                                          HAL_DMA_IRQHandler(SpiHandle->hdmatx);\
                                          }while(HAL_SPI_GetState(SpiHandle) != HAL_SPI_STATE_READY);}


uint8_t S2LPConfigDev(SPI_HandleTypeDef *_spi, S2LPManagePin* _pin)
{
	if (_spi)
	{
		SpiHandle = _spi;
	}
	else
	{
		return 0;
	}
	if (_pin)
	{
	_m_pin = _pin;
	}
	else
	{
		return 0;
	}

	return 1;
}

void SdkEvalSPICSLow(void) {
	HAL_GPIO_WritePin(_m_pin->CS_Pin.Ports, _m_pin->CS_Pin.Pins, GPIO_PIN_RESET);
}
void SdkEvalSPICSHigh(void){
	HAL_GPIO_WritePin(_m_pin->CS_Pin.Ports, _m_pin->CS_Pin.Pins, GPIO_PIN_SET);
}

void SdkResetLow(void){
	HAL_GPIO_WritePin(_m_pin->RESET_Pin.Ports, _m_pin->RESET_Pin.Pins, GPIO_PIN_RESET);
}

void SdkResetHigh(void){
	HAL_GPIO_WritePin(_m_pin->RESET_Pin.Ports, _m_pin->RESET_Pin.Pins, GPIO_PIN_SET);
}

void S2LP_Reset(void){
	SdkResetHigh();
	HAL_Delay(10);
	SdkResetLow();
	HAL_Delay(10);
}


uint16_t S2LPSpiReadRegisters(uint8_t address, uint8_t n_bytes, uint8_t* buffer)
{
  tx_buff[0]=READ_HEADER;
  tx_buff[1]=address;

  uint16_t status;

  //SPI_ENTER_CRITICAL();
  SdkEvalSPICSLow();

  //HAL_SPI_TransmitReceive_DMA(SpiHandle, tx_buff, rx_buff, 2+n_bytes);
  //WAIT_FOR_SPI_TC();

  HAL_SPI_TransmitReceive(SpiHandle, tx_buff, rx_buff, 2+n_bytes, (2+n_bytes)*3/100+50);

  SdkEvalSPICSHigh();
  //SPI_EXIT_CRITICAL();

  for(uint32_t i=0;i<n_bytes;i++)
  {
    buffer[i]=rx_buff[i+2];
  }

  ((uint8_t*)&status)[1]=rx_buff[0];
  ((uint8_t*)&status)[0]=rx_buff[1];

  return status;
}

uint16_t S2LPSpiWriteRegisters(uint8_t address, uint8_t n_bytes, uint8_t* buffer)
{
  tx_buff[0]=WRITE_HEADER;
  tx_buff[1]=address;

  uint16_t status;

  for(uint32_t i=0;i<n_bytes;i++)
  {
    tx_buff[i+2]=buffer[i];
  }

  //SPI_ENTER_CRITICAL();

  /* Puts the SPI chip select low to start the transaction */
  SdkEvalSPICSLow();

//  HAL_SPI_TransmitReceive_DMA(SpiHandle, tx_buff, rx_buff, 2+n_bytes);
//  WAIT_FOR_SPI_TC();

  HAL_SPI_TransmitReceive(SpiHandle, tx_buff, rx_buff, 2+n_bytes, (2+n_bytes)*3/100+50);

  /* Puts the SPI chip select high to end the transaction */
  SdkEvalSPICSHigh();

  //SPI_EXIT_CRITICAL();
  //-----------------------------------------------------------------------

  ((uint8_t*)&status)[1]=rx_buff[0];
  ((uint8_t*)&status)[0]=rx_buff[1];

  return status;
}

uint16_t S2LPSpiCommandStrobes(uint8_t command)
{
  tx_buff[0]=COMMAND_HEADER;
  tx_buff[1]=command;

  uint16_t status;

  //SPI_ENTER_CRITICAL();
  SdkEvalSPICSLow();

  //HAL_SPI_TransmitReceive_DMA(SpiHandle, tx_buff, rx_buff, 2);
  //WAIT_FOR_SPI_TC();

  HAL_SPI_TransmitReceive(SpiHandle, tx_buff, rx_buff, 2, 50);

  SdkEvalSPICSHigh();
  //SPI_EXIT_CRITICAL();

  ((uint8_t*)&status)[1]=rx_buff[0];
  ((uint8_t*)&status)[0]=rx_buff[1];

  return status;
}

uint16_t S2LPSpiReadFifo(uint8_t n_bytes, uint8_t* buffer)
{
  tx_buff[0]=READ_HEADER;
  tx_buff[1]=LINEAR_FIFO_ADDRESS;

  uint16_t status;

  //SPI_ENTER_CRITICAL();
  SdkEvalSPICSLow();

  //HAL_SPI_TransmitReceive_DMA(SpiHandle, tx_buff, rx_buff, 2+n_bytes);
  //WAIT_FOR_SPI_TC();

  HAL_SPI_TransmitReceive(SpiHandle, tx_buff, rx_buff, 2+n_bytes, (2+n_bytes)*3/100+50);

  SdkEvalSPICSHigh();
  //SPI_EXIT_CRITICAL();

  for(uint32_t i=0;i<n_bytes;i++)
  {
    buffer[i]=rx_buff[i+2];
  }
  ((uint8_t*)&status)[1]=rx_buff[0];
  ((uint8_t*)&status)[0]=rx_buff[1];

  return status;
}

uint16_t S2LPSpiWriteFifo(uint8_t n_bytes, uint8_t* buffer)
{
  tx_buff[0]=WRITE_HEADER;
  tx_buff[1]=LINEAR_FIFO_ADDRESS;

  uint16_t status;

  for(uint32_t i=0;i<n_bytes;i++)
  {
    tx_buff[i+2]=buffer[i];
  }

  //SPI_ENTER_CRITICAL();
  SdkEvalSPICSLow();

  //HAL_SPI_TransmitReceive_DMA(SpiHandle, tx_buff, rx_buff, 2+n_bytes);
  //WAIT_FOR_SPI_TC();

  HAL_SPI_TransmitReceive(SpiHandle, tx_buff, rx_buff, 2+n_bytes, (2+n_bytes)*3/100+50);

  SdkEvalSPICSHigh();
  //SPI_EXIT_CRITICAL();

  ((uint8_t*)&status)[1]=rx_buff[0];
  ((uint8_t*)&status)[0]=rx_buff[1];

  return status;
}
__weak void SdkEvalSpiRawTC(void){}

//void S2LPSpiRaw(uint8_t n_bytes, uint8_t* in_buffer, uint8_t* out_buffer, uint8_t can_return_bef_tx)
//{
//  uint8_t* out_buffer_=out_buffer;
//
//  if(out_buffer==NULL)
//      out_buffer_=rx_buff;
//
//  spi_in_use = 1;
//
//  if(can_return_bef_tx)
//  {
//    waiting_irq=1;
//    HAL_NVIC_SetPriority(S2LP_SPI_DMA_TX_IRQn, 1, 1);
//    HAL_NVIC_EnableIRQ(S2LP_SPI_DMA_TX_IRQn);
//    HAL_NVIC_SetPriority(S2LP_SPI_DMA_RX_IRQn, 1, 0);
//    HAL_NVIC_EnableIRQ(S2LP_SPI_DMA_RX_IRQn);
//  }
//  else
//  {
//    waiting_irq=0;
//    HAL_NVIC_DisableIRQ(S2LP_SPI_DMA_RX_IRQn);
//    HAL_NVIC_DisableIRQ(S2LP_SPI_DMA_TX_IRQn);
//    SPI_ENTER_CRITICAL();
//  }
//
//  SdkEvalSPICSLow();
//
//  HAL_SPI_TransmitReceive_DMA(SpiHandle, in_buffer, out_buffer_, n_bytes);
//
//  if(!can_return_bef_tx)
//  {
//    WAIT_FOR_SPI_TC();
//
//    SdkEvalSPICSHigh();
//    SPI_EXIT_CRITICAL();
//  }
//
//  spi_in_use = 0;
//}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
  if(waiting_irq)
  {
    SdkEvalSPICSHigh();
    waiting_irq=0;
    SdkEvalSpiRawTC();
  }
}
/*
  void NUCLEO_SPI_DMA_TX_IRQHandler(void)
  {
    HAL_DMA_IRQHandler(SpiHandle.hdmatx);
  }

  void NUCLEO_SPI_DMA_RX_IRQHandler(void)
  {
    HAL_DMA_IRQHandler(SpiHandle.hdmarx);
  }
*/
void S2LPSetSpiInUse(uint8_t state)
{
  spi_in_use = state;
}

uint8_t S2LPGetSpiInUse()
{
  return spi_in_use;
}

/**
 * @}
 */


/**
 * @}
 */


/**
 * @}
 */

/******************* (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/

