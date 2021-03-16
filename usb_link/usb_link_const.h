/*
 * usb_link_const.h
 *
 *  Created on: 30 џэт. 2021 у.
 *      Author: Rem Norton
 */

#ifndef USB_LINK_USB_LINK_CONST_H_
#define USB_LINK_USB_LINK_CONST_H_
#include "board.h"


#define UL_PREFIX		(uint8_t)0xA5
#define UL_SUFFIX		(uint8_t)0x5A

//! Acces mode
#define UL_READ			(uint8_t)0x01
#define UL_WRITE		(uint8_t)0x02

//! Drive flags
#define UL_DRV_MOVE		(uint8_t)0x01
#define UL_DRV_SCREEN	(uint8_t)0x02

//! Movement direction
#define UL_DIR_FORWARD	(uint8_t)0xFF
#define UL_DIR_REVERSE	(uint8_t)0x00

//! Movement deviation
#define UL_DVT_NONE		(uint8_t)0x00
#define UL_DVT_LEFT		(uint8_t)0x01
#define UL_DVT_RIGHT	(uint8_t)0x02

//! Groups
enum
{
	UL_GROUP_SERVICE = 0,//!< UL_GROUP_SERVICE
	UL_GROUP_DRIVE,       //!< UL_GROUP_DRIVE
	UL_GROUP_LINK,       //!< UL_GROUP_LINK
	UL_GROUP_CALIB = 0xff//!< UL_GROUP_CALIB
};

//! LINK commands
enum
{
	UL_LINK_CMD_FACTORY_DEFAULTS = 0,
	UL_LINK_CMD_CALIB_APPLY,
	UL_LINK_CMD_CALIB_CANCEL,

	UL_LINK_CMD_AUTOSEND_START = 10,
	UL_LINK_CMD_AUTOSEND_STOP
};

//! Drive control commands
enum
{
	UL_DRV_CMD_STOP_ALL = 0,
	UL_DRV_CMD_STOP_MOVE,
	UL_DRV_CMD_STOP_SCREEN,
	UL_DRV_CMD_MOVE,
	UL_DRV_CMD_ROTATE,
	UL_DRV_CMD_SCREEN_ROTATE
};

#endif /* USB_LINK_USB_LINK_CONST_H_ */
