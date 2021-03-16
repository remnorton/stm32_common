/*
 * mfrc522.h
 *
 *  Created on: 22 џэт. 2020 у.
 *      Author: User
 */

#ifndef MFRC522_MFRC522_H_
#define MFRC522_MFRC522_H_

#include "mfrc522_spi_api.h"

typedef struct
{
	SPI_t*			spi;
	MFRC522_Pin_t	reset;
	MFRC522_Pin_t	cs;
}MFRC522_t;


uint8_t mfrc522_init(MFRC522_t* i);


#endif /* MFRC522_MFRC522_H_ */
