/*********************************************************************
 * INCLUDES
 */
#include "ZComDef.h"
#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "MT_SYS.h"
#include "DebugTrace.h"

#include "peripheral.h"
#include "iocc2538.h"
#include "cust_func.h"

/*********************************************************************
 * GLOBAL VARIABLES
 */
byte peripheral_TaskID;
uint8 peripheralSeqNum;

void peripheral_Init( byte task_id )
{
  peripheral_TaskID = task_id;
  
  debug_str("Into peripheral_Init()");

}
void peripheral_ProcessIncomingCommand(peripheralCmd_t *msg_ptr){
  
  if(msg_ptr->hdr.event == PERIPH_TEST_EVENT )
  {
    debug_str("Test event received");
  }
  else{
    debug_str("Test event unrecognized");
  }
}
uint16 peripheral_event_loop( uint8 task_id, uint16 events ){
  (void)task_id;  // Intentionally unreferenced parameter
  
  if ( events & SYS_EVENT_MSG ){
    
    uint8 *msg_ptr = osal_msg_receive(task_id);
    
    if (msg_ptr != NULL)
    {
      peripheral_ProcessIncomingCommand((peripheralCmd_t *)msg_ptr);
      osal_msg_deallocate(msg_ptr);
    }
  }
  // Discard unknown events
  return 0;
}

byte peripheral_taskId(void){
  return peripheral_TaskID;
}

/**
Default pBuf size is: 10 byte
len:1
cmd0: 1
cmd1: 1
srcEP: 1
cmdId: 2
param: 2
param: 2
**/
void  peripheral_TestCmd(uint8 * pBuf){
  
  peripheralCmd_t *pMsg;
  
  pMsg = (peripheralCmd_t *)osal_msg_allocate(sizeof(peripheralCmd_t));
  
  pMsg->hdr.event = PERIPH_TEST_EVENT;
  
  pMsg->cmd = pBuf[3];
  
  osal_msg_send(peripheral_TaskID, (uint8*)pMsg);
  
//  if(pBuf[3] == 0x01 ){
//    osal_msg_send(peripheral_TaskID, (uint8*)pMsg);
//  }
//  else if(pBuf[3] == 0x02){
//    osal_msg_send(peripheral_TaskID, (uint8*)pMsg);
//  }
}
