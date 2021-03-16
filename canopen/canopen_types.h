/*
 * canopen_types.h
 *
 *  Created on: 23 ���. 2021 �.
 *      Author: Rem Norton
 */

#ifndef CANOPEN_CANOPEN_TYPES_H_
#define CANOPEN_CANOPEN_TYPES_H_
#include "board.h"

#pragma pack(1)

//! CAN DLC & Flags
typedef struct
{
	uint8_t		dlc : 4;
	uint8_t		ide : 1;
	uint8_t		rtr : 1;
}CAN_Flags_t;


//! Raw CAN message
typedef struct
{
	uint32_t	id;
	CAN_Flags_t	flags;
	uint8_t		data[8];
}CAN_Mess_t;

//! Error report
typedef struct
{
	uint8_t		command;
	uint16_t	index;
	uint8_t		subindex;
	uint16_t	err_info;
	uint8_t		err_code;
	uint8_t		err_class;
}CANOpen_Error_Pck_t;

//! NMT CANOPEN packet
typedef struct
{
    uint8_t 	command;
    uint8_t 	nodeId;
}CANOpen_NMT_Pck_t;

typedef struct
{
	uint8_t		lseg 	: 1; //last segment
	uint8_t		et  	: 1; //expedited transfer
	uint8_t		ds_code : 2; //data size code (4 - <data size>)
	uint8_t		segment : 1; //Segment toggle bit. ONLY for segmented mode, in expedited mode ALWAYS 0
	uint8_t		cmd_code: 3; //command (2 - write, 4 - read)
}CANOpen_Cmd_t;

//! SDO CANOPEN packet
typedef struct
{
	CANOpen_Cmd_t 	cmd;
    uint16_t 		index;
    uint8_t 		subIndex;
    uint32_t 		data;
}CANOpen_SDO_Pck_t;

//! SDO dictionary cell
typedef struct
{
	uint16_t 	index;
	uint8_t 	subIndex;
	void*		data;
	uint8_t		size;
	uint8_t		writable;
}CANOpen_SDO_Cell_t;

#pragma pack()


//! Callback functions
typedef void (*canopen_sdo_changed_func)(uint16_t /*index*/, uint8_t /*subIndex*/);
typedef uint8_t (*canopen_sdo_access_func)(uint16_t /*index*/, uint8_t /*subIndex*/, uint8_t /*cmd*/, uint8_t /*data_len*/, void* /*data*/);
typedef uint8_t (*canopen_mode_change_request_func)(uint8_t /*cur_mode*/, uint8_t /*new_mode*/);
typedef uint8_t (*canopen_stage_cplt_func)();

#endif /* CANOPEN_CANOPEN_TYPES_H_ */
