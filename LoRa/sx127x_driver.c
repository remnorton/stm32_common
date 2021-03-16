/*
 * sx127x_driver.c
 *
 *  Created on: 20 сент. 2018 г.
 *      Author: User
 */
#include "sx127x_driver.h"
#include "lora_driver.h"
//
//

//! Common registers
#define REG_SX127x_FIFO                 0x00
#define REG_SX127x_OP_MODE              0x01
#define REG_SX127x_FRF_MSB              0x06
#define REG_SX127x_FRF_MID              0x07
#define REG_SX127x_FRF_LSB              0x08
#define REG_SX127x_PA_CONFIG            0x09
#define REG_SX127x_PA_RAMP              0x0a
#define REG_SX127x_LNA                  0x0c
#define REG_SX127x_OCP                  0x0b
#define REG_SX127x_DIO_MAPPING_1        0x40
#define REG_SX127x_DIO_MAPPING_2        0x41
#define REG_SX127x_VERSION              0x42
#define REG_SX127x_XTA					0x4b
#define REG_SX127x_HIPOWER              0x4d

//! LoRa registers
#define REG_SX127x_FIFO_ADDR_PTR        0x0d
#define REG_SX127x_FIFO_TX_BASE_ADDR    0x0e
#define REG_SX127x_FIFO_RX_BASE_ADDR    0x0f
#define REG_SX127x_FIFO_RX_CURRENT_ADDR 0x10
#define REG_SX127x_IRQ_FLAG_MASK        0x11
#define REG_SX127x_IRQ_FLAGS            0x12
#define REG_SX127x_RX_NB_BYTES          0x13
#define REG_SX127x_MODEM_STATUS			0x18
#define REG_SX127x_PKT_SNR_VALUE        0x19
#define REG_SX127x_PKT_RSSI_VALUE       0x1a
#define REG_SX127x_CUR_RSSI_VALUE		0x1b
#define REG_SX127x_MODEM_CONFIG_1       0x1d
#define REG_SX127x_MODEM_CONFIG_2       0x1e
#define REG_SX127x_PREAMBLE_MSB         0x20
#define REG_SX127x_PREAMBLE_LSB         0x21
#define REG_SX127x_PAYLOAD_LENGTH       0x22
#define REG_SX127x_MAX_PAYLOAD_LENGTH   0x23
#define REG_SX127x_FIFO_RX_TAIL         0x25
#define REG_SX127x_MODEM_CONFIG_3       0x26
#define REG_SX127x_PPM_RATIO			0x27
#define REG_SX127x_FREQ_ERROR_MSB       0x28
#define REG_SX127x_FREQ_ERROR_MID       0x29
#define REG_SX127x_FREQ_ERROR_LSB       0x2a
#define REG_SX127x_RSSI_WIDEBAND        0x2c
#define REG_SX127x_DETECTION_OPTIMIZE   0x31
#define REG_SX127x_INVERT_IQ            0x33
#define REG_SX127x_DETECTION_THRESHOLD  0x37
#define REG_SX127x_SYNC_WORD            0x39

//! FSK registers
#define REG_SX127x_BITRATE_MSB          0x02
#define REG_SX127x_BITRATE_LSB          0x03
#define REG_SX127x_FSK_PAYLOAD_LEN		0x32
#define REG_SX127x_FSK_SEQ_CFG1			0x36
#define REG_SX127x_FSK_SEQ_CFG2			0x37
#define REG_SX127x_FSK_IRQ1				0x3e
#define REG_SX127x_FSK_IRQ2				0x3f


// modes
#define MODE_SX127x_FSK_MODE            0x00
#define MODE_SX127x_OOK_MODE            0x20
#define MODE_SX127x_LONG_RANGE_MODE     0x80
#define MODE_SX127x_SLEEP               0x00
#define MODE_SX127x_STDBY               0x01
#define MODE_SX127x_TX                  0x03
#define MODE_SX127x_RX_CONTINUOUS       0x05
#define MODE_SX127x_RX_SINGLE           0x06

// FSK IRQ mask

#define IRQ_SX127x_FSK_LO_BAT				0x01
#define IRQ_SX127x_FSK_CRC_OK				0x02
#define IRQ_SX127x_FSK_PL_READY				0x04
#define IRQ_SX127x_FSK_PCK_SENT				0x08
#define IRQ_SX127x_FSK_FIFO_OVERRUN			0x10
#define IRQ_SX127x_FSK_FIFO_LEVEL			0x20
#define IRQ_SX127x_FSK_FIFO_EMPTY			0x40
#define IRQ_SX127x_FSK_FIFO_FULL			0x80


#define MAX_SX127x_PKT_LENGTH             0xff
#define RX_SX127x_BUFF_SIZE               0xff

#define SX127x_VALID_VERSION            0x12
#define SX127x_MIN_TIMEOUT              100
//
//
//

static uint32_t sx127x_bw[] = {7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3, 41.7E3, 62.5E3, 125E3, 250E3, 500E3};
static uint8_t  modemModes[] = {MODE_SX127x_LONG_RANGE_MODE, MODE_SX127x_FSK_MODE, MODE_SX127x_OOK_MODE};
static uint16_t  rampFSK[] = {3400, 2000, 1000, 500, 250, 125, 100, 62, 50, 40, 31, 25, 20, 15, 12, 10};

#define SX127x_select()     lora_SPI_Select(sx127x_instance->pins)
#define SX127x_deselect()   lora_SPI_Deselect(sx127x_instance->pins)

static uint32_t rnd_value = 0x5affa5ff;

//
//Public functions
//

/**
 * @brief sx127x_initOptions - Fill chip options by default values
 * @param options - pointer to chip options structure
 */
void sx127x_initOptions(void* options)
{
    SX127x_Options_t* opts = (SX127x_Options_t*)options;
    opts->LDRoptim = 0;
    opts->autoAGC = 1;
    opts->boostPA = 1;
    opts->cdRate = 1;
    opts->dioMapping = 0;
    opts->lnaBoostHF = 3;
    opts->lnaBoostLF = 0;
    opts->lnaGain = 0x01;
    opts->rxFifoBase = 0;
    opts->txFifoBase = 0x80;
    opts->crcTxMode = LCRC_Auto;
    opts->crcRxMode = LCRC_Auto;
    opts->modeIQ = LIQ_Normal;
    opts->hiPowerMode = 0;
    opts->txCurrLimit = 100;    //100 mA by default
    opts->oscillator = OSC_SX127x_CRYSTAL; //crystal oscillator by default
}


//
//Aux. functions
//

/**
 * @brief Read value of internal register
 * @param sx127x_instance - Pointer to chip control structure instance
 * @param regAddr - Address of internal register
 * @return
 */
