/*
 * canopen.h
 *
 *  Created on: 23 ���. 2021 �.
 *      Author: Rem Norton
 */

#ifndef CANOPEN_CANOPEN_H_
#define CANOPEN_CANOPEN_H_
#include "canopen_const.h"
#include "canopen_types.h"

#ifndef CAN_PHY
#define CAN_PHY PHY_CAN_BX
#endif

#ifndef CAN_BUF_DEPTH
#define CAN_BUF_DEPTH	10
#endif


//! CANOPen control structure
typedef struct
{
	uint8_t				node_id;
	CANOpen_SDO_Cell_t*	sdo_dict;
	uint16_t			sdo_cell_cnt;
#if (CAN_PHY == PHY_CAN_BX)
	CAN_HandleTypeDef*	can;
#else
	//TODO: Realize FDCAN
	void*				can;
#endif
	uint16_t			heartbeat;
	uint8_t				rx_fifo;
	uint8_t				filter_bank_num;
	uint8_t				process_flags;
	uint8_t				mode;
	canopen_sdo_changed_func			sdo_changed;
	canopen_sdo_access_func				sdo_access;
	canopen_mode_change_request_func	mode_change;
	canopen_stage_cplt_func				reset_comm_cplt;
	canopen_stage_cplt_func				preoper_cplt;
	//For internal use only. Do not modify!!!
	uint32_t			beat_time;
	CAN_Mess_t*			tx_buf;
	CAN_Mess_t*			rx_buf;
	uint8_t				rx_buf_size;
	uint8_t				tx_buf_size;
	uint8_t				rx_idx;
	uint8_t				tx_idx;
}CANOpen_t;


uint8_t	canopen_init(CANOpen_t* instance);
void 	canopen_loop(CANOpen_t* instance);
void 	canopen_set_mode(CANOpen_t* instance, uint8_t mode);

uint8_t canopen_default_mode_change_request(uint8_t cur_mode, uint8_t new_mode);
#endif /* CANOPEN_CANOPEN_H_ */
