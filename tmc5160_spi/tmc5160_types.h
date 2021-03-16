/*
 * tmc5160_types.h
 *
 *  Created on: 3 мар. 2021 г.
 *      Author: Rem Norton
 */

#ifndef TMC5160_SPI_TMC5160_TYPES_H_
#define TMC5160_SPI_TMC5160_TYPES_H_
#include "board.h"


typedef struct
{
	uint32_t	toff		: 4;	//Off time setting controls duration of slow decay phase NCLK= 24 + 32*TOFF
	uint32_t	hstrt		: 3;	//hysteresis start value added to HEND. Attention: Effective HEND+HSTRT ≤ 16.
	uint32_t	hend		: 4;	//hysteresis low value OFFSET sine wave offset
	uint32_t	fd3			: 1;	//TFD [3]. MSB of fast decay time setting TFD
	uint32_t	disfdcc		: 1;	//fast decay mode
	uint32_t	reserved	: 1;	//Set to 0
	uint32_t	chm			: 1;	//chopper mode (Enabled/disabled)
	uint32_t	tbl			: 2;	//blank time select
	uint32_t	reserved2	: 1;	//Set to 0
	uint32_t	vhighfs		: 1;	//high velocity fullstep selection
	uint32_t	vhighchm	: 1;	//high velocity chopper mode
	uint32_t	tpfd		: 4;	//passive fast decay time. Passive fast decay time setting controls duration of the fast decay phase inserted after bridge polarity change NCLK= 128*TPFD
	uint32_t	mres		: 4;	//micro step resolution. step width=2^MRES [microsteps]
	uint32_t	intpol		: 1;	//interpolation to 256 microsteps
	uint32_t	dedge		: 1;	//enable double edge step pulses
	uint32_t	diss2g		: 1;	//short to GND protection disable
	uint32_t	diss2vs		: 1;	//short to supply protection disable
}CHOPPER_Conf_t;

typedef struct
{
	uint32_t	ihold		: 5;	//Standstill current
	uint32_t	unused		: 3;
	uint32_t	irun		: 5;	//Motor run current
	uint32_t	unused2		: 3;
	uint32_t	ihold_delay	: 4;	//number of clock cycles for motor power down after a motion as soon as standstill is detected
}I_Conf_t;


typedef struct
{
	uint32_t	offsset		: 8;	//User defined amplitude (offset)
	uint32_t	gradient	: 8;	//User defined amplitude gradient
	uint32_t	frequency	: 2;	//PWM frequency selection
	uint32_t	autoscale	: 1;	//PWM automatic amplitude scaling
	uint32_t	autograd	: 1;	//PWM automatic gradient adaptation
	uint32_t	freewheel	: 2;	//Stand still option when motor current setting is zero
	uint32_t	reserved	: 2;	//Set to 0
	uint32_t	scale_reg	: 4;	//Regulation loop gradient when using autoscale
	uint32_t	scale_lim	: 4;	//Limit for PWM_SCALE_AUTO when switching back from SpreadCycle to StealthChop
}PWM_Conf_t;

typedef struct
{
	GPIO_TypeDef* 	port;
	uint16_t 		pin;
	GPIO_PinState 	active_state;
}TMC5160_GPIO_t;

typedef struct
{
	uint8_t reset 		:1;
	uint8_t	drv_err		:1;
	uint8_t	sg_active	:1;
	uint8_t standstill	:1;
	uint8_t	vel_reached	:1;
	uint8_t	pos_reached	:1;
	uint8_t	stop_l_ramp	:1;
	uint8_t	stop_r_ramp	:1;
}TMC5160_Status_t;

typedef struct
{
	CHOPPER_Conf_t*			chop_cfg;
	I_Conf_t*				i_cfg;
	PWM_Conf_t*				pwm_conf;
}TMC5160_Settings_t;


typedef struct
{
	SPI_HandleTypeDef*		spi;
	TMC5160_GPIO_t			cs;
	TMC5160_Status_t		status;
	TMC5160_Settings_t		settings;
}TMC5160_t;


#endif /* TMC5160_SPI_TMC5160_TYPES_H_ */