uint8_t sx127x_read_register(LORA_Instance_t* sx127x_instance, uint8_t regAddr)
{
    if (!sx127x_instance) return 0;
    uint8_t buf[2] = {regAddr, 0xff};

    SX127x_select();
    LORA_Result_t res = lora_SPI_ReadWrite(sx127x_instance->spiHandle, buf, 1, &buf[1], 1);
    SX127x_deselect();

    return (res == LRR_Ok)?buf[1]:0;
}

/**
 * @brief Write value into internal register
 * @param sx127x_instance - Pointer to chip control structure instance
 * @param regAddr - Address of internal register
 * @param value - Value for write into register
 * @return
 */
LORA_Result_t sx127x_write_register(LORA_Instance_t* sx127x_instance, uint8_t regAddr, uint8_t value)
{
    if (!sx127x_instance) return LRR_Invalid_handle;
    uint8_t buf[2] = {regAddr | 0x80, value};

    SX127x_select();
    LORA_Result_t res = lora_SPI_Write(sx127x_instance->spiHandle, buf, 2);
    SX127x_deselect();

    return res;
}

/**
 * @brief Set chip operational mode
 * @param sx127x_instance - Pointer to chip control structure instance
 * @param modeMask  - Bit mask for declare new operational mode
 * @return
 */
LORA_Result_t sx127x_set_op_mode(LORA_Instance_t* sx127x_instance, uint8_t modeMask)
{
    if (sx127x_instance->modemMode > LMT_OOK) return LRR_Invalid_mode;
    return sx127x_write_register(sx127x_instance, REG_SX127x_OP_MODE, modemModes[(uint8_t)sx127x_instance->modemMode] | modeMask);
}

/**
 * @brief Calculates ramp rise/fall time code by absolute rise/fall time value. FSK mode only
 * @param sx127x_instance - Pointer to chip control structure instance
 * @param time - Desired absolute rise/fall time in microseconds
 * @return Valid code for rise/fall time
 */
uint8_t sx127x_fsk_ramp_code(LORA_Instance_t* sx127x_instance, uint16_t time)
{
    int8_t ramp = -1;
    for (int i = 0; (i < ARRAY_LEN(rampFSK)) && (ramp < 0); i++)
    {
        if (rampFSK[i] >= time)
        {
            if (i) ramp = ((time - rampFSK[i-1]) < (rampFSK[i] - time))?(i-1):i;
            else ramp = i;
        }
    }
    if (ramp < 0) ramp = 0;
    return (uint8_t)ramp;
}

//
//Driver functions
//

/**
 * @brief Set carrier frequency
 * @param sx127x_instance - Pointer to chip control structure instance
 * @param freq - Frequency in Herz
 * @return
 */
LORA_Result_t sx127x_setFreq(LORA_Instance_t* sx127x_instance, uint32_t freq)
{
    if (!sx127x_instance) return LRR_Not_initialised;
    uint64_t frf = ((uint64_t)freq << 19) / 32000000;
    LORA_Result_t res = sx127x_write_register(sx127x_instance, REG_SX127x_FRF_MSB, (uint8_t)(frf >> 16));
    if (res == LRR_Ok)  res = sx127x_write_register(sx127x_instance, REG_SX127x_FRF_MID, (uint8_t)(frf >> 8));
    if (res == LRR_Ok)  res = sx127x_write_register(sx127x_instance, REG_SX127x_FRF_LSB, (uint8_t)(frf >> 0));

    if (res == LRR_Ok) sx127x_instance->mainFreq = freq;
    return res;
}

/**
 * @brief Returns current carrier frequency
 * @param sx127x_instance - Pointer to chip control structure instance
 * @return Current carrier frequency
 */
uint32_t sx127x_getFreq(LORA_Instance_t* sx127x_instance)
{
    if (!sx127x_instance) return 0;
    uint64_t frf = (sx127x_read_register(sx127x_instance, REG_SX127x_FRF_MSB) << 16)
            | (sx127x_read_register(sx127x_instance, REG_SX127x_FRF_MID) << 8)
            | (sx127x_read_register(sx127x_instance, REG_SX127x_FRF_LSB));
    uint32_t res = (uint32_t)((frf*FREQ_XTAL) >> 19);
    return res;
}

/**
 * @brief Calculates channel bandwidth code by value of bandwidth
 * @param bw - Bandwidth in Herz
 * @return Correct code for the desired or closest bandwidth
 */
uint8_t sx127x_BandwidthCode(uint32_t bw)
{
    uint8_t res = 0;
    for (int i = 0; (i < ARRAY_LEN(sx127x_bw)) && !res; i++)
    {
        if (sx127x_bw[i] >= bw)
        {
            if (i) res = ((bw - sx127x_bw[i-1]) < (sx127x_bw[i] - bw))?(i-1):i;
            else res = i;
        }
    }
    return res;
}

/**
 * @brief Calculates OverCurrent Protection limit code by current value
 * @param current - current limit value (mA)
 * @return OCP code
 */
uint8_t sx127x_Tx_OCP_code(uint8_t current)
{
    if (current < 45) current = 45;
    if (current > 240) current = 240;

    uint8_t res = 0;
    if (current <= 120) res = (current/5-9);
    else res = (current/10+3);
    return res;
}

/**
 * @brief Calculates OverCurrent Protection limit value by code
 * @param code - code of OCP limit
 * @return current (mA)
 */
uint8_t sx127x_Tx_OCP_current(uint8_t code)
{
    if (code > 27) code = 27;

    if (code <= 15) return (45+5*code);
    return (10*code-30);
}

/**
 * @brief Switches chip into sleep mode
 * @param sx127x_instance - Pointer to chip control structure instance
 * @return Result of operation
 */
LORA_Result_t sx127x_sleep(LORA_Instance_t* sx127x_instance)
{
    return sx127x_set_op_mode(sx127x_instance, MODE_SX127x_SLEEP);
}

/**
 * @brief Switches chip into standby mode
 * @param sx127x_instance - Pointer to chip control structure instance
 * @return Result of operation
 */
LORA_Result_t sx127x_idle(LORA_Instance_t* sx127x_instance)
{
    return sx127x_set_op_mode(sx127x_instance, MODE_SX127x_STDBY);
}

/**
 * @brief Setts the power level for transmitter
 * @param sx127x_instance - Pointer to chip control structure instance
 * @param level - Power level value. Valid values are between 2 and 17
 * @return Result of operation
 */
