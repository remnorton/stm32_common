#include "workflow.h"
#include "string.h"
#include "stdio.h"

//#include "udebug.h"



#define WaitTimeUs 300000

extern xUID *_dev_uid;

volatile xRxTxStatus xStatus;
volatile uint8_t xStateSeatPin=0;
//volatile uint32_t int_irq_status=0;
static S2LPManageIRQPin *_irq_pin;

uint8_t message_buff[50] = { 0, };

uint8_t ConfigIRQSpirit(S2LPManageIRQPin *_set_irq_pin)
{
	_irq_pin = _set_irq_pin;
	return 1;
}


uint32_t CalculateUIDDevice(CRC_HandleTypeDef _hcrc, xUID *_sUID)
{
	if (!_sUID->HARD_UID)
	{
	uint32_t GUID[3] = { 0, };
	GUID[0] = HAL_GetUIDw0();
	GUID[1] = HAL_GetUIDw1();
	GUID[2] = HAL_GetUIDw2();
	_sUID->UID=HAL_CRC_Calculate(&_hcrc, GUID, 3);
	}
	else
	{
		_sUID->UID = _sUID->HARD_UID;
	}
	_dev_uid=_sUID;
	return _sUID->UID;

}

uint8_t SetDevType(xDevice work_dev_type)
{
	ThisDev = work_dev_type;
	return SET;
}

uint8_t ConfigSpirit(void)
{
	xStatus.IRQFlag = 0;
	xPresenceStatus dev_status;
	S2LP_Reset();
	if (PresenceCheckRadio(&dev_status))
	{
		if (S2LPManagementRcoCalibration())
		{
			dev_status.RCO_OK = SET;
			//SendDebugF((uint8_t*)"RCO CAL OK\r\n",1);
		}
		else
		{
			dev_status.RCO_OK = RESET;
			//SendDebugF((uint8_t*)"RCO CAL FAIL\r\n",1);
		}
		S2LPSetModeLPM(); //Enable Radio LOW POWER MODE. For HPM comment this string
		Init_S2LP_Radio(); //First init Radio
	}
	else
	{
		return RESET;
	}

	//UID_CRC = CalculateUIDDevice(hcrc);

	//Set Base Device Function in DEFINE
	//ThisDev = BaseDevType;

	if (ThisDev == HeadDevice) {
		//ReceiveData(0);
		StateRadio.CurrentState = RxWait;
		StateRadio.NextState = RX;
	}
	else
	{
		StateRadio.CurrentState = Sleep;
		StateRadio.NextState = None;
	}
	return SET;
}

void MainRadioProc(void) {
		ParsIRQ();
		CheckStateRadio();
}

// ----------------------------------------------------------------------
// PARS IRQ
// ----------------------------------------------------------------------
void ParsIRQ(void) {
	if (xStatus.IRQFlag)
	{
	S2LPIrqs xIrqStatus;

	//Get IRQ status (SPIRIT)
	S2LPGpioIrqGetStatus(&xIrqStatus);
	//

	if (xIrqStatus.IRQ_RX_TIMEOUT) {
		//SendDebugF((uint8_t*)"I_RX_TIMEOUT\r\n",1);
		//S2LPCmdStrobeRx();
		StateRadio.CurrentState = Sleep;
	}
	//----------------------------------------------------------------

	if (xIrqStatus.IRQ_TX_DATA_SENT) {
		/* set the tx_done_flag to manage the event in the main() */
		xStatus.TxComplete = SET;
		//SendDebugF((uint8_t*)"I_TX\r\n",1);
	}

	if (xIrqStatus.IRQ_MAX_BO_CCA_REACH) {
			xStatus.TxError = SET;
			//SendDebugF((uint8_t*)"CCA\r\n",1);
			//xStatus.TxComplete = SET;
	}

	if (xIrqStatus.IRQ_RX_DATA_DISC) {
			xStatus.RxError = SET;
			S2LPCmdStrobeFlushRxFifo();
			S2LPCmdStrobeRx();
			//SendDebugF((uint8_t*)"Bad Data\r\n",1);
	}

	if (xIrqStatus.IRQ_RX_DATA_READY) {
		xStatus.RxComplete = SET;
		//SendDebugF((uint8_t*)"I_RX\r\n",1);
	}

	//int_irq_status=*(uint32_t*)&xIrqStatus;
	S2LPGpioIrqClearStatus();
	xStatus.IRQFlag = RESET;
}

}
//----------------------------------------------------------------------------
void CheckStateRadio(void)
{
	if (xStatus.SendPair)
		{
			xStatus.SendPair = RESET;
			StateRadio.CurrentState = TxEndWait;
			StateRadio.NextState = RxWait;
			S2LPCmdStrobeReady();
			PairDevice();
		}

	if (StateRadio.CurrentState == Sleep)
		{
			S2LPCmdStrobeSleep();
			StateRadio.CurrentState = StateRadio.NextState;
			//SendDebugF((uint8_t*)"Sleep\r\n",1);
		}

	if (StateRadio.CurrentState == RxWait)
		{
			if (xStatus.RxComplete)
				{
					//SendDebugF((uint8_t*)"RxC\r\n",1);
					StateRadio.CurrentState = StateRadio.NextState;
					GetReceiveData();
					xStatus.RxComplete = RESET;

				}
			if (StateRadio.NextState == RX)
				{
					if (ThisDev == HeadDevice){
						ReceiveData(0);
					}
					else
					{
						ReceiveData(WaitTimeUs);
					}
					//SendDebugF((uint8_t*)"Rec\r\n",1);
					StateRadio.NextState = None;
				}
			}

	if (StateRadio.CurrentState == TxEndWait)
		{
			if (xStatus.TxComplete)
				{
					xStatus.TxComplete = RESET;
					//SendDebugF((uint8_t*)"TxC\r\n",1);

					if (StateRadio.NextState == RxWait ){
					  		StateRadio.CurrentState = StateRadio.NextState;
							StateRadio.NextState = RX;
					  		//SendDebugF((uint8_t*)"T_RXW\r\n",1);
						}
					if ( StateRadio.NextState == Ready)
							{
								S2LPCmdStrobeReady();
								StateRadio.CurrentState = None;
							}
					if ( StateRadio.NextState == Sleep)
							{
								//S2LPCmdStrobeSleep();
								StateRadio.CurrentState = Sleep;
								StateRadio.NextState = None;
							}

				}
			if (xStatus.TxError){
				xStatus.TxError = RESET;

				//SendDebugF((uint8_t*)"TxE\r\n",1);
			  }
		}
}

//------------------------------------------------------------------------------
//					IRQ GPIO CALLBACK
//------------------------------------------------------------------------------
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

	if (GPIO_Pin == _irq_pin->IRQ_Pin)
	{
		xStatus.IRQFlag = SET;
	}
//	else
//	if (GPIO_Pin == GPIO_PIN_3)
//	{
//		xStatus.SendPair = SET;
//	}
//	else
//	{
//		xStateSeatPin = SET;
//	}
}





