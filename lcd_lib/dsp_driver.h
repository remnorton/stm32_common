/*
 * dsp_driver.h
 *
 *  Created on: 08.09.2018
 *		Author: Rudenko Roman (aka Rem Norton)
 *
 */


#ifndef DSP_DRIVER_H
#define DSP_DRIVER_H
#include "fonts_lib.h"
#include "intmath.h"
#include "board.h"

typedef struct
{
	GPIO_TypeDef*	port;
	uint16_t		pin;
	GPIO_PinState	activeState;
}DSP_GPIO_PinDef_t;

typedef struct
{
#ifdef HAL_TIM_MODULE_ENABLED
	TIM_HandleTypeDef*	tmr;
#else
	void*				tmr;
#endif
	uint32_t			chan;
}LCD_BlPWM_t;

typedef struct
{
	uint16_t reg_num;
	uint16_t reg_data;
}DSP_LCDReg_t;

typedef struct
{
    uint16_t x;
    uint16_t y;
}DSP_POINT_t;

typedef struct
{
    DSP_POINT_t top_left;
    DSP_POINT_t bot_right;
}DSP_RECT_t;


typedef struct
{
    uint32_t width;
    uint32_t height;
    uint8_t* data;
}DSP_Bitmap_t;

typedef struct
{
	uint32_t width;
	uint32_t height;
	uint8_t	 bit_per_pixel;
	uint8_t* data;
}DSP_Image_t;

typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
}RGB_Color_t;

#pragma pack(1)

typedef struct
{
	uint8_t cmd;
	uint8_t para_count;
	uint8_t delay;
	uint8_t para[];
}Command8_t;

#pragma pack()
//
//
//

enum
{
    DSP_COLOR_Black = 0x00,
    DSP_COLOR_White = 0xFFFFFFFF
};

enum
{
    DSP_GRADIENT_HORIZONTAL = 0,
    DSP_GRADIENT_VERTICAL
};

enum
{
    DSP_Rotate_0 = 0,
    DSP_Rotate_90,
    DSP_Rotate_MirrorX,
    DSP_Rotate_270,
    DSP_Rotate_180,
    DSP_Rotate_MirrorY,
};

enum
{
    DSP_TextDir_LeftRight = 0,
    DSP_TextDir_TopBottom,
    DSP_TextDir_RightLeft,
    DSP_TextDir_BottomTop
};

typedef union
{
    uint16_t color;
    struct
    {
        uint16_t b : 5;
        uint16_t g : 6;
        uint16_t r : 5;
    }rgb;
}RGB16_ColorStruct_t;

typedef union
{
    uint32_t color;
    struct
    {
        uint32_t b : 6;
        uint32_t g : 6;
        uint32_t r : 6;
    }rgb;
}RGB18_ColorStruct_t;

typedef struct
{
	uint8_t R;
	uint8_t G;
	uint8_t B;
}RGB_std_t;

typedef union
{
    uint32_t color;
    struct
    {
        uint32_t b : 8;
        uint32_t g : 8;
        uint32_t r : 8;
    }rgb;
}RGB24_ColorStruct_t;

#ifdef __cplusplus
 extern "C" {
#endif

 //!Driver
 typedef struct
 {
     uint8_t     (*init)(void*);
     void		(*deInit)(void*);
     uint16_t    (*getId)(void*);
     uint16_t    (*screenWidth)(void*);
     uint16_t    (*screenHeight)(void*);
     void        (*clearScreen)(void*,uint32_t);
     void        (*setInverse)(void*,uint8_t);
     void        (*switchOn)(void*,uint8_t);
     void        (*setBrightnes)(void*,uint32_t);
     void        (*drawPixel)(void*,uint32_t, uint32_t, uint32_t);
     void        (*drawLine)(void*,uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
     void        (*drawRectangle)(void*,uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
     void        (*fillRectangle)(void*,uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
     void        (*sendCommands)(void*,uint8_t*, uint8_t);
     void		 (*sendData)(void*,uint8_t*, uint16_t);
     void        (*pushData)(void*);
     uint32_t    (*rgbColor)(void*,uint8_t, uint8_t, uint8_t, uint8_t);
     uint32_t    (*pixelColor)(void*,uint32_t, uint32_t);
     uint32_t    (*bitPerPixel)(void*);
     void        (*drawBitmap)(void*,uint32_t, uint32_t, DSP_Bitmap_t*, uint32_t/*color*/, uint32_t /*bkColor*/, uint8_t /*angle*/);
     void		 (*drawImage)(void*,uint32_t, uint32_t, DSP_Image_t*);
     RGB_Color_t (*decodeColor)(void*,uint32_t);
     void		 (*rotate)(void*,uint8_t);
     void		 (*streamJPGmem)(void*, uint16_t /*x*/, uint16_t /*y*/, uint8_t* /*jpg data*/, uint32_t/*jpg size*/, uint16_t*/*line buf*/, uint8_t* /*work buf*/, uint8_t);
     void		 (*streamJPGfile)(void*, uint16_t /*x*/, uint16_t /*y*/,uint8_t* /*jpg file name*/, uint16_t*/*line buf*/, uint8_t* /*work buf*/, uint8_t);
     uint8_t	 (*readImage)(void*, uint16_t /*x*/, uint16_t /*y*/, DSP_Image_t* /*img*/);
 }DSP_DriverDef_t;

//!Display
 typedef struct
 {
     DSP_DriverDef_t*    driver;
     void*               drvSettings;
 }DSPInitStruct_t;


//
//List of existig drivers
//

extern DSP_DriverDef_t ssd1306_driver;
extern DSP_DriverDef_t ili9325_driver;
extern DSP_DriverDef_t ili9341_driver;
extern DSP_DriverDef_t ili9844_driver;
extern DSP_DriverDef_t ili9325_fmc_driver;
extern DSP_DriverDef_t nt35510_fmc_driver;
extern DSP_DriverDef_t ssd1963_fmc_driver;

#ifdef __cplusplus
}
#endif

#endif // DSP_DRIVER_H