LORA_Result_t sx127x_setTxPWL(LORA_Instance_t* sx127x_instance, int8_t power)
{
    if (!sx127x_instance) return LRR_Not_initialised;
    SX127x_Options_t* opts = (SX127x_Options_t*)sx127x_instance->options;
    if (!opts) return LRR_Invalid_channel_opts;

    uint8_t hi_power = ((opts->boostPA > 0) && (opts->txCurrLimit >= 200))?1:0;
    int8_t min_power = hi_power?2:-4;
    int8_t max_power = hi_power?17:15;

    if (power > max_power) power = max_power;
    if (power < min_power) power = min_power;

    uint8_t level = power+15-max_power;


    uint8_t val = (sx127x_read_register(sx127x_instance, REG_SX127x_PA_CONFIG) & 0x70)
            | (((SX127x_Options_t*)sx127x_instance->options)->boostPA?0x80:0)
            | (((uint8_t)level) & 0x0f);
    return sx127x_write_register(sx127x_instance, REG_SX127x_PA_CONFIG, val);
}

/**
 * @brief Returns current value of bandwidth
 * @param sx127x_instance - Pointer to chip control structure instance
 * @return Bandwidth in Herz
 */
uint32_t sx127x_bandwidth(LORA_Instance_t* sx127x_instance)
{
    uint8_t code = (sx127x_read_register(sx127x_instance, REG_SX127x_MODEM_CONFIG_1) >> 4);
    code = (code < ARRAY_LEN(sx127x_bw))?code:0;
    return sx127x_bw[code];
}

/**
 * @brief Returns current value of spreading factor
 * @param sx127x_instance - Pointer to chip control structure instance
 * @return Spreading factor
 */
uint8_t sx127x_spreadingFactor(LORA_Instance_t* sx127x_instance)
{
    return (sx127x_read_register(sx127x_instance, REG_SX127x_MODEM_CONFIG_2) >> 4);
}

/**
 * @brief Setts new value of bandwidth
 * @param sx127x_instance - Pointer to chip control structure instance
 * @param bw - Bandwidth in Herz
 * @return Result of operation
 */
LORA_Result_t sx127x_setBW(LORA_Instance_t* sx127x_instance, uint32_t bw)
{
    if (!sx127x_instance) return LRR_Not_initialised;
    uint8_t bCode = (sx127x_BandwidthCode(bw) << 4);
    LORA_Result_t res = sx127x_write_register(sx127x_instance, REG_SX127x_MODEM_CONFIG_1, sx127x_read_register(sx127x_instance, REG_SX127x_MODEM_CONFIG_1) | bCode);
    if (res == LRR_Ok) sx127x_instance->bandwidth = sx127x_bandwidth(sx127x_instance);
    return res;
}

/**
 * @brief Setts new value of spreading factor
 * @param sx127x_instance - Pointer to chip control structure instance
 * @param sf - Spreading factor. Valid values are between 6 and 12
 * @return Result of operation
 */
LORA_Result_t sx127x_setSF(LORA_Instance_t* sx127x_instance, uint8_t sf)
{
    if (!sx127x_instance) return LRR_Not_initialised;
    if (sf < 6) sf = 6;
    if (sf > 12) sf = 12;

    SX127x_Options_t* opts = (SX127x_Options_t*)sx127x_instance->options;

    sx127x_instance->spreadFtr = sf;

    sx127x_write_register(sx127x_instance, REG_SX127x_DETECTION_OPTIMIZE, (sx127x_instance->spreadFtr == 6)?0xc5:0xc3);
    sx127x_write_register(sx127x_instance, REG_SX127x_DETECTION_THRESHOLD, (sx127x_instance->spreadFtr == 6)?0x0c:0x0a);
    sx127x_write_register(sx127x_instance, REG_SX127x_MODEM_CONFIG_2, (sx127x_read_register(sx127x_instance, REG_SX127x_MODEM_CONFIG_2) & 0x0f) | ((sx127x_instance->spreadFtr << 4) & 0xf0));

    //recalculate LDO
    long symbolDuration = 1000 / ( sx127x_bandwidth(sx127x_instance) / (1L << sx127x_spreadingFactor(sx127x_instance)) ) ;
    opts->LDRoptim = (symbolDuration > 16)?1:0;
    sx127x_write_register(sx127x_instance, REG_SX127x_MODEM_CONFIG_3, (opts->LDRoptim << 3) | (opts->autoAGC << 2));
    return LRR_Ok;
}

/**
 * @brief Actually does nothing
 * @param sx127x_instance - Pointer to chip control structure instance
 * @return Result of operation
 */
LORA_Result_t sx127x_deinit(LORA_Instance_t* sx127x_instance)
{
    LORA_Result_t res = sx127x_sleep(sx127x_instance);
    return res;
}

/**
 * @brief Makes remapping of functions of GPIO pins
 * @param sx127x_instance - Pointer to chip control structure instance
 * @return Result of operation
 */
LORA_Result_t sx127x_remapDIO(LORA_Instance_t* sx127x_instance)
{

    SX127x_Options_t* opts = (SX127x_Options_t*)sx127x_instance->options;

    uint8_t regMap1 = ((opts->dioMapping & DIO_SX127x_D0_TxDone)?0x40:((opts->dioMapping & DIO_SX127x_D0_CadDone)?0x80:0x00))
                    | ((opts->dioMapping & DIO_SX127x_D1_CadDetected)?0x20:((opts->dioMapping & DIO_SX127x_D1_FhssChangeChannel)?0x10:0x00))
                    | ((opts->dioMapping & DIO_SX127x_D3_ValidHeader)?0x01:((opts->dioMapping & DIO_SX127x_D3_PayloadCrcError)?0x02:0x00))
                    ;
    uint8_t regMap2 = ((opts->dioMapping & DIO_SX127x_D4_PllLock)?0x40:0x00)
                    | ((opts->dioMapping & DIO_SX127x_D5_ClkOut)?0x10:0x00)
                    ;

    LORA_Result_t res = sx127x_write_register(sx127x_instance, REG_SX127x_DIO_MAPPING_1, regMap1);
    if (res == LRR_Ok) res = sx127x_write_register(sx127x_instance, REG_SX127x_DIO_MAPPING_2, regMap2);

    regMap1 = 0;
    regMap1 = sx127x_read_register(sx127x_instance, REG_SX127x_DIO_MAPPING_1);

    return res;
}

/**
 * @brief Calculates time needed to transmit once byte
 * @param sx127x_instance  - Pointer to chip control structure instance
 * @return Time to transmit once byte in microseconds
 */
uint32_t sx127x_symDuration(LORA_Instance_t* sx127x_instance)
{
    return (1000000*(1L << sx127x_spreadingFactor(sx127x_instance)))/sx127x_bandwidth(sx127x_instance);
}

/**
 * @brief Sets length of LoRa preamble
 * @param sx127x_instance - Pointer to chip control structure instance
 * @param prea_len	Desired length of preamble
 * @return Result of operation
 */
