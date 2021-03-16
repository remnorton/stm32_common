/*
 * usb_link.h
 *
 *  Created on: 30 џэт. 2021 у.
 *      Author: Rem Norton
 */

#ifndef USB_LINK_USB_LINK_H_
#define USB_LINK_USB_LINK_H_
#include "usb_link_const.h"
#include "usb_link_types.h"


#ifndef ULINK_RX_BUF_SIZE
#define ULINK_RX_BUF_SIZE	2048
#endif

#ifndef ULINK_TX_BUF_SIZE
#define ULINK_TX_BUF_SIZE	2048
#endif

#ifndef ARR_LEN
#define ARR_LEN(x)	(sizeof(x)/sizeof(x[0]))
#endif


//! Control structure

typedef struct
{
	ULINK_Param_t*				parameters;
	uint8_t						param_count;
	ULINK_Calib_t*				calib;
	uint8_t						calib_count;

	ulink_param_changed_func	service_param_changed;
	ulink_param_changed_func	calib_param_changed;
	ulink_drive_stop_func		drive_stop;
	ulink_drive_move_func		drive_move;
	ulink_drive_rotate_func		drive_rotate;
	ulink_screen_rotate_func	screen_rotate;
}ULINK_t;

enum
{
	UL_DIR_LEFT = 1,
	UL_DIR_RIGHT
};


void usb_link_init(ULINK_t* lnk);
void usb_link_loop();
void usb_link_rx(uint8_t* data, uint32_t len);

#endif /* USB_LINK_USB_LINK_H_ */
