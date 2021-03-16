/*
 * radio.c
 *
 *  Created on: 25 ���. 2019 �.
 *      Author: user
 */

#include "radio.h"
#include "workflow.h"
#include "string.h"
#include "proto.h"
//#include "udebug.h"

//---------------------------------------------------------
//Variable
//---------------------------------------------------------
xAirData xData; //Struct fo RX TX data from AIR
volatile FlagStatus xTxDoneFlag = RESET;
extern volatile xRxTxStatus xStatus;

SRadioInit xRadioInit = {
  BASE_FREQUENCY,
  MODULATION_SELECT,
  DATARATE,
  FREQ_DEVIATION,
  BANDWIDTH
};

PktBasicInit xBasicInit={
  PREAMBLE_LENGTH,
  SYNC_LENGTH,
  SYNC_WORD,
  VARIABLE_LENGTH,
  EXTENDED_LENGTH_FIELD,
  CRC_MODE,
  EN_ADDRESS,
  EN_FEC,
  EN_WHITENING
};

SCsmaInit xCsmaInit={
  PERSISTENT_MODE_EN,
  CS_PERIOD,
  CS_TIMEOUT,
  MAX_NB,
  BU_COUNTER_SEED,
  CU_PRESCALER
};

SGpioInit xGpioIRQ={
		S2LP_GPIO_3,
		S2LP_GPIO_MODE_DIGITAL_OUTPUT_LP,
		S2LP_GPIO_DIG_OUT_IRQ
};

SGpioInit sGpioIRQ={
		S2LP_GPIO_0,
		S2LP_GPIO_MODE_DIGITAL_OUTPUT_HP,
		S2LP_GPIO_DIG_OUT_LBD
};

void Init_S2LP_Hardware(SPI_HandleTypeDef *spi)
{

}


//-----------------------------------------------------------
// Init Radio & Init PACKET FORMAT
//-----------------------------------------------------------
void Init_S2LP_Radio(void) {

	S2LPGpioInit(&xGpioIRQ);

	//S2LPGpioInit(&sGpioIRQ);

	S2LPRadioInit(&xRadioInit);

	S2LPRadioSetMaxPALevel(S_DISABLE);
	S2LPRadioSetPALeveldBm(7, POWER_DBM);
	S2LPRadioSetPALevelMaxIndex(7);

	S2LPPktBasicInit(&xBasicInit);

	S2LPGpioIrqDeInit(NULL);
	S2LPGpioIrqConfig(TX_DATA_SENT, S_ENABLE);
	S2LPGpioIrqConfig(MAX_BO_CCA_REACH, S_ENABLE);
	S2LPGpioIrqConfig(RX_DATA_DISC, S_ENABLE);
	S2LPGpioIrqConfig(RX_DATA_READY, S_ENABLE);
	S2LPGpioIrqConfig(LOW_BATT_LVL, S_ENABLE);
	S2LPGpioIrqConfig(RX_TIMEOUT, S_ENABLE);
	S2LPGpioIrqClearStatus();

	S2LPTimerLdcrMode(S_DISABLE);

	S2LPCsmaInit(&xCsmaInit);

	SRssiInit xSRssiInit = {
			.cRssiFlt = 14,
			.xRssiMode = RSSI_STATIC_MODE,
			.cRssiThreshdBm = RSSI_THR,
	};

	S2LPRadioRssiInit(&xSRssiInit);
}

//---------------------------------------------------------------------
uint8_t PresenceCheckRadio(xPresenceStatus* chip_info)
{
	xPresenceStatus old_p_status;
	old_p_status.PN=0;
	old_p_status.Version=0;

	chip_info->PN=S2LPGeneralGetDevicePN();
	chip_info->Version=S2LPGeneralGetVersion();


	old_p_status.PN = S2LPGeneralGetDevicePN();
	old_p_status.Available = S2LPGeneralGetVersion();

	if ((old_p_status.PN == chip_info->PN) || (old_p_status.Available == chip_info->Available))
	{
		return SET;
	}
	return RESET;
}
//---------------------------------------------------------------------
void S2LPSetModeLPM(void)
{
	  uint8_t tmp[2]={0};
	  tmp[0] = 0x35; /* reg. PM_CONF1 (0x78) */
	  tmp[1] = 0x12; /* reg. PM_CONF0 (0x79) */
	  S2LPSpiWriteRegisters(0x78, 2, tmp);
}
// -------------------------------------------------------------------
//                          RCO calibration function
// -------------------------------------------------------------------
uint8_t S2LPManagementRcoCalibration(void)
{
  uint8_t tmp[10]={0},tmp2;
  uint32_t max_recal = 0; //Remove this crutch after normal board wiring

  uint8_t _s_cal_rco=0;

  S2LPTimerCalibrationRco(S_ENABLE);
  S2LPCmdStrobeSleep();
  HAL_Delay(10);

while (1){
	uint16_t _x_status = S2LPSpiReadRegisters(MC_STATE1_ADDR, 2, tmp);
	if (_x_status&0x1000)
		{
			_s_cal_rco = 1;
			break;
		}
	else
		{
			SdkResetHigh();
			HAL_Delay(5);
			SdkResetLow();
			HAL_Delay(10);

			S2LPTimerCalibrationRco(S_ENABLE);
			S2LPCmdStrobeSleep();
		}
	if (max_recal > 50)
		{
			_s_cal_rco = 0;
			break;
		}
	max_recal++;
}

if (_s_cal_rco){
	S2LPSpiReadRegisters(0x94, 2, tmp);
	S2LPSpiReadRegisters(0x6F, 1, &tmp2);
	tmp[1]=(tmp[1]&0x80)|(tmp2&0x7F);
	S2LPSpiWriteRegisters(0x6E, 2, tmp);
}
else
{
	//try load RCO cal data from flash
}

  S2LPTimerCalibrationRco(S_DISABLE);
  S2LPCmdStrobeReady();
  return _s_cal_rco;
}

