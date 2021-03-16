/*
 * w25q.c
 *
 *  Created on: 7 но€б. 2019 г.
 *      Author: User
 */


#include "w25q.h"
#include <stdlib.h>
#include <string.h>

#define MAN_ID				0xEF

#define SEC_ERASE_TIME		500
#define BLK_ERASE_TIME		2000
#define MAS_ERASE_TIME		10000
#define READ_CHUNK_SIZE		16

//registers
#define W25Q_REG_WRITE		0x02
#define W25Q_REG_READ		0x03
#define W25Q_REG_WEN		0x06
#define W25Q_REG_WDIS		0x04
#define W25Q_REG_FAST_READ	0x0B
#define W25Q_REG_SEC_ERS	0x20
#define W25Q_REG_ERS_32		0x52
#define W25Q_REG_MAS_ERS	0x60
#define W25Q_REG_MAN_ID		0x90
#define W25Q_REG_JEDEC		0x9F
#define W25Q_REG_PWDOWN		0xB9
#define W25Q_REG_PWUP		0xAB
#define W25Q_REG_UID		0x4B
#define W25Q_REG_ERS_64		0xD8




//macros
#define IS_MODEL(x)		(((x >= W25Q40) && (x <= W25Q128))?1:0)
#define SET_PIN(x)		HAL_GPIO_WritePin(x.port, x.pin, x.actState)
#define RESET_PIN(x)	HAL_GPIO_WritePin(x.port, x.pin, !x.actState)

//constant

//
//Private forwards
//

uint32_t calc_capa_by_model(uint8_t model_id);
void populate_details(uint8_t model_id, W25Q_Details_t* details);
void switch_power(W25Q_t* i, uint8_t enable);
uint8_t wait_for_free(W25Q_t* i, uint32_t timeout);

//
//Public members
//
W25Q_Result_t w25q_init(W25Q_t* init)
{
	if (!init) return W25Q_Error;
	if (!init->spi) return W25Q_Error;
	if (init->detail) return W25Q_Error;

	//Turn CS off
	HAL_GPIO_WritePin(init->cs.port, init->cs.pin, !init->cs.actState);

	//read manufacturer ID, type and capacity
	uint8_t	buf[4] = {W25Q_REG_MAN_ID, 0, 0, 0};
	SET_PIN(init->cs);
	HAL_StatusTypeDef res = HAL_SPI_Transmit(init->spi, buf, sizeof(buf), 10);
	if (res == HAL_OK) res = HAL_SPI_Receive(init->spi, buf, 2, 10);
	RESET_PIN(init->cs);

	if ((res != HAL_OK) || (buf[0] != MAN_ID) || (!IS_MODEL(buf[1]))) return W25Q_Error;


	init->detail = (W25Q_Details_t*)malloc(sizeof(W25Q_Details_t));
	if (!init->detail) return W25Q_Error;

	populate_details(buf[1], init->detail);

	return W25Q_Ok;
}

W25Q_Result_t w25q_deinit(W25Q_t* i)
{
	if (!i) return W25Q_Error;

	//power down
	switch_power(i, 0);

	if (i->detail)
	{
		free(i->detail);
		i->detail = 0;
	}

	return W25Q_Ok;
}

uint64_t w25q_uid(W25Q_t* i)
{
	if (!i) return 0;
	if (!i->detail) return 0;

	uint64_t uid = 0;
	uint8_t buf[5] = {W25Q_REG_UID, 0, 0, 0, 0};
	SET_PIN(i->cs);
	HAL_StatusTypeDef res =  HAL_SPI_Transmit(i->spi, buf, sizeof(buf), 10);
	if (res == HAL_OK) res = HAL_SPI_Receive(i->spi, (uint8_t*)&uid, sizeof(uid), 10);
	RESET_PIN(i->cs);

	if (res != HAL_OK) uid = 0;
	return uid;
}

Status_Reg_t w25q_get_status(W25Q_t* i)
{
	Status_Reg_t sr;
	memset(&sr, 0xff, sizeof(sr));
	uint8_t sv[2] = {0,0};

	if (!i) return sr;
	if (!i->detail) return sr;

	uint8_t cmd = 0x05;
	SET_PIN(i->cs);
	HAL_StatusTypeDef res =  HAL_SPI_Transmit(i->spi, &cmd, 1, 10);
	if (res == HAL_OK) res = HAL_SPI_Receive(i->spi, sv, 1, 10);
	RESET_PIN(i->cs);

	if (res != HAL_OK) return sr;

	cmd = 0x35;
	SET_PIN(i->cs);
	res =  HAL_SPI_Transmit(i->spi, &cmd, 1, 10);
	if (res == HAL_OK) res = HAL_SPI_Receive(i->spi, &sv[1], 1, 10);
	RESET_PIN(i->cs);

	if (res != HAL_OK) return sr;
	memcpy(&sr, sv, sizeof(sr));

	return sr;
}

