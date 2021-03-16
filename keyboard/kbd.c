/*
 * kbd.c
 *
 *  Created on: 10 сент. 2019 г.
 *      Author: Rem Norton
 */

#include "kbd.h"

KBD_Init_t* instance = 0;

void kbd_init(KBD_Init_t* init)
{
	if (init)
	{
		if (init->key_count && init->keys)
		{
			for (int i = 0; i < init->key_count; i++)
			{
				init->keys[i].prev_state = HAL_GPIO_ReadPin(init->keys[i].pin.port, init->keys[i].pin.pin);
				init->keys[i].state_change_time = 0;
			}
			instance = init;
		}
	}
}

void kbd_loop()
{
	if (!instance) return;

	KBD_Event_t events[_MAX_EVENTS_] = {0};
	uint8_t evt_idx = 0;

	for (int i = 0; (i < instance->key_count) && (evt_idx < _MAX_EVENTS_); i++)
	{
		GPIO_PinState state = HAL_GPIO_ReadPin(instance->keys[i].pin.port, instance->keys[i].pin.pin);
		if (state != instance->keys[i].prev_state)
		{
			if (!instance->keys[i].state_change_time) instance->keys[i].state_change_time = HAL_GetTick();
			else
			{
				if (HAL_GetTick() > (instance->keys[i].state_change_time + _JITTER_))
				{
					instance->keys[i].state_change_time = 0;
					instance->keys[i].prev_state = state;
					events[evt_idx++] = (KBD_Event_t){(uint8_t)state, instance->keys[i].id};
				}
			}
		}
	}

	if (instance->event_handler && evt_idx) instance->event_handler(events, evt_idx);
}
