/*
 * msc_device.h
 *
 *  Created on: 19 но€б. 2019 г.
 *      Author: User
 */

#ifndef W25Q_MSC_DEVICE_H_
#define W25Q_MSC_DEVICE_H_
#include "iodevice.h"

uint8_t msc_device_register(uint8_t we, void* device, BlockDeviceDriver_t* driver);
uint8_t msc_device_init(uint8_t lun);
uint8_t msc_device_get_capacity(uint8_t lun, uint32_t *block_num, uint16_t *block_size);
uint8_t msc_device_is_ready(uint8_t lun);
uint8_t msc_device_is_write_protected(uint8_t lun);
uint8_t msc_device_read(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
uint8_t msc_device_write(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);

#endif /* W25Q_MSC_DEVICE_H_ */