W25Q_Result_t w25q_enable_write(W25Q_t* i, uint8_t enable)
{
	if (!i) return W25Q_Error;
	//if (!wait_for_free(i, 10)) return W25Q_Error;
	if (w25q_get_status(i).busy) return W25Q_Error;
	uint8_t cmd = enable?W25Q_REG_WEN:W25Q_REG_WDIS;

	SET_PIN(i->cs);
	HAL_StatusTypeDef res = HAL_SPI_Transmit(i->spi, &cmd, 1, 10);
	RESET_PIN(i->cs);

	if (res != HAL_OK) return W25Q_Error;

	Status_Reg_t sr = w25q_get_status(i);
	if (sr.wel != enable)  return W25Q_Error;

	 return W25Q_Ok;
}

W25Q_Result_t w25q_write(W25Q_t* i, uint32_t addr, void* data, uint16_t len)
{
	if (!i) return W25Q_Error;
	//if (!wait_for_free(i, 10)) return W25Q_Error;
	if (w25q_get_status(i).busy) return W25Q_Error;

	uint8_t* dt = (uint8_t*)data;

	Status_Reg_t sr;
	while (len)
	{
		uint16_t btw = (len > 256)?256:len;

		if (w25q_enable_write(i, 1) != W25Q_Ok) return W25Q_Error;

		uint8_t cmd[4] = {W25Q_REG_WRITE, (addr & 0xff0000) >> 16, (addr & 0xff00) >> 8, addr & 0xff};
		SET_PIN(i->cs);
		HAL_StatusTypeDef res = HAL_SPI_Transmit(i->spi, cmd, sizeof(cmd), 10);
		if (res == HAL_OK) res = HAL_SPI_Transmit(i->spi, dt, btw, btw*3/100+10);
		RESET_PIN(i->cs);

		if (res != HAL_OK) return W25Q_Error;

		sr = w25q_get_status(i);
		uint32_t deadtime = HAL_GetTick() + btw + 10;
		while (sr.busy && (HAL_GetTick() < deadtime)) sr = w25q_get_status(i);

		if (sr.busy || sr.wel) return W25Q_Error;
		dt += btw;
		addr += btw;
		len -= btw;

	}
	return (sr.busy || sr.wel)?W25Q_Error:W25Q_Ok;
}

W25Q_Result_t w25q_read(W25Q_t* i, uint32_t addr, void* data, uint16_t len)
{
	if (!i) return W25Q_Error;
	//if (!wait_for_free(i, 10)) return W25Q_Error;
	if (w25q_get_status(i).busy) return W25Q_Error;

	uint8_t* dt = (uint8_t*)data;
	HAL_StatusTypeDef res;
	while(len)
	{
		uint16_t btr = (len > 256)?256:len;

		uint8_t cmd[4] = {W25Q_REG_FAST_READ, (addr & 0xff0000) >> 16, (addr & 0xff00) >> 8, addr & 0xff};
		SET_PIN(i->cs);
		res = HAL_SPI_Transmit(i->spi, cmd, sizeof(cmd), 10);
		//dummy read
		res = HAL_SPI_Receive(i->spi, dt, 1, 10);
		//main read
		if (res == HAL_OK) res = HAL_SPI_Receive(i->spi, dt, btr, btr*3/100 + 10);
		RESET_PIN(i->cs);

		if (res != HAL_OK) return W25Q_Error;
		addr += btr;
		dt += btr;
		len -= btr;
	}
	return (res == HAL_OK)?W25Q_Ok:W25Q_Error;
}

W25Q_Result_t w25q_sector_is_clear(W25Q_t* i, uint16_t sec_num)
{
	if (!i) return W25Q_Error;
	//if (!wait_for_free(i, 10)) return W25Q_Error;
	if (w25q_get_status(i).busy) return W25Q_Error;

	uint32_t addr = ((uint32_t)sec_num)*i->detail->sector_size;
	uint32_t len = i->detail->sector_size;
	uint8_t temp[256] = {0};
	while (len)
	{
		W25Q_Result_t res = w25q_read(i, addr, temp, sizeof(temp));
		if (res != W25Q_Ok) return res;
		for (int i = 0; i < sizeof(temp); i++)
			if (temp[i] != 0xff) return W25Q_Error;
		//addr += sizeof(temp);
		//len -= sizeof(temp);
		len = 0;
	}
	return W25Q_Ok;
}

