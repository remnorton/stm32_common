/*
 * lora_channel.c
 *
 *  Created on: 21 ñåíò. 2018 ã.
 *      Author: User
 *
 * Unificated LoRa™ channel level hardware-independed library.
 * Usage:
 *
 * 1. Declare and fill 'LORA_Instance_t' type variable. For example:
 *        LORA_Instance_t instance = {
 *                                      433000000,       // mainFreq;      //! RF frequency (Hz)
 *                                      125000,          //bandwidth;      //! Channel bandwidth (Hz)
 *                                      9,               //spreadFtr;      //! Channel spreading factor
 *                                      5,               //txPWL;          //! Transmitter power level
 *                                      0x12,            //syncWord;       //! LoRa sync. word. Used like a network ID
 *                                      0x40,            //pldLen;         //! Payload length
 *                                      NULL,            //pins;           //! Pins definition
 *                                      NULL,            //spiHandle;      //! SPI channel handler
 *                                      LHT_Explicit,    //hdrType;        //! Type of LoRa header
 *                                      NULL             //options;        //! Chip specific options
 *                              };
 *
 * 2. Declare and fill 'LORA_PinList_t' type variable. For example:
 *          LORA_PinList_t loraPins = {
 *                                           NULL,                   //PIN_Handle      pinHandle;      //! GPIO handle
 *                                           {Board_LORA_CS, 0},     //LORA_PinDef_t   cs_pin;         //! Chip select pin
 *                                           {Board_LORA_RES, 0},    //LORA_PinDef_t   rst_pin;        //! Reset pin
 *                                     };
 *
 * 3. Declare and fill 'LORA_Channel_t' type variable. For example:
 *          LORA_Channel_t channel = {&sx127x_driver, &instance};
 *
 * 4. Declare driver specific type variable. For example:
 *          SX127x_Options_t options;
 *
 * 5. Fill driver specific by default values and set desired values. For example:
 *          sx127x_initOptions((void*)&options);
 *          options.boostPA = 1;
 *          options.crcTxMode = LCRC_ForceOn;
 *          options.crcRxMode = LCRC_ForceOff;
 *
 *
 * 6. Initialize channel.
 *          loraPins.pinHandle = PIN_open(&loraChPinState, loraPinTable);
 *          instance.spiHandle = SPI_open(Board_SPI0, &spiParams);
 *          instance.pins = &loraPins;
 *          instance.options = (void*)&options;
 *
 *          handle = lora_channel_init(&channel);
 *
 *  7. Initiate receiving if needed. For example:
 *         lora_channel_recieve(handle, 1);
 *
 *  8. Wait for some packet be received. For example:
 *          uint8_t tc = 0;
 *          uint8_t readed = lora_channel_rxAvail(handle, &tc);
 *          if (tc)
 *          {
 *              if (lora_channel_read(handle, {pointer to buffer}, readed) == LRR_Ok)
 *              {
 *                  //Packet received. Do something
 *              }
 *          };
 *
 *  9. Sending packets:
 *          lora_channel_send(handle, {pointer to buffer}, {length}, {timeout in msec});    //Send data
 *          lora_channel_recieve(handle, 1);                                                //Back to receive mode
 *
 */

#include "lora_channel.h"


static LORA_Channel_t* channels[_LORA_MAX_CHANNELS_];
static uint8_t new_handle = 0;


//
//Private members
//
LORA_Channel_t* getChannelByHandle(uint8_t handle, LORA_Result_t* result)
{
	if ((!handle) || (handle > new_handle))
	{
		if (result) *result = LRR_Invalid_handle;
		return 0;
	}

	LORA_Channel_t* res = channels[handle-1];
	if (!res)
	{
		if (result) *result = LRR_Invalid_channel;
		return 0;
	}

	if ((!res->drvSettings) || (!res->driver))
	{
		if (result) *result = LRR_Invalid_channel;
		return 0;
	}

	if (result) *result = LRR_Ok;
	return res;
}

//
//Public members
//

/**
 * @brief Initializes and open for exchange the LoRa channel
 * @param ch_init - Pointer to LoRa channel control structure
 * @return Opened channel handle if succeeded, 0 - if error.
 */
