/*
 * tmc5160.c
 *
 *  Created on: 1 мар. 2021 г.
 *      Author: Rem Norton
 */

#include <string.h>
#include "tmc5160.h"

#define CMD_READ		0x00
#define CMD_WRITE		0x80


#define swap32(x) (((x & 0xff) << 24) | (((x >> 8) & 0xff) << 16) | (((x >> 16) & 0xff) << 8) | ((x >> 24) & 0xff))

#pragma pack(1)
typedef struct
{
	uint8_t	 reg_num;
	uint32_t value;
}REG_t;
#pragma pack()

//Private forwards


//Public members

void tmc5160_default_settings(TMC5160_t* drive)
{
	if (!drive)
		return;
	if (!drive->settings.chop_cfg || !drive->settings.i_cfg)
		return;

	CHOPPER_Conf_t* ccf = drive->settings.chop_cfg;
	memset(ccf, 0, sizeof(CHOPPER_Conf_t));
	ccf->tbl = 2;
	ccf->toff = 3;
	ccf->hend = 1;
	ccf->hstrt = 4;
	ccf->chm = 1;


	I_Conf_t* icf = drive->settings.i_cfg;
	memset(icf, 0, sizeof(I_Conf_t));

	icf->irun = 5;
	icf->ihold = 5;
	icf->ihold_delay = 5;

	PWM_Conf_t* pwm = drive->settings.pwm_conf;
	memset(pwm, 0, sizeof(PWM_Conf_t));

	pwm->frequency = 1;
	pwm->autograd = 1;
	pwm->autoscale = 1;
	pwm->freewheel = 2;
	pwm->scale_reg = 4;
	pwm->scale_lim = 12;
	pwm->offsset = 32;
	pwm->gradient = 16;

}

uint8_t	tmc5160_init(TMC5160_t* drive)
{
	if (!drive)
		return 0;
	if (!drive->spi || !drive->cs.port)
		return 0;

	if (drive->spi->Init.DataSize != SPI_DATASIZE_8BIT)
		return 0;

	if (!drive->settings.chop_cfg || !drive->settings.i_cfg)
		return 0;

	//Turn off NSS signal
	HAL_GPIO_WritePin(drive->cs.port, drive->cs.pin, !drive->cs.active_state);

	uint8_t res = tmc5160_apply_settings(drive);

	//cleanup general status register
	res &= tmc5160_write_reg(drive, REG_GSTAT, 0x07);

	return res;

}

uint8_t tmc5160_read_reg(TMC5160_t* drive, uint8_t reg_addr, uint32_t* reg_value)
{
	REG_t cmd = {reg_addr | CMD_READ, 0x00};
	REG_t ans = {0x00, 0x00};

	HAL_GPIO_WritePin(drive->cs.port, drive->cs.pin, drive->cs.active_state);
	HAL_StatusTypeDef res = HAL_SPI_Transmit(drive->spi, (uint8_t*)&cmd, sizeof(REG_t), 2);
	HAL_GPIO_WritePin(drive->cs.port, drive->cs.pin, !drive->cs.active_state);

	HAL_GPIO_WritePin(drive->cs.port, drive->cs.pin, drive->cs.active_state);
	res &= HAL_SPI_Receive(drive->spi, (uint8_t*)&ans, sizeof(REG_t), 2);
	HAL_GPIO_WritePin(drive->cs.port, drive->cs.pin, !drive->cs.active_state);


	*((uint8_t*)&drive->status) = ans.reg_num;
	*reg_value = swap32(ans.value);
	return ((res == HAL_OK) && (ans.reg_num == cmd.reg_num))?1:0;
}

uint8_t tmc5160_write_reg(TMC5160_t* drive, uint8_t reg_addr, uint32_t reg_value)
{
	REG_t cmd = {reg_addr | CMD_WRITE, 0};
	REG_t ans = {0x00, 0x00};

	cmd.value = swap32(reg_value);

	HAL_GPIO_WritePin(drive->cs.port, drive->cs.pin, drive->cs.active_state);
	HAL_StatusTypeDef res = HAL_SPI_Transmit(drive->spi, (uint8_t*)&cmd, sizeof(REG_t), 2);
	HAL_GPIO_WritePin(drive->cs.port, drive->cs.pin, !drive->cs.active_state);

	HAL_GPIO_WritePin(drive->cs.port, drive->cs.pin, drive->cs.active_state);
	res &= HAL_SPI_Receive(drive->spi, (uint8_t*)&ans, sizeof(REG_t), 2);
	HAL_GPIO_WritePin(drive->cs.port, drive->cs.pin, !drive->cs.active_state);

	*((uint8_t*)&drive->status) = ans.reg_num;

	return ((res == HAL_OK) && (ans.value == cmd.value))?1:0;
}

uint8_t	tmc5160_apply_settings(TMC5160_t* drive)
{
	uint8_t res = 1;
	res &= tmc5160_write_reg(drive, REG_CHOPCONF, *((uint32_t*)drive->settings.chop_cfg));
	res &= tmc5160_write_reg(drive, REG_IHOLD_IRUN, *((uint32_t*)drive->settings.i_cfg)); //current
	res &= tmc5160_write_reg(drive, REG_PWM_CONF, *((uint32_t*)drive->settings.pwm_conf)); //PWM config
	res &= tmc5160_write_reg(drive, REG_TPOWER_DOWN, 0x0A); //power down delay
	res &= tmc5160_write_reg(drive, REG_GCONF, 0x04); //general configuration
	res &= tmc5160_write_reg(drive, REG_TPWMTHRS, 0x3E8); //upper velocity for StealthChop voltage PWM mode

	return res;
}

