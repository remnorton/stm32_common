/*
 * ssd1963_fsmc.c
 *
 *  Created on: 2 дек. 2019 г.
 *      Author: User
 */
#include <stdlib.h>
#include "board.h"
#include "lcd_fmc.h"
//
//
//

#define SSD1963_NOP								0x00
#define SSD1963_SOFT_RESET						0x01

#define SSD1963_SET_PLL							0xE0
#define SSD1963_SET_PLL_MN						0xE2

#define SSD1963_SET_LCD_MODE					0xB0

#define SSD1963_SET_PIXEL_DATA_INTERFACE		0xF0

#define SSD1963_SET_LSHIFT_FREQ					0xE6

#define SSD1963_SET_HORI_PERIOD					0xB4
#define SSD1963_SET_VERT_PERIOD					0xB6

#define SSD1963_SET_COLUMN_ADDRESS				0x2A
#define SSD1963_SET_PAGE_ADDRESS				0x2B
#define SSD1963_SET_DISPLAY_ON					0x29
#define SSD1963_SET_DISPLAY_OFF					0x28

#define SSD1963_WRITE_MEM 						0x2C
#define SSD1963_READ_MEM						0x2E

#define SSD1963_NORMAL_MODE						0x13
#define SSD1963_POST_PROC						0xBC
#define SSD1963_SET_GAMMA_CURVE					0x26
#define SSD1963_SET_ADDR_MODE					0x36

#define SSD1963_TEAR_OFF						0x34
//#define SSD1963_

#define horizontalPulseWidth 10 /*30*/
#define horizontalBackPorch  18 /*38*/
#define horizontalFrontPorch 10 /*20*/

#define verticalPulseWidth  2
#define verticalBackPorch  10 /*40*/
#define verticalFrontPorch  2;
//#define referenceClock  10; /*(MHz)*/
//#define dClk  65 /*6.5 MHz*/

#define pllM  49
#define pllN  1




#define Write_Command(x)		lcd_fmc_write_io_reg(fmc, x)
#define Write_Parameter(x)		lcd_fmc_write_io_data(fmc, x)
#define delay_ms(x)				HAL_Delay(x)

static uint8_t rotation_angle = DSP_Rotate_0;

//
//forwards
//

uint16_t ssd1963_screenWidth(void* i);
uint16_t ssd1963_screenHeight(void* i);
void ssd1963_rotate(void* i, uint8_t angle);

//
//
//
void ssd1963_set_display_window(LCD_FMC_t* fmc, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	Write_Command(SSD1963_SET_COLUMN_ADDRESS);
	Write_Parameter((uint8_t)(x >> 8));                                //Start address High
	Write_Parameter((uint8_t)x);                                //Start address Low
	Write_Parameter((uint8_t)((x+width-1)>>8)); //End address High
	Write_Parameter((uint8_t)(x+width-1));              //End address Low
	Write_Command(SSD1963_SET_PAGE_ADDRESS);
	Write_Parameter((uint8_t)(y >> 8));                                //Start address High
	Write_Parameter((uint8_t)y);                                //Start address Low
	Write_Parameter((uint8_t)((y+height-1)>>8)); //End address High
	Write_Parameter((uint8_t)(y+height-1));              //End address Low
}

void ssd1963_draw_hline(LCD_FMC_t* fmc, uint16_t x, uint16_t y, uint16_t length, uint32_t color)
{
	ssd1963_set_display_window(fmc, x, y, length, 1);
	lcd_fmc_write_io_reg(fmc, SSD1963_WRITE_MEM);
	lcd_fmc_fill_data(fmc, color, length);
}

void ssd1963_draw_vline(LCD_FMC_t* fmc, uint16_t x, uint16_t y, uint16_t length, uint32_t color)
{
	ssd1963_set_display_window(fmc, x, y, 1, length);
	lcd_fmc_write_io_reg(fmc, SSD1963_WRITE_MEM);
	lcd_fmc_fill_data(fmc, color, length);
}

