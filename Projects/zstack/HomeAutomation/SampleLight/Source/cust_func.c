#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "cust_func.h"

#include "hw_memmap.h"
#include "hw_types.h"
#include "hw_ioc.h"
#include "sys_ctrl.h"
#include "uart.h"
#include "gpio.h"
#include "ioc.h"
#include "OSAL.h"
#include "DebugTrace.h"

#include "ZComDef.h"
#include "OnBoard.h"
#include "OSAL_Nv.h"
#include "OSAL.h"

#include "sensor.h"
#include "sensorph.h"
#include "peripheral.h"
#include "hal_led.h"
#include "air_sensor.h"

extern uint8 Hal_TaskID;
extern uint8 peripheral_TaskID;


char buffer[BUFFER_LENGTH];
uint8 recvbuff[BUFFER_LENGTH];
int index;

extern uint16   zclSmartGarden_AlarmStatus;
uint16 zclSmartGarden_Status = 0;
uint8 phcounter = 0, tempcounter = 0, humicounter = 0;

uint8 light_counter = 0;
uint8 temp_humi_counter = 0;

static int counterDefaultKey = 0;

int process_setting_cmd(char *buf);
void cust_delay_1ms(void){
  SysCtrlDelay(SysCtrlClockGet() / 1000 / 3);
}

void cust_delay_2ms(void){
  SysCtrlDelay(SysCtrlClockGet() / 500 / 3);
}
void cust_delay_10ms(void){
  SysCtrlDelay(SysCtrlClockGet() / 100 / 3);
}
void cust_delay_100ms(int n){
  int i;
  for(i=0; i< 50*n; i++){
    SysCtrlDelay(SysCtrlClockGet() / 500 / 3);
  }
}

void cust_uart_init(void){
  SysCtrlPeripheralEnable(SYS_CTRL_PERIPH_UART0);

  /* 
   */ 
#ifdef NEW_BOARD
  IOCPinConfigPeriphOutput(GPIO_C_BASE, GPIO_PIN_5, IOC_MUX_OUT_SEL_UART0_TXD);
  IOCPinConfigPeriphInput(GPIO_C_BASE, GPIO_PIN_4, IOC_UARTRXD_UART0);
  GPIOPinTypeUARTInput(GPIO_C_BASE, GPIO_PIN_4);
  GPIOPinTypeUARTOutput(GPIO_C_BASE, GPIO_PIN_5);  
#else
  IOCPinConfigPeriphOutput(GPIO_A_BASE, GPIO_PIN_3, IOC_MUX_OUT_SEL_UART0_TXD);
  IOCPinConfigPeriphInput(GPIO_A_BASE, GPIO_PIN_2, IOC_UARTRXD_UART0);
  GPIOPinTypeUARTInput(GPIO_A_BASE, GPIO_PIN_2);
  GPIOPinTypeUARTOutput(GPIO_A_BASE, GPIO_PIN_3);  
#endif
  
}

void cust_uart_open(void){
  UARTDisable(CUST_UART_PORT);
  
  UARTConfigSetExpClk(CUST_UART_PORT, SysCtrlClockGet(), CUST_UART_BAUDRATE,
                         (UART_CONFIG_WLEN_8 | UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE));
  
  UARTIntDisable(CUST_UART_PORT, (UART_INT_RX | UART_INT_TX | UART_INT_CTS | UART_INT_RT ));
  
  UARTEnable(CUST_UART_PORT);
}
  
void cust_uart_close(void){
  
  UARTDisable(CUST_UART_PORT);
}

void periph_uart_init(void){
  
}

void periph_uart_open(void){
  UARTDisable(CUST_UART_PORT);
  
  UARTConfigSetExpClk(CUST_UART_PORT, SysCtrlClockGet(), PERIPH_UART_BUADRATE,
                         (UART_CONFIG_WLEN_8 | UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE));
  
  UARTIntDisable(CUST_UART_PORT, (UART_INT_RX | UART_INT_TX | UART_INT_CTS | UART_INT_RT ));
  
  UARTEnable(CUST_UART_PORT);
}


void cust_uart_putChar(char ch){
  
  UARTCharPut(CUST_UART_PORT, ch);
}

void cust_uart_put( char *str, ...){
  int i, len;
  len = strlen( str );
  for(i=0; i< len; i++){
    cust_uart_putChar( str[i] );
  }
  
}

void cust_uart_write(uint8 *pbuf, uint8 len)
{
  uint8 i = 0;
  for(; i<len; i++){
    cust_uart_putChar(*pbuf++);
    cust_delay_2ms();
  }
}

