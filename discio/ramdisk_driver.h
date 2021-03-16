/*
 * ramdisk_driver.h
 *
 *  Created on: 21 но€б. 2019 г.
 *      Author: User
 */

#ifndef W25Q_RAMDISK_DRIVER_H_
#define W25Q_RAMDISK_DRIVER_H_
#include "iodevice.h"

typedef struct
{
	void*		disk;
	uint32_t	size;
}RAMDISK_t;

extern BlockDeviceDriver_t ramdisk_driver;

#endif /* W25Q_RAMDISK_DRIVER_H_ */
