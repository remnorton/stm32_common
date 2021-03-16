/*
 * uuexp_types.h
 *
 *  Created on: 2 сент. 2019 г.
 *      Author: Rem Norton
 */

#ifndef UUEXP_UEXP_TYPES_H_
#define UUEXP_UEXP_TYPES_H_

#include <stdint.h>
#include "hal_includes.h"
#include "uexp_const.h"


typedef void (*uexp_value_changed)(uint16_t /*parameter uid*/);
typedef uint16_t (*uexp_command)(uint16_t/*uid*/, uint8_t /*arg*/, uint8_t* /*data*/);

#pragma pack(1)

//!Packet header
typedef struct
{
	uint8_t		prefix;
	uint16_t	size;
}UEXP_PckHeader_t;

//!Parameter header
typedef struct
{
	uint16_t	uid;
	uint8_t		size;
}UEXP_ParamHeader_t;

//!Command header
typedef struct
{
	uint16_t	command;
	uint8_t		argcnt;
}UEXP_CmdHeader_t;

//!Section header
typedef struct
{
	uint8_t		type;
	union
	{
		UEXP_ParamHeader_t	param;
		UEXP_CmdHeader_t	cmd;
	}payload;
}UEXP_SectionHeader_t;

//!Stored parameter
typedef struct
{
	uint16_t	uid;
	uint8_t		size;
	void*		data;
}UEXP_StoredParam_t;

#pragma pack()


typedef struct
{
	UART_HandleTypeDef*	uart;
	uint8_t				stored_params_count;
	UEXP_StoredParam_t*	params;
	uexp_value_changed	value_changed_handler;
	uexp_command		command_handler;
}UEXP_t;

typedef uint16_t	UEXP_CRC_t;

#endif /* UUEXP_UEXP_TYPES_H_ */
