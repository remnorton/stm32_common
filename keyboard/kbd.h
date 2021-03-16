/*
 * kbd.h
 *
 *  Created on: 10 сент. 2019 г.
 *      Author: Rem Norton
 */

#ifndef KEYBOARD_KBD_H_
#define KEYBOARD_KBD_H_
#include "kbd_types.h"

#define _JITTER_		50
#define _MAX_EVENTS_	10

#define KEYS_ARRAY_LEN(a)	(sizeof(a)/sizeof(KBD_Key_t))

void kbd_init(KBD_Init_t* init);
void kbd_loop();

#endif /* KEYBOARD_KBD_H_ */
