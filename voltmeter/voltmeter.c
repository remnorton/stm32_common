/*
 * voltmeter.c
 *
 *  Created on: 23 θών. 2017 γ.
 *      Author: Rem Norton
 */
#include "voltmeter.h"

VMETER_Int_t* vmeter = 0;

void vmeter_init(VMETER_Int_t* init)
{
	vmeter = init;
}

void vmeter_step()
{
	if (!vmeter) return;
	if (!vmeter->adc_data || !vmeter->voltage) return;

	uint32_t volt_in = ((*vmeter->adc_data)*vmeter->ref_voltage)/vmeter->max_adc_value;

	if (vmeter->r1 && vmeter->r2)
		*(vmeter->voltage) = (volt_in*(vmeter->r1+vmeter->r2))/vmeter->r2;
	else
		*(vmeter->voltage) = volt_in;
}
