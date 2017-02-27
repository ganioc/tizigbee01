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

char buffer[BUFFER_LENGTH];
int index;

int process_setting_cmd(char *buf);

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
  
  IOCPinConfigPeriphOutput(GPIO_A_BASE, GPIO_PIN_3, IOC_MUX_OUT_SEL_UART0_TXD);
  IOCPinConfigPeriphInput(GPIO_A_BASE, GPIO_PIN_2, IOC_UARTRXD_UART0);
  GPIOPinTypeUARTInput(GPIO_A_BASE, GPIO_PIN_2);
  GPIOPinTypeUARTOutput(GPIO_A_BASE, GPIO_PIN_3);  
  
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
        break;
      }
    }

    counter++;
    
    if(counter > MAX_WAIT_TIME ){
      return;
    }
    
    if(counter%100 == 0){
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