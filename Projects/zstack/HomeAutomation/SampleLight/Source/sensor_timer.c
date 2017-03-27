#include "gptimer.h"
#include "hw_memmap.h"
#include "hw_ints.h"
#include "sys_ctrl.h"
#include "sensor_timer.h"
#include "hal_types.h"
#include "rom.h"
#include "sys_ctrl.h"
#include "peripheral.h"
#include "cust_func.h"
#include "sensor.h"
#include "hal_led.h"
#include "air_sensor.h"

extern uint16 PHReadCount;
extern uint8 PHrecvlen;
extern uint8 recvbuff[BUFFER_LENGTH];
extern uint16 zclSmartGarden_PHValue;
extern uint16   zclSmartGarden_AlarmStatus;

extern uint32 zclSmartGarden_LightIntensity;
extern uint16 zclSmartGarden_TempIntensity;
extern uint16 zclSmartGarden_HumiIntensity;

uint8 ph_counter = 0;

void sensor_timer2_init()
{
  SysCtrlPeripheralEnable(SYS_CTRL_PERIPH_GPT2);
  
  TimerDisable(GPTIMER2_BASE, GPTIMER_BOTH);
  TimerIntDisable(GPTIMER2_BASE, GPTIMER_TIMA_TIMEOUT);
  TimerConfigure(GPTIMER2_BASE, (GPTIMER_CFG_SPLIT_PAIR + GPTIMER_CFG_A_PERIODIC + GPTIMER_CFG_B_PERIODIC));
  
  TimerLoadSet(GPTIMER2_BASE, GPTIMER_BOTH, (SysCtrlClockGet() / 1000 * 2));
  //TimerLoadSet(GPTIMER2_BASE, GPTIMER_B, (SysCtrlClockGet() / 1000 * 2));
  
  //TimerIntRegister(GPTIMER2_BASE, GPTIMER_B, ReadAIR_Timer2_Handler);
  TimerIntRegister(GPTIMER2_BASE, GPTIMER_A, ReadPH_Timer2_Handler);
}

void ReadPH_Timer2_Handler()
{
  
  if(GPTIMER_TIMA_TIMEOUT == TimerIntStatus(GPTIMER2_BASE, TRUE)){
    PHReadCount ++;
    PHrecvlen = cust_uart_rxlen();
    if(PHReadCount >= 100){
      ph_counter ++;
      PHReadCount = 0;
      //TimerIntDisable(GPTIMER2_BASE, GPTIMER_TIMA_TIMEOUT);
      TimerDisable(GPTIMER2_BASE, GPTIMER_A);
    }
  
    if(PHrecvlen){
      if(!Cal_Crc16( recvbuff, PHrecvlen)){
          zclSmartGarden_PHValue ^= zclSmartGarden_PHValue;
          zclSmartGarden_PHValue |= recvbuff[3];
          zclSmartGarden_PHValue <<= 8;
          zclSmartGarden_PHValue |= recvbuff[4];
          
          //TimerIntDisable(GPTIMER2_BASE, GPTIMER_TIMA_TIMEOUT);
          zclSmartGarden_AlarmStatus &= ~ZCLSMARTGARDEN_STATE_ERR_PH;
          TimerDisable(GPTIMER2_BASE, GPTIMER_A);
      }else{
        ph_counter ++;
      }
    }
    
    if(ph_counter >= 3){
       ph_counter = 0;
       zclSmartGarden_AlarmStatus += ZCLSMARTGARDEN_STATE_ERR_PH;
       HalLedBlink(HAL_LED_1, 10, 66, 3000);
    }else{
      beep_off();
    }

  TimerIntClear(GPTIMER2_BASE,  GPTIMER_TIMA_TIMEOUT);
}
}
/*
void ReadAIR_Timer2_Handler()
{
  if(GPTIMER_TIMA_TIMEOUT == TimerIntStatus(GPTIMER2_BASE, TRUE)){
    AIRReadCount ++;
    AIRrecvlen = cust_uart_rxlen();
    if(AIRReadCount >= 15){
      air_counter ++;
      AIRReadCount = 0;
      //TimerIntDisable(GPTIMER2_BASE, GPTIMER_TIMA_TIMEOUT);
      TimerDisable(GPTIMER2_BASE, GPTIMER_B);
    }
  
    if(AIRrecvlen){
      if(!CAL_CRC16_TAB( recvbuff, AIRrecvlen)){
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
              
              zclSmartGarden_AlarmStatus = 0;
              TimerDisable(GPTIMER2_BASE, GPTIMER_B);
      }else{
        air_counter ++;
      }
    }
    
    if(air_counter >= 3){
       air_counter = 0;
       zclSmartGarden_AlarmStatus = ZCLSMARTGARDEN_STATE_ENV_SENSOR_ERR;
       HalLedBlink(HAL_LED_1, 10, 66, 3000);
    }else{
      beep_off();
    }
  }
  
  TimerIntClear(GPTIMER2_BASE,  GPTIMER_TIMA_TIMEOUT);
}
*/