//
//
//
uint8_t ssd1963_fmc_init(void* init)
{
	if (!init) return 0;
	LCD_FMC_t* fmc = (LCD_FMC_t*)init;

	lcd_fmc_reset(fmc);
	delay_ms(150);

	 Write_Command(SSD1963_SOFT_RESET);
	 delay_ms(10);

  /* Configure PLL M and N multipliers ******************************************
   *
   *    VCO = REFERENCE_CLOCK * (PLL_M + 1)
   *    F_PLL = VCO / (PLL_N + 1)
   */

	Write_Command(SSD1963_SET_PLL_MN);
	Write_Parameter(pllM);
	Write_Parameter(pllN);
	Write_Parameter(0x54);    //Effective the multiplier and divider value
	delay_ms(10);


	Write_Command(SSD1963_SET_PLL);                //PLL enable
	Write_Parameter(0x01);
	delay_ms(10); //Wait for 5ms to let

	Write_Command(SSD1963_SET_PLL);                //PLL lock
	Write_Parameter(0x03);
	delay_ms(10); //Wait another for 5ms
	Write_Command(SSD1963_SOFT_RESET);               //Software reset
	delay_ms(20);//Wait 10ms

	Write_Command(SSD1963_SET_LSHIFT_FREQ);
	Write_Parameter(0x01);
	Write_Parameter(0x33);
	Write_Parameter(0x32);
	delay_ms(5);

	Write_Command(SSD1963_SET_LCD_MODE);
#ifdef _LCD_18_BIT
	Write_Parameter(0x00);			// set 18-bit for TY430TF480272 4.3" panel
#else
	Write_Parameter(0x20);			// set 24-bit for TY700TF480800 7" panel
#endif
	Write_Parameter(0x00); 			// set Hsync+Vsync mode
	Write_Parameter(((uint8_t)((fmc->width-1)>>8)));
	Write_Parameter(((uint8_t)((fmc->width-1))));
	Write_Parameter(((uint8_t)(((fmc->height-1)>>8))));
	Write_Parameter(((uint8_t)((fmc->height-1))));
	Write_Parameter(0x2D);//00

	/* Set Pixel Format *************************************************************
	   *
	   * Set the pixel data format to 8-bit / 9-bit / 12-bit / 16-bit / 16-bit(565) /
	   * 18-bit / 24-bit in the parallel host processor interface.
	   * A[2:0] : Pixel Data Interface Format (POR = 101)
			000 8-bit
			001 12-bit
			010 16-bit packed
			011 16-bit (565 format)
			100 18-bit
			101 24-bit
			110 9-bit
	   */
	Write_Command(SSD1963_SET_PIXEL_DATA_INTERFACE);
	Write_Parameter(0x03);

	uint16_t HT = fmc->width + horizontalPulseWidth + horizontalBackPorch + horizontalFrontPorch - 1;
	uint16_t HPW = horizontalPulseWidth - 1;
	uint16_t HPS = horizontalPulseWidth + horizontalBackPorch;
	uint16_t LPS = 0;
	uint16_t HDP = fmc->width - 1;
	uint16_t VDP = fmc->height - 1;
	uint16_t VT = fmc->height + verticalPulseWidth + verticalBackPorch + verticalFrontPorch - 1;
	uint16_t VPS = verticalPulseWidth + verticalBackPorch;
	uint16_t VPW = verticalPulseWidth - 1;
	uint16_t FPS = 0;


	Write_Command(SSD1963_SET_HORI_PERIOD);
	Write_Parameter((uint8_t)(HT>>8));   // HT:  Horizontal total period = HT + 1
	Write_Parameter((uint8_t)(HT));        //
	Write_Parameter((uint8_t)(HPS>>8));  // HPS: Horizontal Sync Pulse Start Position = Horizontal Pulse Width +
	Write_Parameter((uint8_t)(HPS));       //      Horizontal Back Porch
	Write_Parameter((uint8_t)(HPW));       // HPW: Horizontal Sync Pulse Width - 1
	Write_Parameter((uint8_t)(LPS>>8));    // LPS: Horizontal Display Period Start Position
	Write_Parameter((uint8_t)(LPS));       //
	Write_Parameter(0x00);     // LPSPP: Horizontal Sync Pulse Subpixel Start Position(for serial
				  //        TFT interface). Dummy value for TFT interface.

	Write_Command(SSD1963_SET_VERT_PERIOD);
	Write_Parameter((uint8_t)(VT>>8));   // VT:  Vertical Total period = VT + 1
	Write_Parameter((uint8_t)(VT));    //
	Write_Parameter((uint8_t)(VPS>>8));  // VPS: Vertical Sync Pulse Start Position =
	Write_Parameter((uint8_t)(VPS));   //      Vertical Pulse Width + Vertical Back Porch
	Write_Parameter((uint8_t)(VPW));   // VPW: Vertical Sync Pulse Width &#8211; 1
	Write_Parameter((uint8_t)(FPS>>8));  // FPS: Vertical Display Period Start Position
	Write_Parameter((uint8_t)(FPS));   //


	Write_Command(SSD1963_TEAR_OFF);

	//Write_Command(SSD1963_SET_ADDR_MODE);
	//Write_Parameter(0x60);

	Write_Command(SSD1963_POST_PROC);
	Write_Parameter(0x40);
	Write_Parameter(0x80); //bright
	Write_Parameter(0x40);
	Write_Parameter(1);

	/* Display ON */
	Write_Command(SSD1963_SET_DISPLAY_ON);

	return 1;
}

