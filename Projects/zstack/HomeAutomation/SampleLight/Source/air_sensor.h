#pragma once

#include "hal_types.h"

#define AIR_CMDLEN      8

#define AIR_SUCCESS         0x0
#define TEMP_HUMI_ERR   0xFF
#define LIGHT_ERR       0xFE

uint16 Read_Air_TempHumi(void);

uint16 Read_Air_Light(void);

void Read_Air_Data(void);

void Read_Air_Sensor(void);

uint16 CAL_CRC16_TAB(uint8* pchMsg, uint16 wDataLen);

void send_air_light();

void send_air_temphumi();

int read_air_light_cb(uint8* buf, uint8 len);

int read_air_temphumi_cb(uint8* buf, uint8 len);