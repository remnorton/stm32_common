/*
 * pn532.h
 *
 *  Created on: 15 θών. 2020 γ.
 *      Author: Rem Norton
 */

#ifndef NFC_PN532_PN532_H_
#define NFC_PN532_PN532_H_

#include "pn532_api.h"

#define MIFARE_UID_MAX_LENGTH               MIFARE_UID_TRIPLE_LENGTH
#define MIFARE_UID_SINGLE_LENGTH            (4)
#define MIFARE_UID_DOUBLE_LENGTH            (7)
#define MIFARE_UID_TRIPLE_LENGTH            (10)
#define MIFARE_KEY_LENGTH                   (6)
#define MIFARE_BLOCK_LENGTH                 (16)
#define PN532_MIFARE_ISO14443A              (0x00)


// Mifare Commands
#define MIFARE_CMD_AUTH_A                   (0x60)
#define MIFARE_CMD_AUTH_B                   (0x61)
#define MIFARE_CMD_READ                     (0x30)
#define MIFARE_CMD_WRITE                    (0xA0)
#define MIFARE_CMD_TRANSFER                 (0xB0)
#define MIFARE_CMD_DECREMENT                (0xC0)
#define MIFARE_CMD_INCREMENT                (0xC1)
#define MIFARE_CMD_STORE                    (0xC2)
#define MIFARE_ULTRALIGHT_CMD_WRITE         (0xA2)

/* Official PN532 Errors Definitions */
#define PN532_ERROR_NONE                                                (0x00)
// Time Out, the target has not answered
#define PN532_ERROR_TIMEOUT                                             (0x01)
// A CRC error has been detected by the CIU
#define PN532_ERROR_CRC                                                 (0x02)
// A Parity error has been detected by the CIU
#define PN532_ERROR_PARITY                                              (0x03)
// During an anti-collision/select operation (ISO/IEC14443-3 Type A and
// ISO/IEC18092 106 kbps passive mode), an erroneous Bit Count has been
// detected
#define PN532_ERROR_COLLISION_BITCOUNT                                  (0x04)
// Framing error during MIFARE operation
#define PN532_ERROR_MIFARE_FRAMING                                      (0x05)
// An abnormal bit-collision has been detected during bit wise
// anti-collision at 106 kbps
#define PN532_ERROR_COLLISION_BITCOLLISION                              (0x06)
// Communication buffer size insufficien
#define PN532_ERROR_NOBUFS                                              (0x07)
// RF Buffer overflow has been detected by the CI
#define PN532_ERROR_RFNOBUFS                                            (0x09)
// In active communication mode, the RF field has not been switched on
// in time by the counterpart (as defined in NFCIP-1 standard
#define PN532_ERROR_ACTIVE_TOOSLOW                                      (0x0a)
// RF Protocol error
#define PN532_ERROR_RFPROTO                                             (0x0b)
// Temperature error: the internal temperature sensor has detected
// overheating, and therefore has automatically switched off the
// antenna drivers
#define PN532_ERROR_TOOHOT                                              (0x0d)
// Internal buffer overflow
#define PN532_ERROR_INTERNAL_NOBUFS                                     (0x0e)
// Invalid parameter (range, format...)
#define PN532_ERROR_INVAL                                               (0x10)
// DEP Protocol: The PN533 configured in target mode does not support
// the command received from the initiator (the command received is not
// one of the following: ATR_REQ, WUP_REQ, PSL_REQ, DEP_REQ, DSL_REQ,
// RLS_REQ)
#define PN532_ERROR_DEP_INVALID_COMMAND                                 (0x12)
// DEP Protocol, MIFARE or ISO/IEC14443-4: The data format does not
// match to the specification.  Depending on the RF protocol used, it
// can be: Bad length of RF received frame, Incorrect value of PCB or
// PFB, Invalid or unexpected RF received frame, NAD or DID incoherence.
#define PN532_ERROR_DEP_BADDATA                                         (0x13)
// MIFARE: Authentication error
#define PN532_ERROR_MIFARE_AUTH                                         (0x14)
// Target or Initiator does not support NFC Secur
#define PN532_ERROR_NOSECURE                                            (0x18)
// I2C bus line is Busy. A TDA transaction is on going
#define PN532_ERROR_I2CBUSY                                             (0x19)
// ISO/IEC14443-3: UID Check byte is wrong
#define PN532_ERROR_UIDCHECKSUM                                         (0x23)
// DEP Protocol: Invalid device state, the system is in a state which
// does not allow the operation
#define PN532_ERROR_DEPSTATE                                            (0x25)
// Operation not allowed in this configuration (host controller
// interface)
#define PN532_ERROR_HCIINVAL                                            (0x26)
// This command is not acceptable due to the current context of the
// PN533 (Initiator vs. Target, unknown target number, Target not in the
// good state, ...)
#define PN532_ERROR_CONTEXT                                             (0x27)
// The PN533 configured as target has been released by its initiator
#define PN532_ERROR_RELEASED                                            (0x29)
// PN533 and ISO/IEC14443-3B only: the ID of the card does not match,
// meaning that the expected card has been exchanged with another one.
#define PN532_ERROR_CARDSWAPPED                                         (0x2a)
// PN533 and ISO/IEC14443-3B only: the card previously activated has
// disappeared.
#define PN532_ERROR_NOCARD                                              (0x2b)
// Mismatch between the NFCID3 initiator and the NFCID3 target in DEP
// 212/424 kbps passive.
#define PN532_ERROR_MISMATCH                                            (0x2c)
// An over-current event has been detected
#define PN532_ERROR_OVERCURRENT                                         (0x2d)
// NAD missing in DEP frame
#define PN532_ERROR_NONAD                                               (0x2e)
/* End: Official PN532 Errors Definitions */

// Other Error Definitions
#define PN532_STATUS_ERROR                                              0
#define PN532_STATUS_OK													1


typedef struct
{
	SPI_t*			spi;
	GPIO_PinDef_t	reset;
	GPIO_PinDef_t	cs;
}PN532_t;


uint8_t	pn532_init(PN532_t* init);
uint8_t PN532_ReadPassiveTarget(PN532_t* pn532, uint8_t* response, uint8_t card_baud, uint16_t* resp_len, uint32_t timeout);
uint8_t PN532_MifareClassicAuthenticateBlock(PN532_t* pn532, uint8_t* uid, uint8_t uid_length, uint16_t block_number, uint16_t key_number, uint8_t* key, uint16_t* resp_len);
uint8_t PN532_MifareClassicReadBlock(PN532_t* pn532, uint8_t* response, uint16_t block_number);

#endif /* NFC_PN532_PN532_H_ */
