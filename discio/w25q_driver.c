/*
 * w25q_driver.c
 *
 *  Created on: 19 но€б. 2019 г.
 *      Author: User
 */

#include "w25q_driver.h"
#include "w25q.h"

#define _DEF_ALLOC_SIZE_	4096

static uint8_t* io_buf = 0;
static uint8_t use_buf = 1;
//
//Private members
//
W25Q_Result_t w25q_driver_is_valid(W25Q_t* i)
{
	if (!i) return W25Q_Error;
	if (!i->detail) return W25Q_Error;
	return W25Q_Ok;
}

uint8_t w25q_au_is_clear(W25Q_t* i, uint32_t addr, uint16_t au_size)
{
	if (!w25q_driver_is_valid(i)) return 0;
	uint8_t clear = 1;
	while (au_size && clear)
	{
		uint8_t tmp[256];
		uint16_t btr = (au_size < 256)?au_size:256;
		w25q_read(i, addr, tmp, btr);
		for (int i = 0; (i < btr) && clear; i++)
			clear = (tmp[i] == 0xff)?1:0;
		addr += btr;
		au_size -= btr;
		//au_size = 0;
	}
	return clear;
}

//
//Driver members
//
uint8_t	w25q_driver_is_ready(void* d)
{
	W25Q_t* w = (W25Q_t*)d;
	if (!w->detail) return 0;
	if (!w25q_driver_is_valid(w)) return 0;
	return (w25q_get_status(w).busy == 1)?0:1;
}

uint8_t w25q_driver_init(void* d)
{
	W25Q_t* w = (W25Q_t*)d;
	if (w->detail) return 1;
	if (w25q_init(w) != W25Q_Ok) return 0;

	if (w->detail->sector_size > _DEF_ALLOC_SIZE_)
	{
		io_buf = (uint8_t*)malloc(w->detail->sector_size);
		return io_buf?1:0;
	}
	use_buf = 0;
	return 1;
}

void w25q_driver_deinit(void* d)
{
	W25Q_t* w = (W25Q_t*)d;
	if (!w->detail) return;
	w25q_deinit(w);
}

uint32_t w25q_driver_get_block_size(void* d)
{
	W25Q_t* w = (W25Q_t*)d;
	if (!w->detail) return 0;
	return use_buf?_DEF_ALLOC_SIZE_:w->detail->sector_size;
}

uint16_t w25q_driver_get_block_count(void* d)
{
	W25Q_t* w = (W25Q_t*)d;
	if (!w->detail) return 0;
	return use_buf?(w->detail->capacity/_DEF_ALLOC_SIZE_):w->detail->sector_count;
}

uint8_t	w25q_driver_write_data(void* d, uint8_t* buf, uint32_t blk_addr, uint16_t blk_len)
{
	W25Q_t* w = (W25Q_t*)d;
	if (!w->detail) return 0;
	if (!w25q_driver_is_ready(d)) return 0;

	if (use_buf)
	{
		uint32_t addr = blk_addr*_DEF_ALLOC_SIZE_;
		if (!w25q_au_is_clear(w, addr, _DEF_ALLOC_SIZE_))
		{
			uint16_t sec_num = w25q_sector_num(w, addr);
			uint32_t sec_addr = sec_num*w->detail->sector_size;
			if (w25q_read(w, sec_addr, io_buf, w->detail->sector_size) != W25Q_Ok) return 0;
			memcpy(&io_buf[addr - sec_addr], buf, _DEF_ALLOC_SIZE_);
			if (w25q_erase_sector(w, sec_num) != W25Q_Ok) return 0;
			return (w25q_write(w, sec_addr, io_buf, w->detail->sector_size) == W25Q_Ok)?1:0;
		}
		else return (w25q_write(w, blk_addr*_DEF_ALLOC_SIZE_, buf, _DEF_ALLOC_SIZE_) == W25Q_Ok)?1:0;
	}
	else
	{
		if (!w25q_sector_is_clear(w, blk_addr)) w25q_erase_sector(w, blk_addr);
		return (w25q_write(w, blk_addr*w->detail->sector_size, buf, blk_len*w->detail->sector_size) == W25Q_Ok)?1:0;
	}
}

uint8_t	w25q_driver_read_data(void* d, uint8_t* buf, uint32_t blk_addr, uint16_t blk_len)
{
	W25Q_t* w = (W25Q_t*)d;
	if (!w->detail) return 0;
	if (!w25q_driver_is_ready(d)) return 0;
	uint32_t addr = use_buf?(blk_addr*_DEF_ALLOC_SIZE_):(blk_addr*w->detail->sector_size);
	uint32_t len = use_buf?_DEF_ALLOC_SIZE_:w->detail->sector_size;

	return (w25q_read(w, addr, buf, len) == W25Q_Ok)?1:0;
}


BlockDeviceDriver_t w25q_driver =
{
		w25q_driver_init,				//uint8_t			(*init)(void*);
		w25q_driver_deinit,				//void				(*deinit)(void*);
		w25q_driver_get_block_size,		//uint32_t			(*get_block_size)(void*);
		w25q_driver_get_block_count,	//uint16_t			(*get_block_count)(void*);
		w25q_driver_write_data,			//uint8_t			(*write_data)(void*, uint8_t*, uint32_t /*blk_addr*/, uint16_t /*blk_len*/);
		w25q_driver_read_data,			//uint8_t			(*read_data)(void*, uint8_t*, uint32_t /*blk_addr*/, uint16_t /*blk_len*/);
		w25q_driver_is_ready			//uint8_t			(*is_ready)(void*);
};
