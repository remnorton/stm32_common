/*
 * iodevice.c
 *
 *  Created on: 19 но€б. 2019 г.
 *      Author: User
 */
#include "iodevice.h"

#define IS_DEVICE_TYPE(x)		(x <= IODT_FATFS)

BlockDevice_t*	iodevice_create(uint8_t type, uint8_t we, void* device, BlockDeviceDriver_t* driver)
{
	if (!driver) return 0;
	if (!device) return 0;
	if (!IS_DEVICE_TYPE(type)) return 0;
	if (!driver->init) return 0;
	if (!driver->init(device)) return 0;

	BlockDevice_t* res = (BlockDevice_t*)malloc(sizeof(BlockDevice_t));

	if (!res)
	{
		if (driver->deinit) driver->deinit(device);
		return 0;
	}
	res->driver = driver;
	res->device = device;
	res->type = type;
	res->we = we;

	return res;
}

void iodevice_release(BlockDevice_t* device)
{
	if (!device) return;
	if (!device->driver) return;

	if (device->driver->deinit) device->driver->deinit(device->device);
	free(device);
}

uint32_t iodevice_get_block_size(BlockDevice_t* device)
{
	if (!device) return 0;
	if (!device->driver) return 0;
	if (!device->driver->get_block_size) return 0;
	return device->driver->get_block_size(device->device);
}

uint16_t iodevice_get_block_count(BlockDevice_t* device)
{
	if (!device) return 0;
	if (!device->driver) return 0;
	if (!device->driver->get_block_count) return 0;
	return device->driver->get_block_count(device->device);
}

uint8_t iodevice_read(BlockDevice_t* device, uint8_t* buf, uint32_t blk_addr, uint16_t blk_len)
{
	if (!device) return 0;
	if (!device->driver) return 0;

	if (!device->driver->read_data) return 0;
	return device->driver->read_data(device->device, buf, blk_addr, blk_len);
}

uint8_t iodevice_write(BlockDevice_t* device, uint8_t* buf, uint32_t blk_addr, uint16_t blk_len)
{
	if (!device) return 0;
	if (!device->driver) return 0;
	if (!device->we) return 0;

	if (!device->driver->write_data) return 0;
	return device->driver->write_data(device->device, buf, blk_addr, blk_len);
}

uint8_t iodevice_is_ready(BlockDevice_t* device)
{
	if (!device) return 0;
	if (!device->driver) return 0;
	if (!device->driver->is_ready) return 0;
	return device->driver->is_ready(device->device);
}
