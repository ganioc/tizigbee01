#include "hal_types.h"
#include "cust_func.h"
#include "air_sensor.h"
#include "hal_led.h"
#include "gptimer.h"

extern uint8 recvbuff[BUFFER_LENGTH];
extern uint32  zclSmartGarden_LightIntensity;
extern uint16  zclSmartGarden_TempIntensity;
extern uint16  zclSmartGarden_HumiIntensity;
extern uint8 air_counter;

extern uint8 AIRrecvlen;
uint8 AIRReadCount = 0;
uint8 read_air_temphumi_cmd[AIR_CMDLEN] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B};

uint8 read_air_data_cmd[AIR_CMDLEN] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x09, 0x85, 0xCC};

uint8 read_air_light_cmd[AIR_CMDLEN] = {0x01, 0x03, 0x00, 0x07, 0x00, 0x02, 0x75, 0xCA};

const uint8 chCRCHTalbe[] =                                 // CRC 高位字节值表
{
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40
};
const uint8 chCRCLTalbe[] =                                 // CRC 低位字节值表
{
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB,
0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
0x41, 0x81, 0x80, 0x40
};

void send_air_temphumi(){
    sensor_switch(port2);
    cust_uart_write(read_air_temphumi_cmd, AIR_CMDLEN);
}

int read_air_temphumi_cb(uint8* buf, uint8 len){
  if(len < 9)
    return -1;
  
    if(len == 9 && !CAL_CRC16_TAB(buf, len)){
      
          zclSmartGarden_HumiIntensity = 0;
          zclSmartGarden_HumiIntensity += buf[3];
          zclSmartGarden_HumiIntensity <<= 8;
          zclSmartGarden_HumiIntensity += buf[4];
          
          zclSmartGarden_TempIntensity = 0;
          zclSmartGarden_TempIntensity += buf[5];
          zclSmartGarden_TempIntensity <<= 8;
          zclSmartGarden_TempIntensity += buf[6];
         
          cust_debug_str("air humi:%d",zclSmartGarden_HumiIntensity);
          cust_debug_str("air temp:%d",zclSmartGarden_TempIntensity);
            return 0;
      }else{
        return -2;
      }
}


uint16 Read_Air_TempHumi()
{ 
   // debug_str(pbuf);
  cust_uart_write(read_air_temphumi_cmd, AIR_CMDLEN);
  uint8 recvlen = 0;
  
  while(!(recvlen = cust_uart_rxlen())){
    AIRReadCount ++;
    if(AIRReadCount >= 0xFF){
      AIRReadCount = 0;
      return TEMP_HUMI_ERR;
    }
    cust_delay_2ms();
  }
  
   if(!CAL_CRC16_TAB(recvbuff, recvlen)){
      
          zclSmartGarden_HumiIntensity = 0;
          zclSmartGarden_HumiIntensity += recvbuff[3];
          zclSmartGarden_HumiIntensity <<= 8;
          zclSmartGarden_HumiIntensity += recvbuff[4];
          
          zclSmartGarden_TempIntensity = 0;
          zclSmartGarden_TempIntensity += recvbuff[5];
          zclSmartGarden_TempIntensity <<= 8;
          zclSmartGarden_TempIntensity += recvbuff[6];
         
            return AIR_SUCCESS;
      }else{
        return TEMP_HUMI_ERR;
      }
}

void send_air_light(){
    sensor_switch(port2);
    //cust_uart_flush();
    cust_uart_write(read_air_light_cmd, AIR_CMDLEN);
}

int read_air_light_cb(uint8* buf, uint8 len){

  if(len < 9)
    return -1;
  
    if(len == 9 && !CAL_CRC16_TAB(buf, len)){
     
          zclSmartGarden_LightIntensity ^= zclSmartGarden_LightIntensity;
          zclSmartGarden_LightIntensity |= buf[3];
          zclSmartGarden_LightIntensity <<= 24;
          zclSmartGarden_LightIntensity |= buf[4];
          zclSmartGarden_LightIntensity <<= 16;
          zclSmartGarden_LightIntensity |= buf[5];
          zclSmartGarden_LightIntensity <<= 8;
          zclSmartGarden_LightIntensity |= buf[6];
          
           cust_debug_str("air light:%d",zclSmartGarden_LightIntensity);
          return 0;   
      }else{
        return -2;
      }
}


