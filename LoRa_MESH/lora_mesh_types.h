/*
 * lora_mesh_types.h
 *
 *  Created on: 26 èþë. 2019 ã.
 *      Author: User
 */

#ifndef LORA_MESH_LORA_MESH_TYPES_H_
#define LORA_MESH_LORA_MESH_TYPES_H_
#include "lora_mesh_const.h"

//! Check for correct values
#define IS_BANDWIDTH(x)			((x == 500) || (x == 250) || (x == 125))
#define IS_SPREAD_FACTOR(x)		((x > 6) && (x < 11))
#define IS_CODING_RATE(x)		(x && (x < 5))
#define IS_SYNC_WORD(x)			((sizeof(x) == 1) && (x != 0x32) && (X != 0x12))

//!Callback types
typedef void (*event_func)();

#pragma pack(1)

//!Node state
typedef struct
{
	uint8_t	rxc : 1;
	uint8_t	txc : 1;
	uint8_t te  : 1;
	uint8_t bsy : 1;
	uint8_t	nc  : 1;
	uint8_t rfu : 3;
}LM_Node_state_t;

//!Packet type
typedef struct
{
	uint8_t		type	: 6;
	uint8_t		confirm	: 1;
	uint8_t		encrypt	: 1;
}LM_Pck_type_t;

//!Header
typedef struct
{
	uint32_t		source_id;
	uint32_t		dest_id;
	uint32_t		transit_sender_id;
	uint32_t		transit_reciever_id;
	uint16_t		pck_num;
	LM_Pck_type_t	type;
	uint8_t			size;
}LM_HEADER_t;

//!Welcome confirm payload
typedef struct
{
	int8_t		rssi;
	uint16_t	snr;
}LM_PCK_WELCOME_CFM_t;

//!Identification payload
typedef struct
{
	uint32_t	node_id;
	uint16_t	pck_num;
}LM_PCK_IDENT_t;

//!Ping confirmation payload
typedef struct
{
	int8_t		rssi;
	uint16_t	snr;
	uint8_t		data[12];
}LM_PCK_PING_conf_t;

//!Channel config
typedef struct
{
	uint16_t	carrier;
	uint16_t	bandwidth;
	uint8_t		spr_factor;
	uint8_t		prea_length;
	uint8_t		coding_rate;
	uint8_t		sync_word;
}LM_PCK_ChannleSetup_t;

//!RF config
typedef struct
{
	uint32_t		carrier;
	uint32_t		bandwidth;
	uint8_t			sp_factor;
	uint8_t			tx_power;
	uint8_t			sync_word;
	uint16_t		prea_len;
}LM_RF_Config_t;

//!FIFO data
typedef struct
{
	int8_t			rssi;
	int16_t			snr;
	uint8_t			data_len;
	uint8_t			fifo[255];
}LM_CHANNEL_Data_t;


//!MESH data and configuration
typedef struct
{
	LM_Node_state_t		node_state;
    uint8_t				error_code;
    LM_RF_Config_t		cfg_serve;
    uint8_t				reserved1;
    uint16_t			pck_count;
    uint32_t			node_id;
    uint8_t				reserved2[10];
    LM_RF_Config_t		cfg_main;
    uint8_t				reserved3;
    uint8_t				main_key[16];
    uint8_t             reserved4[259];
    uint32_t			packet_node_id;
    uint8_t				data_direction;
    uint8_t             data_len;
    uint8_t				pck_type;
    uint8_t             fifo[200];
    uint8_t             reserved10[52];
}MESH_Data_t;


/*
typedef struct
{

}LM_PCK_;
*/

//!Task
typedef struct
{
	uint8_t task_type;
	uint8_t	priority;
	void*	task_data;
}LM_Task_t;

//!Transmit task data
typedef struct
{
	uint8_t			channel;
	LM_Pck_type_t	type;
	uint8_t*		data;
	uint8_t			length;
	uint32_t		source_id;
	uint32_t		dest_id;
	uint32_t		transit_id;
	uint16_t		pck_num;
}LM_TX_TaskData_t;

//!Receive task data
typedef struct
{
	uint8_t			channel;
	int8_t			rssi;
	int16_t			snr;
	uint8_t*		data;
	uint8_t			length;
}LM_RX_TaskData_t;

//!Mesh instance
typedef struct
{
	MESH_Data_t*			mesh_data;
	uint8_t*				serve_handle;
	uint8_t*				main_handle;
	uint8_t*				serve_irq;
	uint8_t*				main_irq;
	event_func				rx_complete_handler;
	event_func				tx_complete_handler;
}LM_INIT_t;

//!Node info
typedef struct
{
	uint32_t node_id;
	uint16_t pck_num;
	int8_t	 rssi;
	uint8_t	 proxy;
}LM_NodeData_t;

//!Confirmation data
typedef struct
{
	uint32_t node_id;
	uint16_t pck_num;
	uint8_t	 type;
	uint8_t	 channel;
	uint8_t* data;
	uint8_t	 size;
	uint32_t deadtime;
	uint8_t	 try_count;
}LM_Confirm_data_t;

#pragma pack()

#endif /* LORA_MESH_LORA_MESH_TYPES_H_ */
