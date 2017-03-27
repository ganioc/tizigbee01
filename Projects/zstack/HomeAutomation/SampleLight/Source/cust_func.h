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
#define CUST_UART0_PORT        UART1_BASE
#define CUST_UART_BAUDRATE     38400

#define PERIPH_UART_BUADRATE   9600


#define SETTING_MODE_CODE   "ruff"

#define MAX_WAIT_TIME   6000/10

#define BUFFER_LENGTH   128

#define UNUSED_VARIABLE(X)  ((void)(X))



//#define ZCL_CLUSTER_ID_SMARTGARDEN                           0x2000


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
#define ATTRID_BASIC_SMARTGARDEN_CHIPID_ACK                   0x4010
#define ATTRID_BASIC_SMARTGARDEN_ATTR_LIST                      0x4011
#define ATTRID_BASIC_SMARTGARDEN_DEVICE_TYPE                  0x4012
#define ATTRID_BASIC_SMARTGARDEN_TEMP_INTENSITY                0x4013
#define ATTRID_BASIC_SMARTGARDEN_HUMI_INTENSITY                 0x4014 

//Coordinator cmdID
#define FUNC_TURN_OFF_LIGHT_CMD                        0x0
#define FUNC_TURN_ON_LIGHT_CMG                          0x1
#define FUNC_LIGHT_TOGGLE_CMD                            0x2
//#define FUNC_TURN_OFF_IRRIGATE_CMD                  0x3
//#define FUNC_TURN_ON_IRRIGATE_CMD                    0x4
#define FUNC_IDENTIFYING_CMD                               0x3

#define FUNC_READ_ATTR_CMD                                          0x10
#define FUNC_READ_LIGHT_STA_CMD                                0x11
#define FUNC_READ_HEATBEAT_PERIOD_CMD                   0x12
#define FUNC_SET_HEARTBEAT_PERIOD_30_CMD                0x13
#define FUNC_SET_HEARTBEAT_PERIOD_15_CMD                0x14
#define FUNC_SEND_HEARTBEAT_REPORT_CMD                  0x20
#define FUNC_READ_SOIL_TEMP_CMD                         0x30
#define FUNC_READ_SOIL_HUMI_CMD                         0x31
#define FUNC_READ_SOIL_PH_CMD                           0x32
#define FUNC_READ_IRRIGATE_CMD                            0x40
#define FUNC_TURN_ON_IRRIGATE_CMD                          0x41
#define FUNC_TURN_OFF_IRRIGATE_CMD                          0x42
#define FUNC_TURN_ON_PERMITJOINING_CMD                          0x43

//SENSOR ERR STATE
#define ZCLSMARTGARDEN_STATE_ERR_TEMP_HUMI   0x01
    
#define ZCLSMARTGARDEN_STATE_ERR_PH   0x02

#define ZCLSMARTGARDEN_STATE_ENV_SENSOR_ERR  0x04

// default key pb3
#define KEY_DEFAULT_BASE         GPIO_B_BASE
#define KEY_DEFAULT_PIN          GPIO_PIN_3      //!< PB3
//default relay
#define ELEC_TOGGLE              GPIO_A_BASE
#define ELEC_TOGGLE_PIN6         GPIO_PIN_6
#define ELEC_TOGGLE_PIN7         GPIO_PIN_7

#define ELEC_RELAY0_ON           GPIO_PIN_6
#define ELEC_RELAY1_ON           GPIO_PIN_7
#define ELEC_RELAY0_OFF          ~GPIO_PIN_6
#define ELEC_RELAY1_OFF          ~GPIO_PIN_7    

//default beep
#define BEEP_BASE               GPIO_C_BASE
#define BEEP_PIN                GPIO_PIN_1
#define BEEP_ON                 GPIO_PIN_1
#define BEEP_OFF                (GPIOPinRead(GPIO_A_BASE, GPIO_PIN_ALL) & (~GPIO_PIN_1)) 

#define GPIO_PIN_ALL            0xFF

typedef enum switch_port{
  port0,
  port1,
  port2,
  port3,
}rs485_port;

//S1,S0 sensor switch
#define RS485_SWITCH_BASE       GPIO_C_BASE
#define S0_PIN                  GPIO_PIN_6
#define S1_PIN                  GPIO_PIN_7

#define MAX_DEFAULT_KEY_COUNTER  30

// Led
#define LED_BASE          GPIO_B_BASE
#define LED_PIN1          GPIO_PIN_4  
#define LED_PIN2          GPIO_PIN_5  

// device type
#define DEVICE_TYPE_1           1
#define DEVICE_TYPE_2           2 


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

void cust_uart_write(uint8 *pbuf, uint8 len);

void cust_uart0_write(uint8 *pbuf, uint8 len);

uint8 cust_uart_rxlen(void);

// uart_print
void cust_uart_print( char *str, ...);

void cust_debug_str( char *fmt, ...);

void wait_setting_mode( int time );


void periph_uart_init(void);

void periph_uart_open(void);

void cust_HalKeyConfig( bool interruptEnable, halKeyCBack_t cback);

void cust_HalKeyPoll(void);

void cust_bspLedInit(void);



void update_soil_ph_sensor(void);

void update_soil_temphumi_sensor(void);

//void soil_alarm_sign(void);

void update_air_light(void);

void update_air_temphumi(void);

//void air_alarm_sign(void);


//relay define
void relay_init(void);

uint32 read_relay0_state(void);

void relay0_turn_on(void);

void relay1_turn_on(void);

void relay0_turn_off(void);

void relay1_turn_off(void);
#endif //CUST_FUNC_H


//beep define
void beep_init(void);

void beep_on(void);

void beep_off(void);

//rs485 switch
void sensor_switch_init(void);

void open_port0(void);

void open_port1(void);

void open_port2(void);

void open_port3(void);

void sensor_switch(uint8 port);