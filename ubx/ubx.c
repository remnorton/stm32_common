/*
 * ubx.cpp
 *
 *  Created on: 25.08.2018
 *		Author:  Rem Norton
 *
 */
#include <strings.h>
#include <stdlib.h>
#include "ubx.h"

ubx_crc_t ubx_crc(uint8_t* data, uint16_t length)
{
    uint8_t CK_A = 0;
    uint8_t CK_B = 0;

    for (int i = 0; i < length; i++)
    {
        CK_A += (uint8_t)data[i];
        CK_B += CK_A;
    }
    return ((CK_B << 8) | CK_A);
}


uint16_t ubx_make_pck(uint8_t* dest, uint8_t class_id, uint8_t mess_id, uint8_t* data, uint16_t data_size)
{
    uint8_t res = 0;
    if (dest && data)
    {
        UBX_HEADER_t* header = (UBX_HEADER_t*)dest;
        header->signature = UBX_SIGNATURE;
        header->id.detail.class_id = class_id;
        header->id.detail.mess_id = mess_id;
        header->size = data_size;

        res = sizeof(UBX_HEADER_t);
        memcpy(dest+res, data, header->size);
        res += header->size;
        uint16_t* crc = (uint16_t*)(dest+res);
        *crc = ubx_crc(dest+sizeof(header->signature), sizeof(UBX_HEADER_t)-sizeof(header->signature)+header->size);
        res += sizeof(crc);
    }

    return res;
}
