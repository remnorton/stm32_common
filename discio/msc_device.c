/*
 * msc_device.c
 *
 *  Created on: 19 но€б. 2019 г.
 *      Author: User
 */

#include "msc_device.h"

#if defined(MSC_DEBUG)
#include <stdio.h>
	#define MSCD_LOG(a,b,c)		printf(a,b,c)
#else
	#define MSCD_LOG(a,b,c)		{}
#endif


#define _MAX_LUN_	5

static BlockDevice_t* luns[_MAX_LUN_] = {0};
static uint8_t lun_idx = 0;

//
//Private members
//
BlockDevice_t* msc_device_get(uint8_t lun)
{
	if (lun >= lun_idx) return 0;
	return luns[lun];
}
//
//Public members
//
uint8_t msc_device_register(uint8_t we, void* device, BlockDeviceDriver_t* driver)
{
	BlockDevice_t* dev = iodevice_create(IODT_MSC, we, device, driver);
	if (!dev) return 0;
	luns[lun_idx++] = dev;
	return lun_idx;
}

uint8_t msc_device_init(uint8_t lun)
{
	return (msc_device_get(lun))?1:0;
}

uint8_t msc_device_get_capacity(uint8_t lun, uint32_t *block_num, uint16_t *block_size)
{
	BlockDevice_t* dev = msc_device_get(lun);
	if (!dev) return 0;
	*block_num = iodevice_get_block_count(dev);
	*block_size = iodevice_get_block_size(dev);
	return 1;
}

uint8_t msc_device_is_ready(uint8_t lun)
{
	BlockDevice_t* dev = msc_device_get(lun);
	if (!dev) return 0;
	return iodevice_is_ready(dev);
}

uint8_t msc_device_is_write_protected(uint8_t lun)
{
	BlockDevice_t* dev = msc_device_get(lun);
	if (!dev) return 1;
	return !dev->we;
}

uint8_t msc_device_read(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
	BlockDevice_t* dev = msc_device_get(lun);
	if (!dev) return 0;
	uint8_t result = iodevice_read(dev, buf, blk_addr, blk_len);
	MSCD_LOG("msc_device_read, sector: %u; result: %u\r\n", blk_addr, result);
	return result;
}

uint8_t msc_device_write(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
	BlockDevice_t* dev = msc_device_get(lun);
	if (!dev) return 0;
	uint8_t result = iodevice_write(dev, buf, blk_addr, blk_len);
	MSCD_LOG("msc_device_write, sector: %u; result: %u\r\n", blk_addr, result);
	return result;
}
