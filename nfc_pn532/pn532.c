/*
 * pn532.c
 *
 *  Created on: 15 θών. 2020 γ.
 *      Author: Rem Norton
 */
#include "pn532.h"
#include <string.h>

#define PN532_PREAMBLE                      (0x00)
#define PN532_STARTCODE1                    (0x00)
#define PN532_STARTCODE2                    (0xFF)
#define PN532_POSTAMBLE                     (0x00)

#define PN532_HOSTTOPN532                   (0xD4)
#define PN532_PN532TOHOST                   (0xD5)

// PN532 Commands
#define PN532_COMMAND_DIAGNOSE              (0x00)
#define PN532_COMMAND_GETFIRMWAREVERSION    (0x02)
#define PN532_COMMAND_GETGENERALSTATUS      (0x04)
#define PN532_COMMAND_READREGISTER          (0x06)
#define PN532_COMMAND_WRITEREGISTER         (0x08)
#define PN532_COMMAND_READGPIO              (0x0C)
#define PN532_COMMAND_WRITEGPIO             (0x0E)
#define PN532_COMMAND_SETSERIALBAUDRATE     (0x10)
#define PN532_COMMAND_SETPARAMETERS         (0x12)
#define PN532_COMMAND_SAMCONFIGURATION      (0x14)
#define PN532_COMMAND_POWERDOWN             (0x16)
#define PN532_COMMAND_RFCONFIGURATION       (0x32)
#define PN532_COMMAND_RFREGULATIONTEST      (0x58)
#define PN532_COMMAND_INJUMPFORDEP          (0x56)
#define PN532_COMMAND_INJUMPFORPSL          (0x46)
#define PN532_COMMAND_INLISTPASSIVETARGET   (0x4A)
#define PN532_COMMAND_INATR                 (0x50)
#define PN532_COMMAND_INPSL                 (0x4E)
#define PN532_COMMAND_INDATAEXCHANGE        (0x40)
#define PN532_COMMAND_INCOMMUNICATETHRU     (0x42)
#define PN532_COMMAND_INDESELECT            (0x44)
#define PN532_COMMAND_INRELEASE             (0x52)
#define PN532_COMMAND_INSELECT              (0x54)
#define PN532_COMMAND_INAUTOPOLL            (0x60)
#define PN532_COMMAND_TGINITASTARGET        (0x8C)
#define PN532_COMMAND_TGSETGENERALBYTES     (0x92)
#define PN532_COMMAND_TGGETDATA             (0x86)
#define PN532_COMMAND_TGSETDATA             (0x8E)
#define PN532_COMMAND_TGSETMETADATA         (0x94)
#define PN532_COMMAND_TGGETINITIATORCOMMAND (0x88)
#define PN532_COMMAND_TGRESPONSETOINITIATOR (0x90)
#define PN532_COMMAND_TGGETTARGETSTATUS     (0x8A)

#define PN532_RESPONSE_INDATAEXCHANGE       (0x41)
#define PN532_RESPONSE_INLISTPASSIVETARGET  (0x4B)

#define PN532_WAKEUP                        (0x55)

#define PN532_SPI_STATREAD                  (0x02)
#define PN532_SPI_DATAWRITE                 (0x01)
#define PN532_SPI_DATAREAD                  (0x03)
#define PN532_SPI_READY                     (0x01)

#define PN532_I2C_ADDRESS                   (0x48 >> 1)
#define PN532_I2C_READBIT                   (0x01)
#define PN532_I2C_BUSY                      (0x00)
#define PN532_I2C_READY                     (0x01)
#define PN532_I2C_READYTIMEOUT              (20)

// NTAG2xx Commands
#define NTAG2XX_BLOCK_LENGTH                (4)

