/*
 * lora_mesh.c
 *
 *  Created on: 26 èþë. 2019 ã.
 *      Author: User
 */

#include "app.h"

#include "lora_channel.h"
#include "lora_mesh.h"
#include "lora_mesh_containers.h"
#include "aes.h"

//!Ping interval (seconds)
#define PING_INTERVAL	10

#define FRIENDS_COUNT	2
#define CONFIRM_TIMEOUT	30
#define WAIT_TRY_COUNT	3

#define MAX_WAIT_QTY	5
#define MAX_TASK_QTY	10

static List_t* tx_task_list = 0;
static List_t* rx_task_list = 0;
static List_t* wait_confirm = 0;

static List_t* nodes_list = 0;


static LM_INIT_t* init = 0;
static uint32_t deadtime = 0;
static List_item_t* current_tx_task = 0;
static List_item_t* current_rx_task = 0;
static uint8_t cur_task_stage = 0;

static uint8_t pck_temp_buf[255] = {0};
static uint16_t main_delay = 0;
static uint16_t serve_delay = 0;
static uint32_t hello_time = 1;
static uint32_t  vote_time = 0;
static LM_NodeData_t friends[FRIENDS_COUNT];

static uint8_t tmp_u8 = 0;
static uint16_t tmp_u16 = 0;
static uint8_t enters_count = 0;
static uint8_t encryption = 0;

//uint32_t ping_time = 0;

//
//Private forwards
//


void lm_kill_task(LM_Task_t* task);
void lm_handle_task_list();
void lm_handle_cur_stage();
void lm_handle_receiver();
void lm_enter();
void lm_handle_tx_task();
void lm_handle_rx_task();
void lm_handle_retranslation(LM_RX_TaskData_t* task_data);
void lm_handle_incoming_data(LM_RX_TaskData_t* task_data);
void lm_parse_incomming_data(LM_RX_TaskData_t* task_data);
LM_NodeData_t* lm_find_neighbor(uint32_t node_id);
LM_NodeData_t* lm_register_node(uint32_t node_id, int8_t rssi);
LM_NodeData_t* lm_find_node(uint32_t node_id);
LM_Confirm_data_t* lm_register_wait(uint32_t node_id, uint16_t pck_num, uint8_t type, uint8_t channel, uint8_t* data, uint8_t size, uint16_t timeout);
int16_t lm_get_wait_index(uint32_t node_id, uint16_t pck_num, uint8_t type);
void lm_check_wait_timeout();
void lm_remove_wait_task(int16_t index);
uint8_t lm_exam_friend(uint8_t idx, uint32_t node_id, int8_t rssi);
uint16_t lm_calc_crc(void* data, uint16_t length);
uint8_t	lm_make_pck(LM_Pck_type_t type, uint32_t src_id, uint32_t dest_id, uint32_t transit_id, uint16_t num, uint8_t* data, uint8_t length, uint8_t* out);
uint8_t lm_send_to_neighborhood(uint8_t priority, uint8_t channel, LM_Pck_type_t type, uint32_t src_id, uint32_t dest_id, uint16_t num, uint8_t* data, uint8_t length, uint16_t timeout);
uint8_t lm_ping_neighborhood(uint8_t channel, uint8_t* data, uint8_t length, uint16_t timeout);
uint8_t lm_add_tx_task(uint8_t priority, uint8_t channel, LM_Pck_type_t type, uint32_t src_id, uint32_t dest_id, uint32_t transit_id, uint16_t num, uint8_t* data, uint8_t length, uint16_t timeout);
uint8_t	lm_add_rx_task(uint8_t channel, int8_t rssi, int16_t snr, uint8_t* data, uint8_t length);
List_item_t* lm_append_task(List_t* task_list, LM_Task_t* task);
uint8_t lm_check_income(uint8_t* irq, uint8_t handle);
uint8_t lm_neighbor_count();
void lm_clear_nodes();
uint16_t lm_get_empty_packet_duration(uint8_t channel);
void lm_set_vote_time();
void lm_voting();
void lm_process_packet(uint8_t channel, uint8_t length, int8_t rssi, int16_t snr);
void lm_ping(uint8_t channel, uint32_t dest_node_id, uint8_t* data, uint8_t len);

//
//Public members
//

void lora_mesh_init(LM_INIT_t* i)
{
	if (!i) return;
	if (init) return;
	init = i;
	if (!tx_task_list) tx_task_list = list_create();
	if (!rx_task_list) rx_task_list = list_create();
	if (!nodes_list) nodes_list = list_create();
	if (!wait_confirm) wait_confirm = list_create();

	main_delay = lora_channel_packetDuration(*init->main_handle, 2) + (init->mesh_data->node_id%(lm_get_empty_packet_duration(LMCH_Main)*2));
	serve_delay = lora_channel_packetDuration(*init->serve_handle, 2) + (init->mesh_data->node_id%(lm_get_empty_packet_duration(LMCH_Serve)*2));

	lora_channel_recieve(*init->main_handle, 0, 1);
	lora_channel_recieve(*init->serve_handle, 0, 1);

	init->mesh_data->node_state.nc = 1;

}

