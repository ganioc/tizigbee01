#include "gptimer.h"
#include "hw_memmap.h"
#include "hw_ints.h"
#include "sys_ctrl.h"
#include "cust_timer.h"
#include "hal_types.h"
#include "rom.h"
#include "sys_ctrl.h"
#include "peripheral.h"
#include "cust_func.h"

extern byte peripheral_TaskID;
extern uint16 zclSmartGarden_HeartbeatPeriod;
extern uint64 zclSmartGarden_ChipId;
extern uint32 chipidH, chipidL;


// Added by Yang
void  init_1shot_cust_timer(){
  
  zclSmartGarden_ChipId = chipidH;
  zclSmartGarden_ChipId <<= 32;
  zclSmartGarden_ChipId += chipidL;

}
void start_1shot_cust_timer(){
    osal_start_timerEx(peripheral_TaskID, PERIPH_HEARTBEAT_REPORT, 
        zclSmartGarden_HeartbeatPeriod * 1000 );
}

void trigger_sensor_timer(){
    //start read sensor
    osal_start_timerEx(peripheral_TaskID, PERIPH_TYPE_SENSOR_UPDATE, 
        PERIOD_SENSOR_UPDATE * 1000); 

}







void cust_timer_init(uint8 period)
{
  SysCtrlPeripheralEnable(SYS_CTRL_PERIPH_GPT1);
  
  TimerDisable(GPTIMER1_BASE, GPTIMER_BOTH);
  TimerIntDisable(GPTIMER1_BASE, GPTIMER_TIMA_TIMEOUT);
  TimerConfigure(GPTIMER1_BASE, GPTIMER_CFG_PERIODIC);
  
  TimerLoadSet(GPTIMER1_BASE, GPTIMER_BOTH, SysCtrlClockGet() * period);
  TimerIntRegister(GPTIMER1_BASE, GPTIMER_BOTH, Timer1_Handler);
  
  TimerIntEnable(GPTIMER1_BASE, GPTIMER_TIMA_TIMEOUT);
  TimerEnable(GPTIMER1_BASE, GPTIMER_BOTH);
  
  //read MAC address
  /*
  zclSmartGarden_ChipId = (*((uint32*)0x00280028));
  zclSmartGarden_ChipId <<= 32;
  zclSmartGarden_ChipId |= (*((uint32*)( 0x0028002c )));
  */
  zclSmartGarden_ChipId = chipidH;
  zclSmartGarden_ChipId <<= 32;
  zclSmartGarden_ChipId += chipidL;
}

void Timer1_Handler()
{
  if(GPTIMER_TIMA_TIMEOUT == TimerIntStatus(GPTIMER1_BASE, TRUE)){
 
  TimerIntClear(GPTIMER1_BASE,  GPTIMER_TIMA_TIMEOUT);
  osal_set_event(peripheral_TaskID, PERIPH_HEARTBEAT_REPORT);
  }
}