uint8_t lora_channel_init(LORA_Channel_t* ch_init)
{
    if (!ch_init) return 0;
    if (!(ch_init->driver && ch_init->drvSettings)) return 0;
    if (!ch_init->driver->init) return 0;

    if ((new_handle+1) < _LORA_MAX_CHANNELS_)
    {
        if (ch_init->driver->init(ch_init->drvSettings) == LRR_Ok)
        {
            channels[new_handle++] = ch_init;
        	return new_handle;
        }
    }
    return 0;
}

/**
 * @brief Sends some data through opened LoRa channel
 * @param handle - LoRa channel handle
 * @param data - Pointer to user data to send
 * @param length - Length of user data in bytes
 * @param timeout - Operation timeout in msec.
 * @return Result of execution (LRR_Ok if succeeded).
 */
LORA_Result_t lora_channel_send(uint8_t handle, const uint8_t* data, uint8_t length, uint16_t timeout)
{
    if (!handle) return LRR_Invalid_handle;
    if (handle > new_handle) return LRR_Invalid_handle;
    LORA_Channel_t* channel = channels[handle-1];
    if (!channel->driver->send) return LRR_Method_not_found;
    return channel->driver->send(channel->drvSettings, data, length, timeout);
}

/**
 * @brief Checks LoRa module internal FIFO buffer of receiver for received data.
 * @param handle - LoRa channel handle
 * @param tc - Pointer to "Transfer Complete" flag. May be zero if not needed
 * @return Quantity of received bytes in internal FIFO. Sets "Transfer Complete" flag (if defined) if operation completely succeeded
 */
uint8_t lora_channel_rxAvail(uint8_t handle, uint8_t* tc)
{
    if (!handle) return 0;
    if (handle > new_handle) return 0;
    LORA_Channel_t* channel = channels[handle-1];
    if (!channel->driver->rxAvail) return 0;
    return channel->driver->rxAvail(channel->drvSettings, tc);
}

/**
 * @brief Reads data from internal FIFO
 * @param handle - LoRa channel handle
 * @param buffer - Destination buffer for transfer read data
 * @param length - Maximum length for read in bytes
 * @return Result of execution (LRR_Ok if succeeded).
 */
LORA_Result_t lora_channel_read(uint8_t handle, uint8_t* buffer, uint8_t length)
{
    if (!handle) return LRR_Invalid_handle;
    if (handle > new_handle) return LRR_Invalid_handle;
    LORA_Channel_t* channel = channels[handle-1];
    if (!channel->driver->read) return LRR_Method_not_found;
    return channel->driver->read(channel->drvSettings, buffer, length);
}

/**
 * @brief Uninitialize LoRa channel but not deletes it
 * @param handle - LoRa channel handle
 * @return Result of execution (LRR_Ok if succeeded).
 */
LORA_Result_t lora_channel_deInit(uint8_t handle)
{
    if (!handle) return LRR_Invalid_handle;
    if (handle > new_handle) return LRR_Invalid_handle;
    LORA_Channel_t* channel = channels[handle-1];
    if (!channel->driver->deInit) return LRR_Method_not_found;
    return channel->driver->deInit(channel->drvSettings);
}

/**
 * @brief Set LoRa channel carrier frequency
 * @param handle - LoRa channel handle
 * @param freq - Carrier frequency in Hz
 * @return Result of execution (LRR_Ok if succeeded)
 */
LORA_Result_t lora_channel_setCarrierFrequency(uint8_t handle, uint32_t freq)
{
    if (!handle) return LRR_Invalid_handle;
    if (handle > new_handle) return LRR_Invalid_handle;
    LORA_Channel_t* channel = channels[handle-1];
    if (!channel->driver->setFreq) return LRR_Method_not_found;
    return channel->driver->setFreq(channel->drvSettings, freq);
}

/**
 * @brief Sets LoRa channel bandwidth
 * @param handle - LoRa channel handle
 * @param bw - Bandwidth in Hz. Incorrect values will automatically replaced by closest correct. (See manual for correct values)
 * @return Result of execution (LRR_Ok if succeeded)
 */