void lora_mesh_step()
{
	if (!init) return;

	enters_count++;

	switch(enters_count)
	{
		case 1:
		{
			if (hello_time && (hello_time < HAL_GetTick()))
			{
				if (lm_neighbor_count()) hello_time = 0;
				else
				{
					init->mesh_data->node_state.nc = 1;
					hello_time = HAL_GetTick() + CONFIRM_TIMEOUT*serve_delay;
				}
			}
			break;
		}
		case 2:
		{
			if (vote_time && (vote_time <= HAL_GetTick()))
			{
				lm_voting();
				vote_time = 0;
			}
			init->mesh_data->node_state.nc = (init->mesh_data->node_state.nc | (init->mesh_data->pck_count == 0))?1:0;

			if (init->mesh_data->node_state.nc && list_is_empty(tx_task_list) && !current_tx_task)
				lm_enter();

			break;
		}
		case 3:
		{
			if (!current_tx_task) //idle
				lm_handle_task_list();
			else
			{
				if (cur_task_stage)
					lm_handle_cur_stage();
			}

			break;
		}
		case 4:
		{
			current_rx_task = list_first(rx_task_list);
			if (current_rx_task)
			{
				list_remove_item(rx_task_list, current_rx_task);
				lm_handle_rx_task();
			}
			break;
		}
		case 5:
		{
			lm_handle_receiver();
			lm_check_wait_timeout();

			break;
		}

		case 6: //check for tx
		{
			uint8_t res = 0;
			if ((init->mesh_data->data_direction == LMPD_TX) && init->mesh_data->data_len && (init->mesh_data->data_len <= sizeof(init->mesh_data->fifo)) && !init->mesh_data->node_state.bsy)
				res = lora_mesh_transmit_data(init->mesh_data->packet_node_id, init->mesh_data->pck_type, init->mesh_data->fifo, init->mesh_data->data_len);

			if (res || init->mesh_data->node_state.bsy)
			{
				init->mesh_data->data_direction = LMPD_NONE;
				init->mesh_data->data_len = 0;
			}
			break;
		}
		default:
			enters_count = 0;
	}


	//check for busy state
	init->mesh_data->node_state.bsy = current_tx_task?1:0;

}

uint8_t lora_mesh_transmit_data(uint32_t dest_node_id, uint8_t type, uint8_t* data, uint8_t length)
{
	//TODO: Add retranslation routes
	return lm_add_tx_task(0, LMCH_Main, (LM_Pck_type_t){type, 0, encryption}, init->mesh_data->node_id, dest_node_id, 0, init->mesh_data->pck_count++, data, length, 0);
}

void lora_mesh_check_encrypt()
{
	encryption = 0;
	for (int i = 0; (i < 16) && !encryption; i++)
		if (init->mesh_data->main_key[i]) encryption = 1;
}

//
//Private members
//

void lm_kill_task(LM_Task_t* task)
{
	if (!task) return;
	if (task->task_data) free(task->task_data);
}

void lm_handle_task_list()
{
	if (list_is_empty(tx_task_list)) return;
	current_tx_task = list_first(tx_task_list);

	if (current_tx_task)
	{
		list_remove_item(tx_task_list, current_tx_task);
		cur_task_stage = LMTS_START;
	}
}

void lm_handle_cur_stage()
{
	if (!current_tx_task) return;

	LM_Task_t* tsk = (LM_Task_t*)current_tx_task->data;
	if (!tsk)
	{
		list_release_item(current_tx_task);
		current_tx_task = 0;
		deadtime = 0;
		cur_task_stage = LMTS_NONE;
	}

	switch(tsk->task_type)
	{
		case LMTT_TX:
		{
			lm_handle_tx_task();
			break;
		}
		/*case LMTT_RX:
		{
			lm_handle_rx_task();
			break;
		}*/
		default: //invalid task
		{
			list_release_item(current_tx_task);
			current_tx_task = 0;
			deadtime = 0;
			cur_task_stage = LMTS_NONE;
		}
	}
}

void lm_handle_receiver()
{

	//Check data channel

	if (init->main_irq)
	{
		tmp_u8 = lm_check_income(init->main_irq, *init->main_handle);
		if (tmp_u8)
			lm_process_packet(LMCH_Main, tmp_u8, lora_channel_rssi(*init->main_handle), lora_channel_snr(*init->main_handle));

	}

	//Check service channel
	if (init->serve_irq)
	{
		tmp_u8 = lm_check_income(init->serve_irq, *init->serve_handle);
		if (tmp_u8)
			lm_process_packet(LMCH_Serve, tmp_u8, lora_channel_rssi(*init->serve_handle), lora_channel_snr(*init->serve_handle));
	}
}

