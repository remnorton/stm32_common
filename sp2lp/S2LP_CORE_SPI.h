/*
 * S2LP_CORE_SPI.h
 *
 *  Created on: 26 янв. 2020 г.
 *      Author: igor
 */

#ifndef APP_S2LP_CORE_SPI_H_
#define APP_S2LP_CORE_SPI_H_

#include "hardware_config.h"
#include "S2LP_Commands.h"

#define __weak __attribute__((weak))

#define HEADER_WRITE_MASK		0x00 /*!< Write mask for header byte*/
#define HEADER_READ_MASK		0x01 /*!< Read mask for header byte*/
#define HEADER_ADDRESS_MASK		0x00 /*!< Address mask for header byte*/
#define HEADER_COMMAND_MASK		0x80 /*!< Command mask for header byte*/

#define LINEAR_FIFO_ADDRESS		0xFF  /*!< Linear FIFO address*/
#define RSSI_LEVEL_RUN_ADDRESS	0xEF	/*!< RSSI RUN Register address*/

//------------------------------------------
#define BUILT_HEADER(add_comm, w_r) (add_comm | w_r)  /*!< macro to build the header byte*/
#define WRITE_HEADER    BUILT_HEADER(HEADER_ADDRESS_MASK, HEADER_WRITE_MASK) /*!< macro to build the write header byte*/
#define READ_HEADER     BUILT_HEADER(HEADER_ADDRESS_MASK, HEADER_READ_MASK)  /*!< macro to build the read header byte*/
#define COMMAND_HEADER  BUILT_HEADER(HEADER_COMMAND_MASK, HEADER_WRITE_MASK) /*!< macro to build the command header byte*/

#define SPI_ENTER_CRITICAL()           __disable_irq()
#define SPI_EXIT_CRITICAL()            __enable_irq()


typedef struct {
	GPIO_TypeDef* Ports;
	uint16_t Pins;
}S2LP_PinDef;

typedef struct {
	S2LP_PinDef CS_Pin;
	S2LP_PinDef RESET_Pin;
}S2LPManagePin;

typedef struct {
	GPIO_TypeDef* IRQ_Port;
	uint16_t IRQ_Pin;
}S2LPManageIRQPin;

uint8_t S2LPConfigDev(SPI_HandleTypeDef* _spi, S2LPManagePin* _pin);

void SdkEvalSPICSLow(void);
void SdkEvalSPICSHigh(void);

void SdkResetLow(void);
void SdkResetHigh(void);

void S2LP_Reset(void);

void S2LPSpiRaw(uint8_t n_bytes, uint8_t* in_buffer, uint8_t* out_buffer, uint8_t can_return_bef_tx);
uint16_t S2LPSpiReadRegisters(uint8_t RegisterAddr, uint8_t NumByteToRead, uint8_t* pBuffer);
uint16_t S2LPSpiWriteRegisters(uint8_t RegisterAddr, uint8_t NumByteToRead, uint8_t* pBuffer);
uint16_t S2LPSpiCommandStrobes(uint8_t command);
uint16_t S2LPSpiReadFifo(uint8_t n_bytes, uint8_t* buffer);
uint16_t S2LPSpiWriteFifo(uint8_t n_bytes, uint8_t* buffer);
void S2LPSetSpiInUse(uint8_t state);
uint8_t S2LPGetSpiInUse(void);
void S2LPSleepWakeUP(void);

#endif /* APP_S2LP_CORE_SPI_H_ */
