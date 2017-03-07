#include "gptimer.h"
#include "hw_memmap.h"
#include "hw_ints.h"
#include "sys_ctrl.h"
#include "cust_timer.h"
#include "hal_types.h"
#include "rom.h"
#include "sys_ctrl.h"

uint16 chipID = 0;
void cust_timer_init()
{
  SysCtrlPeripheralEnable(SYS_CTRL_PERIPH_GPT1);
  
  TimerDisable(GPTIMER1_BASE, GPTIMER_BOTH);
  TimerIntDisable(GPTIMER1_BASE, GPTIMER_TIMA_TIMEOUT);
  TimerConfigure(GPTIMER1_BASE, GPTIMER_CFG_PERIODIC);
  
  TimerLoadSet(GPTIMER1_BASE, GPTIMER_BOTH, SysCtrlClockGet() * 5);
  TimerIntRegister(GPTIMER1_BASE, GPTIMER_BOTH, Timer1_Handler);
  
  TimerIntEnable(GPTIMER1_BASE, GPTIMER_TIMA_TIMEOUT);
  TimerEnable(GPTIMER1_BASE, GPTIMER_BOTH);
}

void Timer1_Handler()
{
  chipID = ROM_GetChipId();
  while(GPTIMER_TIMA_TIMEOUT != TimerIntStatus(GPTIMER1_BASE, TRUE));
  TimerIntClear(GPTIMER1_BASE,  GPTIMER_TIMA_TIMEOUT);
}