void lm_enter()
{
	LM_Pck_type_t type = {LMPT_WELCOME,0,0};
	lm_clear_nodes();
	lm_add_tx_task(0, LMCH_Serve, type, init->mesh_data->node_id, 0, 0, init->mesh_data->pck_count++, (uint8_t*)&init->mesh_data->node_id, sizeof(init->mesh_data->node_id), 0);
	init->mesh_data->node_state.nc = 0;
	hello_time = HAL_GetTick()+CONFIRM_TIMEOUT*serve_delay;
}

uint8_t lm_send_to_neighborhood(uint8_t priority, uint8_t channel, LM_Pck_type_t type, uint32_t src_id, uint32_t dest_id, uint16_t num, uint8_t* data, uint8_t length, uint16_t timeout)
{
	tmp_u8 = 0;
	List_item_t* item = nodes_list->item;
	if (!item) return 0;
	if (item->data)
	{
		LM_NodeData_t* ndata = (LM_NodeData_t*)item->data;
		if (ndata->proxy)
		{
			lm_add_tx_task(priority, channel, type, src_id, dest_id, ndata->node_id, num, data, length, timeout);
			tmp_u8++;
		}

		while (item->next)
		{
			item = item->next;
			ndata = (LM_NodeData_t*)item->data;
			if (ndata->proxy)
			{
				lm_add_tx_task(priority, channel, type, src_id, dest_id, ndata->node_id, num, data, length, timeout);
				tmp_u8++;
			}
		}

	}
	return tmp_u8;
}

uint8_t lm_ping_neighborhood(uint8_t channel, uint8_t* data, uint8_t length, uint16_t timeout)
{
	tmp_u8 = 0;
	List_item_t* item = nodes_list->item;
	if (!item) return 0;
	if (item->data)
	{
		LM_NodeData_t* ndata = (LM_NodeData_t*)item->data;
		if (ndata->proxy)
		{
			lm_ping(channel, ndata->node_id, data, length);
			tmp_u8++;
		}

		while (item->next)
		{
			item = item->next;
			ndata = (LM_NodeData_t*)item->data;
			if (ndata->proxy)
			{
				lm_ping(channel, ndata->node_id, data, length);
				tmp_u8++;
			}
		}
	}
	return tmp_u8;
}

uint8_t lm_add_tx_task(uint8_t priority, uint8_t channel, LM_Pck_type_t type, uint32_t src_id, uint32_t dest_id, uint32_t transit_id, uint16_t num, uint8_t* data, uint8_t length, uint16_t timeout)
{
	if (list_item_count(tx_task_list) >= MAX_TASK_QTY) return 0;
	if (transit_id == init->mesh_data->node_id) return 0;
	if (transit_id == src_id) return 0;
	if (dest_id == src_id) return 0;
	if (list_item_count(wait_confirm) > MAX_WAIT_QTY) return 0;

	LM_Task_t* task = (LM_Task_t*)malloc(sizeof(LM_Task_t));
	if (!task) return 0;

	task->task_data = malloc(sizeof(LM_TX_TaskData_t));
	task->priority = priority;
	if (!task->task_data)
	{
		free(task);
		return 0;
	}


	task->task_type = LMTT_TX;
	LM_TX_TaskData_t* td = (LM_TX_TaskData_t*)task->task_data;
	*td = (LM_TX_TaskData_t){channel, type, 0, length, src_id, dest_id, transit_id, num};
	td->data = (uint8_t*)malloc(length);

	if (!td->data)
	{
		free(td);
		free(task);
		return 0;
	}
	memcpy(td->data, data, length);

	uint16_t enc = 0;
	for (int i = 0; (i < sizeof(init->mesh_data->main_key)) && !enc; i++)
		enc += init->mesh_data->main_key[i];


	if (lm_append_task(tx_task_list, task))
	{
		if (timeout && (channel == LMCH_Main)) lm_register_wait(src_id, num, type.type, channel, data, length, timeout);
		return 1;
	}

	//find

	//Enqueue error
	if (task->task_data) free(task->task_data);
	free(task);
	return 0;
}

