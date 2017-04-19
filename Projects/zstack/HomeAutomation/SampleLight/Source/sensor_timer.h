#pragma once

#include "hal_types.h"

#define SENSOR_TIMER_UPDATE_PEROID  10
void sensor_timer2_init(void);

void ReadPH_Timer2_Handler(void);

void ReadAIR_Timer2_Handler(void);