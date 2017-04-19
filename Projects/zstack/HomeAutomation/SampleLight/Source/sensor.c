#include "stdint.h" 
#include "cust_func.h"
#include "hal_uart.h"
#include "hal_types.h"
#include "OSAL_Memory.h"
#include "OSAL.h"
#include "sensor.h"
#include "DebugTrace.h"
#include "hal_led.h"

extern uint16 zclSmartGarden_Temp;
extern uint16 zclSmartGarden_Humidity;
extern uint8 recvbuff[BUFFER_LENGTH];

uint16 MaxReadCount = 0;

uint8 read_soil_temp_humi_cmd[CMDLEN] = {SLAVE_ADDR, 0x03, 0x00,
                                          0x00, 0x00, 0x02,0xC4,0x0B};

uint8 check_cmd[CMDLEN] = {SLAVE_ADDR, SLAVE, (SLAVE_REG >> 8)&0xff, 
                                          SLAVE_REG & 0xFF, 0x00, 0x01,0x0,0x0};



void send_soil_temp_humi()
{
    sensor_switch(port0);
   // cust_uart_flush();
    cust_uart_write(read_soil_temp_humi_cmd, CMDLEN);
}

int read_soil_temp_humi_cb(uint8* buf, uint8 length)
{
    uint8 i = 0;
    
    if(length < 9){
    
      return -1;
      
    }
    
    if(length == 9 && !Cal_Crc16(buf, length))
    {
      
      
      zclSmartGarden_Temp = 0;
      zclSmartGarden_Temp += buf[3];
      zclSmartGarden_Temp <<= 8;
      zclSmartGarden_Temp += buf[4];
      
      zclSmartGarden_Humidity = 0;
      zclSmartGarden_Humidity += buf[5];
      zclSmartGarden_Humidity <<= 8;
      zclSmartGarden_Humidity += buf[6];
      
      cust_debug_str("temp:%d humi:%d", zclSmartGarden_Temp, zclSmartGarden_Humidity);
      return 0;

    }
    else
    {
      cust_debug_str("rx soil wrong crc, len:%d", length);
      for(; i < length; i++){
         cust_debug_str("recvbuf[%d] = %d", i, buf[i]);
      }
        return -2;
    }

}
uint16 Read_Soil_Temp_Humi()
{ 
   // debug_str(pbuf);
  MaxReadCount = 0;
  cust_uart_write(read_soil_temp_humi_cmd, CMDLEN);
  
  uint8 recvlen = 0;
  
 
  while(!(recvlen = cust_uart_rxlen())){
    MaxReadCount ++;
    if(MaxReadCount >= 0xFF){
      MaxReadCount = 0;
      return TEMP_HUMI_ERR;
    }
    cust_delay_2ms();
  }
 // UARTCharPut(CUST_UART0_PORT, 0xFF);
  //cust_uart0_write(recvbuff, recvlen);
  /*
  cust_debug_str("recvlen :%d", recvlen );
  uint8 i = 0;
  for(;i<recvlen; i++){
    cust_debug_str("recvdata[%d]:%d", i, recvbuff[i]);
  }
  */
  if(!Cal_Crc16(recvbuff, recvlen)){
    uint8 len = 0;
    len = recvbuff[2];
    if(len > 0){
      zclSmartGarden_Temp = 0;
      zclSmartGarden_Temp += recvbuff[3];
      zclSmartGarden_Temp <<= 8;
      zclSmartGarden_Temp += recvbuff[4];

      zclSmartGarden_Humidity = 0;
      zclSmartGarden_Humidity += recvbuff[5];
      zclSmartGarden_Humidity <<= 8;
      zclSmartGarden_Humidity += recvbuff[6];
      return SENSOR_SUCC;
    }else{
      return TEMP_HUMI_ERR;
    }
  }else{
    return TEMP_HUMI_ERR;
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