uint8_t	lm_add_rx_task(uint8_t channel, int8_t rssi, int16_t snr, uint8_t* data, uint8_t length)
{
	if (list_item_count(tx_task_list) >= MAX_TASK_QTY) return 0;
	if (!data || !length) return 0;

	LM_Task_t* task = (LM_Task_t*)malloc(sizeof(LM_Task_t));
	if (!task) return 0;

	task->task_data = malloc(sizeof(LM_RX_TaskData_t));
	task->priority = 255;
	if (!task->task_data)
	{
		free(task);
		return 0;
	}

	task->task_type = LMTT_RX;
	LM_RX_TaskData_t* td = (LM_RX_TaskData_t*)task->task_data;
	*td = (LM_RX_TaskData_t){channel, rssi, snr, 0, length};

	td->data = (uint8_t*)malloc(length);
	if (!td->data)
	{
		free(task->task_data);
		free(task);
		return 0;
	}
	memcpy(td->data, data, length);

	if (list_append(rx_task_list, task)) return 1;

	//Enqueue error
	if (task->task_data) free(task->task_data);
	free(task);
	return 0;
}

List_item_t* lm_append_task(List_t* task_list, LM_Task_t* task)
{
	if (list_is_empty(task_list)) return list_append(task_list, task);
	List_item_t* item = list_last(task_list);
	if (((LM_Task_t*)item->data)->priority <= task->priority) return list_append(task_list, task);

	item = task_list->item;
	List_item_t* res = 0;
	if (list_item_count(task_list) < 2)
	{
		LM_Task_t* t = (LM_Task_t*)item->data;
		if (t->priority < task->priority) return list_insert_after(task_list, item, task);
		else return list_insert_first(task_list, task);
	}
	else
	{
		while (item)
		{
			LM_Task_t* t = (LM_Task_t*)item->data;
			if (t->priority > task->priority)
			{
				res = list_insert_before(task_list, item, task);
				item = 0;
			}
			else item = item->next;
		}

		if (!res) res = list_append(task_list, task);
	}
	return res;
}

uint8_t lm_check_income(uint8_t* irq, uint8_t handle)
{
	tmp_u8 = 0;
	tmp_u16 = 0;
	if (*irq)
	{
		tmp_u16 = lora_channel_rxAvail(handle, &tmp_u8);
		tmp_u16 = tmp_u16 * tmp_u8;
		if (tmp_u8)
			lora_channel_read(handle, pck_temp_buf, tmp_u16);
		lora_channel_recieve(handle, 0, 1);
		*irq = 0;
	}
	return tmp_u16;
}

uint8_t lm_neighbor_count()
{
	tmp_u8 = 0;
	List_item_t* item = nodes_list->item;
	if (!item) return 0;
	if (item->data)
	{
		LM_NodeData_t* ndata = (LM_NodeData_t*)item->data;
		if (ndata->proxy)tmp_u8++;
		while (item->next)
		{
			item = item->next;
			ndata = (LM_NodeData_t*)item->data;
			if (ndata->proxy) tmp_u8++;
		}
	}
	return tmp_u8;
}

void lm_clear_nodes()
{
	List_item_t* item = list_first(nodes_list);
	while (item)
	{
		LM_NodeData_t* node = (LM_NodeData_t*)item->data;
		if (node)
		{
			node->proxy = 0;
			node->rssi = -127;
			node->pck_num = 0;
		}
		item = item->next;
	}
	hello_time = HAL_GetTick()+CONFIRM_TIMEOUT*serve_delay;
}

uint16_t lm_get_empty_packet_duration(uint8_t channel)
{
	uint8_t* handle = 0;

	if (channel == LMCH_Main) handle = init->main_handle;
	if (channel == LMCH_Serve) handle = init->serve_handle;

	if (!handle) return 0;

	return lora_channel_packetDuration(*handle, sizeof(LM_HEADER_t)+sizeof(uint16_t));
}

void lm_set_vote_time()
{
	if (!vote_time) vote_time = HAL_GetTick() + lm_get_empty_packet_duration(LMCH_Serve)*10;
	else vote_time += lm_get_empty_packet_duration(LMCH_Serve)*4;
}

void lm_voting()
{
	for (tmp_u8 = 0; tmp_u8 < FRIENDS_COUNT; tmp_u8++)
		friends[tmp_u8] = (LM_NodeData_t){0,0,-127,0};

	List_item_t* item = nodes_list->item;
	if (!item) return;
	if (item->data)
	{
		//sort by rssi
		LM_NodeData_t* node = (LM_NodeData_t*)item->data;

		for (tmp_u8 = 0; tmp_u8 < FRIENDS_COUNT; tmp_u8++)
			if (lm_exam_friend(tmp_u8, node->node_id, node->rssi)) break;

		while (item->next)
		{
			item = item->next;
			node = (LM_NodeData_t*)item->data;
			for (tmp_u8 = 0; tmp_u8 < FRIENDS_COUNT; tmp_u8++)
				if (lm_exam_friend(tmp_u8, node->node_id, node->rssi)) break;
		}

		//Mark/unmark as friend
		item = nodes_list->item;
		node = (LM_NodeData_t*)item->data;
		node->proxy = 0;
		for (tmp_u8 = 0; (tmp_u8 < FRIENDS_COUNT) && !node->proxy; tmp_u8++)
			if (node->node_id == friends[tmp_u8].node_id)node->proxy = 1;

		while (item->next)
		{
			item = item->next;
			node = (LM_NodeData_t*)item->data;
			node->proxy = 0;
			for (tmp_u8 = 0; (tmp_u8 < FRIENDS_COUNT) && !node->proxy; tmp_u8++)
				if (node->node_id == friends[tmp_u8].node_id)node->proxy = 1;
		}

	}
}

