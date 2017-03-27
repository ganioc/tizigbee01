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
