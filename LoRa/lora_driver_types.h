/*
 * lora_driver_types.h
 *
 *  Created on: 16 îêò. 2018 ã.
 *      Author: User
 */

#ifndef APPLICATION_LORA_DRIVERS_LORA_DRIVER_TYPES_H_
#define APPLICATION_LORA_DRIVERS_LORA_DRIVER_TYPES_H_
#include "platform.h"


//!LoRa result
typedef enum
{
    //!No error
    LRR_Ok = 0,
    //!Channel is not initialized
    LRR_Not_initialised,
    //!Hardware device is not compatible with driver
    LRR_Incompatible_device,
    //!Invalid data. Some data are not valid for use
    LRR_Invalid_data,
    //!Invalid or not defined channel control structure
    LRR_Invalid_channel,
    //!Invalid or not defined chip specific options
    LRR_Invalid_channel_opts,
    //!Channel handle not found
    LRR_Invalid_handle,
    //!Method is not supported by chip driver
    LRR_Method_not_found,
    //!SPI interface transaction error
    LRR_SPI_error,
    //!Timeout
    LRR_Timeout,
    //!Requested mode is not valid
    LRR_Invalid_mode,
    //!LoRa channel is busy
    LRR_Busy,
    //!Some error
    LRR_Unknown_Error
}LORA_Result_t;

//! LoRa header type
typedef enum
{
    //!Header type will calculated automatically
    LHT_Auto = 0,
    //!Force use of implicit header
    LHT_Implicit,
    //!Force use of explicit header
    LHT_Explicit
}LORA_HeaderType_t;

//! LoRa CRC mode
typedef enum
{
    //!CRC mode will calculated automatically
    LCRC_Auto = 0,
    //!Force CRC mode on
    LCRC_ForceOn,
    //!Force CRC mode off
    LCRC_ForceOff
}LORA_CrcMode_t;

//! IQ mode
typedef enum
{
    //!I & Q bits are normal
    LIQ_Normal = 0,
    //!I & Q bits are inverted
    LIQ_Invert = 1
}LORA_IQmode_t;

//! Modulation
typedef enum
{
    //!LoRa™ mode
    LMT_LORA = 0,
    //!FSK mode (may be not supported)
    LMT_FSK,
    //!OOk mode (may be not supported)
    LMT_OOK
}LORA_Modulation_t;

//
//
//

typedef void (*txTC)();

#pragma pack(1)

//! @brief LoRa channel instance control structure
//! @details Contains all channel parameters, need for correct working. Some parameters may be calculated automatically
typedef struct
{
    //! RF carrier frequency (Hz)
    uint32_t            mainFreq;
    //! Channel bandwidth (Hz)
    uint32_t            bandwidth;
    //! Channel spreading factor
    uint8_t             spreadFtr;
    //! @brief Transmitter power [dBm]
    //! @details Bigger value = more power. See manual for correct values
    int8_t            txPWL;
    //! LoRa sync. word. Used like a network ID
    uint8_t             syncWord;
    //! Preamble length
    uint16_t			preambleLen;
    //! @brief Default payload length.
    //! @details Useful in implicit header mode only
    uint8_t             pldLen;
    //! @brief Max. payload length.
    //! @details Useful for filtering packets with wrong size. Included for LoRaWAN
    uint8_t				maxPldLen;
    //! @brief Pins definition
    //! @see LORA_PinList_t
    LORA_PinList_t*     pins;
    //! SPI channel handler
    SPI_Handle          spiHandle;
    //! @brief Type of LoRa header
    //! @see LORA_HeaderType_t
    LORA_HeaderType_t   hdrType;
    //! @brief Modulation type
    //! @see LORA_Modulation_t
    LORA_Modulation_t   modemMode;
    //! Chip specific options
    void*               options;
}LORA_Instance_t;

#pragma pack()

typedef struct
{
    LORA_Result_t   (*init)(LORA_Instance_t*);
    LORA_Result_t   (*send)(LORA_Instance_t*, const uint8_t*,/*data*/uint8_t,/*length*/uint16_t/*timeout*/);
    uint8_t         (*rxAvail)(LORA_Instance_t*, uint8_t*/*transfer complete flag*/);
    LORA_Result_t   (*read)(LORA_Instance_t*, uint8_t*,/*buffer*/uint8_t/*max. length*/);
    LORA_Result_t   (*deInit)(LORA_Instance_t*);
    LORA_Result_t   (*setFreq)(LORA_Instance_t*, uint32_t/*RF frequency*/);
    LORA_Result_t   (*setBW)(LORA_Instance_t*, uint32_t/*Bandwidth*/);
    LORA_Result_t   (*setSF)(LORA_Instance_t*, uint8_t/*spreading factor*/);
    uint32_t        (*getFreq)(LORA_Instance_t*);
    uint32_t        (*getBW)(LORA_Instance_t*);
    uint8_t         (*getSF)(LORA_Instance_t*);
    LORA_Result_t   (*setTxPWL)(LORA_Instance_t*, int8_t/*Tx power [dBm]*/);
    LORA_Result_t   (*sleep)(LORA_Instance_t*);
    LORA_Result_t   (*idle)(LORA_Instance_t*);
    LORA_Result_t   (*recieve)(LORA_Instance_t*, uint8_t/*count*/, uint8_t/*continous*/);
    LORA_Result_t   (*applyOptions)(LORA_Instance_t*);
    LORA_Result_t   (*remapDIO)(LORA_Instance_t*);
    int8_t          (*rssi)(LORA_Instance_t*);
    int16_t         (*snr)(LORA_Instance_t*);
    uint32_t        (*symDuration)(LORA_Instance_t*);
    uint16_t        (*preambleLen)(LORA_Instance_t*);
    LORA_Result_t   (*checkForChannelFree)(LORA_Instance_t*, int8_t /*rssi_free*/, uint16_t/*time*/);
    LORA_Result_t   (*prepareTx)(LORA_Instance_t*, const uint8_t*,/*data*/uint8_t /*length*/);
    LORA_Result_t   (*sendPrepared)(LORA_Instance_t*, uint8_t /*length*/, uint16_t/*timeout*/);
    uint16_t		(*getRandom16)();
    LORA_Result_t	(*writeReg)(LORA_Instance_t*, uint8_t /*reg_num*/, uint8_t /*reg_value*/);
    uint8_t			(*readReg)(LORA_Instance_t*, uint8_t /*reg_num*/);
    uint8_t			(*modemStatus)(LORA_Instance_t*);
    LORA_Result_t	(*setPreambleLen)(LORA_Instance_t*, uint16_t /*prea_len*/);
    uint8_t			(*payloadLen)(LORA_Instance_t* /*instance*/, uint8_t /*length*/);
    int32_t	    	(*freqError)(LORA_Instance_t* /*instance*/);
    LORA_Result_t	(*setPpmCorrection)(LORA_Instance_t* /*instance*/, int8_t /*correction factor*/);
    void			(*initRandomGenerator)(uint32_t);
    LORA_Result_t	(*send_async)(LORA_Instance_t*, const uint8_t*,/*data*/uint8_t/*length*/);
    void			(*setDIO)(LORA_Instance_t*, uint16_t);
    uint16_t		(*getDIO)(LORA_Instance_t*);
}LORA_Driver_t;



#endif /* APPLICATION_LORA_DRIVERS_LORA_DRIVER_TYPES_H_ */
