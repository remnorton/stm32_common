/*
 * mfrc522.c
 *
 *  Created on: 22 џэт. 2020 у.
 *      Author: User
 */
#include "mfrc522.h"

//
//Registers
//

#define	MFRC522_REG_VERSION 			0x37

//
//Private forwards
//
uint8_t mfrc522_read_reg(MFRC522_t* i, uint8_t reg_addr);
void mfrc522_write_reg(MFRC522_t* i, uint8_t reg_addr, uint8_t data);

//
//Public members
//

uint8_t mfrc522_init(MFRC522_t* i)
{
	if (!i) return 0;
	if (!i->spi) return 0;

	mfrc522_set_pin(&i->reset);
	mfrc522_delay_ms(10);
	mfrc522_reset_pin(&i->reset);
	mfrc522_delay_ms(50);

	uint8_t ver = mfrc522_read_reg(i, MFRC522_REG_VERSION);
	if (((ver&0xF0) >> 4) != 9) return 0;

	return 1;
}

//
//Private members
//

uint8_t mfrc522_read_reg(MFRC522_t* i, uint8_t reg_addr)
{
	if (!i) return 0;
	if (reg_addr > 0x3F) return 0;
	uint8_t result = 0;

	uint8_t d = (reg_addr << 1) | 0x80;

	mfrc522_set_pin(&i->cs);
	uint8_t res = mfrc522_spi_transmit(i->spi, &d, 1, 50);
	if (res) mfrc522_spi_receive(i->spi, &result, 1, 50);
	mfrc522_reset_pin(&i->cs);

	if (!res) return 0;
	return result;
}

void mfrc522_write_reg(MFRC522_t* i, uint8_t reg_addr, uint8_t data)
{
	if (!i) return;
	if (reg_addr > 0x3F) return;
	uint8_t d[2] = {(reg_addr << 1), data};
	mfrc522_set_pin(&i->cs);
	mfrc522_spi_transmit(i->spi, d, sizeof(d), 50);
	mfrc522_reset_pin(&i->cs);
}