// Prefixes for NDEF Records (to identify record type)
#define NDEF_URIPREFIX_NONE                 (0x00)
#define NDEF_URIPREFIX_HTTP_WWWDOT          (0x01)
#define NDEF_URIPREFIX_HTTPS_WWWDOT         (0x02)
#define NDEF_URIPREFIX_HTTP                 (0x03)
#define NDEF_URIPREFIX_HTTPS                (0x04)
#define NDEF_URIPREFIX_TEL                  (0x05)
#define NDEF_URIPREFIX_MAILTO               (0x06)
#define NDEF_URIPREFIX_FTP_ANONAT           (0x07)
#define NDEF_URIPREFIX_FTP_FTPDOT           (0x08)
#define NDEF_URIPREFIX_FTPS                 (0x09)
#define NDEF_URIPREFIX_SFTP                 (0x0A)
#define NDEF_URIPREFIX_SMB                  (0x0B)
#define NDEF_URIPREFIX_NFS                  (0x0C)
#define NDEF_URIPREFIX_FTP                  (0x0D)
#define NDEF_URIPREFIX_DAV                  (0x0E)
#define NDEF_URIPREFIX_NEWS                 (0x0F)
#define NDEF_URIPREFIX_TELNET               (0x10)
#define NDEF_URIPREFIX_IMAP                 (0x11)
#define NDEF_URIPREFIX_RTSP                 (0x12)
#define NDEF_URIPREFIX_URN                  (0x13)
#define NDEF_URIPREFIX_POP                  (0x14)
#define NDEF_URIPREFIX_SIP                  (0x15)
#define NDEF_URIPREFIX_SIPS                 (0x16)
#define NDEF_URIPREFIX_TFTP                 (0x17)
#define NDEF_URIPREFIX_BTSPP                (0x18)
#define NDEF_URIPREFIX_BTL2CAP              (0x19)
#define NDEF_URIPREFIX_BTGOEP               (0x1A)
#define NDEF_URIPREFIX_TCPOBEX              (0x1B)
#define NDEF_URIPREFIX_IRDAOBEX             (0x1C)
#define NDEF_URIPREFIX_FILE                 (0x1D)
#define NDEF_URIPREFIX_URN_EPC_ID           (0x1E)
#define NDEF_URIPREFIX_URN_EPC_TAG          (0x1F)
#define NDEF_URIPREFIX_URN_EPC_PAT          (0x20)
#define NDEF_URIPREFIX_URN_EPC_RAW          (0x21)
#define NDEF_URIPREFIX_URN_EPC              (0x22)
#define NDEF_URIPREFIX_URN_NFC              (0x23)

#define PN532_GPIO_VALIDATIONBIT            (0x80)

#define PN532_FRAME_MAX_LENGTH              255
#define PN532_DEFAULT_TIMEOUT               1000


const uint8_t PN532_ACK[] = {0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00};
const uint8_t PN532_FRAME_START[] = {0x00, 0x00, 0xFF};


/*Private forwards*/
void 	PN532_HW_reset(PN532_t* pn532);
uint8_t PN532_SPI_rw(PN532_t* pn532, uint8_t* data, uint16_t count);
uint8_t PN532_ReadData(PN532_t* pn532, uint8_t* data, uint16_t count);
uint8_t PN532_WriteData(PN532_t* pn532, uint8_t *data, uint16_t count);
uint8_t PN532_Wakeup(PN532_t* pn532);
uint8_t PN532_WaitReady(PN532_t* pn532, uint32_t timeout);

uint8_t PN532_WriteFrame(PN532_t* pn532, uint8_t* data, uint16_t length);
uint8_t PN532_ReadFrame(PN532_t* pn532, uint8_t* response, uint16_t length, uint16_t* frame_len);
uint8_t PN532_CallFunction(PN532_t* pn532, uint8_t command, uint8_t* response, uint16_t response_length, uint8_t* params, uint16_t params_length, uint16_t* frame_len, uint32_t timeout);
uint8_t PN532_GetFirmwareVersion(PN532_t* pn532, uint8_t* version);
uint8_t PN532_SamConfiguration(PN532_t* pn532);


/*Public members*/

uint8_t	pn532_init(PN532_t* init)
{
	if (!init) return PN532_STATUS_ERROR;
	if (!init->spi) return PN532_STATUS_ERROR;
	uint8_t res = PN532_Wakeup(init);
	uint8_t fw[4] = {0};
	res = PN532_GetFirmwareVersion(init, fw);
	if (res) res = PN532_SamConfiguration(init);
	return res;
}

uint8_t PN532_ReadPassiveTarget(PN532_t* pn532, uint8_t* response, uint8_t card_baud, uint16_t* resp_len, uint32_t timeout)
{
	 // Send passive read command for 1 card.  Expect at most a 7 byte UUID.
	uint8_t params[] = {0x01, card_baud};
	uint8_t buff[19];
	uint16_t length = 0;
	uint8_t res = PN532_CallFunction(pn532, PN532_COMMAND_INLISTPASSIVETARGET, buff, sizeof(buff), params, sizeof(params), &length, timeout);

	if (res)
	{
		if (length < 1) return PN532_STATUS_ERROR; // No card found

		// Check only 1 card with up to a 7 byte UID is present.
		if (buff[0] != 0x01) return PN532_STATUS_ERROR;
		if (buff[5] > 7) return PN532_STATUS_ERROR;

		*resp_len = buff[5];
		memcpy(response, &buff[6], *resp_len);
	}

	return res;
}

