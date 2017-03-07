#include "stdint.h" 
#include "cust_func.h"
#include "hal_uart.h"
#include "OSAL_Memory.h"
#include "OSAL.h"
#include "sensor.h"
#include "DebugTrace.h"

uint8 read_temp_cmd[CMDLEN] = {SLAVE_ADDR, TEMP, TEMPRATURE_ADDR >> 8,
                                          TEMPRATURE_ADDR & 0xFF, 0x00, 0x01};

uint8 read_humi_cmd[CMDLEN] = {SLAVE_ADDR, HUMI, VWC_ADDR >> 8,
                                          VWC_ADDR & 0xFF, 0x00, 0x01};

uint8 check_cmd[CMDLEN] = {SLAVE_ADDR, SLAVE, SLAVE_REG >> 8, 
                                          SLAVE_REG & 0xFF, 0x00, 0x01};
double Read_Soil_Temp()
{ 
  uint16 crc = Cal_Crc16(read_temp_cmd, CMDLEN - 2);
  read_temp_cmd[CMDLEN - 2] = crc &0xFF;
  read_temp_cmd[CMDLEN - 1] = (crc >> 8) & 0xFF;

   // debug_str(pbuf);

  cust_uart_write(read_temp_cmd, CMDLEN);
  uint8 recvlen = 0;
  while(!(recvlen = cust_uart_rxlen())){
  }
  uint8 *recvdata = NULL;
  recvdata = osal_mem_alloc(recvlen);
  double realtemp = 0;
  if(recvdata != NULL){
    cust_uart_read(recvdata, recvlen);
    if(!Cal_Crc16(recvdata, recvlen)){
      uint8 len = 0;
      len = recvdata[2];
      if(len > 0){
          if(!(recvdata[3] & (1 << 7))){
            realtemp = (double)(recvdata[3] * 256 + recvdata[4]) / 100;
          }
          else{
            realtemp = ((double)(recvdata[3] * 256 + recvdata[4]) - 0xFFFF -1) / 100;
          }
      }
    osal_mem_free(recvdata);
    return realtemp;
    }else{
      osal_mem_free(recvdata);
      return ERR_STAT;
    }
  }else{
    return ERR_STAT;
  }
}


double Read_Soil_Humi()
{

  uint16 crc = Cal_Crc16(read_humi_cmd, CMDLEN - 2);
  read_humi_cmd[CMDLEN - 2] = crc &0xFF;
  read_humi_cmd[CMDLEN - 1] = (crc >> 8) & 0xFF;
  
  cust_uart_write(read_humi_cmd, CMDLEN);
  uint8 recvlen = 0;
  while(!(recvlen = cust_uart_rxlen())){
  }
  uint8 *recvdata = NULL;
  recvdata = osal_mem_alloc(recvlen);
  cust_uart_read(recvdata, recvlen);
  double realhumi;
  if(recvdata != NULL){
    if(!Cal_Crc16(recvdata, recvlen)){
      uint8 len = 0;
      len = recvdata[2];
      if(len > 0){
            realhumi = (double)(recvdata[3] * 256 + recvdata[4]) / 10000;
      }
    osal_mem_free(recvdata);
    return realhumi;
    }else{
      osal_mem_free(recvdata);
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
