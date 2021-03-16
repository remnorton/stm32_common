/*
 * usb_link_types.h
 *
 *  Created on: 30 џэт. 2021 у.
 *      Author: Rem Norton
 */

#ifndef USB_LINK_USB_LINK_TYPES_H_
#define USB_LINK_USB_LINK_TYPES_H_
#include "board.h"


#pragma pack(1)

//! Main packet structure
typedef struct
{
	uint8_t		prefix;
	uint16_t	data_size;
	uint8_t		data[];
}ULINK_Pck_t;

//! Payload chunk
typedef struct
{
	uint8_t		group;
	uint8_t		id;
	uint8_t		data_size;
	uint8_t		data[];
}ULINK_Chunk_t;

//! Autosednd chunk payload
typedef struct
{
	uint8_t		param_id;
	uint16_t	send_interval;
}ULINK_Autosend_t;

//! Movement command
typedef struct
{
	uint8_t		direction;
	uint8_t		deviation;
	uint32_t	path;
	uint16_t	speed;
	uint16_t	acceleration;
	uint16_t	radius;
	uint8_t		hold;
}ULINK_Move_t;

//! Rotation command
typedef struct
{
	uint8_t		deviation;
	uint32_t	path;
	uint16_t	speed;
	uint16_t	acceleration;
	uint8_t		hold;
}ULINK_Rotation_t;

//! Stop command
typedef struct
{
	uint16_t	acceleration;
	uint8_t		hold;
}ULINK_Stop_t;

typedef struct
{
	uint8_t		param_id;
	uint8_t		mode;
	uint32_t*	value;
	//for internal use, do not modify
	uint32_t	send_time;
	uint16_t	send_interval;
}ULINK_Param_t;

typedef struct
{
	uint8_t		calib_id;
	uint8_t		mode;
	uint32_t*	value;
}ULINK_Calib_t;

#pragma pack()

//callback functions
typedef void (*ulink_param_changed_func)(uint8_t /*param_id*/);
typedef void (*ulink_drive_stop_func)(uint8_t /*drive_flags*/, uint16_t /*accel*/, uint8_t /*hold*/);
typedef void (*ulink_drive_move_func)(uint8_t /*direction*/, uint8_t /*deviation*/, uint32_t /*path*/, uint16_t /*speed*/, uint16_t /*acceleration*/, uint16_t /*radius*/, uint8_t /*hold*/);
typedef void (*ulink_drive_rotate_func)(uint8_t	/*deviation*/, uint32_t /*path*/, uint16_t /*speed*/, uint16_t /*acceleration*/, uint8_t /*hold*/);
typedef void (*ulink_screen_rotate_func)(uint8_t /*deviation*/, uint32_t /*path*/, uint16_t /*speed*/, uint16_t /*acceleration*/, uint8_t /*hold*/);

#endif /* USB_LINK_USB_LINK_TYPES_H_ */
