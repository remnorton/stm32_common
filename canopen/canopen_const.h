/*
 * canopen_const.h
 *
 *  Created on: 23 ���. 2021 �.
 *      Author: Rem Norton
 */

#ifndef CANOPEN_CANOPEN_CONST_H_
#define CANOPEN_CANOPEN_CONST_H_


//
//-------------------------CAN OPEN OBJECTS-------------------------------------
#define NMT 0x00
#define EMERGENCY 0x80
#define TIME_STAMP 0x100
#define SYNC 0x80

#define PDO1_RX_SLAVE 0x200
#define PDO2_RX_SLAVE 0x300
#define PDO3_RX_SLAVE 0x400
#define PDO4_RX_SLAVE 0x500

#define PDO1_TX_SLAVE 0x180
#define PDO2_TX_SLAVE 0x280
#define PDO3_TX_SLAVE 0x380
#define PDO4_TX_SLAVE 0x480

#define SDO_TX_SLAVE 0x580
#define SDO_RX_SLAVE 0x600
#define HEARTBEAT 0x700

//-------------------------CAN OPEN NMT COMMANDS---------------------------------

typedef enum
{
    CO_NODE_START = 0x01,
    CO_NODE_STOP,
    CO_NODE_PREOPERATIONAL = 0x80,
    CO_NODE_RESET,
    CO_NODE_RESET_COMMUNICATION
}CANOpen_NMT_CMD_t;


//-------------------------CAN OPEN SDO COMMANDS---------------------------------
enum
{
	CO_CMD_WRITE = 1,
	CO_CMD_READ,
	CO_CMD_WRITE_RESP,
	CO_CMD_ERROR
};


//-------------------------CAN OPEN ERROR CLASS---------------------------------

enum
{
	CO_ERR_CLSS_SERVICE = 5,
	CO_ERR_CLSS_ACCESS
};


//-------------------------PHYSICAL CAN TYPES---------------------------------

#define PHY_CAN_BX		1
#define PHY_CAN_FD		2

//-------------------------OBJECT PROCESSING FLAGS---------------------------------

#define PROCESS_MNT			0x01
#define PROCESS_SDO			0x02
#define PROCESS_PDO			0x04

#endif /* CANOPEN_CANOPEN_CONST_H_ */
