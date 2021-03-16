/*
 * uexp.c
 *
 *  Created on: 2 сент. 2019 г.
 *      Author: Rem Norton
 */
#include <string.h>
#include "uexp.h"
#include "ring_buf.h"

static UEXP_t* instance = 0;
static uint8_t pck_buf[_PCK_MAX_LEN];
static uint8_t tx_buf[_PCK_MAX_LEN];
static uint16_t buf_head = 0;
static uint16_t buf_tail = 0;


//
//Private forwards
//
uint16_t uexp_process_pck(uint8_t* data, uint16_t len);
uint16_t uexp_process_cmd(uint16_t cmd, uint8_t argc, uint8_t* argv);
UEXP_StoredParam_t* uexp_get_param(uint16_t uid);
uint16_t uexp_add_param_section(UEXP_StoredParam_t* param, uint8_t* dest);

//
//Public members
//

uint8_t	uexp_init(UEXP_t* init)
{
	if (!init) return 0;
	if (!init->uart) return 0;
	if (!init->uart->hdmarx) return 0;
	if (!init->uart->pRxBuffPtr) return 0;
	if (!init->uart->RxXferSize) return 0;
	if (!init->stored_params_count) return 0;
	if (!init->params) return 0;

	instance = init;
	return 1;
}

void uexp_step()
{
	if (!instance) return;
	uint16_t rx_buf_size = instance->uart->RxXferSize;
	buf_head = rx_buf_size - __HAL_DMA_GET_COUNTER(instance->uart->hdmarx);

	uint8_t* buf = instance->uart->pRxBuffPtr;

	while ((buf[buf_tail] != _PCK_PREFIX_) && (buf_head != buf_tail))
		buf_tail = ring_add(buf_tail, 1, rx_buf_size);

	uint16_t len = ring_len(buf_head, buf_tail, rx_buf_size);


	if (len >= (sizeof(UEXP_PckHeader_t) + sizeof(UEXP_SectionHeader_t) + sizeof(UEXP_CRC_t)))
	{
		ring_extract(pck_buf, buf, buf_tail, len, rx_buf_size);
		UEXP_PckHeader_t* hdr = (UEXP_PckHeader_t*)pck_buf;
		if (len >= (sizeof(UEXP_PckHeader_t) + hdr->size + sizeof(UEXP_CRC_t)))
		{
			//check CRC
			uint16_t* pcrc = (uint16_t*)&pck_buf[sizeof(UEXP_PckHeader_t) + hdr->size];
			uint16_t crc = uexp_crc(pck_buf, sizeof(UEXP_PckHeader_t) + hdr->size);
			uint16_t offset = 0;
			if ((*pcrc == crc) || (1 == 1))
				offset = uexp_process_pck(&pck_buf[sizeof(UEXP_PckHeader_t)], hdr->size) + sizeof(UEXP_PckHeader_t) + sizeof(UEXP_CRC_t);
			else
				offset = 1;
			buf_tail = ring_add(buf_tail, offset, rx_buf_size);
		}
	}
}

UEXP_CRC_t uexp_crc(void* data, uint16_t len)
{
	uint8_t ra, rb;
	ra = rb = 0;
	uint8_t* dt = (uint8_t*)data;
	for (int i = 0; i < len; i++)
	{
		ra += dt[i];
		rb += ra;
	}
	return (rb << 8)+ra;
}

//
//Private members
//

uint16_t uexp_process_pck(uint8_t* data, uint16_t len)
{
	uint16_t res = 0;
	while (len)
	{
		UEXP_SectionHeader_t* section = (UEXP_SectionHeader_t*)data;
		data += sizeof(UEXP_SectionHeader_t);
		switch (section->type)
		{
			case UEXP_ST_DATA:
			{
				break;
			}
			case UEXP_ST_CMD:
			{
				res += uexp_process_cmd(section->payload.cmd.command, section->payload.cmd.argcnt, data);
				data += section->payload.cmd.argcnt*sizeof(uint16_t);
				break;
			}
			default:
				len = 0;
		}
	}
	return res;
}

uint16_t uexp_process_cmd(uint16_t cmd, uint8_t argc, uint8_t* argv)
{
	uint16_t pck_len = 0 ;
	switch(cmd)
	{
		case 0:	//read parameter
		{
			uint16_t* req_params = (uint16_t*)argv;
			UEXP_PckHeader_t* header = (UEXP_PckHeader_t*)tx_buf;
			header->prefix = _PCK_PREFIX_;
			header->size = 0;
			for (int i = 0; i < argc; i++)
			{
				UEXP_StoredParam_t* p = uexp_get_param(req_params[i]);
				if (p) header->size += uexp_add_param_section(p, tx_buf + sizeof(UEXP_PckHeader_t) + header->size);
			}

			UEXP_CRC_t* crc = (UEXP_CRC_t*)&tx_buf[sizeof(UEXP_PckHeader_t) + header->size];
			*crc = uexp_crc(tx_buf, sizeof(UEXP_PckHeader_t) + header->size);
			pck_len = sizeof(UEXP_PckHeader_t) + header->size + sizeof(UEXP_CRC_t);
			break;
		}
		default: //User command
			if (instance->command_handler) return instance->command_handler(cmd, argc, argv);
	}

	if (pck_len) HAL_UART_Transmit(instance->uart, tx_buf, pck_len, pck_len*3/10);
	return sizeof(UEXP_SectionHeader_t) + (sizeof(uint16_t)*argc);

}

UEXP_StoredParam_t* uexp_get_param(uint16_t uid)
{
	UEXP_StoredParam_t* res = 0;
	for (int i = 0; (i < instance->stored_params_count) && !res; i++)
	{
		if (instance->params[i].uid == uid)
			res = &instance->params[i];
	}
	return res;
}

uint16_t uexp_add_param_section(UEXP_StoredParam_t* param, uint8_t* dest)
{
	UEXP_SectionHeader_t* chunk = (UEXP_SectionHeader_t*)dest;
	chunk->type = UEXP_ST_DATA;
	chunk->payload.param.uid = param->uid;
	chunk->payload.param.size = param->size;
	memcpy(dest+sizeof(UEXP_SectionHeader_t), param->data, param->size);

	return sizeof(UEXP_SectionHeader_t)+param->size;
}