LORA_Result_t sx127x_setPreambleLen(LORA_Instance_t* sx127x_instance, uint16_t prea_len)
{
	LORA_Result_t res = sx127x_write_register(sx127x_instance, REG_SX127x_PREAMBLE_MSB, (prea_len >> 8) & 0xff);
	res |= sx127x_write_register(sx127x_instance, REG_SX127x_PREAMBLE_LSB, prea_len & 0xff);
	return res;
}

/**
 * @brief Applies options. All options with auto values (like a header type) will previously calculated.
 * @param sx127x_instance - Pointer to chip control structure instance
 * @return Result of operation
 */
LORA_Result_t sx127x_applyOptions(LORA_Instance_t* sx127x_instance)
{
    SX127x_Options_t* opts = (SX127x_Options_t*)sx127x_instance->options;

    LORA_Result_t res = sx127x_sleep(sx127x_instance);

    if (!sx127x_instance->maxPldLen) sx127x_instance->maxPldLen = 0xff;
    if (sx127x_instance->pldLen > sx127x_instance->maxPldLen) sx127x_instance->pldLen = sx127x_instance->maxPldLen;

    //! Chip manual note (4.1.1.4): In the lower band (169 MHz), the 250 kHz and 500 kHz bandwidths are not supported
    if ((sx127x_instance->mainFreq <= 169000000) && (sx127x_instance->bandwidth > 125000))
        sx127x_instance->bandwidth = 125000;

    if (opts->hiPowerMode && (opts->txCurrLimit < 200)) opts->txCurrLimit = 200;
    if (!opts->hiPowerMode && (opts->txCurrLimit > 100)) opts->txCurrLimit = 100;

    if (sx127x_instance->spreadFtr == 6)
        sx127x_instance->hdrType = LHT_Implicit;

    if (sx127x_instance->hdrType == LHT_Auto) sx127x_instance->hdrType = (sx127x_instance->pldLen > 0)?LHT_Implicit:LHT_Explicit;

    if (opts->crcTxMode == LCRC_Auto) opts->crcTxMode = LCRC_ForceOn;
    if (opts->crcRxMode == LCRC_Auto) opts->crcRxMode = (sx127x_instance->hdrType == LHT_Implicit)?LCRC_ForceOn:LCRC_ForceOff;

    res |= sx127x_setFreq(sx127x_instance, sx127x_instance->mainFreq);

    res |= sx127x_remapDIO(sx127x_instance);

    res |= sx127x_write_register(sx127x_instance, REG_SX127x_LNA, (opts->lnaGain << 5) | (opts->lnaBoostLF << 3) | (opts->lnaBoostHF));

    res |= sx127x_setTxPWL(sx127x_instance, sx127x_instance->txPWL);

    if (sx127x_instance->spreadFtr < 6) sx127x_instance->spreadFtr = 6;
    if (sx127x_instance->spreadFtr > 12) sx127x_instance->spreadFtr = 12;

    uint8_t rgTmp;

    if (sx127x_instance->modemMode == LMT_LORA)
    {
		res |= sx127x_write_register(sx127x_instance, REG_SX127x_MODEM_CONFIG_1, (sx127x_BandwidthCode(sx127x_instance->bandwidth) << 4)
																				 | (opts->cdRate << 1)
																				 | ((sx127x_instance->hdrType == LHT_Explicit)?0x00:0x01));



		//set spreading factor and CRC enable/disable
		res |= sx127x_write_register(sx127x_instance, REG_SX127x_DETECTION_OPTIMIZE, (sx127x_instance->spreadFtr == 6)?0xc5:0xc3);
		res |= sx127x_write_register(sx127x_instance, REG_SX127x_DETECTION_THRESHOLD, (sx127x_instance->spreadFtr == 6)?0x0c:0x0a);

		rgTmp = sx127x_read_register(sx127x_instance, REG_SX127x_MODEM_CONFIG_2);
		res |= sx127x_write_register(sx127x_instance, REG_SX127x_MODEM_CONFIG_2, (rgTmp & 0x0f) | ((sx127x_instance->spreadFtr & 0x0f) << 4));
		rgTmp = sx127x_read_register(sx127x_instance, REG_SX127x_MODEM_CONFIG_2);

		// set base addresses
		res |= sx127x_write_register(sx127x_instance, REG_SX127x_FIFO_TX_BASE_ADDR, opts->txFifoBase);
		res |= sx127x_write_register(sx127x_instance, REG_SX127x_FIFO_RX_BASE_ADDR, opts->rxFifoBase);

		//set IQ mode
		rgTmp = 0x27;
		if (opts->modeIQ == LIQ_Invert) rgTmp |= 0x40;
		res |= sx127x_write_register(sx127x_instance, REG_SX127x_INVERT_IQ, rgTmp);

		//calculate LDO
		long symbolDuration = sx127x_symDuration(sx127x_instance);
		opts->LDRoptim = (symbolDuration > 16000)?1:0;
		res |= sx127x_write_register(sx127x_instance, REG_SX127x_MODEM_CONFIG_3, (opts->LDRoptim?0x08:0x00) | (opts->autoAGC?0x04:0x00));

		//set sync. word
		res |= sx127x_write_register(sx127x_instance, REG_SX127x_SYNC_WORD, sx127x_instance->syncWord);

		//set preamble length
		res |= sx127x_setPreambleLen(sx127x_instance, sx127x_instance->preambleLen);

		//set max. payload length
		res |= sx127x_write_register(sx127x_instance, REG_SX127x_MAX_PAYLOAD_LENGTH, sx127x_instance->maxPldLen);
    }

    //set OCP limit
    res |= sx127x_write_register(sx127x_instance, REG_SX127x_OCP, 0x20 | sx127x_Tx_OCP_code(opts->txCurrLimit));

    //Read back main options
    sx127x_instance->mainFreq = sx127x_getFreq(sx127x_instance);

    if (sx127x_instance->modemMode == LMT_LORA)
    {
		sx127x_instance->bandwidth = sx127x_bandwidth(sx127x_instance);
		sx127x_instance->spreadFtr = sx127x_spreadingFactor(sx127x_instance);
    }
    rgTmp = sx127x_read_register(sx127x_instance, REG_SX127x_OCP);
    opts->txCurrLimit = sx127x_Tx_OCP_current(rgTmp & 0x1F);

    res |= sx127x_idle(sx127x_instance);

    return res;
}

/**
 * @brief Initializes of chip. All options be applied, auto values be calculated
 * @param sx127x_instance - Pointer to chip control structure instance
 * @return Result of operation
 */
