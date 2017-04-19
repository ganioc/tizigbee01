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

#include "tasks_type1.h"

extern uint16 PHReadCount;
extern uint8 PHrecvlen;
extern uint8 recvbuff[BUFFER_LENGTH];
extern uint16 zclSmartGarden_PHValue;
extern uint16   zclSmartGarden_AlarmStatus;


void sensor_timer2_init()
{
  SysCtrlPeripheralEnable(SYS_CTRL_PERIPH_GPT2);
  
  TimerDisable(GPTIMER2_BASE, GPTIMER_BOTH);
  TimerIntDisable(GPTIMER2_BASE, GPTIMER_TIMA_TIMEOUT);
  TimerConfigure(GPTIMER2_BASE, (GPTIMER_CFG_SPLIT_PAIR | GPTIMER_CFG_A_PERIODIC | GPTIMER_CFG_B_PERIODIC));
  
  TimerLoadSet(GPTIMER2_BASE, GPTIMER_BOTH, (SysCtrlClockGet() / 1000 * SENSOR_TIMER_UPDATE_PEROID));
  
  //TimerLoadSet(GPTIMER2_BASE, GPTIMER_B, (SysCtrlClockGet() / 1000 * 2));
  
  //TimerIntRegister(GPTIMER2_BASE, GPTIMER_B, ReadAIR_Timer2_Handler);
  TimerIntRegister(GPTIMER2_BASE, GPTIMER_A, check_uart_rx_in_timer);
 // TimerIntClear(GPTIMER2_BASE,  GPTIMER_TIMA_TIMEOUT);
}
