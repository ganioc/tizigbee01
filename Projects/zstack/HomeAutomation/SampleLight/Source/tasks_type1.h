#pragma once

#include "hal_types.h"

typedef void (*sensorTaskHandler)( void );
typedef int (*sensorTaskCallbackHandler)( uint8* buf, uint8 len);

void check_uart_rx_in_timer();

void start_sensor_type_timer();


void runSensorTypeTask();