LORA_Result_t sx127x_init(LORA_Instance_t* sx127x_instance)
{
    if (!sx127x_instance) return LRR_Invalid_data;
    if (!sx127x_instance->spiHandle) return LRR_Invalid_channel;

    LORA_Result_t res = lora_Reset(sx127x_instance->pins);
    if (res == LRR_Ok)
    {
    	uint8_t try_count = 5;
        uint8_t tmp = sx127x_read_register(sx127x_instance, REG_SX127x_VERSION);
        while ((tmp != SX127x_VALID_VERSION) && (try_count > 0))
        {
        	delayMs(10);
        	tmp = sx127x_read_register(sx127x_instance, REG_SX127x_VERSION);
        	try_count--;
        }

        if (tmp == SX127x_VALID_VERSION)
        {
        	SX127x_Options_t* opts = (SX127x_Options_t*)sx127x_instance->options;

        	//oscillator configuration
        	sx127x_write_register(sx127x_instance, REG_SX127x_XTA, 0x09 | opts->oscillator);

            // set config
            res |= sx127x_applyOptions(sx127x_instance);

            // set high power mode
            if (opts->hiPowerMode)
            	res += sx127x_write_register(sx127x_instance, REG_SX127x_HIPOWER, 0x87);

            // enable all IRQ
            sx127x_write_register(sx127x_instance, REG_SX127x_IRQ_FLAG_MASK, 0x00);

            // put in standby mode
            res |= sx127x_idle(sx127x_instance);
        }
        else res = LRR_Incompatible_device;
    }

    return res;
}

/**
 * @brief Sends data through RF channel
 * @param sx127x_instance - Pointer to chip control structure instance
 * @param data - Data to be send
 * @param length - Length of data to be send
 * @param timeout - Timeout (msec)
 * @return Result of operation
 */
LORA_Result_t sx127x_send(LORA_Instance_t* sx127x_instance, const uint8_t* data, uint8_t length, uint16_t timeout)
{
	SX127x_Options_t* opts = (SX127x_Options_t*)sx127x_instance->options;
	if (!opts) return LRR_Invalid_channel;

    LORA_Result_t res = LRR_Ok;

    if (timeout < SX127x_MIN_TIMEOUT) timeout = SX127x_MIN_TIMEOUT;
    //prepare
    sx127x_idle(sx127x_instance);

    uint32_t deadtime;
    uint8_t waiting = 1;

    if (sx127x_instance->modemMode == LMT_LORA)
    {

		uint8_t rgTmp = sx127x_read_register(sx127x_instance, REG_SX127x_MODEM_CONFIG_2);
		if (opts->crcTxMode == LCRC_ForceOn) sx127x_write_register(sx127x_instance, REG_SX127x_MODEM_CONFIG_2, rgTmp | 0x04);
		if (opts->crcTxMode == LCRC_ForceOff) sx127x_write_register(sx127x_instance, REG_SX127x_MODEM_CONFIG_2, rgTmp & 0xFB);


		sx127x_write_register(sx127x_instance, REG_SX127x_FIFO_ADDR_PTR, sx127x_read_register(sx127x_instance, REG_SX127x_FIFO_TX_BASE_ADDR));

		rgTmp = sx127x_read_register(sx127x_instance, REG_SX127x_MODEM_CONFIG_1);


		sx127x_write_register(sx127x_instance, REG_SX127x_PAYLOAD_LENGTH, length);

		//write FIFO
		sx127x_write_register(sx127x_instance, REG_SX127x_FIFO_ADDR_PTR, sx127x_read_register(sx127x_instance, REG_SX127x_FIFO_TX_BASE_ADDR));

		rgTmp = length;
		for (int i = 0; i < length; i++)
			sx127x_write_register(sx127x_instance, REG_SX127x_FIFO, data[i]);

		//initiate transmission
		sx127x_set_op_mode(sx127x_instance, MODE_SX127x_TX);

		// wait for TX done

		deadtime = lora_calc_deadtime(timeout);
		rgTmp = sx127x_read_register(sx127x_instance, REG_SX127x_IRQ_FLAGS);
		uint8_t fail = ((rgTmp & IRQ_SX127x_TxDone) == 0)?1:0;
		while (waiting && fail)
		{
			delayMs(20);
			rgTmp = sx127x_read_register(sx127x_instance, REG_SX127x_IRQ_FLAGS);
			fail = ((rgTmp & IRQ_SX127x_TxDone) == 0)?1:0;
			waiting = lora_check_timeout(deadtime);
		}

		if (fail) res = LRR_Timeout;

		// clear IRQ's
		sx127x_write_register(sx127x_instance, REG_SX127x_IRQ_FLAGS, IRQ_SX127x_TxDone);
		//sx127x_set_op_mode(sx127x_instance, MODE_SX127x_RX_CONTINUOUS);
    }
    else //FSK/OOK mode
    {
    	sx127x_write_register(sx127x_instance, REG_SX127x_FSK_PAYLOAD_LEN, length);
    	for (int i = 0; i < length; i++)
    		sx127x_write_register(sx127x_instance, REG_SX127x_FIFO, data[i]);

    	//sx127x_set_op_mode(sx127x_instance, MODE_SX127x_TX);
    	sx127x_write_register(sx127x_instance, REG_SX127x_FSK_SEQ_CFG2, 0xd8);
    	sx127x_write_register(sx127x_instance, REG_SX127x_FSK_SEQ_CFG1, 0x99);

    	deadtime = lora_calc_deadtime(timeout);

    	uint8_t rgTmp = sx127x_read_register(sx127x_instance, REG_SX127x_FSK_IRQ2);
    	uint8_t fail = ((rgTmp & IRQ_SX127x_FSK_PCK_SENT) == 0)?1:0;
		while (waiting && fail)
		{
			rgTmp = sx127x_read_register(sx127x_instance, REG_SX127x_FSK_IRQ2);
			fail = ((rgTmp & IRQ_SX127x_FSK_PCK_SENT) == 0)?1:0;
			waiting = lora_check_timeout(deadtime);
		}

		if (fail) res = LRR_Timeout;
    }

    return res;
}

/**
 * @brief Return count of bytes, available for read
 * @param sx127x_instance - Pointer to chip control structure instance
 * @param tc - Pointer to "transfer completed" flag. Sets to 1 if receiving is fully complete without same errors
 * @return Result of operation
 */
