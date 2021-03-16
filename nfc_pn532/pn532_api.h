/*
 * pn532_api.h
 *
 *  Created on: 15 θών. 2020 γ.
 *      Author: Rem Norton
 */

#ifndef NFC_PN532_PN532_API_H_
#define NFC_PN532_PN532_API_H_
#include "board.h"

#define _SPI_STATREAD                   0x02
#define _SPI_DATAWRITE                  0x01
#define _SPI_DATAREAD                   0x03
#define _SPI_READY                      0x01

#define _SPI_TIMEOUT                    10


typedef SPI_HandleTypeDef	SPI_t;

typedef struct
{
	GPIO_TypeDef* 	port;
	uint16_t 		pin;
	GPIO_PinState 	act_state;
}GPIO_PinDef_t;



#define PN532_GPIO_WRITE(pn ,st)				HAL_GPIO_WritePin(pn.port, pn.pin, st)
#define PN532_Delay(d)							HAL_Delay(d)
#define PN532_SPI_RW(p, d, c)					((HAL_SPI_TransmitReceive(p->spi, d, d, c, _SPI_TIMEOUT) == HAL_OK)?1:0)
#define PN532_CalcDeadLine(t)					(HAL_GetTick() + t)
#define PN532_IsDeadLine(t)						((t < HAL_GetTick())?1:0)

#endif /* NFC_PN532_PN532_API_H_ */
