#ifndef CUST_FUNC_H
#define CUST_FUNC_H


#include "hal_types.h"
#include "hal_key.h"
#include "hal_drivers.h"


// use uart0 for custom port

/**
  使用PA2, PA3作为第二个串口的pin脚
  初始化时使用38400 速率
  在setting mode后使用9600作为与外设交互的串口速率
**/

#define CUST_UART_PORT         UART0_BASE
#define CUST_UART_BAUDRATE     38400

#define PERIPH_UART_BUADRATE   9600


#define SETTING_MODE_CODE   "ruff"

#define MAX_WAIT_TIME   6000/10

#define BUFFER_LENGTH   128

#define UNUSED_VARIABLE(X)  ((void)(X))


// default key pb3
#define KEY_DEFAULT_BASE         GPIO_B_BASE
#define KEY_DEFAULT_PIN          GPIO_PIN_3      //!< PB3


#define MAX_DEFAULT_KEY_COUNTER  30

// Led
#define LED_BASE          GPIO_B_BASE
#define LED_PIN1          GPIO_PIN_4  
#define LED_PIN2          GPIO_PIN_5  


#define CUST_LED1_ON()    GPIOPinWrite(LED_BASE, LED_PIN1, LED_PIN1)

#define CUST_LED2_ON()    GPIOPinWrite(LED_BASE, LED_PIN2, LED_PIN2)

#define CUST_LED1_OFF()    GPIOPinWrite(LED_BASE, LED_PIN1, ~LED_PIN1)

#define CUST_LED2_OFF()       GPIOPinWrite(LED_BASE, LED_PIN2, ~LED_PIN2)

#define CUST_LED1_TOOGLE()    GPIOPinWrite(LED_BASE, LED_PIN1, ~(GPIOPinRead(LED_BASE, LED_PIN1)))

#define CUST_LED2_TOOGLE()    GPIOPinWrite(LED_BASE, LED_PIN2, ~(GPIOPinRead(LED_BASE, LED_PIN2)))


void cust_delay_2ms(void);

void cust_delay_10ms(void);

void cust_delay_100ms(int n);

void cust_uart_init(void);

void cust_uart_close(void);

void cust_uart_open(void);

void cust_uart_putChar(char ch);

// uart_print
void cust_uart_print( char *str, ...);

void cust_debug_str( char *fmt, ...);

void wait_setting_mode( int time );


void periph_uart_init(void);

void periph_uart_open(void);

void cust_HalKeyConfig( bool interruptEnable, halKeyCBack_t cback);

void cust_HalKeyPoll(void);

void cust_bspLedInit(void);

;

#endif //CUST_FUNC_H