uint8_t sx127x_rxAvail(LORA_Instance_t* sx127x_instance, uint8_t* tc)
{
    uint8_t irq_mask = sx127x_read_register(sx127x_instance, REG_SX127x_IRQ_FLAGS);
    uint8_t transferComplette = ((irq_mask & IRQ_SX127x_RxDone) ? 1:0) && ((irq_mask & IRQ_SX127x_PayloadCrcError) == 0)?1:0;
    if (tc)
        *tc = transferComplette;

    if (irq_mask & IRQ_SX127x_RxDone) sx127x_write_register(sx127x_instance, REG_SX127x_IRQ_FLAGS, IRQ_SX127x_RxDone);
    if (irq_mask & IRQ_SX127x_PayloadCrcError) sx127x_write_register(sx127x_instance, REG_SX127x_IRQ_FLAGS, IRQ_SX127x_PayloadCrcError);

    /*uint8_t  res = 0;
    if (transferComplette)
    {
        int f_head = sx127x_read_register(sx127x_instance, REG_SX127x_FIFO_RX_CURRENT_ADDR);
        sx127x_write_register(sx127x_instance, REG_SX127x_FIFO_ADDR_PTR, f_head);
        res = sx127x_read_register(sx127x_instance, REG_SX127x_FIFO);
    }*/
    uint8_t res = sx127x_read_register(sx127x_instance, REG_SX127x_RX_NB_BYTES);

    return res;
}

/**
 * @brief Reads received data from internal buffer.
 * @param sx127x_instance - Pointer to chip control structure instance
 * @param dest - Pointer to buffer for transferring received data.
 * @param length - Maximum count of bytes to be transfered
 * @return Result of operation
 */
LORA_Result_t sx127x_read(LORA_Instance_t* sx127x_instance, uint8_t* dest, uint8_t length)
{
    int f_head = sx127x_read_register(sx127x_instance, REG_SX127x_FIFO_RX_CURRENT_ADDR);
    LORA_Result_t res = LRR_Ok;
    uint8_t bytes_to_read = sx127x_read_register(sx127x_instance, REG_SX127x_RX_NB_BYTES);

    for (int i = 0; (i < bytes_to_read) && (i < length) && (res == LRR_Ok) && dest; i++)
    {
        res = sx127x_write_register(sx127x_instance, REG_SX127x_FIFO_ADDR_PTR, (f_head+i)%(RX_SX127x_BUFF_SIZE+1));
        *dest++ = sx127x_read_register(sx127x_instance, REG_SX127x_FIFO);
    }
    sx127x_write_register(sx127x_instance, REG_SX127x_IRQ_FLAGS, IRQ_SX127x_RxDone);
    return res;
}

/**
 * @brief Switches chip to receive mode
 * @param sx127x_instance - Pointer to chip control structure instance
 * @param count - Quantity of bytes for read
 * @param continuous - Continuous mode flag. If not set receiving in single mode
 * @return Result of operation
 */
LORA_Result_t sx127x_recieve(LORA_Instance_t* sx127x_instance, uint8_t count,  uint8_t continuous)
{
    SX127x_Options_t* opts = (SX127x_Options_t*)sx127x_instance->options;
    LORA_Result_t res = LRR_Ok;
    if (sx127x_instance->modemMode == LMT_LORA)
    {
		uint8_t rgTmp = sx127x_read_register(sx127x_instance, REG_SX127x_MODEM_CONFIG_2);
		if (opts->crcRxMode == LCRC_ForceOn) sx127x_write_register(sx127x_instance, REG_SX127x_MODEM_CONFIG_2, rgTmp | 0x04);
		if (opts->crcRxMode == LCRC_ForceOff) sx127x_write_register(sx127x_instance, REG_SX127x_MODEM_CONFIG_2, rgTmp & 0xFB);
		res = sx127x_write_register(sx127x_instance, REG_SX127x_PAYLOAD_LENGTH, (count == 0)?sx127x_instance->pldLen:count);
		res |= sx127x_write_register(sx127x_instance, REG_SX127x_FIFO_ADDR_PTR, sx127x_read_register(sx127x_instance, REG_SX127x_FIFO_RX_BASE_ADDR));
		res |= sx127x_set_op_mode(sx127x_instance, continuous?MODE_SX127x_RX_CONTINUOUS:MODE_SX127x_RX_SINGLE);
    }
    else
    	res = sx127x_set_op_mode(sx127x_instance, continuous?MODE_SX127x_RX_CONTINUOUS:MODE_SX127x_RX_SINGLE);
    return res;
}

/**
 * @brief Returns hardware measured RSSI of last received packet
 * @param sx127x_instance - Pointer to chip control structure instance
 * @return RSSI of last received packet
 */
int8_t sx127x_rssi(LORA_Instance_t* sx127x_instance)
{
    return (sx127x_read_register(sx127x_instance, REG_SX127x_PKT_RSSI_VALUE) - (sx127x_instance->mainFreq < 868E6 ? 164 : 157));
}

/**
 * @brief Returns hardware measured SNR of last received packet
 * @param sx127x_instance - Pointer to chip control structure instance
 * @return SNR of last received packet. Divide by 1000 for true value
 */
int16_t sx127x_snr(LORA_Instance_t* sx127x_instance)
{
    return ((int16_t)sx127x_read_register(sx127x_instance, REG_SX127x_PKT_SNR_VALUE)) * 250; // divide by 1000 for true value
}

/**
 * @brief Returns configured LoRa preamble length
 * @param sx127x_instance - Pointer to chip control structure instance
 * @return Preamble length in bytes
 */
uint16_t sx127x_preambleLen(LORA_Instance_t* sx127x_instance)
{
    return ((sx127x_read_register(sx127x_instance, REG_SX127x_PREAMBLE_MSB) << 8) | sx127x_read_register(sx127x_instance, REG_SX127x_PREAMBLE_LSB));
}

/**
 * @brief Check for channel activity
 * @param sx127x_instance - Pointer to chip control structure instance
 * @param rssi_free - RSSI level makes as "free"
 * @return LRR_Ok if channel is free, LRR_Busy if channel busy
 */
LORA_Result_t sx127x_checkForChannelFree(LORA_Instance_t* sx127x_instance, int8_t rssi_free, uint16_t time)
{
	int16_t cur_rssi = -127;
	uint8_t sd = 0;
	sx127x_recieve(sx127x_instance, 0, 1);
	while (time && !sd)
	{
		sd = (sx127x_read_register(sx127x_instance, REG_SX127x_MODEM_STATUS) & STATUS_SX127x_SD);
		if (sd)
		{
			int16_t rssi = (sx127x_instance->mainFreq < 868E6 ? -164 : -157) + sx127x_read_register(sx127x_instance, REG_SX127x_CUR_RSSI_VALUE);
			cur_rssi = (cur_rssi > rssi)?cur_rssi:rssi;
		}
		else
		{
			time--;
			HAL_Delay(1);
		}
	}

	return (cur_rssi <= rssi_free)?LRR_Ok:LRR_Busy;
}