void ssd1963_fillRectangle(void* i, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color)
{
	if (!i) return;
	LCD_FMC_t* fmc = (LCD_FMC_t*)i;

	uint16_t sx = x;
	uint16_t sy = y;

	ssd1963_set_display_window(fmc, x, y, width, height);
	Write_Command(SSD1963_WRITE_MEM);
	lcd_fmc_fill_data(fmc, color, width * height);

}

void ssd1963_clearScreen(void* i,uint32_t color)
{
	if (!i) return;
	LCD_FMC_t* fmc = (LCD_FMC_t*)i;
	uint8_t a = rotation_angle;
	ssd1963_fillRectangle(i, 0, 0, fmc->width, fmc->height, color);
}

uint16_t ssd1963_getId(void* i)
{
	UNUSED(i);
	return 0x1963;
}

uint16_t ssd1963_screenWidth(void* i)
{
	if (!i) return 0;
	LCD_FMC_t* fmc = (LCD_FMC_t*)i;
	uint8_t vertical = (rotation_angle == DSP_Rotate_270) || (rotation_angle == DSP_Rotate_90);
	return vertical?fmc->height:fmc->width;
}

uint16_t ssd1963_screenHeight(void* i)
{
	if (!i) return 0;
	LCD_FMC_t* fmc = (LCD_FMC_t*)i;
	uint8_t vertical = (rotation_angle == DSP_Rotate_270) || (rotation_angle == DSP_Rotate_90);
	return vertical?fmc->width:fmc->height;
}

uint32_t ssd1963_bitPerPixel(void* i)
{
	UNUSED(i);
	return 16;
}

uint32_t ssd1963_rgbColor(void* i, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	UNUSED(i);
	UNUSED(a);
	uint16_t color = ((b & 0x00F8) >> 3)| ((g & 0x00FC) << 3)| ((r & 0x00F8) << 8);
	return color;
}

void ssd1963_drawBitmap(void* i, uint32_t x, uint32_t y, DSP_Bitmap_t* bmp, uint32_t color, uint32_t bkColor, uint8_t angle_code)
{
	if (!i) return;
	if (!bmp) return;
	LCD_FMC_t* instance = (LCD_FMC_t*)i;

	uint32_t width = bmp->width;
	while (width % 8) width++;

	uint8_t vertical = (angle_code == DSP_Rotate_90) || (angle_code == DSP_Rotate_270);

	uint16_t swidth = vertical?bmp->height:bmp->width;
	uint16_t sheight = vertical?bmp->width:bmp->height;


	ssd1963_rotate(i, angle_code);
	ssd1963_set_display_window(instance, x, y, swidth, sheight);

	lcd_fmc_write_io_reg(instance, SSD1963_WRITE_MEM);

	uint32_t size = bmp->width*sizeof(uint16_t);
	uint16_t* line = (uint16_t*)malloc(size);
	if (!line) return;

	for (int h = 0; h < bmp->height; h++)
	{
		for (int w = 0; w < bmp->width; w++)
		{
			uint16_t cl = bkColor;
			if (bmp->data[w/8+(h*width/8)] & (0x80 >> (w%8))) cl = color & 0xFFFF;
			else cl = bkColor & 0xFFFF;
			line[w] = cl;
		}
		lcd_fmc_write_data(instance, (uint8_t*)line, size);
	}
	free(line);
	ssd1963_rotate(i, DSP_Rotate_0);
}

void ssd1963_drawImage(void* i, uint32_t x, uint32_t y, DSP_Image_t* img)
{
	LCD_FMC_t* instance = (LCD_FMC_t*)i;
	if (!instance) return;
	if (!img) return;
	if (img->bit_per_pixel != 16) return;
	uint32_t size = img->height * img->width * 2;
	if (!size) return;
	ssd1963_set_display_window(instance, x, y, img->width, img->height);
	lcd_fmc_write_io_reg(instance, SSD1963_WRITE_MEM);
	lcd_fmc_write_data(instance, img->data, size);
}