LORA_Result_t lora_channel_setBandwidth(uint8_t handle, uint32_t bw)
{
    if (!handle) return LRR_Invalid_handle;
    if (handle > new_handle) return LRR_Invalid_handle;
    LORA_Channel_t* channel = channels[handle-1];
    if (!channel->driver->setBW) return LRR_Method_not_found;
    return channel->driver->setBW(channel->drvSettings, bw);
}

/**
 * @brief Sets LoRa channel spreading factor.
 * @param handle - LoRa channel handle
 * @param sf - Spreading factor. Incorrect values will automatically replaced by closest correct. (See manual for correct values)
 * @return Result of execution (LRR_Ok if succeeded)
 */
LORA_Result_t lora_channel_setSpreadingFactor(uint8_t handle, uint8_t sf)
{
    if (!handle) return LRR_Invalid_handle;
    if (handle > new_handle) return LRR_Invalid_handle;
    LORA_Channel_t* channel = channels[handle-1];
    if (!channel->driver->setSF) return LRR_Method_not_found;
    return channel->driver->setSF(channel->drvSettings, sf);
}

/**
 * @brief Returns currently set carrier frequency of LoRa channel
 * @param handle - LoRa channel handle
 * @return Carrier frequency in Hz
 */
uint32_t lora_channel_getCarrierFrequency(uint8_t handle)
{
    if (!handle) return 0;
    if (handle > new_handle) return 0;
    LORA_Channel_t* channel = channels[handle-1];
    if (!channel->driver->getFreq) return 0;
    return channel->driver->getFreq(channel->drvSettings);
}

/**
 * @brief Returns currently set bandwidth of LoRa channel
 * @param handle - LoRa channel handle
 * @return Bandwidth in Hz
 */
uint32_t lora_channel_getBandwidth(uint8_t handle)
{
    if (!handle) return 0;
    if (handle > new_handle) return 0;
    LORA_Channel_t* channel = channels[handle-1];
    if (!channel->driver->getBW) return 0;
    return channel->driver->getBW(channel->drvSettings);
}

/**
 * @brief Returns currently set spreading factor
 * @param handle - LoRa channel handle
 * @return Currently set spreading factor
 */
uint8_t lora_channel_getSpreadingFactor(uint8_t handle)
{
    if (!handle) return 0;
    if (handle > new_handle) return 0;
    LORA_Channel_t* channel = channels[handle-1];
    if (!channel->driver->getSF) return 0;
    return channel->driver->getSF(channel->drvSettings);
}

/**
 * @brief Sets transmitter power level
 * @param handle - LoRa channel handle
 * @param level - Value of desired transmitter power level.  Incorrect values will automatically replaced by closest correct. (See manual for correct values)
 * @return Result of execution (LRR_Ok if succeeded)
 */
LORA_Result_t lora_channel_setTxPowerLevel(uint8_t handle, int8_t level)
{
    if (!handle) return LRR_Invalid_handle;
    if (handle > new_handle) return LRR_Invalid_handle;
    LORA_Channel_t* channel = channels[handle-1];
    if (!channel->driver->setTxPWL) return LRR_Method_not_found;
    return channel->driver->setTxPWL(channel->drvSettings, level);
}

/**
 * @brief Switches LoRa channel to sleep mode
 * @param handle - LoRa channel handle
 * @return Result of execution (LRR_Ok if succeeded)
 */
LORA_Result_t   lora_channel_sleep(uint8_t handle)
{
    if (!handle) return LRR_Invalid_handle;
    if (handle > new_handle) return LRR_Invalid_handle;
    LORA_Channel_t* channel = channels[handle-1];
    if (!channel->driver->sleep) return LRR_Method_not_found;
    return channel->driver->sleep(channel->drvSettings);
}


/**
 * @brief Switches LoRa channel to standby mode
 * @param handle - LoRa channel handle
 * @return Result of execution (LRR_Ok if succeeded)
 */
LORA_Result_t lora_channel_idle(uint8_t handle)
{
    if (!handle) return LRR_Invalid_handle;
    if (handle > new_handle) return LRR_Invalid_handle;
    LORA_Channel_t* channel = channels[handle-1];
    if (!channel->driver->idle) return LRR_Method_not_found;
    return channel->driver->idle(channel->drvSettings);
}

