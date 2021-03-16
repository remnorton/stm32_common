/*
 * lcd_fmc.c
 *
 *  Created on: 25 сент. 2019 г.
 *      Author: User
 */
#include "lcd_fmc.h"


void lcd_fmc_reset(LCD_FMC_t* lcd)
{
	HAL_GPIO_WritePin(lcd->reset.port, lcd->reset.pin, lcd->reset.activeState);
	HAL_Delay(50);
	HAL_GPIO_WritePin(lcd->reset.port, lcd->reset.pin, !lcd->reset.activeState);
	HAL_Delay(10);
}

uint8_t lcd_fmc_write_reg(LCD_FMC_t* lcd, DSP_LCDReg_t reg)
{
	*(lcd->reg) = reg.reg_num;
	*(lcd->data) = reg.reg_data;
	return 1;
}

uint16_t lcd_fmc_read_io_data(LCD_FMC_t* lcd)
{
	return *(lcd->data);
}

uint8_t lcd_fmc_read_reg(LCD_FMC_t* lcd, DSP_LCDReg_t* reg)
{
	*(lcd->reg) = reg->reg_num;
	reg->reg_data = *(lcd->data);
	return 1;
}

uint8_t lcd_fmc_write_multi_reg(LCD_FMC_t* lcd, DSP_LCDReg_t* reg, uint16_t count)
{
	uint8_t res = 1;
	for (int i = 0; i < count; i++)
		res |= lcd_fmc_write_reg(lcd, reg[i]);
	return res;
}

uint8_t lcd_fmc_write_data(LCD_FMC_t* lcd, uint8_t* data, uint32_t size)
{
	if (size%2) return 0;
	while (size)
	{
		*(lcd->data) = *((uint16_t*)data);
		data += 2;
		size -=2;
	}
	return 1;
}

uint8_t lcd_fmc_fill_data(LCD_FMC_t* lcd, uint16_t data, uint32_t count)
{
	for (int i = 0; i < count; i++)
		*(lcd->data) = data;
	return 1;
}

uint8_t lcd_fmc_fill_data24(LCD_FMC_t* lcd, uint32_t data, uint32_t count, uint8_t msb)
{
	uint16_t hi, lo;
	if (msb)
	{
		hi = (data >> 16) & 0xff;
		lo = data & 0xffff;
	}
	else
	{
		lo = (data >> 16) & 0xff;
		hi = data & 0xffff;
	}

	for (int i = 0; i < count; i++)
	{
		*(lcd->data) = lo;
		*(lcd->data) = hi;
	}
	return 1;
}

uint8_t lcd_fmc_write_io_reg(LCD_FMC_t* lcd, uint16_t reg)
{
	if (!lcd) return 0;
	*(lcd->reg) = reg;
	return 1;
}

uint8_t lcd_fmc_write_io_data(LCD_FMC_t* lcd, uint16_t data)
{
	if (!lcd) return 0;
	*(lcd->data) = data;
	return 1;
}
