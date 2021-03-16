/*
 * usb_link.c
 *
 *  Created on: 30 џэт. 2021 у.
 *      Author: Rem Norton
 */
#include <string.h>
#include "usb_link.h"
#include "usbd_cdc_if.h"

static ULINK_t* ulink = 0;

static uint8_t	ulink_rx_buf[ULINK_RX_BUF_SIZE];
static uint8_t	ulink_tx_buf[ULINK_TX_BUF_SIZE];

static uint16_t	ulink_rx_idx = 0;
static uint16_t	ulink_tx_idx = sizeof(ULINK_Pck_t);

//Private members
void ulink_send_data(uint8_t* data, uint16_t len);
void ulink_try_to_send();
void ulink_handle_incoming_data();
void ulink_parse(uint8_t* data, uint16_t len);
void ulink_handle_service_chunk(uint8_t id, uint8_t* data, uint8_t len);
void ulink_handle_link_chunk(uint8_t id, uint8_t* data, uint8_t len);
void ulink_handle_drive_chunk(uint8_t id, uint8_t* data, uint8_t len);
void ulink_handle_calib_chunk(uint8_t id, uint8_t* data, uint8_t len);
void ulink_autosend();

ULINK_Param_t*	ulink_find_service_param(uint8_t id);
ULINK_Calib_t*	ulink_find_calib_param(uint8_t id);

//default callback handlers
void ulink_default_param_changed(uint8_t param_id);
void ulink_default_drive_stop(uint8_t drive_flags, uint16_t accel, uint8_t hold);
void ulink_default_drive_move(uint8_t direction, uint8_t deviation, uint32_t path, uint16_t speed, uint16_t acceleration, uint16_t radius, uint8_t hold);
void ulink_default_drive_rotate(uint8_t deviation, uint32_t path, uint16_t speed, uint16_t acceleration, uint8_t hold);
void ulink_default_screen_rotate(uint8_t deviation, uint32_t path, uint16_t speed, uint16_t acceleration, uint8_t hold);

//Public members
void usb_link_init(ULINK_t* lnk)
{
	ulink = lnk;
	if (ulink)
	{
		if (!ulink->param_count || !ulink->parameters)
			ulink = 0;
	}

	if (ulink)
	{
		//initialize parameters
		for (int i = 0; i < ulink->param_count; i++)
		{
			ULINK_Param_t* p = &ulink->parameters[i];
			p->send_interval = 0;
			p->send_time = 0;
		}

		//initialize default handlers
		ulink->calib_param_changed = ulink_default_param_changed;
		ulink->service_param_changed = ulink_default_param_changed;
		ulink->drive_stop = ulink_default_drive_stop;
		ulink->drive_move = ulink_default_drive_move;
		ulink->drive_rotate = ulink_default_drive_rotate;
		ulink->screen_rotate = ulink_default_screen_rotate;
	}
}

void usb_link_loop()
{
	if (!ulink)
		return;

	if (ulink_rx_idx)
		ulink_handle_incoming_data();

	ulink_autosend();

	if (ulink_tx_idx > sizeof(ULINK_Pck_t))
		ulink_try_to_send();

}

void usb_link_rx(uint8_t* data, uint32_t len)
{
	if (!ulink)
		return;
	if ((ulink_rx_idx+len) >= ULINK_RX_BUF_SIZE)
		return;

	memcpy(&ulink_rx_buf[ulink_rx_idx], data, len);
	ulink_rx_idx += len;
}

//Private members
void ulink_send_data(uint8_t* data, uint16_t len)
{
	if ((ulink_tx_idx+len+1) >= ULINK_TX_BUF_SIZE)
		return;
	memcpy(&ulink_tx_buf[ulink_tx_idx], data, len);
	ulink_tx_idx += len;
}

void ulink_try_to_send()
{
	ULINK_Pck_t* pck = (ULINK_Pck_t*)ulink_tx_buf;
	pck->prefix = UL_PREFIX;
	pck->data_size = ulink_tx_idx - sizeof(ULINK_Pck_t);
	ulink_tx_buf[ulink_tx_idx] = UL_SUFFIX;
	if (CDC_Transmit_FS(ulink_tx_buf, ulink_tx_idx+1) == USBD_OK)
		ulink_tx_idx = sizeof(ULINK_Pck_t);
}