void lm_process_packet(uint8_t channel, uint8_t length, int8_t rssi, int16_t snr)
{
	//validate packet
	LM_HEADER_t* hdr = (LM_HEADER_t*)pck_temp_buf;
	if (length < (sizeof(LM_HEADER_t)+hdr->size+sizeof(uint16_t))) return; //incomplete

	tmp_u16 = 0;
	memcpy(&tmp_u16, &pck_temp_buf[sizeof(LM_HEADER_t)+hdr->size], sizeof(tmp_u16));
	if (tmp_u16 != lm_calc_crc(pck_temp_buf, sizeof(LM_HEADER_t)+hdr->size)) return; //broken

	//add inbounds parsing task
	if (
			(hdr->dest_id == init->mesh_data->node_id)
		|| (hdr->transit_reciever_id == init->mesh_data->node_id)
		|| (!hdr->transit_reciever_id && (rssi > LM_PROXI_RSSI))
		|| !hdr->dest_id
		)
		lm_add_rx_task(channel, rssi, snr, pck_temp_buf, length);

	uint8_t* handle = (channel == LMCH_Main)?init->main_handle:init->serve_handle;
	lora_channel_recieve(*handle, 0, 1);

}

void lm_ping(uint8_t channel, uint32_t dest_node_id, uint8_t* data, uint8_t len)
{
	lm_add_tx_task(5, channel, (LM_Pck_type_t){LMPT_PING, 0, encryption}, init->mesh_data->node_id, dest_node_id, dest_node_id, init->mesh_data->pck_count++, data, len, 0);
}

void lm_handle_tx_task()
{
	LM_TX_TaskData_t* td = (LM_TX_TaskData_t*)(((LM_Task_t*)current_tx_task->data)->task_data);
	LM_Task_t* tsk = (LM_Task_t*)current_tx_task->data;
	uint8_t handle = (td->channel == LMCH_Main)?*init->main_handle:*init->serve_handle;
	tmp_u16 = (td->channel == LMCH_Main)?main_delay:serve_delay;

	switch(cur_task_stage)
	{
		case LMTS_START:
		{
			deadtime = HAL_GetTick()+tmp_u16;
			//stage_try_count = 10;
			cur_task_stage = LMTS_WAIT_ARBITRATION;

			break;
		}
		case LMTS_WAIT_ARBITRATION:
		{
			if (deadtime <= HAL_GetTick())
			{
				LORA_Result_t res = lora_channel_checkForChannelFree(handle, LM_CHANNEL_FREE_RSSI, tmp_u16); //TODO: Neighborhood threshold must be option
				if (res == LRR_Ok)
				{
					cur_task_stage = LMTS_ARBITRATION_OK;
					deadtime = HAL_GetTick()+10;
				}
				else
				{
					list_append(tx_task_list, current_tx_task);
					current_tx_task = 0;
					deadtime = 0;
					cur_task_stage = LMTS_NONE;
				}
			}
			break;
		}
		case LMTS_ARBITRATION_OK:
		{
			LORA_Result_t res = lora_channel_checkForChannelFree(handle, LM_CHANNEL_FREE_RSSI, tmp_u16); //TODO: Neighborhood threshold must be option
			if (res == LRR_Ok)
			{
				cur_task_stage = LMTS_EXECUTE;
				deadtime = HAL_GetTick()+100+lora_channel_packetDuration(handle, td->length)*2;
			}
			else
			{
				cur_task_stage = LMTS_START;
				deadtime = HAL_GetTick()+10;
			}
			break;
		}
		case LMTS_EXECUTE:
		{
			uint8_t* handle = (td->channel == LMCH_Main)?init->main_handle:init->serve_handle;
			uint8_t pc_len = lm_make_pck(td->type, td->source_id, td->dest_id, td->transit_id, td->pck_num, td->data, td->length, pck_temp_buf);

			LORA_Result_t res = lora_channel_send(*handle, pck_temp_buf, pc_len, lora_channel_packetDuration(*handle, pc_len)*2+10);

			lora_channel_recieve(*handle, 0, 1);

			if ((res == LRR_Ok) || (list_item_count(tx_task_list) >= MAX_TASK_QTY))
			{
				if (td->data) free(td->data);
				td->data = 0;
				cur_task_stage = LMTS_FINISH;
			}
			else
			{
				//TODO: error report
				list_append(tx_task_list, current_tx_task);
				current_tx_task = 0;
				deadtime = 0;
				cur_task_stage = LMTS_NONE;
			}
			break;
		}
		case LMTS_FINISH:
		{
			if (td->data) free(td->data);
			lm_kill_task(tsk);
			list_release_item(current_tx_task);
			current_tx_task = 0;
			deadtime = 0;
			cur_task_stage = LMTS_NONE;

			break;
		}
	}
}

