/*
 * lcd_fmc.h
 *
 *  Created on: 25 сент. 2019 г.
 *      Author: User
 */

#ifndef LCD_LIB_LCD_FMC_H_
#define LCD_LIB_LCD_FMC_H_
#include "dsp_driver.h"


typedef struct
{
	uint16_t*			reg;
	uint16_t*			data;
	DSP_GPIO_PinDef_t	reset;
	LCD_BlPWM_t*		bl;
	uint16_t			width;
	uint16_t			height;
}LCD_FMC_t;


void lcd_fmc_reset(LCD_FMC_t* lcd);
uint8_t lcd_fmc_write_reg(LCD_FMC_t* lcd, DSP_LCDReg_t reg);
uint16_t lcd_fmc_read_io_data(LCD_FMC_t* lcd);
uint8_t lcd_fmc_read_reg(LCD_FMC_t* lcd, DSP_LCDReg_t* reg);
uint8_t lcd_fmc_write_multi_reg(LCD_FMC_t* lcd, DSP_LCDReg_t* reg, uint16_t count);
uint8_t lcd_fmc_write_data(LCD_FMC_t* lcd, uint8_t* data, uint32_t size);
uint8_t lcd_fmc_fill_data(LCD_FMC_t* lcd, uint16_t data, uint32_t count);
uint8_t lcd_fmc_fill_data24(LCD_FMC_t* lcd, uint32_t data, uint32_t count, uint8_t msb);
uint8_t lcd_fmc_write_io_reg(LCD_FMC_t* lcd, uint16_t reg);
uint8_t lcd_fmc_write_io_data(LCD_FMC_t* lcd, uint16_t data);


#endif /* LCD_LIB_LCD_FMC_H_ */