RGB_Color_t ssd1963_decodeColor(void* i, uint32_t color)
{
	UNUSED(i);
	RGB16_ColorStruct_t cs;
	uint16_t max_r, max_g, max_b;
	max_r = max_b = 0x1f;
	max_g = 0x3f;
	cs.color = color;
	RGB_Color_t res = {0,0,0,255};

	uint16_t r = cs.rgb.r;
	uint16_t g = cs.rgb.g;
	uint16_t b = cs.rgb.b;

	res.red = (r*255/max_r);
	res.green = (g*255/max_g);
	res.blue = (b*255/max_b);
	return res;
}

void ssd1963_drawPixel(void* i, uint32_t x, uint32_t y, uint32_t color)
{
	if (!i) return;
	LCD_FMC_t* instance = (LCD_FMC_t*)i;
	ssd1963_set_display_window(instance, x, y, 1, 1);
	lcd_fmc_write_io_reg(instance, SSD1963_WRITE_MEM);
	lcd_fmc_write_io_data(instance, color&0xffff);
}

void ssd1963_drawRectangle(void* i, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color)
{
	if (!i) return;
	LCD_FMC_t* instance = (LCD_FMC_t*)i;

	ssd1963_draw_hline(instance, x, y, width, color);
	ssd1963_draw_hline(instance, x, y+height, width, color);
	ssd1963_draw_vline(instance, x, y, height, color);
	ssd1963_draw_vline(instance, x+width, y, height, color);
}

void ssd1963_rotate(void* i, uint8_t angle)
{
	LCD_FMC_t* fmc = (LCD_FMC_t*)i;
	uint8_t mad_cmd = 0;
	switch(angle)
	{
		case DSP_Rotate_90:
		{
			mad_cmd = 0xA0;
			break;
		}
		case DSP_Rotate_270:
		{
			mad_cmd = 0x60;
			break;
		}
	}
	Write_Command(SSD1963_SET_ADDR_MODE);
	Write_Parameter(mad_cmd);

	rotation_angle = angle;
}

void ssd1963_streamJPGmem(void* i, uint16_t x, uint16_t y,  uint8_t* jpg_data, uint32_t jpg_size, uint16_t* line_buf, uint8_t* work_buf, uint8_t lines_per_pass)
{
#if defined(JPEGLIB_H)
	if (!i) return;
	if (!jpg_data || !jpg_size) return;
	if (!line_buf || !work_buf) return;
	LCD_FMC_t* fmc = (LCD_FMC_t*)i;

	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	JSAMPROW buffer[2] = {0};
	buffer[0] = work_buf;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	jpeg_mem_src(&cinfo, jpg_data, jpg_size);

	jpeg_read_header(&cinfo, TRUE);
	cinfo.dct_method = JDCT_IFAST;
	jpeg_start_decompress(&cinfo);

	ssd1963_set_display_window(fmc, x, y, cinfo.output_width, cinfo.output_height);
	lcd_fmc_write_io_reg(fmc, SSD1963_WRITE_MEM);

	while (cinfo.output_scanline < cinfo.output_height)
	{
		uint16_t rows = jpeg_read_scanlines(&cinfo, buffer, lines_per_pass);
		RGB_std_t* RGB_matrix = (RGB_std_t*)buffer[0];
		for (int i = 0; i < cinfo.image_width*rows; i++)
			line_buf[i] = dspRgbColor(RGB_matrix[i].R, RGB_matrix[i].G, RGB_matrix[i].B);

		lcd_fmc_write_data(fmc, (uint8_t*)line_buf, cinfo.image_width*2);
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);


#endif
}

void ssd1963_streamJPGfile(void* i, uint16_t x, uint16_t y, uint8_t* jpg_file_name, uint16_t* line_buf, uint8_t* work_buf, uint8_t lines_per_pass)
{
#if defined(_FATFS) && defined(JPEGLIB_H)
	if (!i) return;
	if (!line_buf || !work_buf) return;
	LCD_FMC_t* fmc = (LCD_FMC_t*)i;

	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FIL	fl;

	if(f_open(&fl, (const TCHAR*)jpg_file_name, FA_READ) == FR_OK)
	{

		JSAMPROW buffer[2] = {0};
		buffer[0] = work_buf;
		cinfo.err = jpeg_std_error(&jerr);
		jpeg_create_decompress(&cinfo);

		jpeg_stdio_src(&cinfo, &fl);

		jpeg_read_header(&cinfo, TRUE);
		cinfo.dct_method = JDCT_FLOAT;
		jpeg_start_decompress(&cinfo);

		ssd1963_set_display_window(fmc, x, y, cinfo.output_width, cinfo.output_height);
		lcd_fmc_write_io_reg(fmc, SSD1963_WRITE_MEM);

		while (cinfo.output_scanline < cinfo.output_height)
		{
			uint16_t rows = jpeg_read_scanlines(&cinfo, buffer, lines_per_pass);
			RGB_std_t* RGB_matrix = (RGB_std_t*)buffer[0];
			for (int i = 0; i < cinfo.image_width*rows; i++)
				line_buf[i] = dspRgbColor(RGB_matrix[i].R, RGB_matrix[i].G, RGB_matrix[i].B);

			lcd_fmc_write_data(fmc, (uint8_t*)line_buf, cinfo.image_width*2);
		}

		jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);

	}
