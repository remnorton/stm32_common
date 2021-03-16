/*
 * sx127x_driver.h
 *
 *  Created on: 20 сент. 2018 г.
 *      Author: User
 */

#ifndef APPLICATION_LORA_DRIVERS_SX127X_DRIVER_H_
#define APPLICATION_LORA_DRIVERS_SX127X_DRIVER_H_
#include "lora_driver_types.h"
#include "lora_driver.h"
//
//
//
//DIO mapping
#define DIO_SX127x_D0_RxDone                0x01
#define DIO_SX127x_D0_TxDone                0x02
#define DIO_SX127x_D0_CadDone               0x04
#define DIO_SX127x_D1_RxTimeout             0x08
#define DIO_SX127x_D1_FhssChangeChannel     0x10
#define DIO_SX127x_D1_CadDetected           0x20
#define DIO_SX127x_D3_CadDone               0x40
#define DIO_SX127x_D3_ValidHeader           0x80
#define DIO_SX127x_D3_PayloadCrcError       0x100
#define DIO_SX127x_D4_CadDetected           0x200
#define DIO_SX127x_D4_PllLock               0x400
#define DIO_SX127x_D5_ModeReady             0x800
#define DIO_SX127x_D5_ClkOut                0x1000

// LoRa IRQ masks
#define IRQ_SX127x_CadDetected              0x01
#define IRQ_SX127x_FhssChangeChannel        0x02
#define IRQ_SX127x_CadDone                  0x04
#define IRQ_SX127x_TxDone                   0x08
#define IRQ_SX127x_ValidHeader              0x10
#define IRQ_SX127x_PayloadCrcError          0x20
#define IRQ_SX127x_RxDone                   0x40
#define IRQ_SX127x_RxTimeout                0x80

//Modulation data shaping (FSK only)
#define FSK_SHAPING_NONE                    0x00
#define FSK_SHAPING_GAUSS_1_0               0x20
#define FSK_SHAPING_GAUSS_0_5               0x40
#define FSK_SHAPING_GAUSS_0_3               0x60


//Modem status
#define STATUS_SX127x_MC	0x10	//Modem clear
#define STATUS_SX127x_HV	0x08	//Header info valid
#define STATUS_SX127x_RX	0x04	//RX on-going
#define STATUS_SX127x_SS	0x02	//Signal synchronized
#define STATUS_SX127x_SD	0x01	//Signal detected

//Oscillator type
#define OSC_SX127x_CRYSTAL	0x00	//Crystal oscillator
#define OSC_SX127x_CLOCK	0x10	//External clock signal

//Crystal freq.
#define FREQ_XTAL			32000000

#ifndef ARRAY_LEN
#define ARRAY_LEN(x)        (sizeof(x)/sizeof(x[0]))
#endif

//
//
//

#pragma pack(1)

typedef struct
{
    uint16_t            dioMapping; //! DIO pins mapping (DIO_SX127x_x)
    uint8_t             rxFifoBase; //! Base address of Tx FIFO
    uint8_t             txFifoBase; //! Base address of Rx FIFO
    uint8_t             boostPA;    //!
    uint8_t             LDRoptim;   //! Low Data Rate Optimize (LDO)
    uint8_t             cdRate;     //! Error coding rate [1..4]
    uint8_t             lnaGain;
    uint8_t             lnaBoostHF;
    uint8_t             lnaBoostLF;
    uint8_t             autoAGC;
    uint8_t             txCurrLimit; //! Maximal Tx current
    uint8_t				hiPowerMode;
    LORA_CrcMode_t      crcTxMode;
    LORA_CrcMode_t      crcRxMode;
    LORA_IQmode_t       modeIQ;
    uint8_t				oscillator;
}SX127x_Options_t;

#pragma pack()
//
//
//
extern LORA_Driver_t sx127x_driver;

//
//Public functions
//
void sx127x_initOptions(void* options);

