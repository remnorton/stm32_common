/*
 * ring_buf.h
 *
 *  Created on: 23 мар. 2019 г.
 *      Author: Rem Norton
 */

#ifndef RING_BUF_H_
#define RING_BUF_H_
#include "board.h"

uint16_t ring_len(uint16_t head, uint16_t tail, uint16_t size);
uint16_t ring_add(uint16_t value, uint16_t shift, uint16_t size);
int16_t	ring_seek(uint16_t head, uint16_t tail, uint8_t sym, uint8_t* data, uint16_t size);
void ring_extract(uint8_t* dest, uint8_t* src, uint16_t tail, uint8_t length, uint16_t size);

#endif /* RING_BUF_H_ */
