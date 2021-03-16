/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
  (C)2013 Semtech-Cycleo

Description:
    Functions used to handle FPGA register access for LoRa concentrator.
    Registers are addressed by name.
    Multi-bytes registers are handled automatically.
    Read-modify-write is handled automatically.

License: Revised BSD License, see LICENSE.TXT file include in the project
Maintainer: Michael Coracin
*/

/* -------------------------------------------------------------------------- */
/* --- DEPENDANCIES --------------------------------------------------------- */

#include <stdint.h>     /* C99 types */
#include <stdbool.h>    /* bool type */
#include <stdio.h>      /* printf fprintf */

#include "loragw_spi.h"
#include "loragw_aux.h"
#include "loragw_hal.h"
#include "loragw_reg.h"
#include "loragw_fpga.h"

/* -------------------------------------------------------------------------- */
/* --- PRIVATE MACROS ------------------------------------------------------- */

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#if DEBUG_REG == 1
    #define DEBUG_MSG(str)                fprintf(stderr, str)
    #define DEBUG_PRINTF(fmt, args...)    fprintf(stderr,fmt,args)
    #define CHECK_NULL(a)                if(a==NULL){fprintf(stderr,"%s:%d: ERROR: NULL POINTER AS ARGUMENT\n", __FUNCTION__, __LINE__);return LGW_REG_ERROR;}
#else
    #define DEBUG_MSG(str)
    #define DEBUG_PRINTF(fmt, args...)
    #define CHECK_NULL(a)                if(a==NULL){return LGW_REG_ERROR;}
#endif

/* -------------------------------------------------------------------------- */
/* --- PRIVATE CONSTANTS ---------------------------------------------------- */

/*
auto generated register mapping for C code : 11-Jul-2013 13:20:40
this file contains autogenerated C struct used to access the LoRa register from the Primer firmware
this file is autogenerated from registers description
293 registers are defined
*/
const struct lgw_reg_s fpga_regs[LGW_FPGA_TOTALREGS] = {
    {-1,0,0,0,1,0,0}, /* SOFT_RESET */
    {-1,0,1,0,4,1,0}, /* FPGA_FEATURE */
    {-1,0,5,0,3,1,0}, /* LBT_INITIAL_FREQ */
    {-1,1,0,0,8,1,0}, /* VERSION */
    {-1,2,0,0,8,1,0}, /* FPGA_STATUS */
    {-1,3,0,0,1,0,0}, /* FPGA_CTRL_FEATURE_START */
    {-1,3,1,0,1,0,0}, /* FPGA_CTRL_RADIO_RESET */
    {-1,3,2,0,1,0,0}, /* FPGA_CTRL_INPUT_SYNC_I */
    {-1,3,3,0,1,0,0}, /* FPGA_CTRL_INPUT_SYNC_Q */
    {-1,3,4,0,1,0,0}, /* FPGA_CTRL_OUTPUT_SYNC */
    {-1,3,5,0,1,0,0}, /* FPGA_CTRL_INVERT_IQ */
    {-1,3,6,0,1,0,0}, /* FPGA_CTRL_ACCESS_HISTO_MEM */
    {-1,3,7,0,1,0,0}, /* FPGA_CTRL_CLEAR_HISTO_MEM */
    {-1,4,0,0,8,0,0}, /* HISTO_RAM_ADDR */
    {-1,5,0,0,8,1,0}, /* HISTO_RAM_DATA */
    {-1,8,0,0,16,0,1000}, /* HISTO_NB_READ */
    {-1,14,0,0,16,1,0}, /* LBT_TIMESTAMP_CH */
    {-1,17,0,0,4,0,0}, /* LBT_TIMESTAMP_SELECT_CH */
    {-1,18,0,0,8,0,0}, /* LBT_CH0_FREQ_OFFSET */
    {-1,19,0,0,8,0,0}, /* LBT_CH1_FREQ_OFFSET */
    {-1,20,0,0,8,0,0}, /* LBT_CH2_FREQ_OFFSET */
    {-1,21,0,0,8,0,0}, /* LBT_CH3_FREQ_OFFSET */
    {-1,22,0,0,8,0,0}, /* LBT_CH4_FREQ_OFFSET */
    {-1,23,0,0,8,0,0}, /* LBT_CH5_FREQ_OFFSET */
    {-1,24,0,0,8,0,0}, /* LBT_CH6_FREQ_OFFSET */
    {-1,25,0,0,8,0,0}, /* LBT_CH7_FREQ_OFFSET */
    {-1,26,0,0,8,0,0}, /* SCAN_FREQ_OFFSET */
    {-1,28,0,0,1,0,0}, /* LBT_SCAN_TIME_CH0 */
    {-1,28,1,0,1,0,0}, /* LBT_SCAN_TIME_CH1 */
    {-1,28,2,0,1,0,0}, /* LBT_SCAN_TIME_CH2 */
    {-1,28,3,0,1,0,0}, /* LBT_SCAN_TIME_CH3 */
    {-1,28,4,0,1,0,0}, /* LBT_SCAN_TIME_CH4 */
    {-1,28,5,0,1,0,0}, /* LBT_SCAN_TIME_CH5 */
    {-1,28,6,0,1,0,0}, /* LBT_SCAN_TIME_CH6 */
    {-1,28,7,0,1,0,0}, /* LBT_SCAN_TIME_CH7 */
    {-1,30,0,0,8,0,160}, /* RSSI_TARGET */
    {-1,31,0,0,24,0,0}, /* HISTO_SCAN_FREQ */
    {-1,34,0,0,6,0,0} /* NOTCH_FREQ_OFFSET */
};

