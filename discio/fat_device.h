/*
 * fat_device.h
 *
 *  Created on: 19 но€б. 2019 г.
 *      Author: User
 */

#ifndef W25Q_FAT_DEVICE_H_
#define W25Q_FAT_DEVICE_H_
#include "iodevice.h"

uint8_t fat_device_register(uint8_t we, void* device, BlockDeviceDriver_t* driver);
uint8_t fat_device_init(uint8_t pdrv);
uint8_t fat_device_get_status(uint8_t pdrv);
uint8_t fat_device_read(uint8_t pdrv, uint8_t* buf, uint32_t sector, uint16_t count);
uint8_t fat_device_write(uint8_t pdrv, uint8_t* buf, uint32_t sector, uint16_t count);
uint8_t fat_device_ioctl(uint8_t pdrv, uint8_t cmd, void* buf);

#endif /* W25Q_FAT_DEVICE_H_ */