/**
 * @brief Loads data into tx. FIFO buffer, but not sends it
 * @param sx127x_instance - Pointer to chip control structure instance
 * @param data - Data to be loaded
 * @param length - Length of data to be loaded
 * @return Result of operation
 */
LORA_Result_t sx127x_prepareTx(LORA_Instance_t* sx127x_instance, const uint8_t* data, uint8_t length)
{
    sx127x_idle(sx127x_instance);

    uint8_t tmp = sx127x_read_register(sx127x_instance, REG_SX127x_FIFO_ADDR_PTR);
    sx127x_write_register(sx127x_instance, REG_SX127x_FIFO_ADDR_PTR, sx127x_read_register(sx127x_instance, REG_SX127x_FIFO_TX_BASE_ADDR));

    //write FIFO
    sx127x_write_register(sx127x_instance, REG_SX127x_FIFO_ADDR_PTR, sx127x_read_register(sx127x_instance, REG_SX127x_FIFO_TX_BASE_ADDR));

    for (int i = 0; i < length; i++)
        sx127x_write_register(sx127x_instance, REG_SX127x_FIFO, data[i]);

    sx127x_write_register(sx127x_instance, REG_SX127x_FIFO_ADDR_PTR, tmp);

    return LRR_Ok;

}

/**
 * @brief Sends data, contains in internal tx. FIFO buffer
 * @param sx127x_instance - Pointer to chip control structure instance
 * @param length - Length of data to be send
 * @param timeout - Timeout (msec)
 * @return Result of operation
 */
LORA_Result_t sx127x_sendPrepared(LORA_Instance_t* sx127x_instance, uint8_t length, uint16_t timeout)
{
    LORA_Result_t res = LRR_Ok;
    SX127x_Options_t* opts = (SX127x_Options_t*)sx127x_instance->options;
    uint8_t rgTmp = sx127x_read_register(sx127x_instance, REG_SX127x_MODEM_CONFIG_2);
    if (opts->crcTxMode == LCRC_ForceOn) sx127x_write_register(sx127x_instance, REG_SX127x_MODEM_CONFIG_2, rgTmp | 0x04);
    if (opts->crcTxMode == LCRC_ForceOff) sx127x_write_register(sx127x_instance, REG_SX127x_MODEM_CONFIG_2, rgTmp & 0xFB);

    sx127x_write_register(sx127x_instance, REG_SX127x_PAYLOAD_LENGTH, length);

    //initiate transmission
    sx127x_set_op_mode(sx127x_instance, MODE_SX127x_TX);

    // wait for TX done
    uint32_t deadtime = lora_calc_deadtime(timeout);
    uint8_t waiting = 1;
    uint8_t fail = ((sx127x_read_register(sx127x_instance, REG_SX127x_IRQ_FLAGS) & IRQ_SX127x_TxDone) == 0)?1:0;
    while (waiting && fail)
    {
        fail = ((sx127x_read_register(sx127x_instance, REG_SX127x_IRQ_FLAGS) & IRQ_SX127x_TxDone) == 0)?1:0;
        waiting = lora_check_timeout(deadtime);
    }

    if (fail) res = LRR_Timeout;

    // clear IRQ's
    sx127x_write_register(sx127x_instance, REG_SX127x_IRQ_FLAGS, IRQ_SX127x_TxDone);

    return res;

}

/**
 * @brief Generates random 16-bit value
 * @param sx127x_instance - Pointer to chip control structure instance
 * @return 16-bit random value
 */
uint16_t sx127x_getRandom16(LORA_Instance_t* sx127x_instance)
{
	if (!sx127x_instance) return 0;
	uint16_t res = rnd_value & 0xffff;
	uint8_t sa = 0;
	uint8_t sb = 0;
	sx127x_recieve(sx127x_instance, 0, 1);
	for (int i = 0; i < 2; i++)
	{
		uint8_t b = sx127x_read_register(sx127x_instance, REG_SX127x_RSSI_WIDEBAND);
		sa += b;
		sb += sa;
		res += ((sb << 8)+sa) + b*( ((b>sa) && (b&1))?10:-10);
	}
	rnd_value+= res;
	sx127x_sleep(sx127x_instance);
	return (uint16_t)(rnd_value%0xffff);
}

/**
 * @brief Read status register
 * @param sx127x_instance - Pointer to chip control structure instance
 * @return Content of modem status register
 */
uint8_t sx127x_modemStatus(LORA_Instance_t* sx127x_instance)
{
	return sx127x_read_register(sx127x_instance, REG_SX127x_MODEM_STATUS);
}

/**
 * @brief Returns actual payload length
 * @param sx127x_instance - Pointer to chip control structure instance
 * @param length - length of user payload
 * @return Actual payload length in bytes
 */
uint8_t sx127x_payloadLen(LORA_Instance_t* sx127x_instance, uint8_t length)
{
	if (!sx127x_instance) return 0;
	SX127x_Options_t* opts = (SX127x_Options_t*)sx127x_instance->options;
	if (!opts) return 0;
	int16_t tmp = ((8*length - 4*sx127x_instance->spreadFtr + 28 + ((opts->crcTxMode == LCRC_ForceOn)?16:0) - ((sx127x_instance->hdrType == LHT_Implicit)?20:0))*10) / (4 * (sx127x_instance->spreadFtr - 2*opts->LDRoptim) );
	tmp += (tmp%10)?10:0;
	tmp = tmp/10;
	tmp *= opts->cdRate+4;
	if (tmp < 0) tmp = 0;
	return (tmp & 0xff)+8;
}

/**
 * @brief Returns actual frequency error
 * @param sx127x_instance - Pointer to chip control structure instance
 * @param f_xtal - Resonance frequency of external resonator
 * @return Absolute frequency deviation
 */
int32_t sx127x_freqError(LORA_Instance_t* sx127x_instance)
{
	uint8_t err[4] = {
			sx127x_read_register(sx127x_instance, REG_SX127x_FREQ_ERROR_LSB),
			sx127x_read_register(sx127x_instance, REG_SX127x_FREQ_ERROR_MID),
			sx127x_read_register(sx127x_instance, REG_SX127x_FREQ_ERROR_MSB),
			0
	};

	if (err[2] & 0x08) err[3] |= 0x80;
	err[2] &= 0x07;
	int32_t error = (*((int32_t*)err));

	error = ((error*(1UL << 24)/FREQ_XTAL) * (sx127x_bandwidth(sx127x_instance)/5000))/100;
	return error;
}

/**
 * @brief Sets correction ratio for compensation of frequency error
 * @param sx127x_instance - Pointer to chip control structure instance
 * @param correction_factor - Correction factor
 * @return Result of operation
 */
LORA_Result_t sx127x_setPpmCorrection(LORA_Instance_t* sx127x_instance, int8_t correction_factor)
{
	return sx127x_write_register(sx127x_instance, REG_SX127x_PPM_RATIO, (uint8_t)correction_factor);
}

