/*
 * ring_buf.c
 *
 *  Created on: 23 мар. 2019 г.
 *      Author: Rem Norton
 */
#include "ring_buf.h"


uint16_t ring_len(uint16_t head, uint16_t tail, uint16_t size)
{
	if (head < tail) head += size;
	return head-tail;
}

uint16_t ring_add(uint16_t value, uint16_t shift, uint16_t size)
{
	uint16_t result = value + shift;
	if (result >= size) result -= size;
	return result;
}

int16_t	ring_seek(uint16_t head, uint16_t tail, uint8_t sym, uint8_t* data, uint16_t size)
{
	int16_t result = (data[tail] == sym)?tail:-1;
	while ((result < 0) && (head != tail))
	{
		tail = ring_add(tail, 1, size);
		result = (data[tail] == sym)?tail:-1;
	}
	if (result < 0)
		result = tail;

	return result;
}

void ring_extract(uint8_t* dest, uint8_t* src, uint16_t tail, uint16_t length, uint16_t size)
{
	for (int i = 0; i < length; i++)
	{
		dest[i] = src[tail];
		tail = ring_add(tail, 1, size);
	}
}
