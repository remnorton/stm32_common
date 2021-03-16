/*
 * intmath.h
 *
 *  Created on: 22 рту. 2016 у.
 *      Author: Rudenko
 */

#ifndef APP_DEVICES_TRIG_H_
#define APP_DEVICES_TRIG_H_
#include <stdint.h>

#define ABS(X)  ((X) > 0 ? (X) : -(X))

int16_t int_sin(int angle);
int16_t int_cos(int angle);
int16_t int_tan(int angle);
int16_t int_ctan(int angle);
uint32_t pow_10(int value);

uint32_t getArcX(uint32_t x, uint16_t angle, uint32_t rad);
uint32_t getArcY(uint32_t y, uint16_t angle, uint32_t rad);

#endif /* APP_DEVICES_TRIG_H_ */
