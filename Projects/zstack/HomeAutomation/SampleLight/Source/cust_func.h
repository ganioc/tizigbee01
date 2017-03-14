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


// Added by Yang
#define ATTRID_BASIC_SMARTGARDEN_HEARTBEAT_PERIOD               0x4001
#define ATTRID_BASIC_SMARTGARDEN_ALARM_STATUS                      0x4012
#define ATTRID_BASIC_SMARTGARDEN_PH_VALUE                      0x4003
#define ATTRID_BASIC_SMARTGARDEN_TEMP                              0x4004
#define ATTRID_BASIC_SMARTGARDEN_HUMIDITY                      0x4005
#define ATTRID_BASIC_SMARTGARDEN_LIGHT_INTENSITY         0x4006
#define ATTRID_BASIC_SMARTGARDEN_IRRIGATE_ONOFF          0x4007
#define ATTRID_BASIC_SMARTGARDEN_STATE                            0x4008
#define ATTRID_BASIC_SMARTGARDEN_CHIPID                           0x4009
#define ATTRID_BASIC_SMARTGARDEN_CHIPID_ACK                     0x4010
#define ATTRID_BASIC_SMARTGARDEN_ATTR_LIST                      0x4011

//Coordinator cmdID
#define FUNC_TURN_OFF_LIGHT_CMD                         0x0
#define FUNC_TURN_ON_LIGHT_CMG                          0x1
#define FUNC_LIGHT_TOGGLE_CMD                           0x2
#define FUNC_READ_ATTR_CMD                              0x10
#define FUNC_READ_LIGHT_STA_CMD                         0x11
#define FUNC_READ_HEATBEAT_PERIOD_CMD                   0x12
#define FUNC_SET_HEARTBEAT_PERIOD_30_CMD                0x13
#define FUNC_SET_HEARTBEAT_PERIOD_15_CMD                0x14
#define FUNC_SEND_HEARTBEAT_REPORT_CMD                  0x20
#define FUNC_READ_SOIL_TEMP_CMD                         0x30
#define FUNC_READ_SOIL_HUMI_CMD                         0x31
#define FUNC_READ_SOIL_PH_CMD                           0x32


//SENSOR ERR STATE
#define ZCLSMARTGARDEN_STATE_ERR_TEMP_HUMI   0x01
    
#define ZCLSMARTGARDEN_STATE_ERR_PH   0x02

// default key pb3
#define KEY_DEFAULT_BASE         GPIO_B_BASE
#define KEY_DEFAULT_PIN          GPIO_PIN_3      //!< PB3
//default relay
#define ELEC_TOGGLE              GPIO_A_BASE
#define ELEC_TOGGLE_PIN6         GPIO_PIN_6
#define ELEC_TOGGLE_PIN7         GPIO_PIN_7

#define ELEC_RELAY0_ON           GPIO_PIN_6
#define ELEC_RELAY1_ON           GPIO_PIN_7
#define ELEC_RELAY0_OFF          (GPIOPinRead(GPIO_A_BASE, GPIO_PIN_ALL) & (~GPIO_PIN_6)) 
#define ELEC_RELAY1_OFF          (GPIOPinRead(GPIO_A_BASE, GPIO_PIN_ALL) & (~GPIO_PIN_7))      

//default beep
#define BEEP_BASE               GPIO_C_BASE
#define BEEP_PIN                GPIO_PIN_1
#define BEEP_ON                 GPIO_PIN_1
#define BEEP_OFF                (GPIOPinRead(GPIO_A_BASE, GPIO_PIN_ALL) & (~GPIO_PIN_1)) 


#define MAX_DEFAULT_KEY_COUNTER  30

// Led
#define LED_BASE          GPIO_B_BASE
#define LED_PIN1          GPIO_PIN_4  
#define LED_PIN2          GPIO_PIN_5  


#define CUST_LED1_ON()    GPIOPinWrite(LED_BASE, LED_PIN1, ~LED_PIN1)

#define CUST_LED2_ON()    GPIOPinWrite(LED_BASE, LED_PIN2, ~LED_PIN2)

#define CUST_LED1_OFF()    GPIOPinWrite(LED_BASE, LED_PIN1, LED_PIN1)

#define CUST_LED2_OFF()       GPIOPinWrite(LED_BASE, LED_PIN2, LED_PIN2)

#define CUST_LED1_TOOGLE()    GPIOPinWrite(LED_BASE, LED_PIN1, ~(GPIOPinRead(LED_BASE, LED_PIN1)))

#define CUST_LED2_TOOGLE()    GPIOPinWrite(LED_BASE, LED_PIN2, ~(GPIOPinRead(LED_BASE, LED_PIN2)))


void cust_delay_2ms(void);

void cust_delay_10ms(void);

void cust_delay_100ms(int n);

void cust_uart_init(void);

void cust_uart_close(void);

void cust_uart_open(void);

void cust_uart_putChar(char ch);

void cust_uart_write(uint8 *pbuf, uint8 len);

uint8 cust_uart_rxlen(void);

void cust_uart_read(uint8 *pbuf, uint8 len);

// uart_print
void cust_uart_print( char *str, ...);

void cust_debug_str( char *fmt, ...);

void wait_setting_mode( int time );


void periph_uart_init(void);

void periph_uart_open(void);

void cust_HalKeyConfig( bool interruptEnable, halKeyCBack_t cback);

void cust_HalKeyPoll(void);

void cust_bspLedInit(void);

uint8 update_sensor(void);

void relay_init(void);

void relay_turn_on(void);

void relay_turn_off(void);
#endif //CUST_FUNC_H

void beep_init(void);

void beep_on();

void beep_off();
