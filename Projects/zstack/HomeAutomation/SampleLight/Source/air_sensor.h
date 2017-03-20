#pragma once

#include "hal_types.h"

#define AIR_CMDLEN      8

#define AIR_SUCCESS         0x0
#define TEMP_HUMI_ERR   0xFF
#define LIGHT_ERR       0xFE

uint16 Read_Air_Temp_Humi(void);

uint16 Read_Air_Light(void);

uint16 CAL_CRC16_TAB(uint8* pchMsg, uint16 wDataLen);