void ulink_handle_incoming_data()
{
	if (ulink_rx_idx < sizeof(ULINK_Pck_t))
		return;

	uint16_t offset = 0;
	while ((ulink_rx_buf[offset] != UL_PREFIX) && (offset < ulink_rx_idx))
		offset++;

	if (offset)
	{
		memcpy(ulink_rx_buf, &ulink_rx_buf[offset], sizeof(ulink_rx_buf)-offset);
		ulink_rx_idx -= offset;
	}


	ULINK_Pck_t* pck = (ULINK_Pck_t*)ulink_rx_buf;
	uint16_t pck_full_size = sizeof(ULINK_Pck_t) + pck->data_size + 1;

	if (ulink_rx_idx < pck_full_size)
		return;

	if (ulink_rx_buf[sizeof(ULINK_Pck_t)+pck->data_size] != UL_SUFFIX)
	{
		memcpy(ulink_rx_buf, &ulink_rx_buf[1], ulink_rx_idx-1);
		ulink_rx_idx -= 1;
		return;
	}

	if (pck->data_size)
		ulink_parse(pck->data, pck->data_size);


	//remove handled data
	memcpy(ulink_rx_buf, &ulink_rx_buf[pck_full_size], sizeof(ulink_rx_buf)-pck_full_size);
	ulink_rx_idx -= pck_full_size;
}

void ulink_parse(uint8_t* data, uint16_t len)
{
	while(len >= sizeof(ULINK_Chunk_t))
	{
		ULINK_Chunk_t* chunk = (ULINK_Chunk_t*)data;
		uint16_t chunk_len = sizeof(ULINK_Chunk_t) + chunk->data_size;

		if (len >= chunk_len)
		{
			switch (chunk->group)
			{
				case UL_GROUP_SERVICE:
				{
					ulink_handle_service_chunk(chunk->id, chunk->data, chunk->data_size);
					break;
				}
				case UL_GROUP_LINK:
				{
					ulink_handle_link_chunk(chunk->id, chunk->data, chunk->data_size);
					break;
				}
				case UL_GROUP_DRIVE:
				{
					ulink_handle_drive_chunk(chunk->id, chunk->data, chunk->data_size);
					break;
				}
				case UL_GROUP_CALIB:
				{
					ulink_handle_calib_chunk(chunk->id, chunk->data, chunk->data_size);
					break;
				}
			}

			data += chunk_len;
			len -= chunk_len;
		}
		else
			len = 0;
	}
}

void ulink_handle_service_chunk(uint8_t id, uint8_t* data, uint8_t len)
{
	ULINK_Param_t* p = ulink_find_service_param(id);
	if (p)
	{
		uint8_t mode = len?UL_WRITE:UL_READ;
		if (p->mode & mode)
		{
			switch(mode)
			{
				case UL_READ:
				{
					ULINK_Chunk_t ch;
					ch.group = UL_GROUP_SERVICE;
					ch.id = id;
					ch.data_size = sizeof(uint32_t);
					*((uint32_t*)ch.data) = *p->value;
					ulink_send_data((uint8_t*)&ch, sizeof(ULINK_Chunk_t)+ch.data_size);
					break;
				}
				case UL_WRITE:
				{
					memcpy(p->value, data, len);
					ulink->service_param_changed(id);
					break;
				}
			}
		}
	}
}


void ulink_handle_calib_chunk(uint8_t id, uint8_t* data, uint8_t len)
{
	ULINK_Calib_t* p = ulink_find_calib_param(id);
	if (p)
	{
		uint8_t mode = len?UL_WRITE:UL_READ;
		if (p->mode & mode)
		{
			switch(mode)
			{
				case UL_READ:
				{
					ULINK_Chunk_t ch;
					ch.group = UL_GROUP_SERVICE;
					ch.id = id;
					ch.data_size = sizeof(uint32_t);
					*((uint32_t*)ch.data) = *p->value;
					ulink_send_data((uint8_t*)&ch, sizeof(ULINK_Chunk_t)+ch.data_size);
					break;
				}
				case UL_WRITE:
				{
					memcpy(p->value, data, len);
					ulink->calib_param_changed(id);
					break;
				}
			}
		}
	}
}

void ulink_handle_link_chunk(uint8_t id, uint8_t* data, uint8_t len)
{
	switch(id)
	{
		case UL_LINK_CMD_AUTOSEND_START:
		{
			if (len == sizeof(ULINK_Autosend_t))
			{
				ULINK_Autosend_t* snd = (ULINK_Autosend_t*)data;
				if (snd->param_id != 0xff)
				{
					ULINK_Param_t* p = ulink_find_service_param(snd->param_id);
					if (p)
					{
						if (p->mode & UL_READ)
						{
							p->send_time = 0;
							p->send_interval = snd->send_interval;
						}
					}
				}
				else
				{
					for (int i = 0; i < ulink->param_count; i++)
					{
						ULINK_Param_t* p = &ulink->parameters[i];
						if (p->mode & UL_READ)
						{
							p->send_time = 0;
							p->send_interval = snd->send_interval;
						}
					}
				}

			}
			break;
		}
		case UL_LINK_CMD_AUTOSEND_STOP:
		{
			uint8_t id = *data;
			if (id != 0xff)
			{
				ULINK_Param_t* p = ulink_find_service_param(id);
				if (p)
					p->send_interval = 0;
			}
			else
			{
				for (int i = 0; i < ulink->param_count; i++)
				{
					ULINK_Param_t* p = &ulink->parameters[i];
					p->send_interval = 0;
				}
			}
			break;
		}
	}
}

