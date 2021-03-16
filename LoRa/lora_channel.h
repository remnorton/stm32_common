/*
 * lora_channel.h
 *
 *  Created on: 21 сент. 2018 г.
 *      Author: User
 */

#ifndef APPLICATION_LORA_DRIVERS_LORA_CHANNEL_H_
#define APPLICATION_LORA_DRIVERS_LORA_CHANNEL_H_
#include "lora_driverlib.h"


#define _LORA_MAX_CHANNELS_     5

typedef struct
{
    LORA_Driver_t*      driver;
    LORA_Instance_t*    drvSettings;
}LORA_Channel_t;



uint8_t         	lora_channel_init(LORA_Channel_t* init);
LORA_Result_t   	lora_channel_send(uint8_t handle, const uint8_t* data, uint8_t length, uint16_t timeout);
uint8_t         	lora_channel_rxAvail(uint8_t handle, uint8_t* tc);
LORA_Result_t   	lora_channel_read(uint8_t handle, uint8_t* buffer, uint8_t length);
LORA_Result_t   	lora_channel_deInit(uint8_t handle);
LORA_Result_t   	lora_channel_setCarrierFrequency(uint8_t handle, uint32_t freq);
LORA_Result_t   	lora_channel_setBandwidth(uint8_t handle, uint32_t bw);
LORA_Result_t   	lora_channel_setSpreadingFactor(uint8_t handle, uint8_t sf);
uint32_t        	lora_channel_getCarrierFrequency(uint8_t handle);
uint32_t        	lora_channel_getBandwidth(uint8_t handle);
uint8_t         	lora_channel_getSpreadingFactor(uint8_t handle);
LORA_Result_t   	lora_channel_setTxPowerLevel(uint8_t handle, int8_t power);
LORA_Result_t   	lora_channel_sleep(uint8_t handle);
LORA_Result_t   	lora_channel_idle(uint8_t handle);
LORA_Result_t   	lora_channel_recieve(uint8_t handle, uint8_t count, uint8_t continous);
LORA_Result_t   	lora_channel_applyOptions(uint8_t handle);
LORA_Result_t   	lora_channel_remapDIO(uint8_t handle);
int8_t          	lora_channel_rssi(uint8_t handle);
int16_t         	lora_channel_snr(uint8_t handle);
uint32_t        	lora_channel_symDuration(uint8_t handle);
uint16_t        	lora_channel_preambleLen(uint8_t handle);
LORA_Result_t   	lora_channel_checkForChannelFree(uint8_t handle, int8_t rssi_free, uint16_t listen_time);
LORA_Result_t   	lora_channel_prepareTx(uint8_t handle, const uint8_t* data, uint8_t length);
LORA_Result_t   	lora_channel_sendPrepared(uint8_t handle, uint8_t length, uint16_t timeout);
uint16_t			lora_channel_getRandom16(uint8_t handle);
uint32_t        	lora_channel_preambleDuration(uint8_t handle);
uint32_t        	lora_channel_packetDuration(uint8_t handle, uint8_t payloadLen);
LORA_Result_t		lora_channel_writeReg(uint8_t handle, uint8_t reg_num, uint8_t reg_value);
uint8_t				lora_channel_readReg(uint8_t handle, uint8_t reg_num);
uint8_t				lora_channel_modemStatus(uint8_t handle);
LORA_Result_t		lora_channel_setPreambleLen(uint8_t handle, uint16_t prea_len);
LORA_Instance_t* 	lora_channel_settings(uint8_t handle);
LORA_Result_t		lora_channel_reset(uint8_t handle);
uint32_t			lora_channel_freqError(uint8_t handle);
LORA_Result_t		lora_channel_setPpmCorrection(uint8_t handle, uint8_t correction_factor);
LORA_Result_t		lora_channel_initRandom(uint8_t handle, uint32_t value);

LORA_Result_t		lora_channel_send_async(uint8_t handle, const uint8_t* data, uint8_t length, txTC callback);

#endif /* APPLICATION_LORA_DRIVERS_LORA_CHANNEL_H_ */