/**
 * @brief Switches LoRa channel to receive mode
 * @param handle - LoRa channel handle
 * @param count - Quantity of bytes to read. Not matters in explicit header mode.
 * @param continuous - Flag of continuous receiving. If is not set LoRa switches to sleep mode after timeout
 * @return Result of execution (LRR_Ok if succeeded)
 */
LORA_Result_t lora_channel_recieve(uint8_t handle, uint8_t count, uint8_t continuous)
{
    if (!handle) return LRR_Invalid_handle;
    if (handle > new_handle) return LRR_Invalid_handle;
    LORA_Channel_t* channel = channels[handle-1];
    if (!channel->driver->recieve) return LRR_Method_not_found;
    return channel->driver->recieve(channel->drvSettings, count, continuous);
}

/**
 * @brief Applies options. All options with auto values (like a header type) will previously calculated
 * @param handle - LoRa channel handle
 * @return Result of execution (LRR_Ok if succeeded)
 */
LORA_Result_t   lora_channel_applyOptions(uint8_t handle)
{
    if (!handle) return LRR_Invalid_handle;
    if (handle > new_handle) return LRR_Invalid_handle;
    LORA_Channel_t* channel = channels[handle-1];
    if (!channel->driver->applyOptions) return LRR_Method_not_found;
    return channel->driver->applyOptions(channel->drvSettings);
}

/**
 * @brief Makes remapping of functions of GPIO pins
 * @param handle - LoRa channel handle
 * @return Result of execution (LRR_Ok if succeeded)
 */
LORA_Result_t   lora_channel_remapDIO(uint8_t handle)
{
    if (!handle) return LRR_Invalid_handle;
    if (handle > new_handle) return LRR_Invalid_handle;
    LORA_Channel_t* channel = channels[handle-1];
    if (!channel->driver->remapDIO) return LRR_Method_not_found;
    return channel->driver->remapDIO(channel->drvSettings);
}

/**
 * @brief Returns RSSI of last received packet
 * @param handle - LoRa channel handle
 * @return RSSI on dBm
 */
int8_t          lora_channel_rssi(uint8_t handle)
{
    if (!handle) return 0;
    if (handle > new_handle) return 0;
    LORA_Channel_t* channel = channels[handle-1];
    if (!channel->driver->rssi) return 0;
    return channel->driver->rssi(channel->drvSettings);
}

/**
 * @brief Returns Signal-to-Noise-Ratio of last received packet
 * @param handle - LoRa channel handle
 * @return SNR (x1000). Divide by 1000 for true value.
 */
int16_t         lora_channel_snr(uint8_t handle)
{
    if (!handle) return 0;
    if (handle > new_handle) return 0;
    LORA_Channel_t* channel = channels[handle-1];
    if (!channel->driver->snr) return 0;
    return channel->driver->snr(channel->drvSettings);
}

/**
 * @brief Returns time needed for transmit of one byte
 * @param handle - LoRa channel handle
 * @return Time needed for transmit of one byte in msec
 */
uint32_t        lora_channel_symDuration(uint8_t handle)
{
    if (!handle) return 0;
    if (handle > new_handle) return 0;
    LORA_Channel_t* channel = channels[handle-1];
    if (!channel->driver->symDuration) return 0;
    return channel->driver->symDuration(channel->drvSettings);
}

/**
 * @brief Returns size of LoRa preamble
 * @param handle - LoRa channel handle
 * @return Preamble size in bytes
 */
uint16_t        lora_channel_preambleLen(uint8_t handle)
{
    if (!handle) return 0;
    if (handle > new_handle) return 0;
    LORA_Channel_t* channel = channels[handle-1];
    if (!channel->driver->preambleLen) return 0;
    return channel->driver->preambleLen(channel->drvSettings);
}

/**
 * @brief Checks for channel activity
 * @param handle - LoRa channel handle
 * @param rssi_free - RSSI level makes as "free"
 * @return LRR_Ok if channel free, LRR_Busy if channel busy. Other values if some error
 */
