/*
 * tmc5160_const.h
 *
 *  Created on: 3 мар. 2021 г.
 *      Author: Rem Norton
 */

#ifndef TMC5160_SPI_TMC5160_CONST_H_
#define TMC5160_SPI_TMC5160_CONST_H_
#include "board.h"

#ifndef ENABLE
#define ENABLE		1
#endif

#ifndef DISABLE
#define DISABLE		0
#endif


//!Registers

#define REG_GCONF			0x00
#define REG_GSTAT			0x01
#define REG_IHOLD_IRUN		0x10
#define REG_TPOWER_DOWN		0x11
#define REG_TPWMTHRS		0x13
#define REG_CHOPCONF		0x6c
#define REG_PWM_CONF		0x70

//#define REG_

//! Microstep resolution
enum
{
	CHOP_MRES_256 = 0,
	CHOP_MRES_128,
	CHOP_MRES_64,
	CHOP_MRES_32,
	CHOP_MRES_16,
	CHOP_MRES_8,
	CHOP_MRES_4,
	CHOP_MRES_2,
	CHOP_MRES_1
};

#endif /* TMC5160_SPI_TMC5160_CONST_H_ */