/* -------------------------------------------------------------------------- */
/* --- INTERNAL SHARED VARIABLES -------------------------------------------- */

extern void *lgw_spi_target; /*! generic pointer to the SPI device */
extern uint8_t lgw_spi_mux_mode; /*! current SPI mux mode used */

/* -------------------------------------------------------------------------- */
/* --- PRIVATE VARIABLES ---------------------------------------------------- */
static bool tx_notch_support = false;
static uint8_t tx_notch_offset;

/* -------------------------------------------------------------------------- */
/* --- PRIVATE FUNCTIONS ---------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* --- PRIVATE FUNCTIONS DEFINITION ----------------------------------------- */

/* -------------------------------------------------------------------------- */
/* --- PUBLIC FUNCTIONS DEFINITION ------------------------------------------ */

float lgw_fpga_get_tx_notch_delay(void) {
    float tx_notch_delay;

    if (tx_notch_support == false) {
        return 0;
    }

    /* Notch filtering performed by FPGA adds a constant delay (group delay) that we need to compensate */
    tx_notch_delay = (31.25 * ((64 + tx_notch_offset) / 2)) / 1E3; /* 32MHz => 31.25ns */

    return tx_notch_delay;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

int lgw_fpga_configure(uint32_t tx_notch_freq) {
    int x;
    int32_t val;
    bool spectral_scan_support, lbt_support;

    /* Check input parameters */
    if ((tx_notch_freq < LGW_MIN_NOTCH_FREQ) || (tx_notch_freq > LGW_MAX_NOTCH_FREQ)) {
        DEBUG_PRINTF("WARNING: FPGA TX notch frequency is out of range (%u - [%u..%u]), setting it to default (%u)\n", tx_notch_freq, LGW_MIN_NOTCH_FREQ, LGW_MAX_NOTCH_FREQ, LGW_DEFAULT_NOTCH_FREQ);
        tx_notch_freq = LGW_DEFAULT_NOTCH_FREQ;
    }

    /* Get supported FPGA features */
    printf("INFO: FPGA supported features:");
    lgw_fpga_reg_r(LGW_FPGA_FEATURE, &val);
    tx_notch_support = TAKE_N_BITS_FROM((uint8_t)val, 0, 1);
    if (tx_notch_support == true) {
        printf(" [TX filter] ");
    }
    spectral_scan_support = TAKE_N_BITS_FROM((uint8_t)val, 1, 1);
    if (spectral_scan_support == true) {
        printf(" [Spectral Scan] ");
    }
    lbt_support = TAKE_N_BITS_FROM((uint8_t)val, 2, 1);
    if (lbt_support == true) {
        printf(" [LBT] ");
    }
    printf("\n");

    x  = lgw_fpga_reg_w(LGW_FPGA_CTRL_INPUT_SYNC_I, 1);
    x |= lgw_fpga_reg_w(LGW_FPGA_CTRL_INPUT_SYNC_Q, 1);
    x |= lgw_fpga_reg_w(LGW_FPGA_CTRL_OUTPUT_SYNC, 0);
    if (x != LGW_REG_SUCCESS) {
        DEBUG_MSG("ERROR: Failed to configure FPGA TX synchro\n");
        return LGW_REG_ERROR;
    }

    /* Required for Semtech AP2 reference design */
    x  = lgw_fpga_reg_w(LGW_FPGA_CTRL_INVERT_IQ, 1);
    if (x != LGW_REG_SUCCESS) {
        DEBUG_MSG("ERROR: Failed to configure FPGA polarity\n");
        return LGW_REG_ERROR;
    }

    /* Configure TX notch filter */
    if (tx_notch_support == true) {
        tx_notch_offset = (32E6 / (2*tx_notch_freq)) - 64;
        x = lgw_fpga_reg_w(LGW_FPGA_NOTCH_FREQ_OFFSET, (int32_t)tx_notch_offset);
        if (x != LGW_REG_SUCCESS) {
            DEBUG_MSG("ERROR: Failed to configure FPGA TX notch filter\n");
            return LGW_REG_ERROR;
        }

        /* Readback to check that notch frequency is programmable */
        x = lgw_fpga_reg_r(LGW_FPGA_NOTCH_FREQ_OFFSET, &val);
        if (x != LGW_REG_SUCCESS) {
            DEBUG_MSG("ERROR: Failed to read FPGA TX notch frequency\n");
            return LGW_REG_ERROR;
        }
        if (val != tx_notch_offset) {
            DEBUG_MSG("WARNING: TX notch filter frequency is not programmable (check your FPGA image)\n");
        } else {
            DEBUG_PRINTF("INFO: TX notch filter frequency set to %u (%i)\n", tx_notch_freq, tx_notch_offset);
        }
    }

    return LGW_REG_SUCCESS;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Write to a register addressed by name */
int lgw_fpga_reg_w(uint16_t register_id, int32_t reg_value) {
    int spi_stat = LGW_SPI_SUCCESS;
    struct lgw_reg_s r;

    /* check input parameters */
    if (register_id >= LGW_FPGA_TOTALREGS) {
        DEBUG_MSG("ERROR: REGISTER NUMBER OUT OF DEFINED RANGE\n");
        return LGW_REG_ERROR;
    }

    /* check if SPI is initialised */
    if (lgw_spi_target == NULL) {
        DEBUG_MSG("ERROR: CONCENTRATOR UNCONNECTED\n");
        return LGW_REG_ERROR;
    }

    /* get register struct from the struct array */
    r = fpga_regs[register_id];

    /* reject write to read-only registers */
    if (r.rdon == 1){
        DEBUG_MSG("ERROR: TRYING TO WRITE A READ-ONLY REGISTER\n");
        return LGW_REG_ERROR;
    }

    spi_stat += reg_w_align32(lgw_spi_target, LGW_SPI_MUX_MODE1, LGW_SPI_MUX_TARGET_FPGA, r, reg_value);

    if (spi_stat != LGW_SPI_SUCCESS) {
        DEBUG_MSG("ERROR: SPI ERROR DURING REGISTER WRITE\n");
        return LGW_REG_ERROR;
    } else {
        return LGW_REG_SUCCESS;
    }
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Read to a register addressed by name */
int lgw_fpga_reg_r(uint16_t register_id, int32_t *reg_value) {
    int spi_stat = LGW_SPI_SUCCESS;
    struct lgw_reg_s r;

    /* check input parameters */
    CHECK_NULL(reg_value);
    if (register_id >= LGW_FPGA_TOTALREGS) {
        DEBUG_MSG("ERROR: REGISTER NUMBER OUT OF DEFINED RANGE\n");
        return LGW_REG_ERROR;
    }

    /* check if SPI is initialised */
    if (lgw_spi_target == NULL) {
        DEBUG_MSG("ERROR: CONCENTRATOR UNCONNECTED\n");
        return LGW_REG_ERROR;
    }

    /* get register struct from the struct array */
    r = fpga_regs[register_id];

    spi_stat += reg_r_align32(lgw_spi_target, LGW_SPI_MUX_MODE1, LGW_SPI_MUX_TARGET_FPGA, r, reg_value);

    if (spi_stat != LGW_SPI_SUCCESS) {
        DEBUG_MSG("ERROR: SPI ERROR DURING REGISTER WRITE\n");
        return LGW_REG_ERROR;
    } else {
        return LGW_REG_SUCCESS;
    }
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Point to a register by name and do a burst write */
int lgw_fpga_reg_wb(uint16_t register_id, uint8_t *data, uint16_t size) {
    int spi_stat = LGW_SPI_SUCCESS;
    struct lgw_reg_s r;

    /* check input parameters */
    CHECK_NULL(data);
    if (size == 0) {
        DEBUG_MSG("ERROR: BURST OF NULL LENGTH\n");
        return LGW_REG_ERROR;
    }
    if (register_id >= LGW_FPGA_TOTALREGS) {
        DEBUG_MSG("ERROR: REGISTER NUMBER OUT OF DEFINED RANGE\n");
        return LGW_REG_ERROR;
    }

    /* check if SPI is initialised */
    if (lgw_spi_target == NULL) {
        DEBUG_MSG("ERROR: CONCENTRATOR UNCONNECTED\n");
        return LGW_REG_ERROR;
    }

    /* get register struct from the struct array */
    r = fpga_regs[register_id];

    /* reject write to read-only registers */
    if (r.rdon == 1){
        DEBUG_MSG("ERROR: TRYING TO BURST WRITE A READ-ONLY REGISTER\n");
        return LGW_REG_ERROR;
    }

    /* do the burst write */
    spi_stat += lgw_spi_wb(lgw_spi_target, LGW_SPI_MUX_MODE1, LGW_SPI_MUX_TARGET_FPGA, r.addr, data, size);

    if (spi_stat != LGW_SPI_SUCCESS) {
        DEBUG_MSG("ERROR: SPI ERROR DURING REGISTER BURST WRITE\n");
        return LGW_REG_ERROR;
    } else {
        return LGW_REG_SUCCESS;
    }
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Point to a register by name and do a burst read */
int lgw_fpga_reg_rb(uint16_t register_id, uint8_t *data, uint16_t size) {
    int spi_stat = LGW_SPI_SUCCESS;
    struct lgw_reg_s r;

    /* check input parameters */
    CHECK_NULL(data);
    if (size == 0) {
        DEBUG_MSG("ERROR: BURST OF NULL LENGTH\n");
        return LGW_REG_ERROR;
    }
    if (register_id >= LGW_FPGA_TOTALREGS) {
        DEBUG_MSG("ERROR: REGISTER NUMBER OUT OF DEFINED RANGE\n");
        return LGW_REG_ERROR;
    }

    /* check if SPI is initialised */
    if (lgw_spi_target == NULL) {
        DEBUG_MSG("ERROR: CONCENTRATOR UNCONNECTED\n");
        return LGW_REG_ERROR;
    }

    /* get register struct from the struct array */
    r = fpga_regs[register_id];

    /* do the burst read */
    spi_stat += lgw_spi_rb(lgw_spi_target, LGW_SPI_MUX_MODE1, LGW_SPI_MUX_TARGET_FPGA, r.addr, data, size);

    if (spi_stat != LGW_SPI_SUCCESS) {
        DEBUG_MSG("ERROR: SPI ERROR DURING REGISTER BURST READ\n");
        return LGW_REG_ERROR;
    } else {
        return LGW_REG_SUCCESS;
    }
}

/* --- EOF ------------------------------------------------------------------ */
