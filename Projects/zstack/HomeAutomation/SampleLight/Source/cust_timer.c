#include "gptimer.h"
#include "hw_memmap.h"
#include "hw_ints.h"
#include "sys_ctrl.h"
#include "cust_timer.h"
#include "hal_types.h"
#include "rom.h"
#include "sys_ctrl.h"
#include "peripheral.h"

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
}

void Timer1_Handler()
{
  while(GPTIMER_TIMA_TIMEOUT != TimerIntStatus(GPTIMER1_BASE, TRUE));
  zclSmartGarden_ChipId = ROM_GetChipId();

  
  osal_set_event(peripheral_TaskID, PERIPH_HEARTBEAT_REPORT);
  TimerIntClear(GPTIMER1_BASE,  GPTIMER_TIMA_TIMEOUT);
}