/*
 * canopen.c
 *
 *  Created on: 23 ���. 2021 �.
 *      Author: Rem Norton
 */

#include <string.h>
#include <stdlib.h>
#include "canopen.h"


//Private forwards
uint8_t canopen_send_message(CANOpen_t* instance, CAN_Mess_t* mess);
uint8_t canopen_configure_filter(CANOpen_t* instance, uint32_t id, uint8_t ide_rtr, uint32_t id_mask, uint8_t ide_rtr_mask, uint8_t mode);
void canopen_process_message(CANOpen_t* instance, CAN_Mess_t* mess);
CANOpen_SDO_Cell_t* canopen_find_sdo_cell(CANOpen_t* instance, uint16_t index, uint8_t subindex);
uint8_t canopen_process_sdo_cell(CANOpen_t* instance, CANOpen_SDO_Cell_t* cell, CANOpen_Cmd_t* cmd, void* data, uint8_t dlc, CAN_Mess_t* answer);
uint8_t canopen_read_sdo_cell(CANOpen_t* instance, CANOpen_SDO_Cell_t* cell, uint8_t dsize, uint8_t offset, void* data);
uint8_t canopen_write_sdo_cell(CANOpen_t* instance, CANOpen_SDO_Cell_t* cell, uint8_t dsize, void* data);
uint8_t canopen_node_id_check(CANOpen_t* instance, uint8_t node_id);
void canopen_report_error(uint8_t cmd, uint16_t index, uint8_t subindex, uint8_t err_class, uint8_t err_code, uint16_t err_info, void* answer);
uint8_t	canopen_try_to_send(CANOpen_t* instance, CAN_Mess_t* mess);


//default handlers forwards
void canopen_default_sdo_changed(uint16_t index, uint8_t subIndex);
uint8_t canopen_default_sdo_access(uint16_t index, uint8_t subIndex, uint8_t cmd, uint8_t data_len, void* data);
uint8_t canopen_default_stage_cplt();


//Public members
uint8_t	canopen_init(CANOpen_t* instance)
{
	if (!instance)
		return 0;
	if (!instance->can)
		return 0;
	if ((instance->node_id > 0x7f) || !instance->node_id)
		return 0;
	if (instance->sdo_cell_cnt && !instance->sdo_dict)
		return 0;

	uint8_t	max_param_size = 4;
	for (int i = 0;  i < instance->sdo_cell_cnt; i++)
		max_param_size = (max_param_size < instance->sdo_dict[i].size)?instance->sdo_dict[i].size:max_param_size;

	//allocation buffers

	instance->tx_buf_size = instance->rx_buf_size = (max_param_size/4 + (max_param_size%4)?1:0)*CAN_BUF_DEPTH;

	instance->rx_buf = (CAN_Mess_t*)malloc(instance->rx_buf_size);
	if (!instance->rx_buf)
		return 0;

	instance->tx_buf = (CAN_Mess_t*)malloc(instance->tx_buf_size);
	if (!instance->tx_buf)
	{
		free(instance->rx_buf);
		return 0;
	}

	instance->rx_idx = instance->tx_idx = 0;

	instance->beat_time = 0;
	instance->mode = CO_NODE_PREOPERATIONAL;

	//initialize default event handlers
	instance->mode_change = canopen_default_mode_change_request;
	instance->preoper_cplt = canopen_default_stage_cplt;
	instance->reset_comm_cplt = canopen_default_stage_cplt;
	instance->sdo_access = canopen_default_sdo_access;
	instance->sdo_changed = canopen_default_sdo_changed;

	//Configure filters
	uint8_t res = canopen_configure_filter(instance, NMT, 0, EMERGENCY, 0x00, CAN_FILTERMODE_IDLIST); //NMT + EMERGENCY
	res &= canopen_configure_filter(instance, PDO1_RX_SLAVE, 0, 0xff80, 0x03, CAN_FILTERMODE_IDMASK); //PDO1
	res &= canopen_configure_filter(instance, PDO2_RX_SLAVE, 0, 0xff80, 0x03, CAN_FILTERMODE_IDMASK); //PDO2
	res &= canopen_configure_filter(instance, PDO3_RX_SLAVE, 0, 0xff80, 0x03, CAN_FILTERMODE_IDMASK); //PDO3
	res &= canopen_configure_filter(instance, PDO4_RX_SLAVE, 0, 0xff80, 0x03, CAN_FILTERMODE_IDMASK); //PDO4
	res &= canopen_configure_filter(instance, SDO_RX_SLAVE, 0, 0xff80, 0x03, CAN_FILTERMODE_IDMASK); //SDO

	return res;
}

