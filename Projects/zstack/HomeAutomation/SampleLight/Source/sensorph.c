#include "cust_func.h"
#include "OSAL_Memory.h"
#include "OSAL.h"
#include "hal_types.h"
#include "sensor.h"
#include "sensorph.h"

uint8 read_addr_cmd[READ_CMD_LEN] = {0x00, 0x20};
uint8 write_addr_cmd[WRITE_CMD_LEN] = {0x00, 0x10, SLAVE_ADDR_PH};
uint8 read_data_cmd[READ_DATA_LEN] = {SLAVE_ADDR_PH, 0x03, 0x00, 0x00, 0x00, 0x01};

bool write_dev_addr()
{
  uint8 addr;
  uint16 crc = Cal_Crc16( write_addr_cmd, WRITE_CMD_LEN - 2);
  write_addr_cmd[WRITE_CMD_LEN - 2] = crc & 0xFF;
  write_addr_cmd[WRITE_CMD_LEN - 1] = (crc >> 8) & 0xFF;
  
  cust_uart_write(write_addr_cmd, WRITE_CMD_LEN);
  uint8 recvlen = 0;
  while(!(recvlen = cust_uart_rxlen()));
  uint8 *recvdata = NULL;
  recvdata = (uint8 *)osal_mem_alloc(recvlen);
  if(recvdata){
    cust_uart_read(recvdata, recvlen);
    if(!Cal_Crc16(recvdata, recvlen)){
      osal_mem_free(recvdata);
      cust_delay_2ms();
      addr = read_dev_addr();
      if(addr == SLAVE_ADDR_PH){
        return TRUE;
      }
      else{
        osal_mem_free(recvdata);
        return FALSE;
      }
    }else{
      osal_mem_free(recvdata);
      return FALSE;
    }
  }   
   return  FALSE;
}

uint8 read_dev_addr()
{
  uint8 addr;
  uint16 crc = Cal_Crc16( read_addr_cmd, 2);
  read_addr_cmd[READ_CMD_LEN - 2] = crc & 0xFF;
  read_addr_cmd[READ_CMD_LEN - 1] = (crc >> 8) & 0xFF;
  
  cust_uart_write(read_addr_cmd, READ_CMD_LEN);
  uint8 recvlen = 0;
  while(!(recvlen = cust_uart_rxlen()));
  uint8 *recvdata = NULL;
  recvdata = (uint8 *)osal_mem_alloc(recvlen);
  if(recvdata){
    cust_uart_read(recvdata, recvlen);
    if(!Cal_Crc16(recvdata, recvlen)){
      addr = recvdata[2];
      osal_mem_free(recvdata);
      return addr;
    }else{
      osal_mem_free(recvdata);
      return ERR_CODE;
    }
  }   
   return  ERR_CODE;
  
}

double Read_Soil_Ph()
{
  double ph = 0.0;
  uint16 crc = Cal_Crc16( read_data_cmd, READ_DATA_LEN - 2);
  read_data_cmd[READ_DATA_LEN - 2] = crc & 0xFF;
  read_data_cmd[READ_DATA_LEN - 1] = (crc >> 8) & 0xFF;
  
  cust_uart_write(read_data_cmd, READ_DATA_LEN);
  uint8 recvlen = 0;
  while(!(recvlen = cust_uart_rxlen())){}
  uint8 *recvdata = NULL;
  recvdata = osal_mem_alloc(recvlen);
  if(recvdata){
    cust_uart_read(recvdata, recvlen);
    if(!Cal_Crc16( recvdata, recvlen)){
      ph = (double)(((recvdata[3] * 256 + recvdata[4]) - 4) / (16.0 / 14.0) / 100);
      return ph;
    }else{
      osal_mem_free(recvdata);
      return ERR_CODE;
    }
  }else{
    return ERR_CODE;
  }
}