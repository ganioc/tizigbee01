#pragma once

#include "hal_types.h"

#define SLAVE_ADDR              0x01
#define TEMP_HUMI_ADDR         0x0000
#define SLAVE_REG               0x0200
#define CMDLEN                  8
#define TEMP_HUMI_ERR           0xFF
#define SENSOR_SUCC             0x0          

typedef enum func_num{
  TEMP_HUMI = 3,
  SLAVE = 6,
}FUNC_NUM;

uint16 Read_Soil_Temp_Humi(void);

uint8 Chk_Device(void);

uint16 Cal_Crc16( uint8 *arr_buff, uint8 len);



void send_soil_temp_humi();

int read_soil_temp_humi_cb(uint8* buf, uint8 length);
  