uint8_t sx127x_read_register(LORA_Instance_t* sx127x_instance, uint8_t regAddr);
LORA_Result_t sx127x_write_register(LORA_Instance_t* sx127x_instance, uint8_t regAddr, uint8_t value);
LORA_Result_t sx127x_set_op_mode(LORA_Instance_t* sx127x_instance, uint8_t modeMask);
uint8_t sx127x_fsk_ramp_code(LORA_Instance_t* sx127x_instance, uint16_t time);
LORA_Result_t sx127x_setFreq(LORA_Instance_t* sx127x_instance, uint32_t freq);
uint32_t sx127x_getFreq(LORA_Instance_t* sx127x_instance);
uint8_t sx127x_BandwidthCode(uint32_t bw);
uint8_t sx127x_Tx_OCP_code(uint8_t current);
uint8_t sx127x_Tx_OCP_current(uint8_t code);
LORA_Result_t sx127x_sleep(LORA_Instance_t* sx127x_instance);
LORA_Result_t sx127x_idle(LORA_Instance_t* sx127x_instance);
LORA_Result_t sx127x_setTxPWL(LORA_Instance_t* sx127x_instance, int8_t power);
uint32_t sx127x_bandwidth(LORA_Instance_t* sx127x_instance);
uint8_t sx127x_spreadingFactor(LORA_Instance_t* sx127x_instance);
LORA_Result_t sx127x_setBW(LORA_Instance_t* sx127x_instance, uint32_t bw);
LORA_Result_t sx127x_setSF(LORA_Instance_t* sx127x_instance, uint8_t sf);
LORA_Result_t sx127x_deinit(LORA_Instance_t* sx127x_instance);
LORA_Result_t sx127x_remapDIO(LORA_Instance_t* sx127x_instance);
uint32_t sx127x_symDuration(LORA_Instance_t* sx127x_instance);
LORA_Result_t sx127x_setPreambleLen(LORA_Instance_t* sx127x_instance, uint16_t prea_len);
LORA_Result_t sx127x_applyOptions(LORA_Instance_t* sx127x_instance);
LORA_Result_t sx127x_init(LORA_Instance_t* sx127x_instance);
LORA_Result_t sx127x_send(LORA_Instance_t* sx127x_instance, const uint8_t* data, uint8_t length, uint16_t timeout);
uint8_t sx127x_rxAvail(LORA_Instance_t* sx127x_instance, uint8_t* tc);
LORA_Result_t sx127x_read(LORA_Instance_t* sx127x_instance, uint8_t* dest, uint8_t length);
LORA_Result_t sx127x_recieve(LORA_Instance_t* sx127x_instance, uint8_t count,  uint8_t continuous);
int8_t sx127x_rssi(LORA_Instance_t* sx127x_instance);
int16_t sx127x_snr(LORA_Instance_t* sx127x_instance);
uint16_t sx127x_preambleLen(LORA_Instance_t* sx127x_instance);
LORA_Result_t sx127x_checkForChannelFree(LORA_Instance_t* sx127x_instance, int8_t rssi_free, uint16_t time);
LORA_Result_t sx127x_prepareTx(LORA_Instance_t* sx127x_instance, const uint8_t* data, uint8_t length);
LORA_Result_t sx127x_sendPrepared(LORA_Instance_t* sx127x_instance, uint8_t length, uint16_t timeout);
uint16_t sx127x_getRandom16(LORA_Instance_t* sx127x_instance);
uint8_t sx127x_modemStatus(LORA_Instance_t* sx127x_instance);
uint8_t sx127x_payloadLen(LORA_Instance_t* sx127x_instance, uint8_t length);
int32_t sx127x_freqError(LORA_Instance_t* sx127x_instance);
LORA_Result_t sx127x_setPpmCorrection(LORA_Instance_t* sx127x_instance, int8_t correction_factor);
void sx127x_initRandomGen(uint32_t value);
LORA_Result_t sx127x_send_async(LORA_Instance_t* sx127x_instance, const uint8_t* data, uint8_t length);
void sx127x_setDIO(LORA_Instance_t* sx127x_instance, uint16_t dioMap);
uint16_t sx127x_getDIO(LORA_Instance_t* sx127x_instance);

#endif /* APPLICATION_LORA_DRIVERS_SX127X_DRIVER_H_ */