void canopen_loop(CANOpen_t* instance)
{
	if ((instance->mode == CO_NODE_RESET_COMMUNICATION) && instance->reset_comm_cplt())
		canopen_set_mode(instance, CO_NODE_PREOPERATIONAL);

	if ((instance->mode == CO_NODE_PREOPERATIONAL) && instance->preoper_cplt())
		canopen_set_mode(instance, CO_NODE_START);

	if (HAL_CAN_GetRxFifoFillLevel(instance->can, instance->rx_fifo))
	{
		CAN_Mess_t mess;
		CAN_RxHeaderTypeDef hdr;
		if (HAL_CAN_GetRxMessage(instance->can, instance->rx_fifo, &hdr, mess.data) == HAL_OK)
		{
			mess.flags.dlc = hdr.DLC;
			mess.flags.rtr = (hdr.RTR == CAN_RTR_REMOTE)?1:0;
			mess.flags.ide = (hdr.IDE == CAN_ID_EXT)?1:0;
			mess.id = (mess.flags.ide)?hdr.ExtId:hdr.StdId;

			if (hdr.DLC < 8)
				memset(&mess.data[hdr.DLC], 0, 8 - hdr.DLC);

			canopen_process_message(instance, &mess);
		}
	}

	if (instance->tx_idx)
	{
		if (canopen_try_to_send(instance, instance->tx_buf))
		{
			instance->tx_idx--;
			memcpy(instance->tx_buf, &instance->tx_buf[1], (instance->tx_buf_size - instance->tx_idx)*sizeof(CAN_Mess_t));
		}
	}
}

void canopen_set_mode(CANOpen_t* instance, uint8_t mode)
{
	switch(mode)
	{
		case CO_NODE_START:
		case CO_NODE_RESET_COMMUNICATION:
		case CO_NODE_STOP:
		case CO_NODE_RESET:
		case CO_NODE_PREOPERATIONAL:
		{
			instance->mode = mode;
			break;
		}
	}
}

//Private members

uint8_t canopen_send_message(CANOpen_t* instance, CAN_Mess_t* mess)
{
	if (instance->tx_idx < instance->tx_buf_size)
		memcpy(&instance->tx_buf[instance->tx_idx++], mess, sizeof(CAN_Mess_t));
	return 0;
}

uint8_t canopen_configure_filter(CANOpen_t* instance, uint32_t id, uint8_t ide_rtr, uint32_t id_mask, uint8_t ide_rtr_mask, uint8_t mode)
{
#if (CAN_PHY == PHY_CAN_BX)
	CAN_FilterTypeDef filter;
	filter.FilterActivation = ENABLE;
	filter.FilterBank = instance->filter_bank_num++;
	filter.FilterFIFOAssignment = instance->rx_fifo;
	filter.FilterMode = mode;
	filter.FilterScale = CAN_FILTERSCALE_32BIT;
	filter.FilterIdHigh = ((id & 0x07FF)<<5)|((id & 0x3E000)>>13);
	filter.FilterIdLow = ((id & 0x1FFF)<<3)|((ide_rtr & 0x03)<<1);
	filter.FilterMaskIdHigh = ((id_mask & 0x07FF)<<5)|((id_mask & 0x3E000)>>13);
	filter.FilterMaskIdLow = ((id_mask & 0x1FFF)<<3)|((ide_rtr_mask & 0x03)<<1);

	return (HAL_CAN_ConfigFilter(instance->can, &filter) == HAL_OK)?1:0;

#else
	//TODO: Realize FDCAN
	return 0;
#endif
}

