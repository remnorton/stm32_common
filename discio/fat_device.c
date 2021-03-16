/*
 * fat_device.c
 *
 *  Created on: 20 но€б. 2019 г.
 *      Author: User
 */
#include "fat_device.h"


#if defined(FFS_DEBUG)
#include <stdio.h>
	#define FFSD_LOG(a,b,c)		printf(a,b,c)
#else
	#define FFSD_LOG(a,b,c)		{}
#endif

#define _MAX_DRV_	5

static BlockDevice_t* drvs[_MAX_DRV_] = {0};
static uint8_t drv_idx = 0;


enum
{
	FAT_RES_OK = 0,		/* 0: Successful */
	FAT_RES_ERROR,		/* 1: R/W Error */
	FAT_RES_WRPRT,		/* 2: Write Protected */
	FAT_RES_NOTRDY,		/* 3: Not Ready */
	FAT_RES_PARERR		/* 4: Invalid Parameter */
};

enum
{
	FAT_CMD_CTRL_SYNC = 0,
	FAT_CMD_GET_SECTOR_COUNT,
	FAT_CMD_GET_SECTOR_SIZE,
	FAT_CMD_GET_BLOCK_SIZE,
	FAT_CMD_CTRL_TRIM
};

//
//Private members
//
BlockDevice_t* fat_device_get(uint8_t pdrv)
{
	if (pdrv >= drv_idx) return 0;
	return drvs[pdrv];
}

uint8_t fat_device_is_valid(BlockDevice_t* d)
{
	if (!d) return 0;
	if (!d->device) return 0;
	if (!d->driver) return 0;
	return (d->type == IODT_FATFS)?1:0;
}

uint8_t fat_device_is_ready(BlockDevice_t* d)
{
	if (!fat_device_is_valid(d)) return 0;
	if (!d->driver->is_ready) return 0;
	return d->driver->is_ready(d->device);
}

//
//Public members
//
uint8_t fat_device_register(uint8_t we, void* device, BlockDeviceDriver_t* driver)
{
	BlockDevice_t* dev = iodevice_create(IODT_FATFS, we, device, driver);
	if (!dev) return 0;
	drvs[drv_idx++] = dev;
	return drv_idx;
}

uint8_t fat_device_init(uint8_t pdrv)
{
	return fat_device_get(pdrv)?1:0;
}

uint8_t fat_device_get_status(uint8_t pdrv)
{
	BlockDevice_t* d = fat_device_get(pdrv);
	if (!d) return 1; //STA_NOINIT
	if (!d->device || !d->driver) return 1; //STA_NOINIT
	uint8_t res = fat_device_is_ready(d)?0:2; //STA_NODISK
	res += (d->we?0:4); //STA_PROTECT
	return res;
}

uint8_t fat_device_read(uint8_t pdrv, uint8_t* buf, uint32_t sector, uint16_t count)
{
	BlockDevice_t* d = fat_device_get(pdrv);
	if (!fat_device_is_valid(d)) return 0;
	if (!fat_device_is_ready(d)) return 0;

	uint8_t result = iodevice_read(d, buf, sector, count);

	FFSD_LOG("fat_device_read, sector: %u; result: %u\r\n", sector, result);

	return result;
}

uint8_t fat_device_write(uint8_t pdrv, uint8_t* buf, uint32_t sector, uint16_t count)
{
	BlockDevice_t* d = fat_device_get(pdrv);
	if (!fat_device_is_valid(d)) return 0;
	if (!fat_device_is_ready(d)) return 0;

	uint8_t result = iodevice_write(d, buf, sector, count);

	FFSD_LOG("fat_device_write, sector: %u; result: %u\r\n", sector, result);

	return result;
}

uint8_t fat_device_ioctl(uint8_t pdrv, uint8_t cmd, void* buf)
{
	BlockDevice_t* d = fat_device_get(pdrv);
	if (!fat_device_is_ready(d)) return FAT_RES_NOTRDY;

	uint8_t res = FAT_RES_OK;

	switch(cmd)
	{
		case FAT_CMD_CTRL_SYNC:
		{
			//TODO: realize later (disk buffer)
			break;
		}
		case FAT_CMD_GET_BLOCK_SIZE:
		case FAT_CMD_GET_SECTOR_SIZE:
		{
			*((uint32_t*)buf) = iodevice_get_block_size(d);
			break;
		}
		case FAT_CMD_GET_SECTOR_COUNT:
		{
			*((uint32_t*)buf) = (uint16_t)iodevice_get_block_count(d);
			break;
		}
		case FAT_CMD_CTRL_TRIM:
		{
			//TODO: realize later
			break;
		}
		default:
			res = FAT_RES_PARERR;
	}

	return res;
}
