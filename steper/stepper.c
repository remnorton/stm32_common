/*
 * stepper.c
 *
 *  Created on: 25 февр. 2021 г.
 *      Author: Rem Norton
 */
#include <string.h>
#include "stepper.h"

//Private forwards

void stepper_calc_timer_data(Stepper_Drive_t* drive, Stepper_Task_t* task, uint32_t freq);

void stepper_set_direction(Stepper_Drive_t* drive, uint8_t dir);

//Public members
uint8_t	stepper_init(Stepper_Drive_t* drive)
{
	//Validate control structure
	if (!drive)
		return 0;
	if (!drive->enable.port || !drive->direction.port || !drive->timer || !drive->steps_per_lap)
		return 0;

	drive->task_list_head = 0;

	//Disable driver chip
	HAL_GPIO_WritePin(drive->enable.port, drive->enable.pin, !drive->enable.active_state);
	drive->state = DS_IDLE;
	drive->error = 0;
	return 1;
}

uint8_t	stepper_add_motion_task(Stepper_Drive_t* drive, uint16_t mres, uint32_t accel, uint32_t speed, uint32_t path, uint8_t direction, uint8_t hold)
{
	uint32_t time_to_accel = speed*1000/accel;
	uint16_t accel_phase_time = 100; //mSec
	uint16_t accel_phase_count = time_to_accel / accel_phase_time;
	uint32_t total_steps_in_path = path?(path*drive->steps_per_lap*mres/MRAD_PER_LAP):0xffffffff;
	uint32_t steps_for_accel = 0;
	uint32_t motion_freq = speed*drive->steps_per_lap*mres/MRAD_PER_LAP;

	if ((accel_phase_count & 0x01) == 0)
		accel_phase_count++;

	if ((2*accel_phase_count) > drive->tasks_list_len)
		return 0;

	//Make clear phase time
	accel_phase_time = time_to_accel / accel_phase_count;

	Stepper_Task_t* task = 0;
	uint32_t remain_steps_for_motion = total_steps_in_path;
	uint32_t steps_lost = 0;

	//Calculate acceleration
	for (int p = 0; (p < accel_phase_count) && remain_steps_for_motion; p++)
	{
		task = &drive->tasks[drive->task_list_head];
		uint16_t mrad_speed = accel*(p+1)*accel_phase_time/1000;
		uint16_t step_speed = mrad_speed*drive->steps_per_lap*mres/MRAD_PER_LAP;
		uint32_t steps_in_phase = accel_phase_time*step_speed/1000;

		if (remain_steps_for_motion < (2*steps_in_phase))
			steps_in_phase = remain_steps_for_motion/2;

		if (steps_in_phase)
		{
			remain_steps_for_motion -= 2*steps_in_phase;

			steps_for_accel += steps_in_phase;

			task->count = steps_in_phase;
			stepper_calc_timer_data(drive, task, step_speed);
			drive->task_list_head++;
		}
		else
		{
			steps_lost = remain_steps_for_motion;
			remain_steps_for_motion = 0;
		}
	}

	if (path)
	{
		//calculate motion
		if (remain_steps_for_motion)
		{
			task = &drive->tasks[drive->task_list_head++];
			task->count = remain_steps_for_motion;
			stepper_calc_timer_data(drive, task, motion_freq);
		}
		else //Make clear acceleration phase count
			accel_phase_count = drive->task_list_head;

		//copy deceleration
		while(accel_phase_count)
		{
			accel_phase_count--;
			task = &drive->tasks[drive->task_list_head++];
			memcpy(task, &drive->tasks[accel_phase_count], sizeof(Stepper_Task_t));
			if (steps_lost)
			{
				task->count += steps_lost;
				steps_lost = 0;
			}
		}
	}
	else //Infinite motion
	{
		task = &drive->tasks[drive->task_list_head];
		task->count = 0;
		stepper_calc_timer_data(drive, task, motion_freq);
	}

	drive->current_direction = direction;
	drive->current_hold = hold;

	return 1;
}

