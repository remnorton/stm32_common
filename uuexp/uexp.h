/*
 * uexp.h
 *
 *  Created on: 2 сент. 2019 г.
 *      Author: Rem Norton
 */

#ifndef UUEXP_UEXP_H_
#define UUEXP_UEXP_H_

#include "hal_includes.h"
#include "uexp_types.h"


#define REGISTER_PARAM(UID, PARAM)			((UEXP_StoredParam_t){UID, sizeof(PARAM), &PARAM})
#define PARAM_LIST_LEN(X)					(sizeof(X)/sizeof(UEXP_StoredParam_t))

uint8_t	uexp_init(UEXP_t* init);
void uexp_step();
UEXP_CRC_t uexp_crc(void* data, uint16_t len);

#endif /* UUEXP_UEXP_H_ */
