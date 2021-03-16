/*
 * iodevice.h
 *
 *  Created on: 19 но€б. 2019 г.
 *      Author: User
 */

#ifndef W25Q_IODEVICE_H_
#define W25Q_IODEVICE_H_

#include <stdint.h>
#include <stdlib.h>


//#define MSC_DEBUG
//#define FFS_DEBUG

//! type of IO device
enum
{
	IODT_NONE = 0,
	IODT_MSC,
	IODT_FATFS
};

typedef struct
{
	uint8_t			(*init)(void*);
	void			(*deinit)(void*);
	uint32_t		(*get_block_size)(void*);
	uint16_t		(*get_block_count)(void*);
	uint8_t			(*write_data)(void*, uint8_t*, uint32_t /*blk_addr*/, uint16_t /*blk_len*/);
	uint8_t			(*read_data)(void*, uint8_t*, uint32_t /*blk_addr*/, uint16_t /*blk_len*/);
	uint8_t			(*is_ready)(void*);
}BlockDeviceDriver_t;


typedef struct
{
	uint8_t					type;	//type of IO device
	uint8_t					we;		//write enable
	void*					device;	//pointer to device control structure
	BlockDeviceDriver_t*	driver;	//low-level driver
}BlockDevice_t;



BlockDevice_t*	iodevice_create(uint8_t type, uint8_t we, void* device, BlockDeviceDriver_t* driver);
void			iodevice_release(BlockDevice_t* device);
uint32_t		iodevice_get_block_size(BlockDevice_t* device);
uint16_t		iodevice_get_block_count(BlockDevice_t* device);
uint8_t			iodevice_read(BlockDevice_t* device, uint8_t* buf, uint32_t blk_addr, uint16_t blk_len);
uint8_t			iodevice_write(BlockDevice_t* device, uint8_t* buf, uint32_t blk_addr, uint16_t blk_len);
uint8_t			iodevice_is_ready(BlockDevice_t* device);
#endif /* W25Q_IODEVICE_H_ */
