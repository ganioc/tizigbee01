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
#include "MT_RPC.h"
   
#include "OSAL_Nv.h"
#include "OnBoard.h"
   
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
  	
	    
		cust_debug_str("srcEp:%d  cmdId :%d, param1:%x, param2:%x", 
			msg_ptr->srcEP,
			msg_ptr->cmdID,
			msg_ptr->param1,
			msg_ptr->param2
		);

		#if (ZG_BUILD_COORDINATOR_TYPE )
			//一些命令是只有协调器处理的
			debug_str("Test event received by coordinator");
		
		#else // router_type
			//一些命令是只有节点板才能处理的
			debug_str("Test event received by router");
		
		#endif
		
		
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
  
  if( events & PERIPH_RESET_EVENT){
    SystemReset();
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
  uint16 app_cmd;
  uint8 srcEp;
  // cmdId;
  uint16 param1;
  uint16 param2;
  peripheralCmd_t *pMsg;
  
    /* parse header */
  //cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;
  
  srcEp = *pBuf++;

  app_cmd = osal_build_uint16( pBuf );
  pBuf = pBuf + sizeof( uint16 );

  param1 = osal_build_uint16( pBuf );
  pBuf = pBuf + sizeof( uint16 );

  param2 = osal_build_uint16( pBuf );
  
  pMsg = (peripheralCmd_t *)osal_msg_allocate(sizeof(peripheralCmd_t));
  
  pMsg->hdr.event = PERIPH_TEST_EVENT;
  
  pMsg->srcEP = srcEp;
  pMsg->cmdID = app_cmd;
  pMsg->param1 = param1;
  pMsg->param2 = param2;
  
  osal_msg_send(peripheral_TaskID, (uint8*)pMsg);
  
//  if(pBuf[3] == 0x01 ){
//    osal_msg_send(peripheral_TaskID, (uint8*)pMsg);
//  }
//  else if(pBuf[3] == 0x02){
//    osal_msg_send(peripheral_TaskID, (uint8*)pMsg);
//  }
}
