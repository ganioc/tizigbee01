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
extern uint16 zclSmartGarden_ChipId;

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
}

void Timer1_Handler()
{
  if(GPTIMER_TIMA_TIMEOUT == TimerIntStatus(GPTIMER1_BASE, TRUE)){
  //zclSmartGarden_ChipId += (*((uint32*)( 0x00280028 ))) << 32;
  //zclSmartGarden_ChipId += (*((uint32*)( 0x00280028 )));
  //zclSmartGarden_ChipId <<= 32;
  //cust_debug_str("chipID : %d", zclSmartGarden_ChipId>>32);
  //zclSmartGarden_ChipId += (*(uint32*)(((uint32*)(0x00280028) + 4)));
 // zclSmartGarden_ChipId += (*(uint32*)((0x00280028 + 4)));
  //cust_debug_str("chipID : %d", zclSmartGarden_ChipId);
  
  //chipID =  (*((uint32*)0x00280028));

  //cust_debug_str("chipID : %d", chipID);
  
  //chipID =  (*((uint32*)( 0x0028002c )));

  //cust_debug_str("chipID : %d", chipID);
  TimerIntClear(GPTIMER1_BASE,  GPTIMER_TIMA_TIMEOUT);
  osal_set_event(peripheral_TaskID, PERIPH_HEARTBEAT_REPORT);
  }
}