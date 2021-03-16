/*
 * kbd_types.h
 *
 *  Created on: 10 сент. 2019 г.
 *      Author: Rem Norton
 */

#ifndef KEYBOARD_KBD_TYPES_H_
#define KEYBOARD_KBD_TYPES_H_
#include "board.h"

typedef struct
{
	uint8_t		type;
	uint8_t		id;
}KBD_Event_t;

typedef void (*kbd_event)(KBD_Event_t*, /*list*/ uint8_t /*count*/);

typedef struct
{
	GPIO_TypeDef*	port;
	uint16_t		pin;
	GPIO_PinState	activeState;
}KBD_Pin_t;

typedef struct
{
	KBD_Pin_t		pin;
	uint8_t			id;
	GPIO_PinState	prev_state;
	uint32_t		state_change_time;
}KBD_Key_t;


typedef struct
{
	KBD_Key_t*			keys;
	uint8_t				key_count;
	kbd_event			event_handler;
}KBD_Init_t;

//! Keyboard event type
enum
{
	KBD_EVT_PRESSED = 1,
	KBD_EVT_RELEASED
};

#endif /* KEYBOARD_KBD_TYPES_H_ */
