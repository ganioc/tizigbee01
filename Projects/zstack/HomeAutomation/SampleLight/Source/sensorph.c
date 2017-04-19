#include "cust_func.h"
#include "OSAL_Memory.h"
#include "OSAL.h"
#include "hal_types.h"
#include "sensor.h"
#include "sensorph.h"
#include "gptimer.h"
#include "hal_led.h"

extern uint8 recvbuff[BUFFER_LENGTH];
extern uint16  zclSmartGarden_PHValue;
uint16 PHReadCount;
uint8 PHrecvlen = 0;
extern uint8 ph_counter;

uint8 read_addr_cmd[READ_CMD_LEN] = {0x00, 0x20};
uint8 write_addr_cmd[WRITE_CMD_LEN] = {0x00, 0x10, SLAVE_ADDR_PH};
uint8 read_data_cmd[READ_DATA_LEN] = {SLAVE_ADDR_PH, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x39};


void send_soil_ph(void){
    sensor_switch(port1);
    //cust_delay_100ms(1);
    
   // cust_uart_flush();
    
    cust_uart_write(read_data_cmd, READ_DATA_LEN);

}


int read_soil_ph_cb(uint8* buf, uint8 len){
  
  //cust_debug_str("rx ph len:%d", len);
  uint8 i = 0;
  if(len < 7){
    
      return -1;
      
  }
    
    if(len ==7 && !Cal_Crc16( buf, len)){

      //? what does this mean????
      zclSmartGarden_PHValue ^= zclSmartGarden_PHValue;
      
      zclSmartGarden_PHValue |= buf[3];
      zclSmartGarden_PHValue <<= 8;
      zclSmartGarden_PHValue |= buf[4];
      
      //print
      cust_debug_str("ph:%d", zclSmartGarden_PHValue);
      
      return 0;
    }else{
      cust_debug_str("rx ph wrong crc, len:%d", len);
       for(; i < len; i++){
         cust_debug_str("recvbuf[%d] = %d", i, buf[i]);
      }
        return -2;
    }

}

void Read_Soil_Ph()
{
  PHrecvlen = 0;
  PHReadCount = 0;
  cust_uart_write(read_data_cmd, READ_DATA_LEN);
 
  if(!(PHrecvlen = cust_uart_rxlen())){
    TimerIntEnable(GPTIMER2_BASE, GPTIMER_TIMA_TIMEOUT);
    TimerEnable(GPTIMER2_BASE, GPTIMER_A);
  }else{
     if(!Cal_Crc16( recvbuff, PHrecvlen)){
      
      zclSmartGarden_PHValue ^= zclSmartGarden_PHValue;
      zclSmartGarden_PHValue |= recvbuff[3];
      zclSmartGarden_PHValue <<= 8;
      zclSmartGarden_PHValue |= recvbuff[4];
      
     }else{
      ph_counter ++;
      if(ph_counter >= 3){
        ph_counter = 0;
        HalLedBlink(HAL_LED_1, 10, 66, 3000);
      }else{
        beep_off();
      }
     }
  }

 // UARTCharPut(CUST_UART0_PORT, 0xFF);
 // cust_uart0_write(recvbuff, recvlen);
}
