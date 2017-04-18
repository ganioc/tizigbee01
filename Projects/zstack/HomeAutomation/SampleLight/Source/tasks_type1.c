#include "tasks_type1.h"
#include "sensorph.h"
#include "sensor.h"
#include "air_sensor.h"
#include "gptimer.h"
#include "hw_memmap.h"
#include "cust_func.h"
#include "cust_timer.h"
#include "hal_led.h"

extern uint16  zclSmartGarden_AlarmStatus;
extern uint16  zclSmartGarden_Sensor_Enable;
extern uint16  zclSmartGarden_Air_Sensor;

void task_ph_sensor_update();
void task_temphumi_sensor_update();
void  task_air_light_update();
void  task_air_temphumi_update();


int task_ph_sensor_update_cb(uint8* buf, uint8 len);
int task_temphumi_sensor_update_cb(uint8* buf, uint8 len);
int  task_air_light_update_cb(uint8* buf, uint8 len);
int  task_air_temphumi_update_cb(uint8* buf, uint8 len);

sensorTaskHandler typeTaskArr[]={
#ifdef TYPE1    
  task_temphumi_sensor_update,
  task_ph_sensor_update,
#endif
#ifdef TYPE2
  task_air_light_update,
  task_air_temphumi_update
#endif
  
};

sensorTaskCallbackHandler typeTaskCallbackArr[]={
    
#ifdef TYPE1      
  task_temphumi_sensor_update_cb,
  task_ph_sensor_update_cb,
#endif
#ifdef TYPE2
  task_air_light_update_cb,
  task_air_temphumi_update_cb

#endif  
};
uint8 ph_counter = 0,soil_counter = 0, air_counter = 0;
uint8* FaultCounter[] = {
#ifdef TYPE1      
  &ph_counter,
  &soil_counter
#endif
    
#ifdef TYPE2
   &air_counter,
   &air_counter
#endif
};

uint8 typeTasksCnt = sizeof( typeTaskArr ) / sizeof( typeTaskArr[0] );
uint8 typeTasksIndex = 0;
uint8 cbTaskIndex = 0;
uint8 recvlen = 0;
uint8 readCount = 0;


uint8 recvbuf[128];
int   recvIndex=0;
int counter = 0;

void start_sensor_type_timer(){
  
    TimerIntEnable(GPTIMER2_BASE, GPTIMER_TIMA_TIMEOUT);
    TimerEnable(GPTIMER2_BASE, GPTIMER_A);
     recvlen = 0;
     readCount = 0;
     recvIndex = 0;
     counter = 0;
}

void stop_sensor_type_timer(){
      
      TimerIntDisable(GPTIMER2_BASE, GPTIMER_TIMA_TIMEOUT);
      TimerDisable(GPTIMER2_BASE, GPTIMER_A);
}

void task_ph_sensor_update(){
  // send a command out
    cust_debug_str("read ph");
    send_soil_ph(); 
    start_sensor_type_timer();
  
    
}

void task_temphumi_sensor_update(){
    send_soil_temp_humi();
    cust_debug_str("read soil");
    start_sensor_type_timer();
 
}


void  task_air_light_update(){
    send_air_light();
    start_sensor_type_timer();
   
}

void  task_air_temphumi_update(){
     send_air_temphumi();
     start_sensor_type_timer();
 
}


int task_ph_sensor_update_cb(uint8* buf, uint8 len){

  return read_soil_ph_cb( buf,len);
  
}

int task_temphumi_sensor_update_cb(uint8* buf, uint8 len){
  
  return read_soil_temp_humi_cb(buf,  len);
  
}

int  task_air_light_update_cb(uint8* buf, uint8 len){
    return read_air_light_cb( buf,  len);

}
int  task_air_temphumi_update_cb(uint8* buf, uint8 len){
    return read_air_temphumi_cb( buf,  len);
}
// check uart rx, TIMER 2 ISR function()
void check_uart_rx_in_timer(){
  if(GPTIMER_TIMA_TIMEOUT == TimerIntStatus(GPTIMER2_BASE, TRUE)){
    uint8 oldIndex = recvIndex;
    int  feedback;

    counter++;

    if(counter > 100 ){ // longer than 1 second, stop the timer, finished 
    // waiting
       (*FaultCounter[cbTaskIndex] )++;
        
        stop_sensor_type_timer();
        
        trigger_sensor_timer();
        
        debug_str("timeout");
        
        TimerIntClear(GPTIMER2_BASE,  GPTIMER_TIMA_TIMEOUT);
        
        return;

    }
    
    
    while(UARTCharsAvail(CUST_UART_PORT)){
        recvbuf[recvIndex++] = UARTCharGetNonBlocking(CUST_UART_PORT);
       
        if( recvIndex >= 128 )
          recvIndex = 0;
    }
   
    if( oldIndex != recvIndex){ // received something
      
        feedback = typeTaskCallbackArr[cbTaskIndex](recvbuf, recvIndex); 
      
        if( feedback == 0){
            // alarmstatus
            *FaultCounter[cbTaskIndex] = 0;
#ifdef  CUST_ALARM
            if(zclSmartGarden_AlarmStatus){
              HalLedStopBlink(HAL_LED_3);
              HalLedStopBlink(HAL_BEEP);
            }
#endif
            
#ifdef TYPE1
            zclSmartGarden_AlarmStatus &= (~(1 << cbTaskIndex));
            cust_debug_str("cbTaskIndex:%d", cbTaskIndex);
#endif
#ifdef TYPE2
            zclSmartGarden_AlarmStatus = 0;
#endif
            stop_sensor_type_timer();
            trigger_sensor_timer();
        }
        else if( feedback == -1){//len 
          //帧长度不完整，继续读
        }
        else if( feedback == -2){// wrong format, consider to retry
            (*FaultCounter[cbTaskIndex]) ++;
            stop_sensor_type_timer();
            trigger_sensor_timer();
        }
    }
    
     if(*FaultCounter[cbTaskIndex] >= 3){
      //处理读传感器错误
      *FaultCounter[cbTaskIndex] = 0;
#ifdef TYPE1
      zclSmartGarden_AlarmStatus |= (1 << cbTaskIndex);
#endif
#ifdef TYPE2
      zclSmartGarden_AlarmStatus |= (1 << 2);
#endif
     
    }
   
  }
  
    TimerIntClear(GPTIMER2_BASE,  GPTIMER_TIMA_TIMEOUT);
}

void runSensorTypeTask(){
#ifdef TYPE1
  if(0 == typeTasksIndex && !(zclSmartGarden_Sensor_Enable & 1)){
    zclSmartGarden_AlarmStatus &= (~1);
    typeTasksIndex ++;
     if(typeTasksIndex >= typeTasksCnt ){
        typeTasksIndex = 0;
    }
    trigger_sensor_timer();
    return;
  }
  if(1 == typeTasksIndex && !(zclSmartGarden_Sensor_Enable & 2)){
    zclSmartGarden_AlarmStatus &= (~2);
    typeTasksIndex ++;
     if(typeTasksIndex >= typeTasksCnt ){
        typeTasksIndex = 0;
    }
     trigger_sensor_timer();
    return;
  }

 
#endif  
    typeTaskArr[typeTasksIndex ++]();
    cbTaskIndex = typeTasksIndex - 1;
    cust_debug_str("typeTaskIndex:%d", typeTasksIndex);
     if(typeTasksIndex >= typeTasksCnt ){
        typeTasksIndex = 0;
    }
}