uint8 cust_uart_rxlen()
{
  osal_memset(recvbuff, '\0', BUFFER_LENGTH);
  uint8 len = 0;
  while(UARTCharsAvail(CUST_UART_PORT)){
    recvbuff[len] = UARTCharGetNonBlocking(CUST_UART_PORT);
    len++;
  }
  return len;
}

void cust_debug_str( char *fmt, ...){
  va_list ap;//初始化指向可变参数列表的指针         
  char string[256];         
  va_start(ap,fmt);//将第一个可变参数的地址付给ap，即ap指向可变参数列表的开始         
  vsprintf(string,fmt,ap);//将参数fmt、ap指向的可变参数一起转换成格式化字符串，放string数组中，其作用同sprintf（），只是参数类型不同         
  debug_str((uint8 *) string); //把格式化字符串从debug
  va_end(ap);    //ap付值为0，没什么实际用处，主要是为程序健壮性   
}

void cust_uart_print( char *fmt, ...){
  va_list ap;//初始化指向可变参数列表的指针         
  char string[256];         
  va_start(ap,fmt);//将第一个可变参数的地址付给ap，即ap指向可变参数列表的开始         
  vsprintf(string,fmt,ap);//将参数fmt、ap指向的可变参数一起转换成格式化字符串，放string数组中，其作用同sprintf（），只是参数类型不同         
  cust_uart_put(string); //把格式化字符串从开发板串口送出去         
  va_end(ap);    //ap付值为0，没什么实际用处，主要是为程序健壮性     
  
}
//char cust_uart_getChar(void){
//  return UARTCharGetNonBlocking();
//}
void reset(void){
  int i;
  for(i=0; i< BUFFER_LENGTH; i++){
  
    buffer[i] = 0;
  }
  index = 0;
}
void remove_CRLF(char *buf){
  int len = strlen(buf);
  
  if(buf[len-1] == '\n' || buf[len-1] == '\r' ){
    buf[len -1] = '\0';
  }
  if(buf[len-2] == '\n' || buf[len-2] == '\r' ){
    buf[len -2] = '\0';
  }
}
void setting_mode_loop(void){
  cust_uart_put("\nInto setting mode\n");
  char ch;
  uint8 led_counter = 0;
  reset();
  
  while(1){
    if(UARTCharsAvail(CUST_UART_PORT)){
      ch = UARTCharGetNonBlocking(CUST_UART_PORT);
      buffer[index++] = ch;
      
      if(ch == '\n'){
        // remove '\n'
        buffer[strlen(buffer)]='\0';
        if(0 == process_setting_cmd(buffer)){
          break;
        }
        reset();
      }
    }
    if(led_counter == 100){
      led_counter = 0;
      HalLedSet(HAL_LED_1, HAL_LED_MODE_TOGGLE);
    }
    led_counter ++;
    cust_delay_10ms();
  }
}

void wait_setting_mode( int time ){
  int  counter =0;
  char ch;
  
  reset();
  
  while(1){
    if(UARTCharsAvail(CUST_UART_PORT)){
      ch = UARTCharGetNonBlocking(CUST_UART_PORT);
      buffer[index++] = ch;
      
      remove_CRLF( buffer );
      
      if(!strcmp(buffer,SETTING_MODE_CODE)){
        //blink_sign = 0;
        break;
      }
    }
    if((counter % 50) == 0){
        HalLedSet(HAL_LED_1, HAL_LED_MODE_TOGGLE);
      }
    counter++;
    
    if(counter > MAX_WAIT_TIME ){
      return;
    }
    
    if(counter % 100 == 0){
      cust_uart_putChar('.');
    }
    
    cust_delay_10ms();
  }
  
  // setting mode message handler
  setting_mode_loop();
}

int process_setting_cmd(char *buf){
  
  
  
  //cust_uart_put("Rcv:%\n", buf[len-1], buf[len-2]);
  //cust_uart_print("Recv:%d  %d\n", buf[len-1], buf[len-2]);
  
  
  cust_uart_print("Rcv:%s\n", buf);
  
  remove_CRLF( buf);

  cust_uart_print("len:%d\n", strlen(buf));
  
  if(!strcmp(buf,"hi")){
    cust_uart_print("Hi\n");
  }
  else if(!strcmp(buf,"quit")){
    cust_uart_print("Quit\n");

    
    return 0;
  }
  else{
    cust_uart_print("Unrecognized cmd\n");
  }
  return 1;
}
void cust_bspLedInit(void){
    GPIOPinTypeGPIOOutput(LED_BASE, LED_PIN1|LED_PIN2);
    HalLedSet (HAL_LED_1|HAL_LED_2, HAL_LED_MODE_OFF);
    
    //IOCPadConfigSet(LED_BASE, LED_PIN1|LED_PIN2, IOC_OVERRIDE_PDE);
    
}