uint8_t PN532_MifareClassicAuthenticateBlock(PN532_t* pn532, uint8_t* uid, uint8_t uid_length, uint16_t block_number, uint16_t key_number, uint8_t* key, uint16_t* resp_len)
{
	// Build parameters for InDataExchange command to authenticate MiFare card.
	uint8_t response[1] = {0xFF};
	uint8_t params[3 + MIFARE_UID_MAX_LENGTH + MIFARE_KEY_LENGTH] = {0};
	params[0] = 0x01;
	params[1] = key_number & 0xFF;
	params[2] = block_number & 0xFF;
	// params[3:3+keylen] = key
	memcpy(&params[3], key, MIFARE_KEY_LENGTH);
	// params[3+keylen:] = uid
	memcpy(&params[3+MIFARE_KEY_LENGTH], uid, uid_length);
	// Send InDataExchange request
	return PN532_CallFunction(pn532, PN532_COMMAND_INDATAEXCHANGE, response, sizeof(response), params, 3 + MIFARE_KEY_LENGTH + uid_length, resp_len, PN532_DEFAULT_TIMEOUT);
}

uint8_t PN532_MifareClassicReadBlock(PN532_t* pn532, uint8_t* response, uint16_t block_number)
{
	uint8_t params[] = {0x01, MIFARE_CMD_READ, block_number & 0xFF};
	uint8_t buff[MIFARE_BLOCK_LENGTH + 1];
	// Send InDataExchange request to read block of MiFare data.
	uint16_t len = 0;
	PN532_CallFunction(pn532, PN532_COMMAND_INDATAEXCHANGE, buff, sizeof(buff), params, sizeof(params), &len, PN532_DEFAULT_TIMEOUT);
	// Check first response is 0x00 to show success.
	if (buff[0] != PN532_ERROR_NONE) {
		return buff[0];
	}
	memcpy(response, &buff[1], MIFARE_BLOCK_LENGTH);
	return buff[0];
}

/*Private members*/

void PN532_HW_reset(PN532_t* pn532)
{
	if (pn532->reset.port) return;
	PN532_GPIO_WRITE(pn532->reset, pn532->reset.act_state);
	PN532_Delay(100);
	PN532_GPIO_WRITE(pn532->reset, !pn532->reset.act_state);
	PN532_Delay(500);
}

uint8_t PN532_SPI_rw(PN532_t* pn532, uint8_t* data, uint16_t count)
{
	PN532_GPIO_WRITE(pn532->cs, pn532->cs.act_state);
	PN532_Delay(1);
	uint8_t res = PN532_SPI_RW(pn532, data, count);
	PN532_Delay(1);
	PN532_GPIO_WRITE(pn532->cs, !pn532->cs.act_state);
	return res;
}

uint8_t PN532_ReadData(PN532_t* pn532, uint8_t* data, uint16_t count)
{
	uint8_t frame[count + 1];
	frame[0] = _SPI_DATAREAD;
	PN532_Delay(5);
	uint8_t res = PN532_SPI_rw(pn532, frame, count + 1);
	if (res) memcpy(data, &frame[1], count);
	return res;
}

uint8_t PN532_WriteData(PN532_t* pn532, uint8_t *data, uint16_t count)
{
	uint8_t frame[count + 1];
	frame[0] = _SPI_DATAWRITE;
	memcpy(&frame[1], data, count);
	return PN532_SPI_rw(pn532, frame, count+1);
}

uint8_t PN532_Wakeup(PN532_t* pn532)
{
	// Send any special commands/data to wake up PN532
	uint8_t data[] = {0x00};
	PN532_Delay(150);
	PN532_GPIO_WRITE(pn532->cs, pn532->cs.act_state);
	PN532_Delay(2); // T_osc_start
	uint8_t res = PN532_SPI_rw(pn532, data, 1);
	PN532_Delay(150);
	return res;
}

uint8_t PN532_WaitReady(PN532_t* pn532, uint32_t timeout)
{
	uint8_t status[] = {_SPI_STATREAD, 0x00};
	uint32_t dline = PN532_CalcDeadLine(timeout);
	uint8_t res = 0;
	while (!PN532_IsDeadLine(dline) && !res)
	{
		PN532_Delay(10);
		PN532_SPI_rw(pn532, status, sizeof(status));
		res = (status[1] == _SPI_READY)?1:0;
		if (!res)
		{
			//status[0] = _SPI_STATREAD;
			PN532_Delay(5);
		}
	}
	return res;
}

