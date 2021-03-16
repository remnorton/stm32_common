/*
 * proto.c
 *
 *  Created on: 21 янв. 2020 г.
 *      Author: igor
 */

#include "proto.h"
#include "string.h"
#include "stdlib.h"
//#include "udebug.h"


uint32_t HARD_HEAD_DEVICE_UID = 0xFFFFFFFF;

xUID *_dev_uid;

xSaveData PairData = {
		.PairAddr = 0,
	};

xStateRadio StateRadio;

volatile uint32_t packet_counter=0;

uint8_t ThisDev;

xPacket BuildPacketX(xPCommand d_command, uint32_t _intCRC, uint8_t SizeMsg, uint8_t *DMsg)
{
	xPacket mPacket;

	xCmdType mCmdType;

	mCmdType.command = d_command; //Pair function
	mCmdType.dev_type = ThisDev; //End Button box

	mPacket.CmdTypeData = mCmdType;

	mPacket.NumberPacket = packet_counter;

	packet_counter++;

	mPacket.U_CRC = _intCRC;

	mPacket.LenData = SizeMsg;

	uint8_t* s_buff;

	uint8_t size_with_len = sizeof(xPacket)-sizeof(unsigned char*);

	uint8_t size_for_buff = size_with_len+SizeMsg;

	s_buff = (uint8_t*)malloc(sizeof(uint8_t)*size_for_buff);

	memset(s_buff,0,size_for_buff);

	memcpy(s_buff, (uint8_t*)&mPacket, size_with_len);

	if (SizeMsg)
	{
		memcpy(&s_buff[size_with_len], DMsg, SizeMsg);
	}
	else
	{
		size_with_len-=1;
	}
	TransmitData(s_buff, size_with_len+SizeMsg);
	//SendDebugF((uint8_t*)"S->\r\n",2);
	free(s_buff);
	return mPacket;

}


uint8_t ParsePacket(uint8_t *buff, uint8_t _s_buff){

	xPacket *RPacket;

	uint8_t RetStatus=0xFF;

	uint8_t size_with_len = sizeof(xPacket)-sizeof(unsigned char*);
	uint8_t size_wo_len = sizeof(xPacket)-sizeof(unsigned char*)-sizeof(unsigned char);
	RPacket = (xPacket*)malloc(sizeof(xPacket));

	if (_s_buff > size_wo_len)
	{
		memset(RPacket,0,size_with_len);
		memcpy(RPacket,buff, size_with_len);

		RPacket->EndData = (uint8_t*)malloc(_s_buff-size_with_len+1);
		memset(RPacket->EndData,0,_s_buff-size_with_len+1);
		memcpy(RPacket->EndData,&buff[_s_buff-(_s_buff-size_with_len)], _s_buff-size_with_len);
	}
	else
	{
		memset(RPacket,0,size_wo_len);
		memcpy(RPacket,buff, size_wo_len);
		RPacket->LenData = 0;
		RPacket->EndData = NULL;
	}

	uint8_t size_data = _s_buff-size_with_len;

	if (CheckAirUID(RPacket->U_CRC))
	{
		ParsePacketCommand(RPacket, size_data);
	}
	else
	{
		if (RPacket->CmdTypeData.command == Pair)
		{
			SaveAirUID(RPacket->U_CRC);
			ParsePacketCommand(RPacket,size_data);
		}
		RetStatus = IncorrectUID;
	}

	if (RPacket->NumberPacket == 1)
	{

	}

	if(RPacket->CmdTypeData.dev_type == EBB){

		}

	if (RPacket->EndData != NULL )
	{
		free (RPacket->EndData);
	}
	free(RPacket);

return RetStatus;
}