void canopen_process_message(CANOpen_t* instance, CAN_Mess_t* mess)
{
	uint8_t res = 0;
	CAN_Mess_t ans;

	//Process SDO & PDO
	if (canopen_node_id_check(instance, mess->id & 0x7f) && mess->flags.dlc)
	{
		//extract packet type
		uint16_t ptype = mess->id & 0xff80;
		memset(&ans, 0, sizeof(ans));
		switch(ptype)
		{
			case SDO_RX_SLAVE:
			{
				if (((instance->mode == CO_NODE_START) || (instance->mode == CO_NODE_PREOPERATIONAL)) && (instance->process_flags & PROCESS_SDO))
				{
					CANOpen_SDO_Pck_t* pck = (CANOpen_SDO_Pck_t*)mess->data;

					res = instance->sdo_access(pck->index, pck->subIndex, pck->cmd.cmd_code, 4-pck->cmd.ds_code, &pck->data);
					if (!res)
					{
						CANOpen_SDO_Cell_t* cell = canopen_find_sdo_cell(instance, pck->index, pck->subIndex);
						if (cell)
							res = canopen_process_sdo_cell(instance, cell, &pck->cmd, &pck->data, mess->flags.dlc, &ans);
					}

					if(!res) //Error reporting
					{
						CANOpen_SDO_Pck_t* apck = (CANOpen_SDO_Pck_t*)ans.data;
						*((uint8_t*)&ans.flags) = 0;
						*((uint8_t*)&apck->cmd) = 0;
						apck->cmd.cmd_code = CO_CMD_ERROR;
						ans.flags.dlc = 8;
						ans.id = SDO_TX_SLAVE + instance->node_id;
						canopen_report_error(pck->cmd.cmd_code, pck->index, pck->subIndex, CO_ERR_CLSS_ACCESS, 4, 0, ans.data);
						res = 1;
					}
				}
				break;
			}
			case PDO1_RX_SLAVE:
			case PDO2_RX_SLAVE:
			case PDO3_RX_SLAVE:
			case PDO4_RX_SLAVE:
			{
				if ((instance->mode == CO_NODE_START) && (instance->process_flags & PROCESS_PDO))
				{

				}
				break;
			}
		}
	}

	//Process NMT
	if ((mess->id == NMT) && (mess->flags.dlc == 2) && (instance->process_flags & PROCESS_MNT))
	{
		CANOpen_NMT_Pck_t* pck = (CANOpen_NMT_Pck_t*)mess->data;
		if (canopen_node_id_check(instance,pck->nodeId))
		{
			if (instance->mode_change(instance->mode, pck->command))
				canopen_set_mode(instance, pck->command);
		}
	}

	//handle operation result
	if (res)
		canopen_send_message(instance, &ans);

}

CANOpen_SDO_Cell_t* canopen_find_sdo_cell(CANOpen_t* instance, uint16_t index, uint8_t subindex)
{
	CANOpen_SDO_Cell_t* cell = 0;
	for (int i = 0; (i < instance->sdo_cell_cnt) && !cell; i++)
	{
		cell = &instance->sdo_dict[i];
		if ((cell->index != index) || (cell->subIndex != subindex))
			cell = 0;
	}

	return cell;
}

uint8_t canopen_process_sdo_cell(CANOpen_t* instance, CANOpen_SDO_Cell_t* cell, CANOpen_Cmd_t* cmd, void* data, uint8_t dlc, CAN_Mess_t* answer)
{
	uint8_t dsize = (4-cmd->ds_code);
	if (!dsize)
		dsize = (dlc - 4);

	if (!dsize)
	{
		//TODO: error reporting
		return 1;
	}
	uint8_t res = 0;
	uint8_t success = 0;
	uint8_t resp_code = CO_CMD_ERROR;
	CANOpen_SDO_Pck_t* apck = (CANOpen_SDO_Pck_t*)answer->data;
	switch(cmd->cmd_code)
	{
		case CO_CMD_READ:
		{
			success = canopen_read_sdo_cell(instance, cell, dsize, 0, &apck->data);
			if (success)
				resp_code = *((uint8_t*)cmd);
			res = 1;
			break;
		}
		case CO_CMD_WRITE:
		{
			success = canopen_write_sdo_cell(instance, cell, dsize, data);
			if (success)
			{
				memcpy(&apck->data, data, dsize);
				CANOpen_Cmd_t c = {0, 0, 0, 0, CO_CMD_WRITE_RESP};
				resp_code =  *((uint8_t*)&c);
			}
			res = 1;
			break;
		}
	}

	if (res)
	{
		if (success)
		{
			*((uint8_t*)&answer->flags) = 0;
			*((uint8_t*)&apck->cmd) = resp_code;
			answer->flags.dlc = 8;
			answer->id = SDO_TX_SLAVE + instance->node_id;

			apck->index = cell->index;
			apck->subIndex = cell->subIndex;
		}
		else
		{
			*((uint8_t*)&answer->flags) = 0;
			*((uint8_t*)&apck->cmd) = 0;
			apck->cmd.cmd_code = CO_CMD_ERROR;
			answer->flags.dlc = 8;
			answer->id = SDO_TX_SLAVE + instance->node_id;
			canopen_report_error(cmd->cmd_code, cell->index, cell->subIndex, CO_ERR_CLSS_ACCESS, 1, 0, answer->data);
		}
	}

	return res;
}

