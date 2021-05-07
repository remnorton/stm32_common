/*
 * voltmeter.c
 *
 *  Created on: 23 θών. 2017 γ.
 *      Author: Rem Norton
 */
#include <math.h>
#include "voltmeter.h"

#define ADC_WINDOW	20
uint16_t raw_data[ADC_WINDOW];
uint32_t prev_adc_data = 0;
VMETER_Int_t* vmeter = 0;

//Private forwards
uint32_t filter(uint32_t prev, uint32_t cur, uint8_t rate);


//Public members
void vmeter_init(VMETER_Int_t* init)
{
	vmeter = init;
	memset(raw_data, 0, sizeof(raw_data));
}

void vmeter_calib(uint16_t voltage)
{
	if (!vmeter) return;
	if (!vmeter->r1 || !vmeter->r2) return;

	uint32_t adc_data = 0;
	for(int i = 0; i < ADC_WINDOW; i++)
		adc_data += raw_data[i];

	adc_data = adc_data / ADC_WINDOW;
	adc_data = filter(prev_adc_data, adc_data, vmeter->filter_rate);

	uint32_t volt_in = (adc_data*vmeter->ref_voltage)/vmeter->max_adc_value;

	vmeter->r1 = voltage*vmeter->r2/volt_in - vmeter->r2;

	vmeter->r1 = (vmeter->r1/10 + (((vmeter->r1 % 10) > 4)?1:0))*10;

}

void vmeter_step()
{
	if (!vmeter) return;
	if (!vmeter->adc_data || !vmeter->voltage) return;

	if (!raw_data[ADC_WINDOW-1])
		for (int i = 0; i < ADC_WINDOW; i++)
			raw_data[i] = (*vmeter->adc_data);

	memcpy(raw_data, &raw_data[1], (ADC_WINDOW-1)*sizeof(raw_data[0]));
	raw_data[ADC_WINDOW-1] = (*vmeter->adc_data);

	uint32_t adc_data = 0;
	for(int i = 0; i < ADC_WINDOW; i++)
		adc_data += raw_data[i];

	adc_data = adc_data / ADC_WINDOW;

	if (!prev_adc_data)
		prev_adc_data = adc_data;

	adc_data = (prev_adc_data+adc_data)/2;
	adc_data = filter(prev_adc_data, adc_data, vmeter->filter_rate);

	uint32_t volt_in = (adc_data*vmeter->ref_voltage)/vmeter->max_adc_value;


	if (vmeter->r1 && vmeter->r2)
		*(vmeter->voltage) = (volt_in*(vmeter->r1+vmeter->r2))/vmeter->r2;
	else
		*(vmeter->voltage) = volt_in;
}


//Private members
uint32_t filter(uint32_t prev, uint32_t cur, uint8_t rate)
{
	return (cur*(100-rate) + prev*rate)/100;
}