void ulink_handle_drive_chunk(uint8_t id, uint8_t* data, uint8_t len)
{
	switch(id)
	{
		case UL_DRV_CMD_STOP_ALL:
		{
			if (len == sizeof(ULINK_Stop_t))
			{
				ULINK_Stop_t* cmd = (ULINK_Stop_t*)data;
				ulink->drive_stop(UL_DRV_MOVE | UL_DRV_SCREEN, cmd->acceleration, cmd->hold);
			}
			break;
		}
		case UL_DRV_CMD_STOP_MOVE:
		{
			if (len == sizeof(ULINK_Stop_t))
			{
				ULINK_Stop_t* cmd = (ULINK_Stop_t*)data;
				ulink->drive_stop(UL_DRV_MOVE, cmd->acceleration, cmd->hold);
			}
			break;
		}
		case UL_DRV_CMD_STOP_SCREEN:
		{
			if (len == sizeof(ULINK_Stop_t))
			{
				ULINK_Stop_t* cmd = (ULINK_Stop_t*)data;
				ulink->drive_stop(UL_DRV_SCREEN, cmd->acceleration, cmd->hold);
			}
			break;
		}
		case UL_DRV_CMD_MOVE:
		{
			if (len == sizeof(ULINK_Move_t))
			{
				ULINK_Move_t* cmd = (ULINK_Move_t*)data;
				if (
						((cmd->direction == UL_DIR_FORWARD) || (cmd->direction == UL_DIR_REVERSE)) &&
						((cmd->deviation >= UL_DVT_NONE) && (cmd->deviation <= UL_DVT_RIGHT))
					)
					ulink->drive_move(cmd->direction, cmd->deviation, cmd->path, cmd->speed, cmd->acceleration, cmd->radius, cmd->hold);
			}
			break;
		}
		case UL_DRV_CMD_ROTATE:
		{
			if (len == sizeof(ULINK_Rotation_t))
			{
				ULINK_Rotation_t* cmd = (ULINK_Rotation_t*)data;
				if ((cmd->deviation >= UL_DVT_NONE) && (cmd->deviation <= UL_DVT_RIGHT))
					ulink->drive_rotate(cmd->deviation, cmd->path, cmd->speed, cmd->acceleration, cmd->hold);
			}
			break;
		}
		case UL_DRV_CMD_SCREEN_ROTATE:
		{
			if (len == sizeof(ULINK_Rotation_t))
			{
				ULINK_Rotation_t* cmd = (ULINK_Rotation_t*)data;
				if ((cmd->deviation >= UL_DVT_NONE) && (cmd->deviation <= UL_DVT_RIGHT))
					ulink->screen_rotate(cmd->deviation, cmd->path, cmd->speed, cmd->acceleration, cmd->hold);
			}
			break;
		}
	}
}



void ulink_autosend()
{
	uint32_t now = HAL_GetTick();
	for (int i = 0; i < ulink->param_count; i++)
	{
		ULINK_Param_t* p = &ulink->parameters[i];
		if (p->send_interval && ((p->send_time + p->send_interval) <= now))
		{
			p->send_time = now;
			ULINK_Chunk_t ch;
			ch.group = UL_GROUP_SERVICE;
			ch.id = p->param_id;
			ch.data_size = sizeof(uint32_t);
			*((uint32_t*)ch.data) = *p->value;
			ulink_send_data((uint8_t*)&ch, sizeof(ULINK_Chunk_t)+ch.data_size);
		}
	}
}

ULINK_Param_t*	ulink_find_service_param(uint8_t id)
{
	ULINK_Param_t* res = 0;
	for (int i = 0; (i < ulink->param_count) && !res; i++)
	{
		res = &ulink->parameters[i];
		if (res->param_id != id)
			res = 0;
	}

	return res;
}

ULINK_Calib_t* ulink_find_calib_param(uint8_t id)
{
	ULINK_Calib_t* res = 0;
	for (int i = 0; i < ulink->calib && !res; i++)
	{
		res = &ulink->calib[i];
		if (res->calib_id != id)
			res = 0;
	}

	return res;
}

//default callback handlers
void ulink_default_param_changed(uint8_t param_id)
{

}


void ulink_default_drive_stop(uint8_t drive_flags, uint16_t accel, uint8_t hold)
{

}

void ulink_default_drive_move(uint8_t direction, uint8_t deviation, uint32_t path, uint16_t speed, uint16_t acceleration, uint16_t radius, uint8_t hold)
{

}

void ulink_default_drive_rotate(uint8_t deviation, uint32_t path, uint16_t speed, uint16_t acceleration, uint8_t hold)
{

}

void ulink_default_screen_rotate(uint8_t deviation, uint32_t path, uint16_t speed, uint16_t acceleration, uint8_t hold)
{

}