uint8_t canopen_read_sdo_cell(CANOpen_t* instance, CANOpen_SDO_Cell_t* cell, uint8_t dsize, uint8_t offset, void* data)
{
	if (cell->size >= dsize)
	{
		memcpy(data, cell->data + offset, dsize);
		return 1;
	}
	return 0;
}

uint8_t canopen_write_sdo_cell(CANOpen_t* instance, CANOpen_SDO_Cell_t* cell, uint8_t dsize, void* data)
{
	if (cell->writable && (cell->size >= dsize))
	{
		memcpy(cell->data, data, dsize);
		instance->sdo_changed(cell->index, cell->subIndex);

		return 1;
	}
	return 0;
}

uint8_t canopen_node_id_check(CANOpen_t* instance, uint8_t node_id)
{
	return ((!node_id) || (instance->node_id == node_id))?1:0;
}

void canopen_report_error(uint8_t cmd, uint16_t index, uint8_t subindex, uint8_t err_class, uint8_t err_code, uint16_t err_info, void* answer)
{
	CANOpen_Error_Pck_t* pck = (CANOpen_Error_Pck_t*)answer;
	pck->command = cmd;
	pck->err_class = err_class;
	pck->err_code = err_code;
	pck->err_info = err_info;
	pck->index = index;
	pck->subindex = subindex;
}

uint8_t	canopen_try_to_send(CANOpen_t* instance, CAN_Mess_t* mess)
{
#if (CAN_PHY == PHY_CAN_BX)
	if (HAL_CAN_GetTxMailboxesFreeLevel(instance->can))
	{
		uint32_t mailbox = 0;
		CAN_TxHeaderTypeDef hdr;
		hdr.ExtId = hdr.StdId = mess->id;
		hdr.DLC = mess->flags.dlc;
		hdr.RTR = CAN_RTR_DATA;
		hdr.IDE = CAN_ID_STD;

		return (HAL_CAN_AddTxMessage(instance->can, &hdr, mess->data, &mailbox) == HAL_OK)?1:0;
	}
#else
	//TODO: Realize FDCAN
#endif

	return 0;
}

//default handlers
void canopen_default_sdo_changed(uint16_t index, uint8_t subIndex)
{

}

uint8_t canopen_default_sdo_access(uint16_t index, uint8_t subIndex, uint8_t cmd, uint8_t data_len, void* data)
{
	return 0;
}

uint8_t canopen_default_mode_change_request(uint8_t cur_mode, uint8_t new_mode)
{
	//Default CANOpen work flow
	uint8_t res = 0;
	switch(cur_mode)
	{
		case CO_NODE_RESET:
		{
			res = (new_mode == CO_NODE_RESET_COMMUNICATION)?1:0;
			break;
		}
		case CO_NODE_RESET_COMMUNICATION:
		{
			res = (new_mode == CO_NODE_PREOPERATIONAL)?1:0;
			break;
		}
		case CO_NODE_PREOPERATIONAL:
		{
			res = ((new_mode == CO_NODE_STOP) || (new_mode == CO_NODE_START))?1:0;
			break;
		}
		case CO_NODE_START:
		case CO_NODE_STOP:
		{
			res = 1;
			break;
		}
	}

	return res;
}

uint8_t canopen_default_stage_cplt()
{
	return 1;
}
