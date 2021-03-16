/*
 * stepper.h
 *
 *  Created on: 25 февр. 2021 г.
 *      Author: Rem Norton
 */

#ifndef STEPER_STEPPER_H_
#define STEPER_STEPPER_H_
#include "board.h"

#define MRAD_PER_LAP	6283

typedef struct
{
	uint32_t	count;
	uint16_t	prescaler;
	uint16_t	period;
}Stepper_Task_t;

typedef struct
{
	GPIO_TypeDef* 	port;
	uint16_t 		pin;
	GPIO_PinState 	active_state;
}Stepper_GPIO_t;

typedef struct
{
	TIM_HandleTypeDef*	timer;
	uint32_t			step_channel;
	Stepper_GPIO_t		enable;
	Stepper_GPIO_t		direction;
	uint32_t			timer_clock;
	uint16_t			hold_timeout;
	Stepper_Task_t*		tasks;
	uint16_t			tasks_list_len;
	uint16_t			steps_per_lap;
	uint8_t				current_direction;
	uint8_t				current_hold;
	//Read only flags
	uint8_t				error;
	uint8_t				state;
	//For internal use only, do not modify
	uint16_t			task_list_head;
	uint16_t			cur_task_idx;
	uint32_t			total_step_count;
	uint32_t			current_task_step_count;
	uint32_t			hold_time;
}Stepper_Drive_t;


//!Drive state
enum
{
	DS_IDLE = 0,
	DS_HOLD,
	DS_IN_MOTION,
};

uint8_t	stepper_init(Stepper_Drive_t* drive);
uint8_t	stepper_add_motion_task(Stepper_Drive_t* drive, uint16_t mres, uint32_t accel, uint32_t speed, uint32_t path, uint8_t direction, uint8_t hold);
uint8_t stepper_add_stop_task(Stepper_Drive_t* drive, uint16_t mres, uint32_t accel, uint8_t hold);
uint8_t stepper_stop_motion(Stepper_Drive_t* drive, uint16_t mres, uint32_t accel, uint8_t hold);
uint8_t stepper_exec_task(Stepper_Drive_t* drive);

uint8_t stepper_step(Stepper_Drive_t* drive);
void	stepper_enable(Stepper_Drive_t* drive, uint8_t enable);

void 	stepper_loop(Stepper_Drive_t* drive);

#endif /* STEPER_STEPPER_H_ */