LORA_Result_t   lora_channel_checkForChannelFree(uint8_t handle, int8_t rssi_free, uint16_t listen_time)
{
    if (!handle) return LRR_Invalid_handle;
    if (handle > new_handle) return LRR_Invalid_handle;
    LORA_Channel_t* channel = channels[handle-1];
    if (!channel->driver->checkForChannelFree) return LRR_Method_not_found;
    return channel->driver->checkForChannelFree(channel->drvSettings, rssi_free, listen_time);

}
/**
 * @brief Loads data into tx. FIFO buffer, but not sends it
 * @param handle - LoRa channel handle
 * @param data - Data to be loaded
 * @param length - Length of data to be loaded
 * @return Result of operation
 */
LORA_Result_t   lora_channel_prepareTx(uint8_t handle, const uint8_t* data, uint8_t length)
{
    if (!handle) return LRR_Invalid_handle;
    if (handle > new_handle) return LRR_Invalid_handle;
    LORA_Channel_t* channel = channels[handle-1];
    if (!channel->driver->prepareTx) return LRR_Method_not_found;
    return channel->driver->prepareTx(channel->drvSettings, data, length);
}

/**
 * @brief Sends data, contains in internal tx. FIFO buffer
 * @param handle - LoRa channel handle
 * @param length - Length of data to be send
 * @param timeout - Timeout (msec)
 * @return Result of operation
 */
LORA_Result_t   lora_channel_sendPrepared(uint8_t handle, uint8_t length, uint16_t timeout)
{
    if (!handle) return LRR_Invalid_handle;
    if (handle > new_handle) return LRR_Invalid_handle;
    LORA_Channel_t* channel = channels[handle-1];
    if (!channel->driver->sendPrepared) return LRR_Method_not_found;
    return channel->driver->sendPrepared(channel->drvSettings, length, timeout);
}

/**
 * @brief Generates random 16-bit value
 * @param handle - LoRa channel handle
 * @return 16-bit random value
 */
uint16_t lora_channel_getRandom16(uint8_t handle)
{
	if (!handle) return 0;
	if (handle > new_handle) return 0;
	LORA_Channel_t* channel = channels[handle-1];
	if (!channel->driver->getRandom16) return LRR_Method_not_found;
	return channel->driver->getRandom16(channel->drvSettings);
}

/**
 * @brief Returns time needed for transmit LoRa preamble
 * @param handle - LoRa channel handle
 * @return Time needed for transmit LoRa preamble in msec
 */
uint32_t        lora_channel_preambleDuration(uint8_t handle)
{
    return lora_channel_preambleLen(handle)*lora_channel_symDuration(handle);
}

/**
 * @brief Returns time needed for transmit data
 * @param handle - LoRa channel handle
 * @param payloadLen - Size of packet payload in bytes
 * @return Time needed for transmit data through LoRa channel in msec
 */
uint32_t lora_channel_packetDuration(uint8_t handle, uint8_t payloadLen)
{
	LORA_Channel_t* channel = channels[handle-1];
	if (!channel) return 0;
	if (handle > new_handle) return 0;
	int16_t pl = 0;
	if (channel->driver->payloadLen)
		pl = channel->driver->payloadLen(channel->drvSettings, payloadLen);
    return ((lora_channel_preambleLen(handle)*100 + 425 + pl*100)*lora_channel_symDuration(handle))/100000;
}

/**
 * @brief Direct write to modem register
 * @param handle - LoRa channel handle
 * @param reg_num - Address of register
 * @param reg_value - New value for register
 * @return Result of operation
 */
LORA_Result_t lora_channel_writeReg(uint8_t handle, uint8_t reg_num, uint8_t reg_value)
{
	if (!handle) return 0;
	if (handle > new_handle) return 0;
	LORA_Channel_t* channel = channels[handle-1];
	if (!channel->driver->writeReg) return LRR_Method_not_found;
	return channel->driver->writeReg(channel->drvSettings, reg_num, reg_value);
}

/**
 * @brief Direct read of modem register
 * @param handle - LoRa channel handle
 * @param reg_num - Address of register
 * @return Register value
 */
