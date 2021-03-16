/*
 * lcd_gpio.c
 *
 *  Created on: 10 но€б. 2018 г.
 *      Author: Rem Norton
 */

#include "lcd_gpio.h"

//
//private defines
//


//
//Public members
//

void lcd_reset(LCD_GPIO_Pins_t* pins)
{
	if (!pins) return;
	LCD_SET_PIN(pins->reset);
	HAL_Delay(10);
	LCD_CLEAR_PIN(pins->reset);
}

uint8_t lcd_write_reg(LCD_GPIO_Type_t* lcd, DSP_LCDReg_t reg)
{
	if (!lcd) return 0;

	LCD_SET_PIN(lcd->pins->cs);
	LCD_CLEAR_PIN(lcd->pins->rs);
	LCD_CLEAR_PIN(lcd->pins->rd);
	lcd->dataPort->ODR = reg.reg_num;
	LCD_SET_PIN(lcd->pins->wr);
	LCD_CLEAR_PIN(lcd->pins->wr);

	LCD_SET_PIN(lcd->pins->rs);
	lcd->dataPort->ODR = reg.reg_data;
	LCD_SET_PIN(lcd->pins->wr);
	LCD_CLEAR_PIN(lcd->pins->wr);
	LCD_CLEAR_PIN(lcd->pins->cs);

	return 1;
}

uint16_t lcd_read_io_data(LCD_GPIO_Type_t* lcd)
{
	uint16_t res = 0;

	LCD_SET_PIN(lcd->pins->rs);
	LCD_SET_PIN(lcd->pins->rd);
	lcd->dataPort->CRH = 0x44444444;    /* Reset GPIO [15:8] */
	lcd->dataPort->CRL = 0x44444444;    /* Reset GPIO [7:0] */
	res = lcd->dataPort->IDR;
	lcd->dataPort->CRH = 0x33333333;    /* Set GPIO [15:8]: Output mode, max speed 50 MHz. */
	lcd->dataPort->CRL = 0x33333333;    /* Set GPIO [15:8]: Output mode, max speed 50 MHz. */
	LCD_CLEAR_PIN(lcd->pins->rd);
	LCD_CLEAR_PIN(lcd->pins->cs);

	return res;
}

uint8_t lcd_read_reg(LCD_GPIO_Type_t* lcd, DSP_LCDReg_t* reg)
{

	if (!reg) return 0;

	LCD_SET_PIN(lcd->pins->cs);
	LCD_CLEAR_PIN(lcd->pins->rs);
	LCD_CLEAR_PIN(lcd->pins->rd);
	lcd->dataPort->ODR = reg->reg_num;
	LCD_SET_PIN(lcd->pins->wr);
	LCD_CLEAR_PIN(lcd->pins->wr);

	reg->reg_data = lcd_read_io_data(lcd);

	return 1;
}

uint8_t lcd_write_multi_reg(LCD_GPIO_Type_t* lcd, DSP_LCDReg_t* reg, uint16_t count)
{
	if(!lcd) return 0;
	LCD_SET_PIN(lcd->pins->cs);
	for(int i = 0; i < count; i++)
	{
		LCD_CLEAR_PIN(lcd->pins->rs);
		LCD_CLEAR_PIN(lcd->pins->rd);
		lcd->dataPort->ODR = reg[i].reg_num;
		LCD_SET_PIN(lcd->pins->wr);
		LCD_CLEAR_PIN(lcd->pins->wr);

		LCD_SET_PIN(lcd->pins->rs);
		lcd->dataPort->ODR = reg[i].reg_data;
		LCD_SET_PIN(lcd->pins->wr);
		LCD_CLEAR_PIN(lcd->pins->wr);
	}
	LCD_CLEAR_PIN(lcd->pins->cs);
	return 1;
}


uint8_t lcd_write_data(LCD_GPIO_Type_t* lcd, uint8_t* data, uint32_t size)
{
	if (!lcd) return 0;
	if (size%2) return 0;
	while(size)
	{

		LCD_SET_PIN(lcd->pins->cs);
		LCD_SET_PIN(lcd->pins->rs);
		lcd->dataPort->ODR = *(uint16_t*)data;
		LCD_SET_PIN(lcd->pins->wr);
		LCD_CLEAR_PIN(lcd->pins->wr);
		LCD_CLEAR_PIN(lcd->pins->cs);

	    data += 2;
		size -=2;
	}
	return 1;
}

uint8_t lcd_fill_data(LCD_GPIO_Type_t* lcd, uint16_t data, uint32_t count)
{
	if (!lcd) return 0;
	LCD_SET_PIN(lcd->pins->cs);
	lcd->dataPort->ODR = data;
	while(count)
	{
		LCD_SET_PIN(lcd->pins->rs);
		LCD_CLEAR_PIN(lcd->pins->rd);
		LCD_SET_PIN(lcd->pins->wr);
		LCD_CLEAR_PIN(lcd->pins->wr);
		count--;
	}
	LCD_CLEAR_PIN(lcd->pins->cs);
	return 1;
}

uint8_t lcd_write_io_reg(LCD_GPIO_Type_t* lcd, uint8_t reg)
{
  if (!lcd) return 0;

  LCD_SET_PIN(lcd->pins->cs);
  LCD_CLEAR_PIN(lcd->pins->rs);
  LCD_CLEAR_PIN(lcd->pins->rd);
  lcd->dataPort->ODR = reg;
  LCD_SET_PIN(lcd->pins->wr);
  LCD_CLEAR_PIN(lcd->pins->wr);
  LCD_CLEAR_PIN(lcd->pins->cs);

  return 1;
}


//
//private members
//

