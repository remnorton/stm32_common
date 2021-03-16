/*
 * lcd_gpio.h
 *
 *  Created on: 10 но€б. 2018 г.
 *      Author: Rem Norton
 */

#ifndef LCD_LIB_LCD_GPIO_H_
#define LCD_LIB_LCD_GPIO_H_
#include "dsp_driver.h"


typedef struct
{
	DSP_GPIO_PinDef_t	reset;
	DSP_GPIO_PinDef_t	rs;
	DSP_GPIO_PinDef_t	cs;
	DSP_GPIO_PinDef_t	rd;
	DSP_GPIO_PinDef_t	wr;
	DSP_GPIO_PinDef_t	bl;
}LCD_GPIO_Pins_t;


typedef struct
{
	GPIO_TypeDef* 		dataPort;
	LCD_GPIO_Pins_t*	pins;
	LCD_BlPWM_t*	bl;
	uint16_t			width;
	uint16_t			height;
}LCD_GPIO_Type_t;


#define PIN_HI(GPIOx, GPIO_Pin)		(GPIOx->BSRR = GPIO_Pin)
#define PIN_LO(GPIOx, GPIO_Pin)		(GPIOx->BSRR = (uint32_t)GPIO_Pin << 16U)
#define LCD_SET_PIN(x)		(x.activeState == GPIO_PIN_RESET)?PIN_LO(x.port, x.pin):PIN_HI(x.port, x.pin)
#define LCD_CLEAR_PIN(x)	(x.activeState == GPIO_PIN_RESET)?PIN_HI(x.port, x.pin):PIN_LO(x.port, x.pin)

//#define LCD_SET_PIN(x)		HAL_GPIO_WritePin(x.port, x.pin, x.activeState)
//#define LCD_CLEAR_PIN(x)	HAL_GPIO_WritePin(x.port, x.pin, !x.activeState)

void lcd_reset(LCD_GPIO_Pins_t* pins);
uint8_t lcd_write_reg(LCD_GPIO_Type_t* lcd, DSP_LCDReg_t reg);
uint16_t lcd_read_io_data(LCD_GPIO_Type_t* lcd);
uint8_t lcd_read_reg(LCD_GPIO_Type_t* lcd, DSP_LCDReg_t* reg);
uint8_t lcd_write_multi_reg(LCD_GPIO_Type_t* lcd, DSP_LCDReg_t* reg, uint16_t count);
uint8_t lcd_write_data(LCD_GPIO_Type_t* lcd, uint8_t* data, uint32_t size);
uint8_t lcd_fill_data(LCD_GPIO_Type_t* lcd, uint16_t data, uint32_t count);
uint8_t lcd_write_io_reg(LCD_GPIO_Type_t* lcd, uint8_t reg);


#endif /* LCD_LIB_LCD_GPIO_H_ */