uint8_t lora_channel_readReg(uint8_t handle, uint8_t reg_num)
{
	if (!handle) return 0;
	if (handle > new_handle) return 0;
	LORA_Channel_t* channel = channels[handle-1];
	if (!channel->driver->readReg) return LRR_Method_not_found;
	return channel->driver->readReg(channel->drvSettings, reg_num);
}

/**
 * @brief Returns the modem status bitmask
 * @param handle - LoRa channel handle
 * @return Modem status bitmask
 */
uint8_t lora_channel_modemStatus(uint8_t handle)
{
	if (!handle) return LRR_Invalid_handle;
	if (handle > new_handle) return LRR_Invalid_handle;
	LORA_Channel_t* channel = channels[handle-1];
	if (!channel->driver->modemStatus) return LRR_Method_not_found;
	return channel->driver->modemStatus(channel->drvSettings);
}

/**
 * @brief Sets the length of LoRa PHY preamble
 * @param handle - LoRa channel handle
 * @param prea_len - Length of preamble
 * @return Result of operation
 */
LORA_Result_t lora_channel_setPreambleLen(uint8_t handle, uint16_t prea_len)
{
	if (!handle) return 0;
	if (handle > new_handle) return 0;
	LORA_Channel_t* channel = channels[handle-1];
	if (!channel->driver->setPreambleLen) return LRR_Method_not_found;
	return channel->driver->setPreambleLen(channel->drvSettings, prea_len);
}

/**
 * @brief Returns current channel settings
 * @param handle - LoRa channel handle
 * @return Pointer to channel settings
 */
LORA_Instance_t* 	lora_channel_settings(uint8_t handle)
{
	if (!handle) return 0;
	if (handle > new_handle) return 0;
	LORA_Channel_t* channel = channels[handle-1];
	return channel->drvSettings;
}

LORA_Result_t lora_channel_reset(uint8_t handle)
{
	if ((!handle) || (handle > new_handle)) return LRR_Invalid_handle;
	LORA_Instance_t* instance = channels[handle-1]->drvSettings;
	if (!instance) return LRR_Invalid_channel;
	if (!instance->pins) return LRR_Invalid_data;
	HAL_GPIO_WritePin(instance->pins->rst_pin.port, instance->pins->rst_pin.pin, instance->pins->rst_pin.actState);
	//HAL_Delay(50);
	for (int i = 0; i < 1000; i++);
	HAL_GPIO_WritePin(instance->pins->rst_pin.port, instance->pins->rst_pin.pin, !instance->pins->rst_pin.actState);
	//HAL_Delay(150);
	for (int i = 0; i < 100000; i++);
	return LRR_Ok;
}

uint32_t lora_channel_freqError(uint8_t handle)
{
	if (!handle) return 0;
	if (handle > new_handle) return 0;
	LORA_Channel_t* channel = channels[handle-1];
	if (!channel->driver->freqError) return 0;
	return channel->driver->freqError(channel->drvSettings);
}

LORA_Result_t lora_channel_setPpmCorrection(uint8_t handle, uint8_t correction_factor)
{
	if (!handle) return LRR_Invalid_handle;
	if (handle > new_handle) return LRR_Invalid_handle;
	LORA_Channel_t* channel = channels[handle-1];
	if (!channel->driver->setPpmCorrection) return LRR_Method_not_found;
	return channel->driver->setPpmCorrection(channel->drvSettings, correction_factor);
}

LORA_Result_t lora_channel_initRandom(uint8_t handle, uint32_t value)
{
	if (!handle) return LRR_Invalid_handle;
	if (handle > new_handle) return LRR_Invalid_handle;
	LORA_Channel_t* channel = channels[handle-1];
	if (!channel->driver->initRandomGenerator) return LRR_Method_not_found;
	channel->driver->initRandomGenerator(value);
	return LRR_Ok;
}

LORA_Result_t lora_channel_send_async(uint8_t handle, const uint8_t* data, uint8_t length, txTC callback)
{
	if (!handle) return LRR_Invalid_handle;
	if (handle > new_handle) return LRR_Invalid_handle;
	LORA_Channel_t* channel = channels[handle-1];
	if (!channel->driver->send_async) return LRR_Method_not_found;

	channel->driver->send_async(channel->drvSettings, data, length);
	return LRR_Ok;
}
