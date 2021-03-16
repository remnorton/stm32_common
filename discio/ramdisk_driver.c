/*
 * ramdisk_driver.c
 *
 *  Created on: 21 но€б. 2019 г.
 *      Author: User
 */

#include "ramdisk_driver.h"
#include <string.h>

#define _RAMD_BLOCK_SIZE_	512

//
//
//

uint8_t ramd_is_valid(RAMDISK_t* r)
{
	if (!r) return 0;
	if (!r->disk || !r->size) return 0;
	return 1;
}

//
//
//
uint8_t ramdisk_driver_init(void* d)
{
	RAMDISK_t* r = (RAMDISK_t*)d;
	return ramd_is_valid(r);
}

void ramdisk_driver_deinit(void* d)
{

}

uint32_t ramdisk_driver_get_block_size(void* d)
{
	RAMDISK_t* r = (RAMDISK_t*)d;
	if (!ramd_is_valid(r)) return 0;
	return _RAMD_BLOCK_SIZE_;
}

uint16_t ramdisk_driver_get_block_count(void* d)
{
	RAMDISK_t* r = (RAMDISK_t*)d;
	if (!ramd_is_valid(r)) return 0;
	return r->size/_RAMD_BLOCK_SIZE_;
}

uint8_t ramdisk_driver_write_data(void* d, uint8_t* buf, uint32_t sector, uint16_t count)
{
	RAMDISK_t* r = (RAMDISK_t*)d;
	if (!ramd_is_valid(r)) return 0;
	uint32_t addr = sector*_RAMD_BLOCK_SIZE_;
	if ((addr + count*_RAMD_BLOCK_SIZE_) >= r->size) return 0;
	uint8_t* data = (uint8_t*)r->disk;
	memcpy(&data[addr], buf, count*_RAMD_BLOCK_SIZE_);
	return 1;
}

uint8_t ramdisk_driver_read_data(void* d, uint8_t* buf, uint32_t sector, uint16_t count)
{
	RAMDISK_t* r = (RAMDISK_t*)d;
	if (!ramd_is_valid(r)) return 0;
	uint32_t addr = sector*_RAMD_BLOCK_SIZE_;
	if ((addr + count*_RAMD_BLOCK_SIZE_) >= r->size) return 0;
	uint8_t* data = (uint8_t*)r->disk;
	memcpy(buf, &data[addr], count*_RAMD_BLOCK_SIZE_);
	return 1;
}

uint8_t ramdisk_driver_is_ready(void* d)
{
	RAMDISK_t* r = (RAMDISK_t*)d;
	return ramd_is_valid(r);
}

BlockDeviceDriver_t ramdisk_driver =
{
		ramdisk_driver_init,				//uint8_t			(*init)(void*);
		ramdisk_driver_deinit,				//void				(*deinit)(void*);
		ramdisk_driver_get_block_size,		//uint32_t			(*get_block_size)(void*);
		ramdisk_driver_get_block_count,		//uint16_t			(*get_block_count)(void*);
		ramdisk_driver_write_data,			//uint8_t			(*write_data)(void*, uint8_t*, uint32_t /*blk_addr*/, uint16_t /*blk_len*/);
		ramdisk_driver_read_data,			//uint8_t			(*read_data)(void*, uint8_t*, uint32_t /*blk_addr*/, uint16_t /*blk_len*/);
		ramdisk_driver_is_ready				//uint8_t			(*is_ready)(void*);
};