uint16 Read_Air_Light()
{
  cust_uart_write(read_air_light_cmd, AIR_CMDLEN);
  
  uint8 recvlen = 0;
  while(!(recvlen = cust_uart_rxlen())){
    AIRReadCount ++;
    if(AIRReadCount >= 0xFF){
      AIRReadCount = 0;
      return LIGHT_ERR;
    }
    cust_delay_2ms();
  }
 
  //UARTCharPut(CUST_UART0_PORT, 1);
  //cust_uart0_write(recvbuff, recvlen);

    if(!CAL_CRC16_TAB(recvbuff, recvlen)){
     
          zclSmartGarden_LightIntensity ^= zclSmartGarden_LightIntensity;
          zclSmartGarden_LightIntensity |= recvbuff[3];
          zclSmartGarden_LightIntensity <<= 24;
          zclSmartGarden_LightIntensity |= recvbuff[4];
          zclSmartGarden_LightIntensity <<= 16;
          zclSmartGarden_LightIntensity |= recvbuff[5];
          zclSmartGarden_LightIntensity <<= 8;
          zclSmartGarden_LightIntensity |= recvbuff[6];
          
          return AIR_SUCCESS;   
      }else{
        return LIGHT_ERR;
      }
}

void Read_Air_Sensor()
{
  uint8 recvlen = 0;
  
  if(UART_INT_RX == UARTIntStatus(CUST_UART_PORT, TRUE)){
    
  while(UARTCharsAvail(CUST_UART_PORT)){
    recvbuff[recvlen] = UARTCharGetNonBlocking(CUST_UART_PORT);
    recvlen++;
  }
      
      if(!CAL_CRC16_TAB(recvbuff, recvlen)){
         
             zclSmartGarden_HumiIntensity = 0;
              zclSmartGarden_HumiIntensity += recvbuff[3];
              zclSmartGarden_HumiIntensity <<= 8;
              zclSmartGarden_HumiIntensity += recvbuff[4];
              
              zclSmartGarden_TempIntensity = 0;
              zclSmartGarden_TempIntensity += recvbuff[5];
              zclSmartGarden_TempIntensity <<= 8;
              zclSmartGarden_TempIntensity += recvbuff[6];
              
              zclSmartGarden_LightIntensity = 0;
              zclSmartGarden_LightIntensity += recvbuff[17];
              zclSmartGarden_LightIntensity <<= 24;
              zclSmartGarden_LightIntensity += recvbuff[18];
              zclSmartGarden_LightIntensity <<= 16;
              zclSmartGarden_LightIntensity += recvbuff[19];
              zclSmartGarden_LightIntensity <<= 8;
              zclSmartGarden_LightIntensity += recvbuff[20];
              
              air_counter = 0;
      }else{
        if(air_counter >= 3){
          air_counter = 0;
          HalLedBlink(HAL_LED_1, 10, 66, 3000);
          // beep_on();
        }else{
           beep_off();
        }
      }
    }
  UARTIntClear(CUST_UART_PORT, UART_INT_RX | UART_INT_RT);
}

void Read_Air_Data()
{ 
   // debug_str(pbuf);
 // UARTIntEnable(CUST_UART_PORT, UART_INT_RX);
   AIRrecvlen = 0;
   cust_uart_write(read_air_data_cmd, AIR_CMDLEN);
   
    TimerIntEnable(GPTIMER2_BASE, GPTIMER_TIMA_TIMEOUT);
    TimerEnable(GPTIMER2_BASE, GPTIMER_A);
}


uint16 CAL_CRC16_TAB(uint8* pchMsg, uint16 wDataLen)
{
        uint8 chCRCHi = 0xFF; // 高CRC字节初始化
        uint8 chCRCLo = 0xFF; // 低CRC字节初始化
        uint16 wIndex;            // CRC循环中的索引
        while (wDataLen--)
        {
                // 计算CRC
                wIndex = chCRCLo ^ *pchMsg++ ;
                chCRCLo = chCRCHi ^ chCRCHTalbe[wIndex];
                chCRCHi = chCRCLTalbe[wIndex] ;
        }
        return ((uint16)(chCRCHi << 8) + chCRCLo) ;
}
