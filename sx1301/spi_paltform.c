/*
 * spi_paltform.c
 *
 *  Created on: 3 окт. 2019 г.
 *      Author: User
 */
#include "spi_platform.h"
#include "loragw_spi.h"
#include "loragw_aux.h"

static SPI_t* spi = 0;

#define READ_ACCESS     0x00
#define WRITE_ACCESS    0x80

//
//private members
//

//#define DELAY_MS(x)		{for (int i = 0; i < x; i++);}


void on_pin(GPIO_Pin_t* pin)
{
	HAL_GPIO_WritePin(pin->port, pin->pin, pin->active_state);
}

void off_pin(GPIO_Pin_t* pin)
{
	HAL_GPIO_WritePin(pin->port, pin->pin, !pin->active_state);
}

//
//
//


void platform_spi_init(SPI_t* instance)
{
	if (!instance) return;
	if (!instance->spi) return;
	spi = instance;
	off_pin(&spi->cs);
	on_pin(&spi->rst);
	HAL_Delay(5);
	off_pin(&spi->rst);
	HAL_Delay(5);
}

int lgw_spi_open(void **spi_target_ptr)
{
	if (!spi) return LGW_SPI_ERROR;
	*spi_target_ptr = spi;

	return LGW_SPI_SUCCESS;
}

int lgw_spi_close(void *spi_target)
{
	if (spi_target != spi) return LGW_SPI_ERROR;
	HAL_SPI_Abort(spi->spi);
	return LGW_SPI_SUCCESS;
}

int lgw_spi_w(void *spi_target, uint8_t spi_mux_mode, uint8_t spi_mux_target, uint8_t address, uint8_t data)
{
	SPI_t* spi_dev = (SPI_t*)spi_target;
	if (!spi_dev) return LGW_SPI_ERROR;

	uint8_t out_buf[3] = {0};
	uint8_t command_size = 0;

	if (spi_mux_mode == LGW_SPI_MUX_MODE1)
	{
		out_buf[0] = spi_mux_target;
		out_buf[1] = WRITE_ACCESS | (address & 0x7F);
		out_buf[2] = data;
		command_size = 3;
	}
	else
	{
		out_buf[0] = WRITE_ACCESS | (address & 0x7F);
		out_buf[1] = data;
		command_size = 2;
	}

	on_pin(&spi_dev->cs);
	HAL_StatusTypeDef res = HAL_SPI_Transmit(spi_dev->spi, out_buf, command_size, 5);
	off_pin(&spi_dev->cs);

	return (res == HAL_OK)?LGW_SPI_SUCCESS:LGW_SPI_ERROR;
}

int lgw_spi_r(void *spi_target, uint8_t spi_mux_mode, uint8_t spi_mux_target, uint8_t address, uint8_t *data)
{
	SPI_t* spi_dev = (SPI_t*)spi_target;
	if (!spi_dev) return LGW_SPI_ERROR;

	uint8_t out_buf[2] = {0};
	uint8_t in_buf[2] = {0};
	uint8_t command_size = 0;

    if (spi_mux_mode == LGW_SPI_MUX_MODE1)
    {
        out_buf[0] = spi_mux_target;
        out_buf[1] = READ_ACCESS | (address & 0x7F);
        command_size = 2;
    }
    else
    {
        out_buf[0] = READ_ACCESS | (address & 0x7F);
        command_size = 1;
    }

    on_pin(&spi_dev->cs);
    HAL_StatusTypeDef res = HAL_SPI_Transmit(spi_dev->spi, out_buf, command_size, 10);
    if (res == HAL_OK) res = HAL_SPI_Receive(spi_dev->spi, in_buf, command_size, 20);
    off_pin(&spi_dev->cs);

    if (res == HAL_OK)
    	*data = in_buf[command_size-1];

    return (res == HAL_OK)?LGW_SPI_SUCCESS:LGW_SPI_ERROR;
}

int lgw_spi_wb(void *spi_target, uint8_t spi_mux_mode, uint8_t spi_mux_target, uint8_t address, uint8_t *data, uint16_t size)
{
	SPI_t* spi_dev = (SPI_t*)spi_target;
	if (!spi_dev) return LGW_SPI_ERROR;

	uint8_t command[2] = {0};
	uint8_t command_size;
	int size_to_do, chunk_size, offset;
	int i;

	if (spi_mux_mode == LGW_SPI_MUX_MODE1)
	{
		command[0] = spi_mux_target;
		command[1] = WRITE_ACCESS | (address & 0x7F);
		command_size = 2;
	}
	else
	{
		command[0] = WRITE_ACCESS | (address & 0x7F);
		command_size = 1;
	}
	size_to_do = size;

	HAL_StatusTypeDef res = HAL_OK;

	on_pin(&spi_dev->cs);
	res = HAL_SPI_Transmit(spi_dev->spi, command, command_size, 20);
	for (i=0; (size_to_do > 0) && (res == HAL_OK); ++i)
	{

		chunk_size = (size_to_do < LGW_BURST_CHUNK) ? size_to_do : LGW_BURST_CHUNK;
		offset = i * LGW_BURST_CHUNK;
		res = HAL_SPI_Transmit(spi_dev->spi, data+offset, chunk_size, chunk_size*3/100+20);
		size_to_do -= chunk_size;
	}
	off_pin(&spi_dev->cs);


	return (res == HAL_OK)?LGW_SPI_SUCCESS:LGW_SPI_ERROR;

}

int lgw_spi_rb(void *spi_target, uint8_t spi_mux_mode, uint8_t spi_mux_target, uint8_t address, uint8_t *data, uint16_t size)
{
	SPI_t* spi_dev = (SPI_t*)spi_target;
	if (!spi_dev) return LGW_SPI_ERROR;

	uint8_t command[2] = {0};
	uint8_t command_size;
	int size_to_do, chunk_size, offset;
	int i;

	if (spi_mux_mode == LGW_SPI_MUX_MODE1)
	{
		command[0] = spi_mux_target;
		command[1] = READ_ACCESS | (address & 0x7F);
		command_size = 2;
	}
	else
	{
		command[0] = READ_ACCESS | (address & 0x7F);
		command_size = 1;
	}
	size_to_do = size;

	int res = HAL_OK;

	on_pin(&spi_dev->cs);
	res = HAL_SPI_Transmit(spi_dev->spi, command, command_size, 20);
	for (i=0; (size_to_do > 0) && (res == HAL_OK); ++i)
	{
		chunk_size = (size_to_do < LGW_BURST_CHUNK) ? size_to_do : LGW_BURST_CHUNK;
		offset = i * LGW_BURST_CHUNK;
		res = HAL_SPI_Receive(spi_dev->spi, data+offset, chunk_size, chunk_size*3/100+20);
		size_to_do -= chunk_size;
	}
	off_pin(&spi_dev->cs);

	return (res == LGW_SPI_SUCCESS)?LGW_SPI_SUCCESS:LGW_SPI_ERROR;
}