void lm_handle_rx_task()
{
	LM_Task_t* tsk = (LM_Task_t*)current_rx_task->data;
	LM_RX_TaskData_t* td = (LM_RX_TaskData_t*)(tsk->task_data);
	LM_HEADER_t* hdr = (LM_HEADER_t*)td->data;
	if (td->channel == LMCH_Serve)
	{
		lm_register_node(hdr->transit_sender_id, td->rssi);
		lm_set_vote_time();
	}

	if (!hdr->dest_id || (hdr->dest_id != init->mesh_data->node_id)) //packet to retranslation
		lm_handle_retranslation(td);

	if ((hdr->dest_id && (hdr->dest_id == init->mesh_data->node_id)) || !hdr->dest_id)
		lm_handle_incoming_data(td);


	if (td->data) free(td->data);
	lm_kill_task(tsk);
	list_release_item(current_rx_task);
	current_rx_task = 0;
}

void lm_handle_retranslation(LM_RX_TaskData_t* task_data)
{
	LM_HEADER_t* hdr = (LM_HEADER_t*)task_data->data;
	switch(hdr->type.type)
	{
		case LMPT_WELCOME:
		case LMPT_CHANNEL_SETUP:
		case LMPT_PING:
		case LMPT_PINGME:
		//case LMPT_GODBY: //???
			break; //Retranslation is not allowed

		default:
		{
			uint8_t* payload = task_data->data+sizeof(LM_HEADER_t);
			LM_NodeData_t* node = lm_register_node(hdr->dest_id, (task_data->channel == LMCH_Serve)?task_data->rssi:-127);

			if (node)
			{
				if (node->proxy) //transfer directly to destination
				{
					if (node->pck_num < hdr->pck_num)
					{
						lm_add_tx_task(2, task_data->channel, hdr->type, hdr->source_id, hdr->dest_id, hdr->dest_id, hdr->pck_num, payload, hdr->size, 0);
						node->pck_num = hdr->pck_num;
					}
				}
				else //transfer via "friends"
				{
					if (node->pck_num < hdr->pck_num)
					{
						lm_send_to_neighborhood(2, task_data->channel, hdr->type, hdr->source_id, hdr->dest_id, hdr->pck_num, payload, hdr->size, 0);
						node->pck_num = hdr->pck_num;
					}
				}
			}
//			else //NODE is not registered, but transfer via friends
//				lm_send_to_neighborhood(task_data->channel, hdr->type, hdr->source_id, hdr->dest_id, hdr->pck_num, payload, hdr->size, 0);

		}//default
	}
}

void lm_handle_incoming_data(LM_RX_TaskData_t* task_data)
{
	LM_HEADER_t* hdr = (LM_HEADER_t*)task_data->data;

	if (hdr->type.confirm)
	{
		int idx = lm_get_wait_index(hdr->dest_id, hdr->pck_num, hdr->type.type);
		if (idx > -1)
		{
			List_item_t* d = list_get_item(wait_confirm, idx);
			list_remove_item(wait_confirm, d);
			LM_Confirm_data_t* cfd = (LM_Confirm_data_t*)d->data;
			if (cfd->data) free(cfd->data);
			list_release_item(d);

			init->mesh_data->node_state.txc = 1;

			//call event handler
			if (init->tx_complete_handler)
				init->tx_complete_handler();
		}
	}
	else
		lm_parse_incomming_data(task_data);
}

