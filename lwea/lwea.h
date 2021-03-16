#ifndef LWEA_H
#define LWEA_H

/***********************************
 * Lightweight encryption algorithm
 * Copyright (c) 2019
 ***********************************/

#include <stdint.h>

typedef uint8_t stm_uid[12];

void lwea_encrypt(uint8_t* data, uint32_t len, stm_uid uid, uint8_t* out);
void lwea_decrypt(uint8_t* data, uint32_t len, stm_uid uid, uint8_t* out);

#endif // LWEA_H