W25Q_Result_t w25q_erase_sector(W25Q_t* i, uint16_t sector)
{
	if (!i) return W25Q_Error;
	if (i->detail->sector_count <= sector) return W25Q_Error;
	if (w25q_enable_write(i, 1) != W25Q_Ok) return W25Q_Error;

	//calc address
	uint32_t addr = ((uint32_t)i->detail->sector_size)*sector;
	uint8_t cmd[4] = {W25Q_REG_SEC_ERS, (addr & 0xff0000) >> 16, (addr & 0xff00) >> 8, addr & 0xff};

	HAL_StatusTypeDef res;
	Status_Reg_t sr;

	uint8_t try_count = 10;

	do
	{
		SET_PIN(i->cs);
		res = HAL_SPI_Transmit(i->spi, cmd, sizeof(cmd), 10);
		RESET_PIN(i->cs);
		if (res == HAL_OK) sr = w25q_get_status(i);
		else for (int i = 0; i < 1000; i++);
		try_count--;
	}while(try_count && (res != HAL_OK) && (!sr.busy));

	if (res != HAL_OK) return W25Q_Error;

	uint32_t deadtime = HAL_GetTick() + SEC_ERASE_TIME;


	while (sr.busy && (HAL_GetTick() < deadtime))
	{
		sr = w25q_get_status(i);
		if (sr.busy) for (int i = 0; i < 1000; i++);
	}
	return sr.busy?W25Q_Error:W25Q_Ok;
}

W25Q_Result_t w25q_erase_block(W25Q_t* i, uint16_t block)
{
	if (!i) return W25Q_Error;
	if (i->detail->block_count <= block) return W25Q_Error;
	if (w25q_enable_write(i, 1) != W25Q_Ok) return W25Q_Error;

	uint32_t addr = ((uint32_t)i->detail->block_size) * block * 1024;
	uint8_t cmd[4] = {0, (addr & 0xff0000) >> 16, (addr & 0xff00) >> 8, addr & 0xff};
	cmd[0] = (i->detail->block_size > 32)?W25Q_REG_ERS_64:W25Q_REG_ERS_32;

	SET_PIN(i->cs);
	HAL_StatusTypeDef res = HAL_SPI_Transmit(i->spi, cmd, sizeof(cmd), 10);
	RESET_PIN(i->cs);

	if (res != HAL_OK) return W25Q_Error;

	uint32_t deadtime = HAL_GetTick() + BLK_ERASE_TIME;

	Status_Reg_t sr = w25q_get_status(i);
	while (sr.busy && (HAL_GetTick() < deadtime))
	{
		//HAL_Delay(1);
		sr = w25q_get_status(i);
	}

	return sr.busy?W25Q_Error:W25Q_Ok;
}

W25Q_Result_t w25q_mass_erase(W25Q_t* i)
{
	if (!i) return W25Q_Error;
	if (w25q_enable_write(i, 1) != W25Q_Ok) return W25Q_Error;
	uint8_t cmd = W25Q_REG_MAS_ERS;

	SET_PIN(i->cs);
	HAL_StatusTypeDef res = HAL_SPI_Transmit(i->spi, &cmd, sizeof(cmd), 10);
	RESET_PIN(i->cs);

	if (res != HAL_OK) return W25Q_Error;

	uint32_t deadtime = HAL_GetTick() + MAS_ERASE_TIME;

	Status_Reg_t sr = w25q_get_status(i);
	while (sr.busy && (HAL_GetTick() < deadtime))
	{
		//HAL_Delay(1);
		sr = w25q_get_status(i);
	}

	return sr.busy?W25Q_Error:W25Q_Ok;
}

uint16_t w25q_sector_num(W25Q_t* i, uint32_t addr)
{
	if (!i) return 0;
	return addr/(i->detail->sector_size);
}

uint16_t w25q_block_num(W25Q_t* i, uint32_t addr)
{
	if (!i) return 0;
	return addr/(i->detail->block_size*1024);
}

//
//Private members
//

uint32_t calc_capa_by_model(uint8_t model_id)
{
	uint32_t res = 512*1024;
	for (int i = W25Q40; i < model_id; i++)
		res *= 2;
	return res;
}

void populate_details(uint8_t model_id, W25Q_Details_t* details)
{
	details->model = model_id;
	details->sector_size = 4*1024;
	details->capacity = calc_capa_by_model(model_id);
	details->addr_size = 3;

	switch(model_id)
	{
		case W25Q40:
		case W25Q80:
		case W25Q16:
		case W25Q32:
		{
			details->block_size = 32*1024;
			break;
		}
		case W25Q64:
		case W25Q128:
		{
			details->block_size = 64*1024;
			break;
		}
	}
	details->sector_count = details->capacity / details->sector_size;
	details->block_count = details->capacity / details->block_size;

}

void switch_power(W25Q_t* i, uint8_t enable)
{
	uint8_t cmd = enable?W25Q_REG_PWUP:W25Q_REG_PWDOWN;
	SET_PIN(i->cs);
	HAL_SPI_Transmit(i->spi, &cmd, 1, 10);
	RESET_PIN(i->cs);
	if (enable) HAL_Delay(2);
}

uint8_t wait_for_free(W25Q_t* i, uint32_t timeout)
{
	if (!i) return 0;
	if (!i->detail) return 0;
	uint32_t deadtime = HAL_GetTick() + timeout;
	Status_Reg_t sr;
	do { sr = w25q_get_status(i); }while (sr.busy && (deadtime < HAL_GetTick()));
	return !sr.busy;
}