uint8_t ParsePacketCommand(xPacket* _in_data, uint8_t _size_data)
{
	switch (_in_data->CmdTypeData.command){
// ---------------------------------------------------------------------------------------
// -------------------- Response from the head device ------------------------------------
// ---------------------------------------------------------------------------------------
	case PairGood:
	{
		//SendDebugF((uint8_t*)"PairGood\r\n",2);
		if (_in_data->CmdTypeData.dev_type == HeadDevice) //if dev_type is HEAD_DEVICE
		{
			BuildPacketX(EndCommand, _dev_uid->UID, 0, (uint8_t*)"");
			StateRadio.PrevState = StateRadio.CurrentState;
			StateRadio.CurrentState = TxEndWait;
			StateRadio.NextState = Sleep;
		}
		break;
	}
// ----------------------------------------------------------------------------------------
	case DataGood:
	{
		//SendDebugF((uint8_t*)"DataGood\r\n",2);
		if (_in_data->CmdTypeData.dev_type == HeadDevice) //if dev_type is HEAD_DEVICE
				{
					BuildPacketX(EndCommand, _dev_uid->UID, 0, (uint8_t*)"");
					StateRadio.PrevState = StateRadio.CurrentState;
					StateRadio.CurrentState = TxEndWait;
					StateRadio.NextState = Sleep;
				}
		break;
	}
// ----------------------------------------------------------------------------------------
// ---------------------- END Response from the head device -------------------------------
// ----------------------------------------------------------------------------------------

case Pair:
	{
		//SendDebugF((uint8_t*)"Pair\n\r",2);
		if (ThisDev == HeadDevice && _in_data->CmdTypeData.dev_type != HeadDevice)
		{
			StateRadio.PrevState = StateRadio.CurrentState;
			StateRadio.CurrentState = TxEndWait;
			StateRadio.NextState = RxWait;

			BuildPacketX(PairGood, _dev_uid->UID, 0, (uint8_t*)"");
		}
		break;
	}

case Alert:
	{
		//SendDebugF((uint8_t*)"Alert\r\n",2);
		if (ThisDev == HeadDevice && _in_data->CmdTypeData.dev_type != HeadDevice)
		{
			StateRadio.PrevState = StateRadio.CurrentState;
			StateRadio.CurrentState = TxEndWait;
			StateRadio.NextState = RxWait;

			BuildPacketX(DataGood, _dev_uid->UID, 0, (uint8_t*)"");
		}
		break;
	}

case Message:
	{
		//SendDebugF((uint8_t*)"Message\n\r",2);
		if (ThisDev == HeadDevice && _in_data->CmdTypeData.dev_type != HeadDevice )
		{
			StateRadio.PrevState = StateRadio.CurrentState;
			StateRadio.CurrentState = TxEndWait;
			StateRadio.NextState = RxWait;

//			MessageReceive(_in_data->EndData, _size_data, _in_data->U_CRC);

			BuildPacketX(DataGood,_dev_uid->UID,0,(uint8_t*)"");
		}
		break;
	}
case LowBattery:
	{
		//SendDebugF((uint8_t*)"LowBattery",2);
		if (ThisDev == HeadDevice && _in_data->CmdTypeData.dev_type != HeadDevice)
		{
			StateRadio.PrevState = StateRadio.CurrentState;
			StateRadio.CurrentState = TxEndWait;
			StateRadio.NextState = RxWait;

			BuildPacketX(DataGood,_dev_uid->UID,0,(uint8_t*)"");
		}
		break;
	}

case EndCommand:
	{
		//SendDebugF((uint8_t*)"EndCommand\r\n",2);
		if ( ThisDev == HeadDevice )
		{
			StateRadio.PrevState = StateRadio.CurrentState;
			StateRadio.CurrentState = RxWait;
			StateRadio.NextState = RX;
		}
		break;
	}
	default:
		break;
	}
	ReceiveParsedDataCallback(_in_data, &_size_data);
	return 0;
}

void PairDevice(void){

     BuildPacketX(Pair, _dev_uid->UID, 0, (uint8_t*)"" );
}


void SendMessageState(uint8_t* msg, uint8_t _size_msg)
{
	StateRadio.PrevState = StateRadio.CurrentState;
	StateRadio.CurrentState = TxEndWait;
	StateRadio.NextState = RxWait;
	S2LPCmdStrobeReady();
	BuildPacketX(Message, _dev_uid->UID, _size_msg, msg);
}

__weak void MessageReceive(uint8_t* buff, uint8_t _size_buff, uint32_t UID)
{
	UNUSED(buff);
	UNUSED(_size_buff);
	UNUSED(UID);
}

__weak void ReceiveParsedDataCallback(xPacket* p_data, uint8_t* _size_buff)
{
	UNUSED(p_data);
	UNUSED(_size_buff);
}

void ReceiveProtoPacket(void)
{
	//

}

void ReceivePacketRAW(void)
{
//
}

void TransmitProtoPacket(void)
{
//
}

void TransmitPacketRAW(void)
{
//
}

uint8_t CheckAirUID(uint32_t _uid)
{
	uint8_t _status=0;

	if (ThisDev == HeadDevice)
	{
		//Read from flash xSaveData struct and check device is paired
		_status++;
		return _status;
	}
	else
	{
		//Read from flash xSaveData struct and check
		_status++;
		return _status;
	}
	return _status;
}

uint32_t GetAirUID(void){
	if (PairData.PairAddr == 0)
	{
		//This function for get struct of PAIR INFO
	}
	return PairData.PairAddr = HARD_HEAD_DEVICE_UID;
}

void SaveAirUID(uint32_t air_uid){
	PairData.PairAddr = air_uid;
}

