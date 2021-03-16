/*
 * proto.h
 *
 *  Created on: 21 янв. 2020 г.
 *      Author: igor
 */

#ifndef INC_PROTO_H_
#define INC_PROTO_H_

#include "radio.h"

#ifdef EN_DEBUG
//#include "udebug.h"
#endif

#pragma pack(push, 1)

typedef struct _xUID{
		uint32_t UID;
		uint32_t HARD_UID;
}xUID;

typedef struct xStateRadio {
	uint8_t CurrentState;
	uint8_t NextState;
	uint8_t PrevState;
}xStateRadio;

typedef struct RxTxStatus {
	uint8_t IRQFlag;
	uint8_t TxComplete;  //Complete tx operation
	uint8_t RxComplete; 	//Complete rx operation
	uint8_t TxWait;
	uint8_t RxWait;

	uint8_t RxEn;
	uint8_t TxEn;

	uint8_t RxError;
	uint8_t TxError;

	uint8_t SendPair;
}xRxTxStatus;

typedef struct xPairInfo{
	unsigned char PairFlag:1;
	unsigned char CountSeat:2;
	unsigned char DevType:3;
	unsigned char AnyData:2;
}xPairInfo;

typedef struct xSaveData{
	uint32_t PairAddr;
	xPairInfo _xPairInfo;
}xSaveData;

typedef struct CmdType{
	unsigned char command:4;
	unsigned char dev_type:4;
}xCmdType;

typedef struct Packet{
	uint32_t U_CRC; // 4 byte
	unsigned char NumberPacket; // 1 byte
	xCmdType CmdTypeData; // 1 byte
	unsigned char LenData; // 1 byte
	unsigned char* EndData; // 4 byte
}xPacket;
#pragma pack(pop)


union xPacketData{
	unsigned char buff[128];
	xPacket symbol;
};

typedef enum xCurrentState{
	RxWait,
	TxEndWait,
	RX,
	Ready,
	Sleep,
	None
}xCurrentState;

typedef enum {
	UnsetCommand, //0
	Pair,		 //1
	UnPair,
	Alert,
	Message,
	Sleeps,
	BigData,
	DataGood,
	PairGood,
	HealthGood,
	INC_DBM,
	DEC_DBM,
	GET_RSSI,
	LowBattery,
	EndCommand
}xPCommand;

typedef enum {
	UnsetDevice,
	HeadDevice,
	SBC,  //Seat Bealt Controller (Seat BOX)
	EBB, //End Button Box
	SBB, //SOS Button Box
	NFC, //NFC device
}xDevice;

typedef enum {
	IncorrectUID,
	CorrectPacket,
	IncorrectPacket
}StatusParsePacket;




extern xStateRadio StateRadio;
extern uint8_t ThisDev;

xPacket BuildPacketX(xPCommand d_command, uint32_t _intCRC, uint8_t SizeMsg, uint8_t *DMsg);
uint8_t ParsePacket(uint8_t *buff, uint8_t _size);
uint8_t ParsePacketCommand(xPacket* _in_data, uint8_t _size_data);
void PairDevice(void);

void SendMessageState(uint8_t* msg, uint8_t _size_msg);
void MessageReceive(uint8_t* buff, uint8_t _size_buff, uint32_t UID);


void ReceiveParsedDataCallback(xPacket* p_data, uint8_t* _size_buff);
void ReceiveProtoPacket(void);
void ReceivePacketRAW(void);

void TransmitProtoPacket(void);
void TransmitPacketRAW(void);

uint8_t CheckAirUID(uint32_t _uid);
uint32_t GetAirUID(void);
void SaveAirUID(uint32_t air_uid);


#endif /* INC_PROTO_H_ */