uint8_t stepper_add_stop_task(Stepper_Drive_t* drive, uint16_t mres, uint32_t accel, uint8_t hold)
{
	if (!drive)
		return 0;
	if (!drive->tasks || !drive->task_list_head)
		return 0;

	Stepper_Task_t* task = &drive->tasks[drive->task_list_head];
	uint32_t freq = drive->timer_clock/(task->prescaler*task->period);
	uint32_t speed = (freq*MRAD_PER_LAP)/(drive->steps_per_lap*mres);

	uint32_t time_to_accel = speed*1000/accel;
	uint16_t accel_phase_time = 100; //mSec
	uint16_t accel_phase_count = time_to_accel / accel_phase_time;

	if ((accel_phase_count & 0x01) == 0)
		accel_phase_count++;

	while (accel_phase_count)
	{
		drive->task_list_head++;
		task = &drive->tasks[drive->task_list_head];
		uint16_t mrad_speed = accel*accel_phase_count*accel_phase_time/1000;
		uint16_t step_speed = mrad_speed*drive->steps_per_lap*mres/MRAD_PER_LAP;
		uint32_t steps_in_phase = accel_phase_time*step_speed/1000;

		task->count = steps_in_phase;
		stepper_calc_timer_data(drive, task, step_speed);

		accel_phase_count--;
	}


	return 1;
}

uint8_t stepper_stop_motion(Stepper_Drive_t* drive, uint16_t mres, uint32_t accel, uint8_t hold)
{
	if (!drive)
		return 0;
	if (!drive->tasks || !drive->task_list_head)
		return 0;

	drive->current_task_step_count = 0;

	drive->task_list_head = drive->cur_task_idx - 1;
	stepper_add_stop_task(drive, mres, accel, hold);
	drive->current_task_step_count = 1;
	drive->current_hold = hold;

	return 1;
}

uint8_t stepper_exec_task(Stepper_Drive_t* drive)
{
	Stepper_Task_t* task = &drive->tasks[drive->cur_task_idx];
	uint8_t infin = (!task->count && task->period)?1:0;

	if ((drive->cur_task_idx >= drive->task_list_head) && !infin)
		return 0;

	drive->current_task_step_count = task->count;

	__HAL_TIM_SET_COMPARE(drive->timer, drive->step_channel, task->period/2-1);
	__HAL_TIM_SET_AUTORELOAD(drive->timer, task->period-1);
	__HAL_TIM_SET_PRESCALER(drive->timer, task->prescaler-1);

	if (!drive->cur_task_idx)
	{
		stepper_set_direction(drive, drive->current_direction);
		stepper_enable(drive, 1);
		HAL_TIM_PWM_Start_IT(drive->timer, drive->step_channel);
		drive->state = DS_IN_MOTION;
	}
	drive->cur_task_idx++;
	return 1;
}

uint8_t stepper_step(Stepper_Drive_t* drive)
{
	drive->total_step_count++;
	if (drive->current_task_step_count > 0)
	{
		if( --drive->current_task_step_count == 0)
		{
			if (!stepper_exec_task(drive))
			{
				HAL_TIM_PWM_Stop_IT(drive->timer, drive->step_channel);
				stepper_enable(drive, drive->current_hold);
				if (drive->current_hold && drive->hold_timeout)
					drive->hold_time = HAL_GetTick();


				drive->cur_task_idx = drive->task_list_head = 0;
				drive->state = drive->current_hold?DS_HOLD:DS_IDLE;
			}
		}
	}

	return 1;
}

void stepper_enable(Stepper_Drive_t* drive, uint8_t enable)
{
	if (!drive)
		return;
	if (!drive->enable.port)
		return;
	HAL_GPIO_WritePin(drive->enable.port, drive->enable.pin, enable?drive->enable.active_state:!drive->enable.active_state);
}

void stepper_loop(Stepper_Drive_t* drive)
{
	if (!drive)
		return;
	if (!drive->enable.port)
		return;

	if (drive->hold_time && ((HAL_GetTick() - drive->hold_time) >= drive->hold_timeout))
	{
		if (drive->state == DS_HOLD)
			stepper_enable(drive, 0);
		drive->hold_time = 0;
	}
}

//Private members
inline void stepper_calc_timer_data(Stepper_Drive_t* drive, Stepper_Task_t* task, uint32_t freq)
{
	task->period = 10000;
	task->prescaler = drive->timer_clock/(freq * task->period);

	task->period = drive->timer_clock/(freq * task->prescaler);

}

inline void stepper_set_direction(Stepper_Drive_t* drive, uint8_t dir)
{
	HAL_GPIO_WritePin(drive->direction.port, drive->direction.pin, dir?drive->direction.active_state:!drive->direction.active_state);
}