#endif
}

uint8_t ssd1963_readImage(void* i, uint16_t x, uint16_t y, DSP_Image_t* img)
{
	if (!i) return 0;
	LCD_FMC_t* instance = (LCD_FMC_t*)i;
	if (!img) return 0;
	uint32_t count = img->height*img->width;
	if (!count) return 0;
	uint16_t* dta = (uint16_t*)img->data;
	img->bit_per_pixel = 24;
	ssd1963_set_display_window(instance, x, y, img->width, img->height);
	lcd_fmc_write_io_reg(instance, SSD1963_READ_MEM);
	uint32_t idx = 0;
	for (uint32_t i = 0; i < count; i++)
	{
		RGB_Color_t pix_color = ssd1963_decodeColor(i, lcd_fmc_read_io_data(instance));
		dta[idx++] = pix_color.red;
		dta[idx++] = pix_color.green;
		dta[idx++] = pix_color.blue;
	}
	return 1;
}

//
//
//
DSP_DriverDef_t ssd1963_fmc_driver =
{
		ssd1963_fmc_init,							/*uint8_t     (*init)(void*);*/
		0,/*void		(*deInit)(void*);*/
		ssd1963_getId,								/*uint16_t    (*getId)(void*);*/
		ssd1963_screenWidth,						/*uint16_t    (*screenWidth)(void*);*/
		ssd1963_screenHeight,						/*uint16_t    (*screenHeight)(void*);*/
		ssd1963_clearScreen,						/*void        (*clearScreen)(void*,uint32_t);*/
		0,/*void        (*setInverse)(void*,uint8_t);*/
		0,/*void        (*switchOn)(void*,uint8_t);*/
		0,/*void        (*setBrightnes)(void*,uint32_t);*/
		ssd1963_drawPixel,							/*void        (*drawPixel)(void*,uint32_t, uint32_t, uint32_t);*/
		0,/*void        (*drawLine)(void*,uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);*/
		ssd1963_drawRectangle,						/*void        (*drawRectangle)(void*,uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);*/
		ssd1963_fillRectangle,					/*void        (*fillRectangle)(void*,uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);*/
		0,/*void        (*sendCommands)(void*,uint8_t*, uint8_t);*/
		0,/*void		 (*sendData)(void*,uint8_t*, uint16_t);*/
		0,/*void        (*pushData)(void*);*/
		ssd1963_rgbColor,							/*uint32_t    (*rgbColor)(void*,uint8_t, uint8_t, uint8_t, uint8_t);*/
		0,/*uint32_t    (*pixelColor)(void*,uint32_t, uint32_t);*/
		ssd1963_bitPerPixel,						/*uint32_t    (*bitPerPixel)(void*);*/
		ssd1963_drawBitmap,							/*void        (*drawBitmap)(void*,uint32_t, uint32_t, DSP_Bitmap_t*, uint32_t, uint32_t );*/
		ssd1963_drawImage,							/*void		 (*drawImage)(void*,uint32_t, uint32_t, DSP_Image_t*);*/
		ssd1963_decodeColor,						/*RGB_Color_t (*decodeColor)(void*,uint32_t);*/
		ssd1963_rotate,								/*void		 (*rotate)(void*,uint8_t);*/
		ssd1963_streamJPGmem,						//void		 (*streamJPGmem)(uint8_t* /*jpg data*/, uint32_t/*jpg size*/, uint16_t*/*line buf*/, uint8_t* /*work buf*/, uint8_t);
		ssd1963_streamJPGfile,						//void		 (*streamJPGfile)(uint8_t* /*jpg file name*/, uint16_t*/*line buf*/, uint8_t* /*work buf*/, uint8_t);
		ssd1963_readImage,							//uint8_t	 (*readImage)(void*, uint16_t x, uint16_t y, DSP_Image_t* img);
};