uint8_t PN532_WriteFrame(PN532_t* pn532, uint8_t* data, uint16_t length)
{
	 if (length > PN532_FRAME_MAX_LENGTH || length < 1)
		 return PN532_STATUS_ERROR; // Data must be array of 1 to 255 bytes.

	// Build frame to send as:
	// - Preamble (0x00)
	// - Start code  (0x00, 0xFF)
	// - Command length (1 byte)
	// - Command length checksum
	// - Command bytes
	// - Checksum
	// - Postamble (0x00)

	uint8_t frame[PN532_FRAME_MAX_LENGTH + 7];
	uint8_t checksum = 0;
	frame[0] = PN532_PREAMBLE;
	frame[1] = PN532_STARTCODE1;
	frame[2] = PN532_STARTCODE2;
	for (uint8_t i = 0; i < 3; i++) {
		checksum += frame[i];
	}
	frame[3] = length & 0xFF;
	frame[4] = (~length + 1) & 0xFF;
	for (uint8_t i = 0; i < length; i++) {
		frame[5 + i] = data[i];
		checksum += data[i];
	}
	frame[length + 5] = ~checksum & 0xFF;
	frame[length + 6] = PN532_POSTAMBLE;

	return PN532_WriteData(pn532, frame, length + 7);
}

uint8_t PN532_ReadFrame(PN532_t* pn532, uint8_t* response, uint16_t length, uint16_t* frame_len)
{
	uint8_t buff[PN532_FRAME_MAX_LENGTH + 7];
	uint8_t checksum = 0;
	// Read frame with expected length of data.
	uint8_t res = PN532_ReadData(pn532, buff, length + 7);
	if (res)
	{
		// Swallow all the 0x00 values that preceed 0xFF.
		volatile uint8_t offset = 0;
		while ((buff[offset] == 0x00) && (offset < (length + 8))) offset++;

		if ((offset >= (length + 7)) || (buff[offset] != 0xFF)) return PN532_STATUS_ERROR;
		offset++;

		// Check length & length checksum match.
		*frame_len = buff[offset];
		if ((( (*frame_len) + buff[offset+1]) & 0xFF) != 0) return PN532_STATUS_ERROR;

		// Check frame checksum value matches bytes.
		for (uint8_t i = 0; i < *frame_len + 1; i++) {
			checksum += buff[offset + 2 + i];
		}
		checksum &= 0xFF;
		if (checksum != 0) return PN532_STATUS_ERROR;

		// Return frame data.
		memcpy(response, &buff[offset+2], *frame_len);
	}

	return res;
}

uint8_t PN532_CallFunction(PN532_t* pn532, uint8_t command, uint8_t* response, uint16_t response_length, uint8_t* params, uint16_t params_length, uint16_t* frame_len, uint32_t timeout)
{
	// Build frame data with command and parameters.
	uint8_t buff[PN532_FRAME_MAX_LENGTH] = {PN532_HOSTTOPN532, command};
	memcpy(&buff[2], params, params_length);

	// Send frame and wait for response.
	if (PN532_WriteFrame(pn532, buff, params_length + 2) != PN532_STATUS_OK)
	{
		PN532_Wakeup(pn532);
		return PN532_STATUS_ERROR;
	}

	if (!PN532_WaitReady(pn532, timeout)) return PN532_STATUS_ERROR;

	// Verify ACK response and wait to be ready for function response.
	PN532_ReadData(pn532, buff, sizeof(PN532_ACK));
	if (memcmp(PN532_ACK, buff, sizeof(PN532_ACK)) != 0) return PN532_STATUS_ERROR;

	if (!PN532_WaitReady(pn532, timeout)) return PN532_STATUS_ERROR;

	// Read response bytes.
	*frame_len = 0;
	uint8_t res = PN532_ReadFrame(pn532, buff, response_length + 2, frame_len);

	if (!res) return PN532_STATUS_ERROR;

	// Check that response is for the called function.
	if (! ((buff[0] == PN532_PN532TOHOST) && (buff[1] == (command+1)))) return PN532_STATUS_ERROR;

	// Return response data.
	memcpy(response, &buff[2], response_length);
	(*frame_len) -= 2;

	return res;
}

uint8_t PN532_GetFirmwareVersion(PN532_t* pn532, uint8_t* version)
{
	uint16_t frame_len = 0;
	return PN532_CallFunction(pn532, PN532_COMMAND_GETFIRMWAREVERSION, version, 4, 0, 0, &frame_len, 500);
}

uint8_t PN532_SamConfiguration(PN532_t* pn532)
{
	uint8_t params[] = {0x01, 0x14, 0x01};
	uint16_t len = 0;
	return PN532_CallFunction(pn532, PN532_COMMAND_SAMCONFIGURATION, 0, 0, params, sizeof(params), &len, PN532_DEFAULT_TIMEOUT);
}

