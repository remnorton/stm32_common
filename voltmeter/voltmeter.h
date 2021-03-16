/*
 * voltmeter.h
 *
 *  Created on: 23 θών. 2017 γ.
 *      Author: Rem Norton
 */

#ifndef APP_DEVICE_VOLTMETER_H_
#define APP_DEVICE_VOLTMETER_H_

#include "board.h"

typedef struct
{
	uint16_t*	adc_data;
	uint32_t*	voltage;
	uint16_t	ref_voltage;
	uint32_t	max_adc_value;
	uint16_t	r1;
	uint16_t	r2;
}VMETER_Int_t;


void vmeter_init(VMETER_Int_t* init);
void vmeter_step();

#endif /* APP_DEVICE_VOLTMETER_H_ */