//-------------------------------------------------------------
void S2LPSleepWakeUP(void)
{
	  S2LPCmdStrobeReady();
	  S2LPTimerCalibrationRco(S_DISABLE);
	  //S2LPCmdStrobeSleep();
	  uint32_t t_start = HAL_GetTick();
	  do {
		  __NOP();
	  }while((t_start+10)<HAL_GetTick());
	  S2LPTimerCalibrationRco(S_ENABLE);
	  S2LPCmdStrobeSleep();
	  //S2LPCmdStrobeReady();
}
//-------------------------------------------------------------
// Transmit Data Without ASK
//------------------------------------------------------------
void TransmitData(uint8_t* data, uint8_t size)
{
	S2LPCmdStrobeSabort();

	if (S2LPCsmaGetCsma()==S_DISABLE)
		{
			S2LPCsma(S_ENABLE);
		}
	S2LPPktBasicSetPayloadLength(size);
    S2LPCmdStrobeFlushTxFifo();
    S2LPSpiWriteFifo(size, data);
    S2LPGpioIrqClearStatus();
    /* send the TX command */
    S2LPCmdStrobeTx();
}

//---------------------------------------------------------------
// Change state device to receive packet
//---------------------------------------------------------------
void ReceiveData(uint32_t sTimeOut){
	if (S2LPCsmaGetCsma()==S_ENABLE)
	{
		S2LPCsma(S_DISABLE);
	}
	  if (!sTimeOut){
		  SET_INFINITE_RX_TIMEOUT();
	  }
	  else {
	  	  S2LPTimerSetRxTimerUs(sTimeOut);
	  }
	  /* IRQ registers blanking */
	  S2LPGpioIrqClearStatus();
	  S2LPCmdStrobeRx();
}

void GetReceiveData(void){
	xData.cRxData = S2LPFifoReadNumberBytesRxFifo();
	S2LPSpiReadFifo(xData.cRxData, xData.RxData);
	ParsePacket(xData.RxData, xData.cRxData);
}

//--------------------------------------------------------------------------------
// Transmit Data width ASK
//--------------------------------------------------------------------------------
void TransmitDataASK(uint8_t* data, uint8_t size)
{

	S2LPCmdStrobeSabort();

	S2LPCsma(S_ENABLE);

	S2LPPktBasicSetPayloadLength(size);

    S2LPCmdStrobeFlushTxFifo();

    S2LPSpiWriteFifo(size, data);

    /* send the TX command */
    S2LPCmdStrobeTx();
}

void TogleStateRadio(void){
	if (xStatus.RxWait){
		ReceiveData(0);
	}
	if (xStatus.TxWait){
	//
	}
}

void TransmitBase(void)
{
	  S2LP_Reset();
	  //---------------------------------
	  S2LPGpioInit(&xGpioIRQ);

	  S2LPRadioInit(&xRadioInit);
	  //---------------------------------
	  S2LPRadioSetMaxPALevel(S_DISABLE);
	  //---------------------------------
	  S2LPRadioSetPALeveldBm(7,POWER_DBM);
	  //---------------------------------
	  S2LPRadioSetPALevelMaxIndex(7);
	  //---------------------------------
	  S2LPPktBasicInit(&xBasicInit);
	  //---------------------------------
	  S2LPGpioIrqDeInit(NULL);
	  S2LPGpioIrqConfig(TX_DATA_SENT , S_ENABLE);
	  //---------------------------------

	  S2LPGpioIrqClearStatus();
	  S2LPPktBasicSetPayloadLength(strlen((char*)xData.TxData));
	  S2LPCmdStrobeFlushTxFifo();
	  S2LPSpiWriteFifo(strlen((char*)xData.TxData), xData.TxData);
	  S2LPCmdStrobeTx();
	  while(!xTxDoneFlag);
	  xTxDoneFlag = RESET;
}