void lm_parse_incomming_data(LM_RX_TaskData_t* task_data)
{
	LM_HEADER_t* hdr = (LM_HEADER_t*)task_data->data;
	uint8_t* payload = task_data->data+sizeof(LM_HEADER_t);
	switch(hdr->type.type)
	{
		case LMPT_WELCOME:
		{
			if (task_data->channel == LMCH_Serve)
			{
				uint32_t* p_id = (uint32_t*)payload;
				if ((*p_id == hdr->source_id) && !hdr->pck_num)
				{
					LM_NodeData_t* node = lm_register_node(hdr->source_id, task_data->rssi);
					if (node)
					{
						//Send confirmation
						LM_PCK_WELCOME_CFM_t wconf = {task_data->rssi, task_data->snr};
						lm_add_tx_task(1, LMCH_Serve, (LM_Pck_type_t){LMPT_WELCOME, 1, encryption}, init->mesh_data->node_id, hdr->source_id, hdr->source_id, hdr->pck_num, (uint8_t*)&wconf, sizeof(LM_PCK_WELCOME_CFM_t), 0);
						node->pck_num = 0;
						//lm_set_vote_time();
					}
				}
			}
			break;
		}
		case LMPT_PING:
		{
			LM_NodeData_t* node = lm_register_node(hdr->source_id, (task_data->channel = LMCH_Serve)?task_data->rssi:-127);
			if (node)
			{
				if (node->pck_num <= hdr->pck_num)
				{
					LM_PCK_PING_conf_t ping = {task_data->rssi, task_data->snr};
					memcpy(ping.data, task_data->data, hdr->size);
					lm_add_tx_task(5, task_data->channel, (LM_Pck_type_t){LMPT_PING, 1, encryption}, init->mesh_data->node_id, hdr->source_id, hdr->source_id, hdr->pck_num, (uint8_t*)&ping, sizeof(ping.rssi)+sizeof(ping.snr)+hdr->size, 0);
					node->pck_num = hdr->pck_num;
				}
			}
			break;
		}
		case LMPT_LORA_DATA:
		{

			//store data
			memcpy(init->mesh_data->fifo, payload, hdr->size);
			init->mesh_data->data_len = hdr->size;
			init->mesh_data->packet_node_id = hdr->source_id;
			init->mesh_data->data_direction = LMPD_RX;
			init->mesh_data->pck_type = hdr->type.type;
			init->mesh_data->node_state.txc = 1;
			init->mesh_data->node_state.te = 0;

			if (hdr->type.encrypt)
				AES_ECB_decrypt(init->mesh_data->fifo, init->mesh_data->main_key, init->mesh_data->fifo, hdr->size);

			//send confirmation
			hdr->type.confirm = 1;
			lm_add_tx_task(0, LMCH_Serve, hdr->type, init->mesh_data->node_id, hdr->source_id, hdr->transit_sender_id, hdr->pck_num, 0, 0, 0);

			//call event handler
			if (init->rx_complete_handler) init->rx_complete_handler();

			break;
		}
	}
}

LM_NodeData_t* lm_find_neighbor(uint32_t node_id)
{
	LM_NodeData_t* node = lm_find_node(node_id);
	if (node)
		if (node->proxy) return node;
	return 0;
}

LM_NodeData_t* lm_register_node(uint32_t node_id, int8_t rssi)
{
	if (node_id == init->mesh_data->node_id) return 0;
	LM_NodeData_t* node = lm_find_node(node_id);
	if (!node)
	{
		node = (LM_NodeData_t*)malloc(sizeof(LM_NodeData_t));
		if (node)
		{
			node->node_id = node_id;
			node->pck_num = 0;
			node->proxy = 0;
			list_append(nodes_list, node);
		}
	}

	if (node) node->rssi = (rssi > -127)?rssi:node->rssi;

	return node;
}

LM_NodeData_t* lm_find_node(uint32_t node_id)
{
	List_item_t* item = nodes_list->item;
	if (!item->data) return 0;
	if (item)
	{
		LM_NodeData_t* node = (LM_NodeData_t*)item->data;
		if (node->node_id == node_id) return node;

		while (item->next)
		{
			item = item->next;
			node = (LM_NodeData_t*)item->data;
			if (node->node_id == node_id) return node;
		}

	}
	return 0;
}

LM_Confirm_data_t* lm_register_wait(uint32_t node_id, uint16_t pck_num, uint8_t type, uint8_t channel, uint8_t* data, uint8_t size, uint16_t timeout)
{
	LM_Confirm_data_t* pck = (LM_Confirm_data_t*)malloc(sizeof(LM_Confirm_data_t));
	if (!pck) return 0;

	*pck = (LM_Confirm_data_t){node_id, pck_num, type, channel, 0, size, HAL_GetTick()+timeout, WAIT_TRY_COUNT};
	pck->data = (uint8_t*)malloc(size);
	if (!pck->data)
	{
		free(pck);
		return 0;
	}
	memcpy(pck->data, data, size);
	list_append(wait_confirm, pck);
	return pck;
}

int16_t lm_get_wait_index(uint32_t node_id, uint16_t pck_num, uint8_t type)
{
	List_item_t* item = wait_confirm->item;
	if (!item) return -1;
	if (!item->data) return -1;


	LM_Confirm_data_t* pck = (LM_Confirm_data_t*)item->data;
	if ((pck->node_id == node_id) && (pck->pck_num == pck_num) && (pck->type == type)) return 0;

	int16_t idx = 0;
	while (item->next)
	{
		item = item->next;
		idx++;
		pck = (LM_Confirm_data_t*)item->data;
		if ((pck->node_id == node_id) && (pck->pck_num == pck_num) && (pck->type == type)) return idx;
	}

	return -1;
}