void sx127x_initRandomGen(uint32_t value)
{
	rnd_value += value;
}

LORA_Result_t sx127x_send_async(LORA_Instance_t* sx127x_instance, const uint8_t* data, uint8_t length)
{
	if (!sx127x_instance) return LRR_Invalid_handle;

	SX127x_Options_t* opts = (SX127x_Options_t*)sx127x_instance->options;
	if (!opts) return LRR_Invalid_channel;

	sx127x_idle(sx127x_instance);

	LORA_Result_t res = LRR_Invalid_mode;

	if (sx127x_instance->modemMode == LMT_LORA)
	{

		uint8_t rgTmp = sx127x_read_register(sx127x_instance, REG_SX127x_MODEM_CONFIG_2);
		if (opts->crcTxMode == LCRC_ForceOn) sx127x_write_register(sx127x_instance, REG_SX127x_MODEM_CONFIG_2, rgTmp | 0x04);
		if (opts->crcTxMode == LCRC_ForceOff) sx127x_write_register(sx127x_instance, REG_SX127x_MODEM_CONFIG_2, rgTmp & 0xFB);


		sx127x_write_register(sx127x_instance, REG_SX127x_FIFO_ADDR_PTR, sx127x_read_register(sx127x_instance, REG_SX127x_FIFO_TX_BASE_ADDR));

		rgTmp = sx127x_read_register(sx127x_instance, REG_SX127x_MODEM_CONFIG_1);


		sx127x_write_register(sx127x_instance, REG_SX127x_PAYLOAD_LENGTH, length);

		//write FIFO
		sx127x_write_register(sx127x_instance, REG_SX127x_FIFO_ADDR_PTR, sx127x_read_register(sx127x_instance, REG_SX127x_FIFO_TX_BASE_ADDR));

		rgTmp = length;
		for (int i = 0; i < length; i++)
			sx127x_write_register(sx127x_instance, REG_SX127x_FIFO, data[i]);

		//initiate transmission
		res = sx127x_set_op_mode(sx127x_instance, MODE_SX127x_TX);
	}

	return res;
}

void sx127x_setDIO(LORA_Instance_t* sx127x_instance, uint16_t dioMap)
{
	if (!sx127x_instance) return;
	SX127x_Options_t* opts = (SX127x_Options_t*)sx127x_instance->options;
	opts->dioMapping = dioMap;
	sx127x_remapDIO(sx127x_instance);
}

uint16_t sx127x_getDIO(LORA_Instance_t* sx127x_instance)
{
	if (!sx127x_instance) return 0;
	SX127x_Options_t* opts = (SX127x_Options_t*)sx127x_instance->options;
	return opts->dioMapping;
}

//
//
//
LORA_Driver_t sx127x_driver =
{
     sx127x_init,               /*LORA_Result_t   (*init)(LORA_Init_t*);*/
     sx127x_send,               /*LORA_Result_t   (*send)(const uint8_t*,uint8_t,uint16_t);*/
     sx127x_rxAvail,            /*uint8_t        (*rxAvail)(uint8_t);*/
     sx127x_read,               /*LORA_Result_t   (*read)(uint8_t*,uint8_t);*/
     sx127x_deinit,             /*LORA_Result_t   (*deInit)();*/
     sx127x_setFreq,            /*LORA_Result_t   (*setFreq)(uint32_t);*/
     sx127x_setBW,              /*LORA_Result_t   (*setBW)(uint32_t);*/
     sx127x_setSF,              /*LORA_Result_t   (*setSF)(uint8_t);*/
     sx127x_getFreq,            /*uint32_t        (*getFreq)();*/
     sx127x_bandwidth,          /*uint32_t        (*getBW)();*/
     sx127x_spreadingFactor,    /*uint8_t         (*getSF)();*/
     sx127x_setTxPWL,           /*LORA_Result_t   (*setTxPWL)(int8_t)*/
     sx127x_sleep,              /*LORA_Result_t   (*sleep)();*/
     sx127x_idle,               /*LORA_Result_t   (*idle)();*/
     sx127x_recieve,            /*LORA_Result_t   (*recieve)(LORA_Instance_t*);*/
     sx127x_applyOptions,       /*LORA_Result_t   (*applyOptions)(LORA_Instance_t*);*/
     sx127x_remapDIO,           /*LORA_Result_t   (*remapDIO)(LORA_Instance_t*);*/
     sx127x_rssi,               /*int8_t          (*rssi)(LORA_Instance_t*);*/
     sx127x_snr,                /*int16_t         (*snr)(LORA_Instance_t*);*/
     sx127x_symDuration,        /*uint32_t        (*symDuration)(LORA_Instance_t*);*/
     sx127x_preambleLen,        /*uint16_t        (*preambleLen)(LORA_Instance_t*);*/
     sx127x_checkForChannelFree,/*LORA_Result_t   (*checkForChannelFree)(LORA_Instance_t*, int8_t);*/
     sx127x_prepareTx,          /*LORA_Result_t   (*prepareTx)(LORA_Instance_t*, const uint8_t*,uint8_t);*/
     sx127x_sendPrepared,       /*LORA_Result_t   (*sendPrepared)(LORA_Instance_t*, uint16_t);*/
	 sx127x_getRandom16,		/*uint32_t		  (*getRandom16)();*/
	 sx127x_write_register,		/*LORA_Result_t	  (*writeReg)(LORA_Instance_t*, uint8_t, uint8_t );*/
	 sx127x_read_register,		/*uint8_t		  (*readReg)(LORA_Instance_t*, uint8_t reg_num);*/
	 sx127x_modemStatus,		/*uint8_t		  (*modemStatus)(LORA_Instance_t*);*/
	 sx127x_setPreambleLen,		/*LORA_Result_t	  (*setPreambleLen)(LORA_Instance_t*, uint16_t );*/
	 sx127x_payloadLen,			/*uint8_t		  (*payloadLen)(LORA_Instance_t* , uint8_t );*/
	 sx127x_freqError,			/*uint32_t		  (*freqError)(LORA_Instance_t*);*/
	 sx127x_setPpmCorrection,	/*LORA_Result_t	  (*setPpmCorrection)(LORA_Instance_t*, uint8_t );*/
	 sx127x_initRandomGen,
	 sx127x_send_async, 		/*void (*send_async)(LORA_Instance_t*, const uint8_t*, uint8_t);*/
	 sx127x_setDIO,				/*void (*setDIO)(LORA_Instance_t*, uint16_t);*/
	 sx127x_getDIO,				/*uint16_t		(*getDIO)(LORA_Instance_t*);*/
};