void cust_bspKeyInit(uint8_t ui8Mode){
  
      //
    // Store mode
    //
    //ui8BspKeyMode = ui8Mode;

    //
    // Initialize keys on GPIO port B (input pullup)
    //
    GPIOPinTypeGPIOInput(KEY_DEFAULT_BASE, KEY_DEFAULT_PIN);
    IOCPadConfigSet(KEY_DEFAULT_BASE, KEY_DEFAULT_PIN, IOC_OVERRIDE_PUE);
    //GPIOPinIntDisable(KEY_DEFAULT_BASE, KEY_DEFAULT_PIN);
    
}

// key poll function
void cust_HalKeyConfig( bool interruptEnable, halKeyCBack_t cback){
  
  cust_bspKeyInit(BSP_KEY_MODE_POLL);
  
}

void cust_HalKeyPoll(void){
  //uint8 value = 3;
  
  if( !GPIOPinRead(KEY_DEFAULT_BASE, KEY_DEFAULT_PIN) ){
    counterDefaultKey++;   
    
  }else{
    counterDefaultKey = 0;
  }
  
  if(counterDefaultKey > MAX_DEFAULT_KEY_COUNTER ){
    cust_debug_str("counter:%d", counterDefaultKey);
    debug_str("Valid Key");
    counterDefaultKey = 0;

    CUST_LED1_ON();

    osal_start_timerEx( peripheral_TaskID , PERIPH_RESET_EVENT , 2000);
		
    /*
    if ((osal_nv_write(3, 0, 1, &value)) == ZSUCCESS)
    {
       // 
       CUST_LED2_ON();
       // delay 1 second
       
       osal_start_timerEx( peripheral_TaskID , PERIPH_RESET_EVENT , 1000);
    }
    */
    
  }
  
}

uint8 update_soil_sensor()
{
  uint16 retStatue = 0;
  uint8 statue = 0;
  
  sensor_switch(port1);
  retStatue = Read_Soil_Ph();
  if(retStatue == 0xFA){
    statue |= 0x1;
  }
  
  sensor_switch(port0);
  retStatue = Read_Soil_Temp();
  if(retStatue == 0xFF){
    statue |= 0x2;
  }
  
  retStatue = Read_Soil_Humi();
  if(retStatue == 0xFC){
    statue |= 0x4;
  }
  
  return statue;
}

uint8 update_air_sensor()
{
  uint16 retStatue = 0;
  uint8 statue = 0;
  
 // sensor_switch(port0);
  retStatue = Read_Air_Light();
  if(retStatue == 0xFE){
    statue |= 0x1;
  }

 retStatue = Read_Air_Temp_Humi();
  if(retStatue == 0xFF){
    statue |= 0x2;
  }
  
  return statue;
}

void soil_alarm_sign()
{
    uint8 statue = update_soil_sensor();
    
    if(statue & 0x1){
      phcounter ++;
    }
    if(statue & 0x2){
      tempcounter ++;
    }
    
    if(statue & 0x4){
      humicounter ++;
    }
    if(phcounter >= 3){
       phcounter = 0;
       zclSmartGarden_Status |= ZCLSMARTGARDEN_STATE_ERR_PH;
    }
    if(tempcounter >= 3 || humicounter >= 3){
      if(tempcounter >= 3){
        tempcounter = 0;
      }
      
      if(humicounter >= 3){
        humicounter = 0;
      }
     
      zclSmartGarden_Status |= ZCLSMARTGARDEN_STATE_ERR_TEMP_HUMI;
    }
    if(zclSmartGarden_Status){
      HalLedBlink(HAL_LED_1, 10, 66, 3000);
     // beep_on();
      zclSmartGarden_AlarmStatus = zclSmartGarden_Status;
      zclSmartGarden_Status = 0;
    }else{
      beep_off();
      zclSmartGarden_AlarmStatus = 0;
    }
}

