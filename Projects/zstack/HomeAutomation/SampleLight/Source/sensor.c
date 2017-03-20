#include "stdint.h" 
#include "cust_func.h"
#include "hal_uart.h"
#include "hal_types.h"
#include "OSAL_Memory.h"
#include "OSAL.h"
#include "sensor.h"
#include "DebugTrace.h"

extern uint16 zclSmartGarden_Temp;
extern uint16 zclSmartGarden_Humidity;
extern uint8 recvbuff[BUFFER_LENGTH];

uint16 MaxReadCount = 0;

uint8 read_temp_cmd[CMDLEN] = {SLAVE_ADDR, TEMP, TEMPRATURE_ADDR >> 8,
                                          TEMPRATURE_ADDR & 0xFF, 0x00, 0x01,0,0};

uint8 read_humi_cmd[CMDLEN] = {SLAVE_ADDR, HUMI, VWC_ADDR >> 8,
                                          VWC_ADDR & 0xFF, 0x00, 0x01};

uint8 check_cmd[CMDLEN] = {SLAVE_ADDR, SLAVE, (SLAVE_REG >> 8)&0xff, 
                                          SLAVE_REG & 0xFF, 0x00, 0x01,0x0,0x0};
uint16 Read_Soil_Temp()
{ 
  uint16 crc = Cal_Crc16(read_temp_cmd, CMDLEN - 2);
  read_temp_cmd[CMDLEN - 2] = crc &0xFF;
  read_temp_cmd[CMDLEN - 1] = (crc >> 8) & 0xFF;

   // debug_str(pbuf);

  cust_uart_write(read_temp_cmd, CMDLEN);
  
  uint8 recvlen = 0;
  while(!(recvlen = cust_uart_rxlen())){
    MaxReadCount ++;
    if(MaxReadCount >= 0xFF){
      MaxReadCount = 0;
      return TEMP_ERR;
    }
    cust_delay_2ms();
  }

    if(!Cal_Crc16(recvbuff, recvlen)){
      uint8 len = 0;
      len = recvbuff[2];
      if(len > 0){
          zclSmartGarden_Temp ^= zclSmartGarden_Temp;
          zclSmartGarden_Temp |= recvbuff[3];
          zclSmartGarden_Temp <<= 8;
          zclSmartGarden_Temp |= recvbuff[4];
          return SENSOR_SUCC;
      }else{
        return TEMP_ERR;
      }
    }else{
      return TEMP_ERR;
    }
}


uint16 Read_Soil_Humi()
{

  uint16 crc = Cal_Crc16(read_humi_cmd, CMDLEN - 2);
  read_humi_cmd[CMDLEN - 2] = crc &0xFF;
  read_humi_cmd[CMDLEN - 1] = (crc >> 8) & 0xFF;
  
  cust_uart_write(read_humi_cmd, CMDLEN);
  uint8 recvlen = 0;
  while(!(recvlen = cust_uart_rxlen())){
     MaxReadCount ++;
    if(MaxReadCount >= 0xFF){
      MaxReadCount = 0;
      return ERR_STAT;
    }
  }
    if(!Cal_Crc16(recvbuff, recvlen)){
      uint8 len = 0;
      len = recvbuff[2];
      if(len > 0){
        zclSmartGarden_Humidity ^= zclSmartGarden_Humidity;
        zclSmartGarden_Humidity |= recvbuff[3];
        zclSmartGarden_Humidity <<= 8;
        zclSmartGarden_Humidity |= recvbuff[4];
        return SENSOR_SUCC;
      }else{
        return ERR_STAT;
      }
    }else{
      return ERR_STAT;
    }
}

uint8 Chk_Device()
{
  uint16 crc = Cal_Crc16(check_cmd, CMDLEN - 2);
  check_cmd[CMDLEN - 2] = crc & 0xFF;
  check_cmd[CMDLEN - 1] = (crc >> 8) & 0xFF;
  cust_uart_write(check_cmd, CMDLEN);
  if(!cust_uart_rxlen()){
    return FALSE;
  }else{
    return TRUE;
  }
}

uint16 Cal_Crc16( uint8 *arr_buff, uint8 len)
{
 uint32 crc=0xFFFF;
 uint8 i, j;
 for ( j=0; j<len; j++)
 {
   crc=crc ^*arr_buff++;
   for ( i=0; i<8; i++)
  {
       if( ( crc & 0x0001) >0)
       {
           crc=crc >> 1;
           crc=crc ^ 0xa001;
        }
      else
          crc=crc>>1;
   }
 }
return ( crc);
}