void lm_check_wait_timeout()
{
	List_item_t* item = wait_confirm->item;
	if (!item) return;
	if (!item->data) return;

	uint32_t now = HAL_GetTick();

	LM_Confirm_data_t* pck = (LM_Confirm_data_t*)item->data;
	if (pck->deadtime < now)
	{
		//retry
		if (pck->try_count)
		{
			lm_send_to_neighborhood(1, pck->channel, (LM_Pck_type_t){pck->type, 0, encryption}, init->mesh_data->node_id, pck->node_id, init->mesh_data->pck_count++, pck->data, pck->size, 0);
			pck->deadtime = HAL_GetTick() + CONFIRM_TIMEOUT*serve_delay;
			pck->try_count--;
		}
		else
		{
			init->mesh_data->node_state.txc = 0;
			init->mesh_data->node_state.te = 1;

			//Rebuild friend-list
			lm_ping_neighborhood(LMCH_Serve, (uint8_t*)&init->mesh_data->node_id, sizeof(init->mesh_data->node_id), 0);
			lm_clear_nodes();
			//Remove from wait-list
			lm_remove_wait_task(0);
		}
		return;
	}

	uint16_t idx = 0;

	while (item->next)
	{
		item = item->next;
		idx++;
		pck = (LM_Confirm_data_t*)item->data;
		if (pck->deadtime < now)
		{
			//retry
			if (pck->try_count)
			{
				lm_send_to_neighborhood(0, pck->channel, (LM_Pck_type_t){pck->type, 0, encryption}, init->mesh_data->node_id, pck->node_id, init->mesh_data->pck_count++, pck->data, pck->size, 0);
				pck->deadtime = HAL_GetTick() + CONFIRM_TIMEOUT*serve_delay;
				pck->try_count--;
			}
			else
			{
				//TODO: report error
				//Rebuild friend-list
				lm_ping_neighborhood(LMCH_Serve, (uint8_t*)&init->mesh_data->node_id, sizeof(init->mesh_data->node_id), 0);
				lm_clear_nodes();
				//Remove from wait-list
				lm_remove_wait_task(idx);
			}
			return;
		}
	}
}

void lm_remove_wait_task(int16_t index)
{
	List_item_t* ld = list_get_item(wait_confirm, index);
	if (!ld) return;
	list_remove_item(wait_confirm, ld);
	LM_Confirm_data_t* cfd = (LM_Confirm_data_t*)ld->data;
	if (cfd->data) free(cfd->data);
	list_release_item(ld);
}

uint8_t lm_exam_friend(uint8_t idx, uint32_t node_id, int8_t rssi)
{
	if ((rssi >= LM_PROXI_RSSI) || (idx < 3))
	{
		if (friends[idx].rssi < rssi)
		{
			if (idx < (FRIENDS_COUNT-1)) lm_exam_friend(idx+1, friends[idx].node_id, friends[idx].rssi);
			friends[idx].rssi = rssi;
			friends[idx].node_id = node_id;
			return 1;
		}
	}
	return 0;
}

uint16_t lm_calc_crc(void* data, uint16_t length)
{
    uint8_t ra = 0;
    uint8_t rb = 0;
    for (int i = 0; i < length; i++)
    {
        rb += *((uint8_t*)(data+i));
        ra += rb;
    }
    return((ra << 8) | rb);
}

uint8_t	lm_make_pck(LM_Pck_type_t type, uint32_t src_id, uint32_t dest_id, uint32_t transit_id, uint16_t num, uint8_t* data, uint8_t length, uint8_t* out)
{
	LM_HEADER_t* hdr = (LM_HEADER_t*)out;
	hdr->dest_id = dest_id;
	hdr->source_id = src_id;
	hdr->transit_reciever_id = transit_id;
	hdr->transit_sender_id = init->mesh_data->node_id;
	hdr->type = type;
	hdr->size = length;
	hdr->pck_num = num;

	memcpy(out+sizeof(LM_HEADER_t), data, length);

	if (type.encrypt && (src_id == init->mesh_data->node_id))
		AES_ECB_encrypt(out+sizeof(LM_HEADER_t), init->mesh_data->main_key, out+sizeof(LM_HEADER_t), length);

	uint16_t crc = lm_calc_crc(out, sizeof(LM_HEADER_t)+length);
	memcpy(out +sizeof(LM_HEADER_t)+length, &crc, sizeof(crc));

	return (sizeof(LM_HEADER_t)+length+sizeof(uint16_t));
}