void air_alarm_sign()
{
    uint8 statue = update_air_sensor();
    
    if(statue & 0x1){
      light_counter ++;
    }
    if(statue & 0x2){
      temp_humi_counter ++;
    }
    
    if(light_counter >= 3 || temp_humi_counter >= 3){
      if(light_counter >= 3){
       light_counter = 0;
      }
      if(temp_humi_counter >= 3){
        temp_humi_counter = 0;
      }
       zclSmartGarden_Status |= ZCLSMARTGARDEN_STATE_ENV_SENSOR_ERR;
    }
     
    if(zclSmartGarden_Status){
      HalLedBlink(HAL_LED_1, 10, 66, 3000);
      //beep_on();
      zclSmartGarden_AlarmStatus = zclSmartGarden_Status;
      zclSmartGarden_Status = 0;
    }else{
      beep_off();
      zclSmartGarden_AlarmStatus = 0;
    }
}

void relay_init()
{
  GPIOPinTypeGPIOOutput(ELEC_TOGGLE, ELEC_TOGGLE_PIN6|ELEC_TOGGLE_PIN7);
  IOCPadConfigSet(ELEC_TOGGLE, ELEC_TOGGLE_PIN6, IOC_OVERRIDE_PDE);
  GPIOPinWrite(ELEC_TOGGLE, ELEC_TOGGLE_PIN6, ELEC_RELAY0_OFF);
  GPIOPinWrite(ELEC_TOGGLE, ELEC_TOGGLE_PIN7, ELEC_RELAY1_OFF);
}

void relay0_turn_on()
{
  GPIOPinWrite(ELEC_TOGGLE, ELEC_TOGGLE_PIN6, ELEC_RELAY0_ON);
  //GPIOPinWrite(ELEC_TOGGLE, ELEC_TOGGLE_PIN7, ELEC_RELAY1_ON);
}

void relay1_turn_on()
{
  GPIOPinWrite(ELEC_TOGGLE, ELEC_TOGGLE_PIN7, ELEC_RELAY1_ON);
}

void relay0_turn_off()
{
   GPIOPinWrite(ELEC_TOGGLE, ELEC_TOGGLE_PIN6, ELEC_RELAY0_OFF);
   //GPIOPinWrite(ELEC_TOGGLE, ELEC_TOGGLE_PIN7, ELEC_RELAY1_OFF);
}

void relay1_turn_off()
{
   GPIOPinWrite(ELEC_TOGGLE, ELEC_TOGGLE_PIN7, ELEC_RELAY1_OFF);
}

void beep_init()
{
  GPIOPinTypeGPIOOutput(BEEP_BASE, BEEP_PIN);
  IOCPadConfigSet(BEEP_BASE, BEEP_PIN, IOC_OVERRIDE_PUE|IOC_OVERRIDE_OE);
  GPIOPinWrite(BEEP_BASE, BEEP_PIN, BEEP_OFF);
}

void beep_on()
{
  GPIOPinWrite(BEEP_BASE, BEEP_PIN, BEEP_ON);
}

void beep_off()
{
   GPIOPinWrite(BEEP_BASE, BEEP_PIN, BEEP_OFF);
}

                      
void sensor_switch_init()
{
  GPIOPinTypeGPIOOutput(RS485_SWITCH_BASE, S0_PIN | S1_PIN);
  IOCPadConfigSet(RS485_SWITCH_BASE, S0_PIN | S1_PIN, IOC_OVERRIDE_OE);
}

void open_port0()
{
   GPIOPinWrite(RS485_SWITCH_BASE, S1_PIN, ~S1_PIN);
   GPIOPinWrite(RS485_SWITCH_BASE, S1_PIN, ~S0_PIN);
}

void open_port1()
{
   GPIOPinWrite(RS485_SWITCH_BASE, S1_PIN, ~S1_PIN);
   GPIOPinWrite(RS485_SWITCH_BASE, S1_PIN, S0_PIN);
}

void open_port2()
{
   GPIOPinWrite(RS485_SWITCH_BASE, S1_PIN, S1_PIN);
   GPIOPinWrite(RS485_SWITCH_BASE, S1_PIN, ~S0_PIN);
}

void open_port3()
{
   GPIOPinWrite(RS485_SWITCH_BASE, S1_PIN, S1_PIN);
   GPIOPinWrite(RS485_SWITCH_BASE, S1_PIN, S0_PIN);
}

void sensor_switch(uint8 port)
{
  switch(port){
  case port0:
    open_port0();
    break;
  case port1:
    open_port1();
    break;
  case port2:
    open_port2();
    break;
  case port3:
    open_port3();
    break;
  default:
    debug_str("unknown port");
  }
}
