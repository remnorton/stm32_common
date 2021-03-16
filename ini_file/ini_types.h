/*
 * ini_types.h
 *
 *  Created on: 22 θών. 2020 γ.
 *      Author: Rem Norton
 */

#ifndef INI_FILE_INI_TYPES_H_
#define INI_FILE_INI_TYPES_H_

enum
{
    INI_CT_UNKNOWN = 0,
    INI_CT_PARAM,
    INI_CT_TEXT,
    INI_CT_COMMENT
};


#pragma pack(1)
typedef struct
{
    uint8_t*    name;
    uint16_t    name_len;
    List_t*     content;
    uint32_t    hash;
}INI_Section_t;

typedef struct
{
    uint8_t     type;
    uint8_t*    name;
    uint16_t    name_len;
    uint16_t    data_size;
    void*       data;
    uint32_t    hash;
}INI_Parameter_t;

typedef struct
{
    uint8_t     type;
    uint8_t*    text;
    uint16_t    size;
    uint32_t    hash;
}INI_TextBlock_t;

#pragma pack()

#endif /* INI_FILE_INI_TYPES_H_ */
