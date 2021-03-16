#ifndef WORKFLOW_H
#define WORKFLOW_H

#include "hardware_config.h"
#include "S2LP_Config.h"
#include "radio.h"
#include "proto.h"


void ParsIRQ(void);
void MainRadioProc(void);
uint32_t CalculateUIDDevice(CRC_HandleTypeDef _hcrc, xUID* _sUID);
uint8_t SetDevType(uint8_t);
uint8_t ConfigSpirit(void);
uint8_t ConfigIRQSpirit(S2LPManageIRQPin*);

void CheckStateRadio(void);

extern volatile uint8_t xStateSeatPin;

#endif /* WORKFLOW_H */
