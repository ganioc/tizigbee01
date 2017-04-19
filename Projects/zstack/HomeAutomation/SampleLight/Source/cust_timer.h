#pragma once
#include "hal_types.h"

void cust_timer_init(uint8 period);

void Timer1_Handler(void);

// For heart beat timer init
void  init_1shot_cust_timer();

void start_1shot_cust_timer();


void trigger_sensor_timer();

// sensor update period , in seconds
#define PERIOD_SENSOR_UPDATE           4