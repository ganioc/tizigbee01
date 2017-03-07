#pragma once

#define SLAVE_ADDR              0x0001
#define TEMPRATURE_ADDR         0x0000
#define VWC_ADDR                0x0001
#define SLAVE_REG               0x0200
#define CMDLEN                  8
#define ERR_STAT                -1

typedef enum func_num{
  TEMP = 3,
  HUMI = 4,
  SLAVE = 6,
}FUNC_NUM;

double Read_Soil_Temp(void);

double Read_Soil_Humi(void);

uint8 Chk_Device(void);

uint16 Cal_Crc16( uint8 *arr_buff, uint8 len);
