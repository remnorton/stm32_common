#ifndef HARDWARE_CONGIH_H
#define HARDWARE_CONGIH_H



#if defined(STM32F103xB)
#include "stm32f1xx_hal.h"
#elif defined(STM32F407xx)
#include "stm32f4xx_hal.h"
#elif defined(STM32L431xx)
#include "stm32l4xx_hal.h"
#elif defined(STM32L0)
#include "stm32l0xx_hal.h"
#elif defined(STM32L4)
#include "stm32l4xx_hal.h"
#elif defined(STM32F3)
#include "stm32f3xx_hal.h"
#elif defined(STM32H7)
#include "stm32h7xx_hal.h"
#else
 #error "This library tested width STM32F103C8, STM32F407VG. Please define your device"
#endif

#endif /*HARDWARE_CONGIH_H*/
