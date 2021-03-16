/*
 * nt35510_fmc.c
 *
 *  Created on: 12 но€б. 2019 г.
 *      Author: User
 */

#include "lcd_fmc.h"


#ifndef ARRAY_LEN
#define ARRAY_LEN(x)	(sizeof(x)/sizeof(x[0]))
#endif

//
//
//

typedef struct
{
	uint16_t	wrramcmd;
	uint16_t	rdramcmd;
	uint16_t	setxcmd;
	uint16_t	setycmd;
}LCD_Dev_t;


static LCD_Dev_t lcd_dev =
{
		0x2c00,
		0x2e00,
		0x2b00,
		0x2a00,
};

//
//Private members
//

void Init_data(LCD_FMC_t* fmc, uint16_t reg, uint16_t data)
{
	lcd_fmc_write_io_reg(fmc, reg); lcd_fmc_write_io_data(fmc, data);
}

void nt35510_set_window(LCD_FMC_t* fmc, uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
{
	lcd_fmc_write_io_reg(fmc, lcd_dev.setxcmd); lcd_fmc_write_io_data(fmc, xStart>>8);
	lcd_fmc_write_io_reg(fmc, lcd_dev.setxcmd+1); lcd_fmc_write_io_data(fmc, xStart&0xFF);
	lcd_fmc_write_io_reg(fmc, lcd_dev.setxcmd+2); lcd_fmc_write_io_data(fmc, xEnd>>8);
	lcd_fmc_write_io_reg(fmc, lcd_dev.setxcmd+3); lcd_fmc_write_io_data(fmc, xEnd&0xFF);
	lcd_fmc_write_io_reg(fmc, lcd_dev.setycmd); lcd_fmc_write_io_data(fmc, yStart>>8);
	lcd_fmc_write_io_reg(fmc, lcd_dev.setycmd+1); lcd_fmc_write_io_data(fmc, yStart&0xFF);
	lcd_fmc_write_io_reg(fmc, lcd_dev.setycmd+2); lcd_fmc_write_io_data(fmc, yEnd>>8);
	lcd_fmc_write_io_reg(fmc, lcd_dev.setycmd+3); lcd_fmc_write_io_data(fmc, yEnd&0xFF);
}

void nt35510_write_gram(LCD_FMC_t* fmc)
{
	lcd_fmc_write_io_reg(fmc, lcd_dev.wrramcmd);
}

//
//Driver members
//
uint8_t nt35510_fmc_init(void* init)
{
	if (!init) return 0;
	LCD_FMC_t* fmc = (LCD_FMC_t*)init;

	lcd_fmc_reset(fmc);
	HAL_Delay(50);

	DSP_LCDReg_t reg = {0x0401,0};
	lcd_fmc_read_reg(fmc, &reg);
	if (reg.reg_data != 0x80) return 0;

//	lcd_fmc_write_io_reg(fmc, 0xF000); lcd_fmc_write_io_data(fmc, 0x55);
//	lcd_fmc_write_io_reg(fmc, 0xF001); lcd_fmc_write_io_data(fmc, 0xAA);
//	lcd_fmc_write_io_reg(fmc, 0xF002); lcd_fmc_write_io_data(fmc, 0x52);
//	lcd_fmc_write_io_reg(fmc, 0xF003); lcd_fmc_write_io_data(fmc, 0x08);
//	lcd_fmc_write_io_reg(fmc, 0xF004); lcd_fmc_write_io_data(fmc, 0x01);
//	//# AVDD: manual); lcd_fmc_write_io_data(fmc,
//	lcd_fmc_write_io_reg(fmc, 0xB600); lcd_fmc_write_io_data(fmc, 0x34);
//	lcd_fmc_write_io_reg(fmc, 0xB601); lcd_fmc_write_io_data(fmc, 0x34);
//	lcd_fmc_write_io_reg(fmc, 0xB602); lcd_fmc_write_io_data(fmc, 0x34);
//
//	lcd_fmc_write_io_reg(fmc, 0xB000); lcd_fmc_write_io_data(fmc, 0x0D);//09
//	lcd_fmc_write_io_reg(fmc, 0xB001); lcd_fmc_write_io_data(fmc, 0x0D);
//	lcd_fmc_write_io_reg(fmc, 0xB002); lcd_fmc_write_io_data(fmc, 0x0D);
//	//# AVEE: manual); lcd_fmc_write_io_data(fmc,  -6V
//	lcd_fmc_write_io_reg(fmc, 0xB700); lcd_fmc_write_io_data(fmc, 0x24);
//	lcd_fmc_write_io_reg(fmc, 0xB701); lcd_fmc_write_io_data(fmc, 0x24);
//	lcd_fmc_write_io_reg(fmc, 0xB702); lcd_fmc_write_io_data(fmc, 0x24);
//
//	lcd_fmc_write_io_reg(fmc, 0xB100); lcd_fmc_write_io_data(fmc, 0x0D);
//	lcd_fmc_write_io_reg(fmc, 0xB101); lcd_fmc_write_io_data(fmc, 0x0D);
//	lcd_fmc_write_io_reg(fmc, 0xB102); lcd_fmc_write_io_data(fmc, 0x0D);
//	//#Power Control for
//	//VCL
//	lcd_fmc_write_io_reg(fmc, 0xB800); lcd_fmc_write_io_data(fmc, 0x24);
//	lcd_fmc_write_io_reg(fmc, 0xB801); lcd_fmc_write_io_data(fmc, 0x24);
//	lcd_fmc_write_io_reg(fmc, 0xB802); lcd_fmc_write_io_data(fmc, 0x24);
//
//	lcd_fmc_write_io_reg(fmc, 0xB200); lcd_fmc_write_io_data(fmc, 0x00);
//
//	//# VGH: Clamp Enable); lcd_fmc_write_io_data(fmc,
//	lcd_fmc_write_io_reg(fmc, 0xB900); lcd_fmc_write_io_data(fmc, 0x24);
//	lcd_fmc_write_io_reg(fmc, 0xB901); lcd_fmc_write_io_data(fmc, 0x24);
//	lcd_fmc_write_io_reg(fmc, 0xB902); lcd_fmc_write_io_data(fmc, 0x24);
//
//	lcd_fmc_write_io_reg(fmc, 0xB300); lcd_fmc_write_io_data(fmc, 0x05);
//	lcd_fmc_write_io_reg(fmc, 0xB301); lcd_fmc_write_io_data(fmc, 0x05);
//	lcd_fmc_write_io_reg(fmc, 0xB302); lcd_fmc_write_io_data(fmc, 0x05);
//
//	///lcd_fmc_write_io_reg(fmc, 0xBF00); lcd_fmc_write_io_data(fmc, 0x01);
//
//	//# VGL(LVGL):
//	lcd_fmc_write_io_reg(fmc, 0xBA00); lcd_fmc_write_io_data(fmc, 0x34);
//	lcd_fmc_write_io_reg(fmc, 0xBA01); lcd_fmc_write_io_data(fmc, 0x34);
//	lcd_fmc_write_io_reg(fmc, 0xBA02); lcd_fmc_write_io_data(fmc, 0x34);
//	//# VGL_REG(VGLO)
//	lcd_fmc_write_io_reg(fmc, 0xB500); lcd_fmc_write_io_data(fmc, 0x0B);
//	lcd_fmc_write_io_reg(fmc, 0xB501); lcd_fmc_write_io_data(fmc, 0x0B);
//	lcd_fmc_write_io_reg(fmc, 0xB502); lcd_fmc_write_io_data(fmc, 0x0B);
//	//# VGMP/VGSP:
//	lcd_fmc_write_io_reg(fmc, 0xBC00); lcd_fmc_write_io_data(fmc, 0X00);
//	lcd_fmc_write_io_reg(fmc, 0xBC01); lcd_fmc_write_io_data(fmc, 0xA3);
//	lcd_fmc_write_io_reg(fmc, 0xBC02); lcd_fmc_write_io_data(fmc, 0X00);
//	//# VGMN/VGSN
//	lcd_fmc_write_io_reg(fmc, 0xBD00); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xBD01); lcd_fmc_write_io_data(fmc, 0xA3);
//	lcd_fmc_write_io_reg(fmc, 0xBD02); lcd_fmc_write_io_data(fmc, 0x00);
//	//# VCOM=-0.1
//	lcd_fmc_write_io_reg(fmc, 0xBE00); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xBE01); lcd_fmc_write_io_data(fmc, 0x63);//4f
//		//  VCOMH+0x01;
//	//#R+
//	lcd_fmc_write_io_reg(fmc, 0xD100); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD101); lcd_fmc_write_io_data(fmc, 0x37);
//	lcd_fmc_write_io_reg(fmc, 0xD102); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD103); lcd_fmc_write_io_data(fmc, 0x52);
//	lcd_fmc_write_io_reg(fmc, 0xD104); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD105); lcd_fmc_write_io_data(fmc, 0x7B);
//	lcd_fmc_write_io_reg(fmc, 0xD106); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD107); lcd_fmc_write_io_data(fmc, 0x99);
//	lcd_fmc_write_io_reg(fmc, 0xD108); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD109); lcd_fmc_write_io_data(fmc, 0xB1);
//	lcd_fmc_write_io_reg(fmc, 0xD10A); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD10B); lcd_fmc_write_io_data(fmc, 0xD2);
//	lcd_fmc_write_io_reg(fmc, 0xD10C); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD10D); lcd_fmc_write_io_data(fmc, 0xF6);
//	lcd_fmc_write_io_reg(fmc, 0xD10E); lcd_fmc_write_io_data(fmc, 0x01);
//	lcd_fmc_write_io_reg(fmc, 0xD10F); lcd_fmc_write_io_data(fmc, 0x27);
//	lcd_fmc_write_io_reg(fmc, 0xD110); lcd_fmc_write_io_data(fmc, 0x01);
//	lcd_fmc_write_io_reg(fmc, 0xD111); lcd_fmc_write_io_data(fmc, 0x4E);
//	lcd_fmc_write_io_reg(fmc, 0xD112); lcd_fmc_write_io_data(fmc, 0x01);
//	lcd_fmc_write_io_reg(fmc, 0xD113); lcd_fmc_write_io_data(fmc, 0x8C);
//	lcd_fmc_write_io_reg(fmc, 0xD114); lcd_fmc_write_io_data(fmc, 0x01);
//	lcd_fmc_write_io_reg(fmc, 0xD115); lcd_fmc_write_io_data(fmc, 0xBE);
//	lcd_fmc_write_io_reg(fmc, 0xD116); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD117); lcd_fmc_write_io_data(fmc, 0x0B);
//	lcd_fmc_write_io_reg(fmc, 0xD118); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD119); lcd_fmc_write_io_data(fmc, 0x48);
//	lcd_fmc_write_io_reg(fmc, 0xD11A); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD11B); lcd_fmc_write_io_data(fmc, 0x4A);
//	lcd_fmc_write_io_reg(fmc, 0xD11C); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD11D); lcd_fmc_write_io_data(fmc, 0x7E);
//	lcd_fmc_write_io_reg(fmc, 0xD11E); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD11F); lcd_fmc_write_io_data(fmc, 0xBC);
//	lcd_fmc_write_io_reg(fmc, 0xD120); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD121); lcd_fmc_write_io_data(fmc, 0xE1);
//	lcd_fmc_write_io_reg(fmc, 0xD122); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD123); lcd_fmc_write_io_data(fmc, 0x10);
//	lcd_fmc_write_io_reg(fmc, 0xD124); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD125); lcd_fmc_write_io_data(fmc, 0x31);
//	lcd_fmc_write_io_reg(fmc, 0xD126); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD127); lcd_fmc_write_io_data(fmc, 0x5A);
//	lcd_fmc_write_io_reg(fmc, 0xD128); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD129); lcd_fmc_write_io_data(fmc, 0x73);
//	lcd_fmc_write_io_reg(fmc, 0xD12A); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD12B); lcd_fmc_write_io_data(fmc, 0x94);
//	lcd_fmc_write_io_reg(fmc, 0xD12C); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD12D); lcd_fmc_write_io_data(fmc, 0x9F);
//	lcd_fmc_write_io_reg(fmc, 0xD12E); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD12F); lcd_fmc_write_io_data(fmc, 0xB3);
//	lcd_fmc_write_io_reg(fmc, 0xD130); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD131); lcd_fmc_write_io_data(fmc, 0xB9);
//	lcd_fmc_write_io_reg(fmc, 0xD132); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD133); lcd_fmc_write_io_data(fmc, 0xC1);
//	//#G+
//	lcd_fmc_write_io_reg(fmc, 0xD200); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD201); lcd_fmc_write_io_data(fmc, 0x37);
//	lcd_fmc_write_io_reg(fmc, 0xD202); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD203); lcd_fmc_write_io_data(fmc, 0x52);
//	lcd_fmc_write_io_reg(fmc, 0xD204); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD205); lcd_fmc_write_io_data(fmc, 0x7B);
//	lcd_fmc_write_io_reg(fmc, 0xD206); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD207); lcd_fmc_write_io_data(fmc, 0x99);
//	lcd_fmc_write_io_reg(fmc, 0xD208); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD209); lcd_fmc_write_io_data(fmc, 0xB1);
//	lcd_fmc_write_io_reg(fmc, 0xD20A); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD20B); lcd_fmc_write_io_data(fmc, 0xD2);
//	lcd_fmc_write_io_reg(fmc, 0xD20C); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD20D); lcd_fmc_write_io_data(fmc, 0xF6);
//	lcd_fmc_write_io_reg(fmc, 0xD20E); lcd_fmc_write_io_data(fmc, 0x01);
//	lcd_fmc_write_io_reg(fmc, 0xD20F); lcd_fmc_write_io_data(fmc, 0x27);
//	lcd_fmc_write_io_reg(fmc, 0xD210); lcd_fmc_write_io_data(fmc, 0x01);
//	lcd_fmc_write_io_reg(fmc, 0xD211); lcd_fmc_write_io_data(fmc, 0x4E);
//	lcd_fmc_write_io_reg(fmc, 0xD212); lcd_fmc_write_io_data(fmc, 0x01);
//	lcd_fmc_write_io_reg(fmc, 0xD213); lcd_fmc_write_io_data(fmc, 0x8C);
//	lcd_fmc_write_io_reg(fmc, 0xD214); lcd_fmc_write_io_data(fmc, 0x01);
//	lcd_fmc_write_io_reg(fmc, 0xD215); lcd_fmc_write_io_data(fmc, 0xBE);
//	lcd_fmc_write_io_reg(fmc, 0xD216); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD217); lcd_fmc_write_io_data(fmc, 0x0B);
//	lcd_fmc_write_io_reg(fmc, 0xD218); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD219); lcd_fmc_write_io_data(fmc, 0x48);
//	lcd_fmc_write_io_reg(fmc, 0xD21A); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD21B); lcd_fmc_write_io_data(fmc, 0x4A);
//	lcd_fmc_write_io_reg(fmc, 0xD21C); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD21D); lcd_fmc_write_io_data(fmc, 0x7E);
//	lcd_fmc_write_io_reg(fmc, 0xD21E); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD21F); lcd_fmc_write_io_data(fmc, 0xBC);
//	lcd_fmc_write_io_reg(fmc, 0xD220); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD221); lcd_fmc_write_io_data(fmc, 0xE1);
//	lcd_fmc_write_io_reg(fmc, 0xD222); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD223); lcd_fmc_write_io_data(fmc, 0x10);
//	lcd_fmc_write_io_reg(fmc, 0xD224); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD225); lcd_fmc_write_io_data(fmc, 0x31);
//	lcd_fmc_write_io_reg(fmc, 0xD226); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD227); lcd_fmc_write_io_data(fmc, 0x5A);
//	lcd_fmc_write_io_reg(fmc, 0xD228); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD229); lcd_fmc_write_io_data(fmc, 0x73);
//	lcd_fmc_write_io_reg(fmc, 0xD22A); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD22B); lcd_fmc_write_io_data(fmc, 0x94);
//	lcd_fmc_write_io_reg(fmc, 0xD22C); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD22D); lcd_fmc_write_io_data(fmc, 0x9F);
//	lcd_fmc_write_io_reg(fmc, 0xD22E); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD22F); lcd_fmc_write_io_data(fmc, 0xB3);
//	lcd_fmc_write_io_reg(fmc, 0xD230); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD231); lcd_fmc_write_io_data(fmc, 0xB9);
//	lcd_fmc_write_io_reg(fmc, 0xD232); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD233); lcd_fmc_write_io_data(fmc, 0xC1);
//	//#B+
//	lcd_fmc_write_io_reg(fmc, 0xD300); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD301); lcd_fmc_write_io_data(fmc, 0x37);
//	lcd_fmc_write_io_reg(fmc, 0xD302); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD303); lcd_fmc_write_io_data(fmc, 0x52);
//	lcd_fmc_write_io_reg(fmc, 0xD304); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD305); lcd_fmc_write_io_data(fmc, 0x7B);
//	lcd_fmc_write_io_reg(fmc, 0xD306); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD307); lcd_fmc_write_io_data(fmc, 0x99);
//	lcd_fmc_write_io_reg(fmc, 0xD308); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD309); lcd_fmc_write_io_data(fmc, 0xB1);
//	lcd_fmc_write_io_reg(fmc, 0xD30A); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD30B); lcd_fmc_write_io_data(fmc, 0xD2);
//	lcd_fmc_write_io_reg(fmc, 0xD30C); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD30D); lcd_fmc_write_io_data(fmc, 0xF6);
//	lcd_fmc_write_io_reg(fmc, 0xD30E); lcd_fmc_write_io_data(fmc, 0x01);
//	lcd_fmc_write_io_reg(fmc, 0xD30F); lcd_fmc_write_io_data(fmc, 0x27);
//	lcd_fmc_write_io_reg(fmc, 0xD310); lcd_fmc_write_io_data(fmc, 0x01);
//	lcd_fmc_write_io_reg(fmc, 0xD311); lcd_fmc_write_io_data(fmc, 0x4E);
//	lcd_fmc_write_io_reg(fmc, 0xD312); lcd_fmc_write_io_data(fmc, 0x01);
//	lcd_fmc_write_io_reg(fmc, 0xD313); lcd_fmc_write_io_data(fmc, 0x8C);
//	lcd_fmc_write_io_reg(fmc, 0xD314); lcd_fmc_write_io_data(fmc, 0x01);
//	lcd_fmc_write_io_reg(fmc, 0xD315); lcd_fmc_write_io_data(fmc, 0xBE);
//	lcd_fmc_write_io_reg(fmc, 0xD316); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD317); lcd_fmc_write_io_data(fmc, 0x0B);
//	lcd_fmc_write_io_reg(fmc, 0xD318); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD319); lcd_fmc_write_io_data(fmc, 0x48);
//	lcd_fmc_write_io_reg(fmc, 0xD31A); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD31B); lcd_fmc_write_io_data(fmc, 0x4A);
//	lcd_fmc_write_io_reg(fmc, 0xD31C); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD31D); lcd_fmc_write_io_data(fmc, 0x7E);
//	lcd_fmc_write_io_reg(fmc, 0xD31E); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD31F); lcd_fmc_write_io_data(fmc, 0xBC);
//	lcd_fmc_write_io_reg(fmc, 0xD320); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD321); lcd_fmc_write_io_data(fmc, 0xE1);
//	lcd_fmc_write_io_reg(fmc, 0xD322); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD323); lcd_fmc_write_io_data(fmc, 0x10);
//	lcd_fmc_write_io_reg(fmc, 0xD324); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD325); lcd_fmc_write_io_data(fmc, 0x31);
//	lcd_fmc_write_io_reg(fmc, 0xD326); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD327); lcd_fmc_write_io_data(fmc, 0x5A);
//	lcd_fmc_write_io_reg(fmc, 0xD328); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD329); lcd_fmc_write_io_data(fmc, 0x73);
//	lcd_fmc_write_io_reg(fmc, 0xD32A); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD32B); lcd_fmc_write_io_data(fmc, 0x94);
//	lcd_fmc_write_io_reg(fmc, 0xD32C); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD32D); lcd_fmc_write_io_data(fmc, 0x9F);
//	lcd_fmc_write_io_reg(fmc, 0xD32E); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD32F); lcd_fmc_write_io_data(fmc, 0xB3);
//	lcd_fmc_write_io_reg(fmc, 0xD330); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD331); lcd_fmc_write_io_data(fmc, 0xB9);
//	lcd_fmc_write_io_reg(fmc, 0xD332); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD333); lcd_fmc_write_io_data(fmc, 0xC1);
//
//	//#R-///////////////////////////////////////////
//	lcd_fmc_write_io_reg(fmc, 0xD400); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD401); lcd_fmc_write_io_data(fmc, 0x37);
//	lcd_fmc_write_io_reg(fmc, 0xD402); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD403); lcd_fmc_write_io_data(fmc, 0x52);
//	lcd_fmc_write_io_reg(fmc, 0xD404); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD405); lcd_fmc_write_io_data(fmc, 0x7B);
//	lcd_fmc_write_io_reg(fmc, 0xD406); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD407); lcd_fmc_write_io_data(fmc, 0x99);
//	lcd_fmc_write_io_reg(fmc, 0xD408); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD409); lcd_fmc_write_io_data(fmc, 0xB1);
//	lcd_fmc_write_io_reg(fmc, 0xD40A); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD40B); lcd_fmc_write_io_data(fmc, 0xD2);
//	lcd_fmc_write_io_reg(fmc, 0xD40C); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD40D); lcd_fmc_write_io_data(fmc, 0xF6);
//	lcd_fmc_write_io_reg(fmc, 0xD40E); lcd_fmc_write_io_data(fmc, 0x01);
//	lcd_fmc_write_io_reg(fmc, 0xD40F); lcd_fmc_write_io_data(fmc, 0x27);
//	lcd_fmc_write_io_reg(fmc, 0xD410); lcd_fmc_write_io_data(fmc, 0x01);
//	lcd_fmc_write_io_reg(fmc, 0xD411); lcd_fmc_write_io_data(fmc, 0x4E);
//	lcd_fmc_write_io_reg(fmc, 0xD412); lcd_fmc_write_io_data(fmc, 0x01);
//	lcd_fmc_write_io_reg(fmc, 0xD413); lcd_fmc_write_io_data(fmc, 0x8C);
//	lcd_fmc_write_io_reg(fmc, 0xD414); lcd_fmc_write_io_data(fmc, 0x01);
//	lcd_fmc_write_io_reg(fmc, 0xD415); lcd_fmc_write_io_data(fmc, 0xBE);
//	lcd_fmc_write_io_reg(fmc, 0xD416); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD417); lcd_fmc_write_io_data(fmc, 0x0B);
//	lcd_fmc_write_io_reg(fmc, 0xD418); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD419); lcd_fmc_write_io_data(fmc, 0x48);
//	lcd_fmc_write_io_reg(fmc, 0xD41A); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD41B); lcd_fmc_write_io_data(fmc, 0x4A);
//	lcd_fmc_write_io_reg(fmc, 0xD41C); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD41D); lcd_fmc_write_io_data(fmc, 0x7E);
//	lcd_fmc_write_io_reg(fmc, 0xD41E); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD41F); lcd_fmc_write_io_data(fmc, 0xBC);
//	lcd_fmc_write_io_reg(fmc, 0xD420); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD421); lcd_fmc_write_io_data(fmc, 0xE1);
//	lcd_fmc_write_io_reg(fmc, 0xD422); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD423); lcd_fmc_write_io_data(fmc, 0x10);
//	lcd_fmc_write_io_reg(fmc, 0xD424); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD425); lcd_fmc_write_io_data(fmc, 0x31);
//	lcd_fmc_write_io_reg(fmc, 0xD426); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD427); lcd_fmc_write_io_data(fmc, 0x5A);
//	lcd_fmc_write_io_reg(fmc, 0xD428); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD429); lcd_fmc_write_io_data(fmc, 0x73);
//	lcd_fmc_write_io_reg(fmc, 0xD42A); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD42B); lcd_fmc_write_io_data(fmc, 0x94);
//	lcd_fmc_write_io_reg(fmc, 0xD42C); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD42D); lcd_fmc_write_io_data(fmc, 0x9F);
//	lcd_fmc_write_io_reg(fmc, 0xD42E); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD42F); lcd_fmc_write_io_data(fmc, 0xB3);
//	lcd_fmc_write_io_reg(fmc, 0xD430); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD431); lcd_fmc_write_io_data(fmc, 0xB9);
//	lcd_fmc_write_io_reg(fmc, 0xD432); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD433); lcd_fmc_write_io_data(fmc, 0xC1);
//
//	//#G-//////////////////////////////////////////////
//	lcd_fmc_write_io_reg(fmc, 0xD500); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD501); lcd_fmc_write_io_data(fmc, 0x37);
//	lcd_fmc_write_io_reg(fmc, 0xD502); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD503); lcd_fmc_write_io_data(fmc, 0x52);
//	lcd_fmc_write_io_reg(fmc, 0xD504); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD505); lcd_fmc_write_io_data(fmc, 0x7B);
//	lcd_fmc_write_io_reg(fmc, 0xD506); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD507); lcd_fmc_write_io_data(fmc, 0x99);
//	lcd_fmc_write_io_reg(fmc, 0xD508); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD509); lcd_fmc_write_io_data(fmc, 0xB1);
//	lcd_fmc_write_io_reg(fmc, 0xD50A); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD50B); lcd_fmc_write_io_data(fmc, 0xD2);
//	lcd_fmc_write_io_reg(fmc, 0xD50C); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD50D); lcd_fmc_write_io_data(fmc, 0xF6);
//	lcd_fmc_write_io_reg(fmc, 0xD50E); lcd_fmc_write_io_data(fmc, 0x01);
//	lcd_fmc_write_io_reg(fmc, 0xD50F); lcd_fmc_write_io_data(fmc, 0x27);
//	lcd_fmc_write_io_reg(fmc, 0xD510); lcd_fmc_write_io_data(fmc, 0x01);
//	lcd_fmc_write_io_reg(fmc, 0xD511); lcd_fmc_write_io_data(fmc, 0x4E);
//	lcd_fmc_write_io_reg(fmc, 0xD512); lcd_fmc_write_io_data(fmc, 0x01);
//	lcd_fmc_write_io_reg(fmc, 0xD513); lcd_fmc_write_io_data(fmc, 0x8C);
//	lcd_fmc_write_io_reg(fmc, 0xD514); lcd_fmc_write_io_data(fmc, 0x01);
//	lcd_fmc_write_io_reg(fmc, 0xD515); lcd_fmc_write_io_data(fmc, 0xBE);
//	lcd_fmc_write_io_reg(fmc, 0xD516); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD517); lcd_fmc_write_io_data(fmc, 0x0B);
//	lcd_fmc_write_io_reg(fmc, 0xD518); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD519); lcd_fmc_write_io_data(fmc, 0x48);
//	lcd_fmc_write_io_reg(fmc, 0xD51A); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD51B); lcd_fmc_write_io_data(fmc, 0x4A);
//	lcd_fmc_write_io_reg(fmc, 0xD51C); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD51D); lcd_fmc_write_io_data(fmc, 0x7E);
//	lcd_fmc_write_io_reg(fmc, 0xD51E); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD51F); lcd_fmc_write_io_data(fmc, 0xBC);
//	lcd_fmc_write_io_reg(fmc, 0xD520); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD521); lcd_fmc_write_io_data(fmc, 0xE1);
//	lcd_fmc_write_io_reg(fmc, 0xD522); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD523); lcd_fmc_write_io_data(fmc, 0x10);
//	lcd_fmc_write_io_reg(fmc, 0xD524); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD525); lcd_fmc_write_io_data(fmc, 0x31);
//	lcd_fmc_write_io_reg(fmc, 0xD526); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD527); lcd_fmc_write_io_data(fmc, 0x5A);
//	lcd_fmc_write_io_reg(fmc, 0xD528); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD529); lcd_fmc_write_io_data(fmc, 0x73);
//	lcd_fmc_write_io_reg(fmc, 0xD52A); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD52B); lcd_fmc_write_io_data(fmc, 0x94);
//	lcd_fmc_write_io_reg(fmc, 0xD52C); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD52D); lcd_fmc_write_io_data(fmc, 0x9F);
//	lcd_fmc_write_io_reg(fmc, 0xD52E); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD52F); lcd_fmc_write_io_data(fmc, 0xB3);
//	lcd_fmc_write_io_reg(fmc, 0xD530); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD531); lcd_fmc_write_io_data(fmc, 0xB9);
//	lcd_fmc_write_io_reg(fmc, 0xD532); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD533); lcd_fmc_write_io_data(fmc, 0xC1);
//	//#B-///////////////////////////////
//	lcd_fmc_write_io_reg(fmc, 0xD600); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD601); lcd_fmc_write_io_data(fmc, 0x37);
//	lcd_fmc_write_io_reg(fmc, 0xD602); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD603); lcd_fmc_write_io_data(fmc, 0x52);
//	lcd_fmc_write_io_reg(fmc, 0xD604); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD605); lcd_fmc_write_io_data(fmc, 0x7B);
//	lcd_fmc_write_io_reg(fmc, 0xD606); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD607); lcd_fmc_write_io_data(fmc, 0x99);
//	lcd_fmc_write_io_reg(fmc, 0xD608); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD609); lcd_fmc_write_io_data(fmc, 0xB1);
//	lcd_fmc_write_io_reg(fmc, 0xD60A); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD60B); lcd_fmc_write_io_data(fmc, 0xD2);
//	lcd_fmc_write_io_reg(fmc, 0xD60C); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xD60D); lcd_fmc_write_io_data(fmc, 0xF6);
//	lcd_fmc_write_io_reg(fmc, 0xD60E); lcd_fmc_write_io_data(fmc, 0x01);
//	lcd_fmc_write_io_reg(fmc, 0xD60F); lcd_fmc_write_io_data(fmc, 0x27);
//	lcd_fmc_write_io_reg(fmc, 0xD610); lcd_fmc_write_io_data(fmc, 0x01);
//	lcd_fmc_write_io_reg(fmc, 0xD611); lcd_fmc_write_io_data(fmc, 0x4E);
//	lcd_fmc_write_io_reg(fmc, 0xD612); lcd_fmc_write_io_data(fmc, 0x01);
//	lcd_fmc_write_io_reg(fmc, 0xD613); lcd_fmc_write_io_data(fmc, 0x8C);
//	lcd_fmc_write_io_reg(fmc, 0xD614); lcd_fmc_write_io_data(fmc, 0x01);
//	lcd_fmc_write_io_reg(fmc, 0xD615); lcd_fmc_write_io_data(fmc, 0xBE);
//	lcd_fmc_write_io_reg(fmc, 0xD616); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD617); lcd_fmc_write_io_data(fmc, 0x0B);
//	lcd_fmc_write_io_reg(fmc, 0xD618); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD619); lcd_fmc_write_io_data(fmc, 0x48);
//	lcd_fmc_write_io_reg(fmc, 0xD61A); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD61B); lcd_fmc_write_io_data(fmc, 0x4A);
//	lcd_fmc_write_io_reg(fmc, 0xD61C); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD61D); lcd_fmc_write_io_data(fmc, 0x7E);
//	lcd_fmc_write_io_reg(fmc, 0xD61E); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD61F); lcd_fmc_write_io_data(fmc, 0xBC);
//	lcd_fmc_write_io_reg(fmc, 0xD620); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xD621); lcd_fmc_write_io_data(fmc, 0xE1);
//	lcd_fmc_write_io_reg(fmc, 0xD622); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD623); lcd_fmc_write_io_data(fmc, 0x10);
//	lcd_fmc_write_io_reg(fmc, 0xD624); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD625); lcd_fmc_write_io_data(fmc, 0x31);
//	lcd_fmc_write_io_reg(fmc, 0xD626); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD627); lcd_fmc_write_io_data(fmc, 0x5A);
//	lcd_fmc_write_io_reg(fmc, 0xD628); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD629); lcd_fmc_write_io_data(fmc, 0x73);
//	lcd_fmc_write_io_reg(fmc, 0xD62A); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD62B); lcd_fmc_write_io_data(fmc, 0x94);
//	lcd_fmc_write_io_reg(fmc, 0xD62C); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD62D); lcd_fmc_write_io_data(fmc, 0x9F);
//	lcd_fmc_write_io_reg(fmc, 0xD62E); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD62F); lcd_fmc_write_io_data(fmc, 0xB3);
//	lcd_fmc_write_io_reg(fmc, 0xD630); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD631); lcd_fmc_write_io_data(fmc, 0xB9);
//	lcd_fmc_write_io_reg(fmc, 0xD632); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xD633); lcd_fmc_write_io_data(fmc, 0xC1);
//
//
//
//	//#Enable Page0
//	lcd_fmc_write_io_reg(fmc, 0xF000); lcd_fmc_write_io_data(fmc, 0x55);
//	lcd_fmc_write_io_reg(fmc, 0xF001); lcd_fmc_write_io_data(fmc, 0xAA);
//	lcd_fmc_write_io_reg(fmc, 0xF002); lcd_fmc_write_io_data(fmc, 0x52);
//	lcd_fmc_write_io_reg(fmc, 0xF003); lcd_fmc_write_io_data(fmc, 0x08);
//	lcd_fmc_write_io_reg(fmc, 0xF004); lcd_fmc_write_io_data(fmc, 0x00);
//	//# RGB I/F Setting
//	lcd_fmc_write_io_reg(fmc, 0xB000); lcd_fmc_write_io_data(fmc, 0x08);
//	lcd_fmc_write_io_reg(fmc, 0xB001); lcd_fmc_write_io_data(fmc, 0x05);
//	lcd_fmc_write_io_reg(fmc, 0xB002); lcd_fmc_write_io_data(fmc, 0x02);
//	lcd_fmc_write_io_reg(fmc, 0xB003); lcd_fmc_write_io_data(fmc, 0x05);
//	lcd_fmc_write_io_reg(fmc, 0xB004); lcd_fmc_write_io_data(fmc, 0x02);
//	//## SDT:
//	lcd_fmc_write_io_reg(fmc, 0xB600); lcd_fmc_write_io_data(fmc, 0x08);
//	lcd_fmc_write_io_reg(fmc, 0xB500); lcd_fmc_write_io_data(fmc, 0x6B);//0x6b ???? 480x854       0x50 ???? 480x800
//
//	//## Gate EQ:
//	lcd_fmc_write_io_reg(fmc, 0xB700); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xB701); lcd_fmc_write_io_data(fmc, 0x00);
//
//	//## Source EQ:
//	lcd_fmc_write_io_reg(fmc, 0xB800); lcd_fmc_write_io_data(fmc, 0x01);
//	lcd_fmc_write_io_reg(fmc, 0xB801); lcd_fmc_write_io_data(fmc, 0x05);
//	lcd_fmc_write_io_reg(fmc, 0xB802); lcd_fmc_write_io_data(fmc, 0x05);
//	lcd_fmc_write_io_reg(fmc, 0xB803); lcd_fmc_write_io_data(fmc, 0x05);
//
//	//# Inversion: Column inversion (NVT)
//	lcd_fmc_write_io_reg(fmc, 0xBC00); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xBC01); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xBC02); lcd_fmc_write_io_data(fmc, 0x00);
//
//	//# BOE's Setting(default)
//	lcd_fmc_write_io_reg(fmc, 0xCC00); lcd_fmc_write_io_data(fmc, 0x03);
//	lcd_fmc_write_io_reg(fmc, 0xCC01); lcd_fmc_write_io_data(fmc, 0x00);
//	lcd_fmc_write_io_reg(fmc, 0xCC02); lcd_fmc_write_io_data(fmc, 0x00);
//
//	//# Display Timing:
//	lcd_fmc_write_io_reg(fmc, 0xBD00); lcd_fmc_write_io_data(fmc, 0x01);
//	lcd_fmc_write_io_reg(fmc, 0xBD01); lcd_fmc_write_io_data(fmc, 0x84);
//	lcd_fmc_write_io_reg(fmc, 0xBD02); lcd_fmc_write_io_data(fmc, 0x07);
//	lcd_fmc_write_io_reg(fmc, 0xBD03); lcd_fmc_write_io_data(fmc, 0x31);
//	lcd_fmc_write_io_reg(fmc, 0xBD04); lcd_fmc_write_io_data(fmc, 0x00);
//
//	lcd_fmc_write_io_reg(fmc, 0xBA00); lcd_fmc_write_io_data(fmc, 0x01);
//
//	lcd_fmc_write_io_reg(fmc, 0xFF00); lcd_fmc_write_io_data(fmc, 0xAA);
//	lcd_fmc_write_io_reg(fmc, 0xFF01); lcd_fmc_write_io_data(fmc, 0x55);
//	lcd_fmc_write_io_reg(fmc, 0xFF02); lcd_fmc_write_io_data(fmc, 0x25);
//	lcd_fmc_write_io_reg(fmc, 0xFF03); lcd_fmc_write_io_data(fmc, 0x01);
//
//	//lcd_fmc_write_io_reg(fmc, 0x3400);  //TEOFF
//	//lcd_fmc_write_io_reg(fmc, 0x3500); lcd_fmc_write_io_data(fmc, 0x01); //TEON
//	lcd_fmc_write_io_reg(fmc, 0x3600); lcd_fmc_write_io_data(fmc, 0x00); //MADCTL R-G-B color order
//	lcd_fmc_write_io_reg(fmc, 0x3a00); lcd_fmc_write_io_data(fmc, 0x55);  ////55=16?/////66=18?
//	//lcd_fmc_write_io_reg(fmc, 0x2100); //INVON

	Init_data(fmc, 0xB001,0x0009);
	Init_data(fmc, 0xB002,0x0009);

	//#AVDD ratio
	Init_data(fmc, 0xB600,0x0034);
	Init_data(fmc, 0xB601,0x3004);
	Init_data(fmc, 0xB602,0x0034);

	//#AVEE  -5.2V
	Init_data(fmc, 0xB100,0x0009);
	Init_data(fmc, 0xB101,0x0009);
	Init_data(fmc, 0xB102,0x0009);

	//#AVEE ratio
	Init_data(fmc, 0xB700,0x0034);
	Init_data(fmc, 0xB701,0x0034);
	Init_data(fmc, 0xB702,0x0034);

	//#VGH 15V  (Free pump)
	Init_data(fmc, 0xB300,0x0008);
	Init_data(fmc, 0xB301,0x0008);
	Init_data(fmc, 0xB302,0x0008);

	//#VGH ratio
	Init_data(fmc, 0xB900,0x0034);
	Init_data(fmc, 0xB901,0x0034);
	Init_data(fmc, 0xB902,0x0034);


	//#VGL_REG -10V
	Init_data(fmc, 0xB500,0x000a);
	Init_data(fmc, 0xB501,0x000A);
	Init_data(fmc, 0xB502,0x000A);

	//#VGLX ratio
	Init_data(fmc, 0xBA00,0x0014);
	Init_data(fmc, 0xBA01,0x0014);
	Init_data(fmc, 0xBA02,0x0014);

	//#VGMP/VGSP 4.5V/0V
	Init_data(fmc, 0xBC00,0x0000);
	Init_data(fmc, 0xBC01,0x00B4);
	Init_data(fmc, 0xBC02,0x0000);

	//VGMN/VGSN -4.5V/0V
	Init_data(fmc, 0xBD00,0x0000);
	Init_data(fmc, 0xBD01,0x00B4);
	Init_data(fmc, 0xBD02,0x0000);

	Init_data(fmc, 0xBE00,0x0000);
	Init_data(fmc, 0xBE01,0x0046);

	Init_data(fmc, 0xD000,0x000F);
	Init_data(fmc, 0xD001,0x000F);
	Init_data(fmc, 0xD002,0x0010);
	Init_data(fmc, 0xD003,0x0010);

	Init_data(fmc, 0xD700,0x0001);

	//#Gamma Setting
	Init_data(fmc, 0xD100,0x0000);
	Init_data(fmc, 0xD101,0x0037);
	Init_data(fmc, 0xD102,0x0000);
	Init_data(fmc, 0xD103,0x0081);
	Init_data(fmc, 0xD104,0x0000);
	Init_data(fmc, 0xD105,0x00AA);
	Init_data(fmc, 0xD106,0x0000);
	Init_data(fmc, 0xD107,0x00C8);
	Init_data(fmc, 0xD108,0x0000);
	Init_data(fmc, 0xD109,0x00DB);
	Init_data(fmc, 0xD10A,0x0000);
	Init_data(fmc, 0xD10B,0x00FF);

	Init_data(fmc, 0xD10C,0x0001);
	Init_data(fmc, 0xD10D,0x001A);
	Init_data(fmc, 0xD10E,0x0001);
	Init_data(fmc, 0xD10F,0x0045);
	Init_data(fmc, 0xD110,0x0001);
	Init_data(fmc, 0xD111,0x0067);
	Init_data(fmc, 0xD112,0x0001);
	Init_data(fmc, 0xD113,0x009B);
	Init_data(fmc, 0xD114,0x0001);
	Init_data(fmc, 0xD115,0x00C5);
	Init_data(fmc, 0xD116,0x0002);
	Init_data(fmc, 0xD117,0x0008);
	Init_data(fmc, 0xD118,0x0002);
	Init_data(fmc, 0xD119,0x0041);
	Init_data(fmc, 0xD11A,0x0002);
	Init_data(fmc, 0xD11B,0x0043);
	Init_data(fmc, 0xD11C,0x0002);
	Init_data(fmc, 0xD11D,0x0075);
	Init_data(fmc, 0xD11E,0x0002);
	Init_data(fmc, 0xD11F,0x00AE);
	Init_data(fmc, 0xD120,0x0002);
	Init_data(fmc, 0xD121,0x00D2);

	Init_data(fmc, 0xD122,0x0003);
	Init_data(fmc, 0xD123,0x0000);
	Init_data(fmc, 0xD124,0x0003);
	Init_data(fmc, 0xD125,0x001F);
	Init_data(fmc, 0xD126,0x0003);
	Init_data(fmc, 0xD127,0x0048);
	Init_data(fmc, 0xD128,0x0003);
	Init_data(fmc, 0xD129,0x0067);
	Init_data(fmc, 0xD12A,0x0003);
	Init_data(fmc, 0xD12B,0x0085);
	Init_data(fmc, 0xD12C,0x0003);
	Init_data(fmc, 0xD12D,0x009F);
	Init_data(fmc, 0xD12E,0x0003);
	Init_data(fmc, 0xD12F,0x00AC);
	Init_data(fmc, 0xD130,0x0003);
	Init_data(fmc, 0xD131,0x00BA);
	Init_data(fmc, 0xD132,0x0003);
	Init_data(fmc, 0xD133,0x00C1);

	//Positive Gamma for GREEN
	Init_data(fmc, 0xD200,0x0000);
	Init_data(fmc, 0xD201,0x0037);
	Init_data(fmc, 0xD202,0x0000);
	Init_data(fmc, 0xD203,0x0081);
	Init_data(fmc, 0xD204,0x0000);
	Init_data(fmc, 0xD205,0x00AA);
	Init_data(fmc, 0xD206,0x0000);
	Init_data(fmc, 0xD207,0x00C8);
	Init_data(fmc, 0xD208,0x0000);
	Init_data(fmc, 0xD209,0x00DB);
	Init_data(fmc, 0xD20A,0x0000);
	Init_data(fmc, 0xD20B,0x00FF);

	Init_data(fmc, 0xD20C,0x0001);
	Init_data(fmc, 0xD20D,0x001A);
	Init_data(fmc, 0xD20E,0x0001);
	Init_data(fmc, 0xD20F,0x0045);
	Init_data(fmc, 0xD210,0x0001);
	Init_data(fmc, 0xD211,0x0067);
	Init_data(fmc, 0xD212,0x0001);
	Init_data(fmc, 0xD213,0x009B);
	Init_data(fmc, 0xD214,0x0001);
	Init_data(fmc, 0xD215,0x00C5);
	Init_data(fmc, 0xD216,0x0002);
	Init_data(fmc, 0xD217,0x0008);
	Init_data(fmc, 0xD218,0x0002);
	Init_data(fmc, 0xD219,0x0041);
	Init_data(fmc, 0xD21A,0x0002);
	Init_data(fmc, 0xD21B,0x0043);
	Init_data(fmc, 0xD21C,0x0002);
	Init_data(fmc, 0xD21D,0x0075);
	Init_data(fmc, 0xD21E,0x0002);
	Init_data(fmc, 0xD21F,0x00AE);
	Init_data(fmc, 0xD220,0x0002);
	Init_data(fmc, 0xD221,0x00D2);

	Init_data(fmc, 0xD222,0x0003);
	Init_data(fmc, 0xD223,0x0000);
	Init_data(fmc, 0xD224,0x0003);
	Init_data(fmc, 0xD225,0x001F);
	Init_data(fmc, 0xD226,0x0003);
	Init_data(fmc, 0xD227,0x0048);
	Init_data(fmc, 0xD228,0x0003);
	Init_data(fmc, 0xD229,0x0067);
	Init_data(fmc, 0xD22A,0x0003);
	Init_data(fmc, 0xD22B,0x0085);
	Init_data(fmc, 0xD22C,0x0003);
	Init_data(fmc, 0xD22D,0x009F);
	Init_data(fmc, 0xD22E,0x0003);
	Init_data(fmc, 0xD22F,0x00AC);
	Init_data(fmc, 0xD230,0x0003);
	Init_data(fmc, 0xD231,0x00BA);
	Init_data(fmc, 0xD232,0x0003);
	Init_data(fmc, 0xD233,0x00C1);

	//Positive Gamma for Blue
	Init_data(fmc, 0xD300,0x0000);
	Init_data(fmc, 0xD301,0x0037);
	Init_data(fmc, 0xD302,0x0000);
	Init_data(fmc, 0xD303,0x0081);
	Init_data(fmc, 0xD304,0x0000);
	Init_data(fmc, 0xD305,0x00AA);
	Init_data(fmc, 0xD306,0x0000);
	Init_data(fmc, 0xD307,0x00C8);
	Init_data(fmc, 0xD308,0x0000);
	Init_data(fmc, 0xD309,0x00DB);
	Init_data(fmc, 0xD30A,0x0000);
	Init_data(fmc, 0xD30B,0x00FF);

	Init_data(fmc, 0xD30C,0x0001);
	Init_data(fmc, 0xD30D,0x001A);
	Init_data(fmc, 0xD30E,0x0001);
	Init_data(fmc, 0xD30F,0x0045);
	Init_data(fmc, 0xD310,0x0001);
	Init_data(fmc, 0xD311,0x0067);
	Init_data(fmc, 0xD312,0x0001);
	Init_data(fmc, 0xD313,0x009B);
	Init_data(fmc, 0xD314,0x0001);
	Init_data(fmc, 0xD315,0x00C5);
	Init_data(fmc, 0xD316,0x0002);
	Init_data(fmc, 0xD317,0x0008);
	Init_data(fmc, 0xD318,0x0002);
	Init_data(fmc, 0xD319,0x0041);
	Init_data(fmc, 0xD31A,0x0002);
	Init_data(fmc, 0xD31B,0x0043);
	Init_data(fmc, 0xD31C,0x0002);
	Init_data(fmc, 0xD31D,0x0075);
	Init_data(fmc, 0xD31E,0x0002);
	Init_data(fmc, 0xD31F,0x00AE);
	Init_data(fmc, 0xD320,0x0002);
	Init_data(fmc, 0xD321,0x00D3);

	Init_data(fmc, 0xD322,0x0003);
	Init_data(fmc, 0xD323,0x0000);
	Init_data(fmc, 0xD324,0x0003);
	Init_data(fmc, 0xD325,0x001F);
	Init_data(fmc, 0xD326,0x0003);
	Init_data(fmc, 0xD327,0x0048);
	Init_data(fmc, 0xD328,0x0003);
	Init_data(fmc, 0xD329,0x0067);
	Init_data(fmc, 0xD32A,0x0003);
	Init_data(fmc, 0xD32B,0x0085);
	Init_data(fmc, 0xD32C,0x0003);
	Init_data(fmc, 0xD32D,0x009F);
	Init_data(fmc, 0xD32E,0x0003);
	Init_data(fmc, 0xD32F,0x00AC);
	Init_data(fmc, 0xD330,0x0003);
	Init_data(fmc, 0xD331,0x00BA);
	Init_data(fmc, 0xD332,0x0003);
	Init_data(fmc, 0xD333,0x00C1);

	//Negative Gamma for RED
	Init_data(fmc, 0xD400,0x0000);
	Init_data(fmc, 0xD401,0x0037);
	Init_data(fmc, 0xD402,0x0000);
	Init_data(fmc, 0xD403,0x0081);
	Init_data(fmc, 0xD404,0x0000);
	Init_data(fmc, 0xD405,0x00AA);
	Init_data(fmc, 0xD406,0x0000);
	Init_data(fmc, 0xD407,0x00C8);
	Init_data(fmc, 0xD408,0x0000);
	Init_data(fmc, 0xD409,0x00DB);
	Init_data(fmc, 0xD40A,0x0000);
	Init_data(fmc, 0xD40B,0x00FF);

	Init_data(fmc, 0xD40C,0x0001);
	Init_data(fmc, 0xD40D,0x001A);
	Init_data(fmc, 0xD40E,0x0001);
	Init_data(fmc, 0xD40F,0x0045);
	Init_data(fmc, 0xD410,0x0001);
	Init_data(fmc, 0xD411,0x0067);
	Init_data(fmc, 0xD412,0x0001);
	Init_data(fmc, 0xD413,0x009B);
	Init_data(fmc, 0xD414,0x0001);
	Init_data(fmc, 0xD415,0x00C5);
	Init_data(fmc, 0xD416,0x0002);
	Init_data(fmc, 0xD417,0x0008);
	Init_data(fmc, 0xD418,0x0002);
	Init_data(fmc, 0xD419,0x0041);
	Init_data(fmc, 0xD41A,0x0002);
	Init_data(fmc, 0xD41B,0x0043);
	Init_data(fmc, 0xD41C,0x0002);
	Init_data(fmc, 0xD41D,0x0075);
	Init_data(fmc, 0xD41E,0x0002);
	Init_data(fmc, 0xD41F,0x00AE);
	Init_data(fmc, 0xD420,0x0002);
	Init_data(fmc, 0xD421,0x00D3);

	Init_data(fmc, 0xD422,0x0003);
	Init_data(fmc, 0xD423,0x0000);
	Init_data(fmc, 0xD424,0x0003);
	Init_data(fmc, 0xD425,0x001F);
	Init_data(fmc, 0xD426,0x0003);
	Init_data(fmc, 0xD427,0x0048);
	Init_data(fmc, 0xD428,0x0003);
	Init_data(fmc, 0xD429,0x0067);
	Init_data(fmc, 0xD42A,0x0003);
	Init_data(fmc, 0xD42B,0x0085);
	Init_data(fmc, 0xD42C,0x0003);
	Init_data(fmc, 0xD42D,0x009F);
	Init_data(fmc, 0xD42E,0x0003);
	Init_data(fmc, 0xD42F,0x00AC);
	Init_data(fmc, 0xD430,0x0003);
	Init_data(fmc, 0xD431,0x00BA);
	Init_data(fmc, 0xD432,0x0003);
	Init_data(fmc, 0xD433,0x00C1);

	//Negative Gamma for Green
	Init_data(fmc, 0xD500,0x0000);
	Init_data(fmc, 0xD501,0x0037);
	Init_data(fmc, 0xD502,0x0000);
	Init_data(fmc, 0xD503,0x0081);
	Init_data(fmc, 0xD504,0x0000);
	Init_data(fmc, 0xD505,0x00AA);
	Init_data(fmc, 0xD506,0x0000);
	Init_data(fmc, 0xD507,0x00C8);
	Init_data(fmc, 0xD508,0x0000);
	Init_data(fmc, 0xD509,0x00DB);
	Init_data(fmc, 0xD50A,0x0000);
	Init_data(fmc, 0xD50B,0x00FF);

	Init_data(fmc, 0xD50C,0x0001);
	Init_data(fmc, 0xD50D,0x001A);
	Init_data(fmc, 0xD50E,0x0001);
	Init_data(fmc, 0xD50F,0x0045);
	Init_data(fmc, 0xD510,0x0001);
	Init_data(fmc, 0xD511,0x0067);
	Init_data(fmc, 0xD512,0x0001);
	Init_data(fmc, 0xD513,0x009B);
	Init_data(fmc, 0xD514,0x0001);
	Init_data(fmc, 0xD515,0x00C5);
	Init_data(fmc, 0xD516,0x0002);
	Init_data(fmc, 0xD517,0x0008);
	Init_data(fmc, 0xD518,0x0002);
	Init_data(fmc, 0xD519,0x0041);
	Init_data(fmc, 0xD51A,0x0002);
	Init_data(fmc, 0xD51B,0x0043);
	Init_data(fmc, 0xD51C,0x0002);
	Init_data(fmc, 0xD51D,0x0075);
	Init_data(fmc, 0xD51E,0x0002);
	Init_data(fmc, 0xD51F,0x00AE);
	Init_data(fmc, 0xD520,0x0002);
	Init_data(fmc, 0xD521,0x00D3);

	Init_data(fmc, 0xD522,0x0003);
	Init_data(fmc, 0xD523,0x0000);
	Init_data(fmc, 0xD524,0x0003);
	Init_data(fmc, 0xD525,0x001F);
	Init_data(fmc, 0xD526,0x0003);
	Init_data(fmc, 0xD527,0x0048);
	Init_data(fmc, 0xD528,0x0003);
	Init_data(fmc, 0xD529,0x0067);
	Init_data(fmc, 0xD52A,0x0003);
	Init_data(fmc, 0xD52B,0x0085);
	Init_data(fmc, 0xD52C,0x0003);
	Init_data(fmc, 0xD52D,0x009F);
	Init_data(fmc, 0xD52E,0x0003);
	Init_data(fmc, 0xD52F,0x00AC);
	Init_data(fmc, 0xD530,0x0003);
	Init_data(fmc, 0xD531,0x00BA);
	Init_data(fmc, 0xD532,0x0003);
	Init_data(fmc, 0xD533,0x00C1);

	//Negative Gamma for Blue
	Init_data(fmc, 0xD600,0x0000);
	Init_data(fmc, 0xD601,0x0037);
	Init_data(fmc, 0xD602,0x0000);
	Init_data(fmc, 0xD603,0x0081);
	Init_data(fmc, 0xD604,0x0000);
	Init_data(fmc, 0xD605,0x00AA);
	Init_data(fmc, 0xD606,0x0000);
	Init_data(fmc, 0xD607,0x00C8);
	Init_data(fmc, 0xD608,0x0000);
	Init_data(fmc, 0xD609,0x00DB);
	Init_data(fmc, 0xD60A,0x0000);
	Init_data(fmc, 0xD60B,0x00FF);

	Init_data(fmc, 0xD60C,0x0001);
	Init_data(fmc, 0xD60D,0x001A);
	Init_data(fmc, 0xD60E,0x0001);
	Init_data(fmc, 0xD60F,0x0045);
	Init_data(fmc, 0xD610,0x0001);
	Init_data(fmc, 0xD611,0x0067);
	Init_data(fmc, 0xD612,0x0001);
	Init_data(fmc, 0xD613,0x009B);
	Init_data(fmc, 0xD614,0x0001);
	Init_data(fmc, 0xD615,0x00C5);
	Init_data(fmc, 0xD616,0x0002);
	Init_data(fmc, 0xD617,0x0008);
	Init_data(fmc, 0xD618,0x0002);
	Init_data(fmc, 0xD619,0x0041);
	Init_data(fmc, 0xD61A,0x0002);
	Init_data(fmc, 0xD61B,0x0043);
	Init_data(fmc, 0xD61C,0x0002);
	Init_data(fmc, 0xD61D,0x0075);
	Init_data(fmc, 0xD61E,0x0002);
	Init_data(fmc, 0xD61F,0x00AE);
	Init_data(fmc, 0xD620,0x0002);
	Init_data(fmc, 0xD621,0x00D3);

	Init_data(fmc, 0xD622,0x0003);
	Init_data(fmc, 0xD623,0x0000);
	Init_data(fmc, 0xD624,0x0003);
	Init_data(fmc, 0xD625,0x001F);
	Init_data(fmc, 0xD626,0x0003);
	Init_data(fmc, 0xD627,0x0048);
	Init_data(fmc, 0xD628,0x0003);
	Init_data(fmc, 0xD629,0x0067);
	Init_data(fmc, 0xD62A,0x0003);
	Init_data(fmc, 0xD62B,0x0085);
	Init_data(fmc, 0xD62C,0x0003);
	Init_data(fmc, 0xD62D,0x009F);
	Init_data(fmc, 0xD62E,0x0003);
	Init_data(fmc, 0xD62F,0x00AC);
	Init_data(fmc, 0xD630,0x0003);
	Init_data(fmc, 0xD631,0x00BA);
	Init_data(fmc, 0xD632,0x0003);
	Init_data(fmc, 0xD633,0x00C1);

	//#LV2 Page 0 enable
	Init_data(fmc, 0xF000,0x0055);
	Init_data(fmc, 0xF001,0x00AA);
	Init_data(fmc, 0xF002,0x0052);
	Init_data(fmc, 0xF003,0x0008);
	Init_data(fmc, 0xF004,0x0000);

	//#Timing control 4H w/ 4-delay
	Init_data(fmc, 0xB100,0x00FC);
	Init_data(fmc, 0xB101,0x0000);

	Init_data(fmc, 0xB000, 0x0000); //RGB Interface Signals Control

	//#480x800
	Init_data(fmc, 0xB500, 0x006B); //SET 480*800 LCD

	//#Source hold time
	Init_data(fmc, 0xB600,0x0005);

	//#Gate EQ control
	Init_data(fmc, 0xB700,0x0000);
	Init_data(fmc, 0xB701,0x0000);

	//#Source EQ control (Mode 2)
	Init_data(fmc, 0xB800,0x0001);
	Init_data(fmc, 0xB801,0x0005);
	Init_data(fmc, 0xB802,0x0005);
	Init_data(fmc, 0xB803,0x0005);

	Init_data(fmc, 0xBA00,0x0001);

	//#Inversion mode  (2-dot)
	Init_data(fmc, 0xBC00,0x0000);
	Init_data(fmc, 0xBC01,0x0000);
	Init_data(fmc, 0xBC02,0x0000);

	Init_data(fmc, 0xCC00,0x0003);
	Init_data(fmc, 0xCC01,0x0000);
	Init_data(fmc, 0xCC02,0x0000);

	//#Frame rate
	Init_data(fmc, 0xBD00,0x0001);
	Init_data(fmc, 0xBD01,0x0064);//
	Init_data(fmc, 0xBD02,0x0007);
	Init_data(fmc, 0xBD03,0x0031);
	Init_data(fmc, 0xBD04,0x0000);

	Init_data(fmc, 0x3a00,0x0055);//16 BIT interface
	Init_data(fmc, 0x3600,0x0000);

	//Column: Start Address 480
	Init_data(fmc, 0x2A00,0x0000);
	Init_data(fmc, 0x2A01,0x0000);
	Init_data(fmc, 0x2A02,0x0001);
	Init_data(fmc, 0x2A03,0x00DF);



	lcd_fmc_write_io_reg(fmc, 0x2000); //INVOFF

	lcd_fmc_write_io_reg(fmc, 0x1100);
	HAL_Delay(120);
	lcd_fmc_write_io_reg(fmc, 0x2900);
	//lcd_fmc_write_io_reg(fmc, 0x2300);
	//lcd_fmc_write_io_reg(fmc, 0x2c00);

	reg.reg_num = 0x0A00;
	lcd_fmc_read_reg(fmc, &reg);

	return (reg.reg_data == 0x9c)?1:0;
}

uint16_t nt35510_fmc_screenWidth(void* i)
{
	return ((LCD_FMC_t*)i)->width;
}

uint16_t nt35510_fmc_screenHeight(void* i)
{
	return ((LCD_FMC_t*)i)->height;
}

void nt35510_fmc_clearScreen(void* i, uint32_t color)
{
	if (!i) return;
	LCD_FMC_t* fmc = (LCD_FMC_t*)i;
	nt35510_set_window(fmc, 0, 0, fmc->width-1, fmc->height-1);
	nt35510_write_gram(fmc);
	lcd_fmc_fill_data(fmc, color&0xffff, fmc->width*fmc->height);
}

void nt35510_fmc_drawPixel(void* i, uint32_t x, uint32_t y, uint32_t color)
{
	if (!i) return;
	LCD_FMC_t* fmc = (LCD_FMC_t*)i;
	nt35510_set_window(fmc, x, y, x, y);
	nt35510_write_gram(fmc);
	lcd_fmc_write_io_data(fmc, color&0xffff);
}

void nt35510_fmc_fillRectangle(void* i, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color)
{
	if (!i) return;
	LCD_FMC_t* fmc = (LCD_FMC_t*)i;
	nt35510_set_window(fmc, x, y, x+width-1, y+height-1);
	nt35510_write_gram(fmc);
	lcd_fmc_fill_data(fmc, color&0xffff, width*height);
}

//
//Driver
//

DSP_DriverDef_t nt35510_fmc_driver =
{
		nt35510_fmc_init,							/*uint8_t     (*init)(void*);*/
		0,/*void		(*deInit)(void*);*/
		0,							/*uint16_t    (*getId)(void*);*/
		nt35510_fmc_screenWidth,					/*uint16_t    (*screenWidth)(void*);*/
		nt35510_fmc_screenHeight,					/*uint16_t    (*screenHeight)(void*);*/
		nt35510_fmc_clearScreen,					/*void        (*clearScreen)(void*,uint32_t);*/
		0,/*void        (*setInverse)(void*,uint8_t);*/
		0,/*void        (*switchOn)(void*,uint8_t);*/
		0,/*void        (*setBrightnes)(void*,uint32_t);*/
		nt35510_fmc_drawPixel,						/*void        (*drawPixel)(void*,uint32_t, uint32_t, uint32_t);*/
		0,/*void        (*drawLine)(void*,uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);*/
		0,/*void        (*drawRectangle)(void*,uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);*/
		nt35510_fmc_fillRectangle,					/*void        (*fillRectangle)(void*,uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);*/
		0,/*void        (*sendCommands)(void*,uint8_t*, uint8_t);*/
		0,/*void		 (*sendData)(void*,uint8_t*, uint16_t);*/
		0,/*void        (*pushData)(void*);*/
		0,						/*uint32_t    (*rgbColor)(void*,uint8_t, uint8_t, uint8_t, uint8_t);*/
		0,/*uint32_t    (*pixelColor)(void*,uint32_t, uint32_t);*/
		0,/*uint32_t    (*bitPerPixel)(void*);*/
		0,						/*void        (*drawBitmap)(void*,uint32_t, uint32_t, DSP_Bitmap_t*, uint32_t, uint32_t );*/
		0,						/*void		 (*drawImage)(void*,uint32_t, uint32_t, DSP_Image_t*);*/
		0,/*RGB_Color_t (*decodeColor)(void*,uint32_t);*/
		0,/*void		 (*rotate)(void*,uint8_t);*/
};
