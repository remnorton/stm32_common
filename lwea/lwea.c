#include "lwea.h"
//
//Private members
//

/**
 * @brief xor_block - Bitwise XOR for long data blocks
 * @param block_src - Source data block
 * @param block_xor - Operand data block
 * @param result - Result data block
 * @param length - Length of source data block
 */
static void lwea_xor_block(uint8_t* block_src, uint8_t* block_xor, uint8_t* result, uint8_t length)
{
    for (int i = 0; i < length; i++)
        *(result+i) = *(block_src+i) ^ *(block_xor+i);
}

/**
 * @brief chl - Cycle shift left
 * @param data - Input data
 * @param count - Shifts count
 * @return
 */
static uint32_t lwea_chl(uint32_t data, uint8_t count)
{
    uint32_t res = data;
    while (count)
    {
        uint32_t t = res & 0x01;
        res = res >> 1;
        if (t) res |= 0x80000000;
        count--;
    }
    return res;
}

/**
 * @brief chr - Cycle shift right
 * @param data - Input data
 * @param count- Shifts count
 * @return
 */
static uint32_t lwea_chr(uint32_t data, uint8_t count)
{
    uint32_t res = data;
    while (count)
    {
        uint32_t t = res & 0x80000000;
        res = res << 1;
        if (t) res |= 0x01;
        count--;
    }
    return res;
}

/**
 * @brief calc_cryptor - Covert 12 byte STM32 chip UID to 32 bit integer
 * @param uid - 12 byte STM32 chip UID
 * @return - 32 bit integer
 */
static uint32_t lwea_calc_cryptor(stm_uid uid)
{
    uint32_t res = 0;
    uint8_t idx = 0;
    while (idx < 8)
    {
        uint32_t d;
        memcpy(&d, &uid[idx], sizeof(d));
        res += d;
        idx +=2;
    }
    return res;
}

/**
 * @brief calc_shift - Calculates direction and qty. for cycle shift operation
 * @param cryptor - 32 bit block
 * @return - Direction and qty. for cycle shift operation. > 0 - shift right, < 0 - shift left
 */
static int8_t  lwea_calc_shift(uint32_t cryptor)
{
    int8_t res = 0;
    for (int i = 0; i < 8; i++)
    {
        uint8_t d = cryptor & 0x0f;

        if (d%2) res -= d;
        else res += d;

        cryptor = cryptor >> 4;
    }

    res = res%31;

    return res;
}

/**
 * @brief calc_key - Calculates encryption key
 * @param cryptor - 32 bit data block
 * @return - Encryption key
 */
static uint32_t lwea_calc_key(uint32_t cryptor)
{
    int8_t shift_code = lwea_calc_shift(cryptor);
    uint32_t res;
    if (shift_code > 0) res = lwea_chr(cryptor, shift_code);
    else res = lwea_chl(cryptor, -1*shift_code);
    return res;
}

//
//Public members
//

/**
 * @brief lwea_encript - Encrypt long data block
 * @param data - Input data block
 * @param len - Length of input data block
 * @param key - 12 byte key (STM32 chip UID)
 * @param out - Output encrypted block
 */
void lwea_encrypt(uint8_t* data, uint32_t len, stm_uid uid, uint8_t* out)
{
    uint32_t key = lwea_calc_key(lwea_calc_cryptor(uid));
    while(len)
    {
        uint32_t next_key;
        memcpy(&next_key, data, sizeof(next_key));

        uint8_t l = (len>sizeof(key))?sizeof(key):len;
        lwea_xor_block(data, (uint8_t*)&key, out, l);

        if (len >= sizeof(key)) key = lwea_calc_key(next_key);

        data += l;
        out += l;
        len -= l;
    }
}

/**
 * @brief lwea_decript - Decrypt long data block
 * @param data - Input data block
 * @param len - Length of input data block
 * @param key - 12 byte key (STM32 chip UID)
 * @param out - Output encrypted block
 */
void lwea_decrypt(uint8_t* data, uint32_t len, stm_uid uid, uint8_t* out)
{
    uint32_t key = lwea_calc_key(lwea_calc_cryptor(uid));
    while(len)
    {

        uint8_t l = (len>sizeof(key))?sizeof(key):len;
        lwea_xor_block(data, (uint8_t*)&key, out, l);
        uint32_t next_key;
        memcpy(&next_key, out, sizeof(next_key));

        if (len >= sizeof(key)) key = lwea_calc_key(next_key);

        data += l;
        out += l;
        len -= l;
    }
}
