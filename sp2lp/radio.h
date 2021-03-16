/*
 * radio.h
 *
 *  Created on: 25 ���. 2019 �.
 *      Author: user
 */

#ifndef RADIO_H_
#define RADIO_H_

#include "S2LP_Config.h"
#include "hardware_config.h"

#define USE_HIGH_BAND

#ifdef USE_MIDDLE_BAND
#define BASE_FREQUENCY              433000000
#endif

#ifdef USE_HIGH_BAND
#define BASE_FREQUENCY              915000000
#endif

#define MODULATION_SELECT           MOD_2FSK
#define DATARATE                    19200
#define FREQ_DEVIATION              40000
#define BANDWIDTH                   140000

#define POWER_DBM                  0/*-30 .. 14*/



/*  Packet configuration parameters  */
#define PREAMBLE_LENGTH             PREAMBLE_BYTE(25)
#define SYNC_LENGTH                 SYNC_BYTE(4)
#define SYNC_WORD                   0x88888888
#define VARIABLE_LENGTH             S_ENABLE
#define EXTENDED_LENGTH_FIELD       S_DISABLE
#define CRC_MODE                    PKT_CRC_MODE_32BITS
#define EN_ADDRESS                  S_DISABLE
#define EN_FEC                      S_DISABLE
#define EN_WHITENING                S_ENABLE


/* Wake Up timer in ms for LDC mode */
#define WAKEUP_TIMER                100/*.0*/


#define PREAMBLE_BYTE(v)        (4*v)
#define SYNC_BYTE(v)            (8*v)


 /* CSMA configuration parameters */
 #define PERSISTENT_MODE_EN              S_DISABLE/*S_ENABLE*/
 #define CS_PERIOD                       CSMA_PERIOD_64TBIT
 #define CS_TIMEOUT                      5
 #define MAX_NB                          5
 #define BU_COUNTER_SEED                 0xFA21
 #define CU_PRESCALER                    32

 #define RSSI_THR -80

//

typedef struct AirData{
		uint8_t TxData[128]; //Tx Data Buffer to send end "\0"
		uint8_t RxData[128]; //RxBuffer
		uint8_t cRxData; 	 //count RxBuffer
}xAirData;

typedef struct {
	uint8_t PN;
	uint8_t Version;
	uint8_t Available;
	uint8_t RCO_OK;
}xPresenceStatus;


extern xAirData xData;
extern volatile S2LPStatus g_xStatus;

uint8_t PresenceCheckRadio(xPresenceStatus*);
void Init_S2LP_Radio(void);

uint8_t S2LPManagementRcoCalibration(void);
void S2LPSetModeLPM(void);
void S2LP_Reset(void);

void Init_Transmitter(void);
void Init_Receiver(uint32_t timeot);

void TransmitBase(void);
void TransmitDataASK(uint8_t* data, uint8_t size);
void TransmitData(uint8_t* data, uint8_t size);

void ReceiveData(uint32_t sTimeOut);
void GetReceiveData(void);


#endif /* RADIO_H_ */
