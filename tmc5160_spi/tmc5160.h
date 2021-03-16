/*
 * tmc5160.h
 *
 *  Created on: 1 мар. 2021 г.
 *      Author: Rem Norton
 */

#ifndef TMC5160_SPI_TMC5160_H_
#define TMC5160_SPI_TMC5160_H_

#include "tmc5160_types.h"
#include "tmc5160_const.h"

void tmc5160_default_settings(TMC5160_t* drive);

uint8_t	tmc5160_init(TMC5160_t* drive);
uint8_t tmc5160_read_reg(TMC5160_t* drive, uint8_t reg_addr, uint32_t* reg_value);
uint8_t tmc5160_write_reg(TMC5160_t* drive, uint8_t reg_addr, uint32_t reg_value);
uint8_t	tmc5160_apply_settings(TMC5160_t* drive);

#endif /* TMC5160_SPI_TMC5160_H_ */
