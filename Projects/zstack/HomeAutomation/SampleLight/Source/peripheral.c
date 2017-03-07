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
#include "zcl_general.h"

#include "peripheral.h"
#include "iocc2538.h"
#include "cust_func.h"
#include "MT_RPC.h"

#include "OSAL_Nv.h"
#include "OnBoard.h"

#include "zcl_samplelight.h"

/*********************************************************************
 * GLOBAL VARIABLES
 */
byte peripheral_TaskID;
uint8 peripheralSeqNum=0;
extern uint8 zclSmartGarden_HeartbeatPeriod;

#ifdef ZCL_ON_OFF
afAddrType_t zclSample_DstAddr;
#endif

void peripheral_reset(void);
void peripheralCoordinator_ProcessIncomingCommand(peripheralCmd_t *msg_ptr);
void peripheralRouter_ProcessIncomingCommand(peripheralCmd_t *msg_ptr);

void peripheral_Init(byte task_id)
{
    peripheral_TaskID = task_id;

    debug_str("Peripheral_Init()");


    // Set destination address to indirect
    zclSample_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
    zclSample_DstAddr.endPoint = 0;
    zclSample_DstAddr.addr.shortAddr = 0;



}


void peripheral_ProcessIncomingCommand(peripheralCmd_t *msg_ptr)
{


    if(msg_ptr->hdr.event == PERIPH_TEST_EVENT)
    {
        cust_debug_str("srcEp:%d  cmdId :%d, param1:%x, param2:%x",
                       msg_ptr->srcEP,
                       msg_ptr->cmdID,
                       msg_ptr->param1,
                       msg_ptr->param2
                      );

//#if (ZG_BUILD_COORDINATOR_TYPE )
        //一些命令是只有协调器处理的
        peripheralCoordinator_ProcessIncomingCommand(msg_ptr);

//#else // router_type
        //一些命令是只有节点板才能处理的
        //peripheralRouter_ProcessIncomingCommand(msg_ptr);

//#endif

    }
    else
    {
        debug_str("Test event unrecognized");
    }
}
uint16 peripheral_event_loop(uint8 task_id, uint16 events)
{
    (void)task_id;  // Intentionally unreferenced parameter

    if(events & SYS_EVENT_MSG)
    {

        uint8 *msg_ptr = osal_msg_receive(task_id);

        if(msg_ptr != NULL)
        {
            peripheral_ProcessIncomingCommand((peripheralCmd_t *)msg_ptr);
            osal_msg_deallocate(msg_ptr);
        }
    }

    if(events & PERIPH_RESET_EVENT)
    {
        //SystemReset();
        peripheral_reset();


    }
    // Discard unknown events
    return 0;
}

byte peripheral_taskId(void)
{
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
void  peripheral_TestCmd(uint8 * pBuf)
{
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

    app_cmd = osal_build_uint16(pBuf);
    pBuf = pBuf + sizeof(uint16);

    param1 = osal_build_uint16(pBuf);
    pBuf = pBuf + sizeof(uint16);

    param2 = osal_build_uint16(pBuf);

    pMsg = (peripheralCmd_t *)osal_msg_allocate(sizeof(peripheralCmd_t));

    pMsg->hdr.event = PERIPH_TEST_EVENT;

    pMsg->srcEP = srcEp;
    pMsg->cmdID = app_cmd;
    pMsg->param1 = param1;
    pMsg->param2 = param2;

    osal_msg_send(peripheral_TaskID, (uint8*)pMsg);

}

void peripheral_reset(void)
{
    NLME_LeaveReq_t leaveReq;

    debug_str("_basicResetCB");
    // Set every field to 0
    osal_memset(&leaveReq, 0, sizeof(NLME_LeaveReq_t));

    // This will enable the device to rejoin the network after reset.
    leaveReq.rejoin = TRUE;

    // Set the NV startup option to force a "new" join.
    zgWriteStartupOptions(ZG_STARTUP_SET, ZCD_STARTOPT_DEFAULT_NETWORK_STATE);

    // Leave the network, and reset afterwards
    if(NLME_LeaveReq(&leaveReq) != ZSuccess)
    {
        // Couldn't send out leave; prepare to reset anyway
        ZDApp_LeaveReset(FALSE);
    }

}
/***
  srcEP=8
  开灯的命令集
  cmdID=0   toggle
  cmdID=1   on
  cmdID=2   off

  param1 = dstAddr

  param2 = dstEP, 8

  cmdID=


***/
void peripheralCoordinator_ProcessIncomingCommand(peripheralCmd_t *msg_ptr)
{

    ZStatus_t        status;
    zclReadCmd_t *readCmd;
    zclWriteCmd_t *writeCmd;
    zclReportCmd_t  *reportCmd;
    
    //uint16             attrId;

    debug_str("Test event received by ");

    if(msg_ptr->srcEP != 8)
    {
        debug_str("Unrecognized EP");
        return;
    }
    
    zclSample_DstAddr.addrMode =  (afAddrMode_t)Addr16Bit;
    zclSample_DstAddr.endPoint =          msg_ptr->param2;
    zclSample_DstAddr.addr.shortAddr = msg_ptr->param1;
    
     // toggle it
    if(msg_ptr->cmdID == 0)
    {
        //zclGeneral_SendOnOff_CmdToggle( SAMPLESW_ENDPOINT, &zclSampleSw_DstAddr, FALSE, 0 );


        status = zclGeneral_SendOnOff_CmdToggle(
                     8,
                     &zclSample_DstAddr,
                     true,
                     0
                 );

        cust_debug_str("status toggle %d", status);

    }
    else if(msg_ptr->cmdID == 1)
    {
        status = zclGeneral_SendOnOff_CmdOn(
                     8,
                     &zclSample_DstAddr,
                     true,
                     0
                 );

        cust_debug_str("son %d", status);

    }
    else if(msg_ptr->cmdID == 2)
    {
        status = zclGeneral_SendOnOff_CmdOff(
                     8,
                     &zclSample_DstAddr,
                     true,
                     0
                 );

        cust_debug_str("so %d", status);

    }    
    // read attribute
    else if(msg_ptr->cmdID == 0x10){

        ;

    }
    else if(msg_ptr->cmdID == 0x11){
        //read light status
        readCmd = osal_mem_alloc(sizeof(zclReadCmd_t) + sizeof(uint16));

        if(readCmd){
          
            readCmd->numAttr = 1;
            readCmd->attrID[0] = ATTRID_ON_OFF;

            zcl_SendRead(
                8, 
                &zclSample_DstAddr, 
                ZCL_CLUSTER_ID_GEN_ON_OFF, 
                readCmd, 
                ZCL_FRAME_CLIENT_SERVER_DIR,
                0, 
                peripheralSeqNum++);
            osal_mem_free(readCmd);
                
        }
    }
    else if(msg_ptr->cmdID == 0x12){
        // read HEARTBEAT_PERIOD uint8
        readCmd = osal_mem_alloc(sizeof(zclReadCmd_t) + sizeof(uint16));
        
        if(readCmd){
          
            readCmd->numAttr = 1;
            readCmd->attrID[0] = ATTRID_BASIC_SMARTGARDEN_HEARTBEAT_PERIOD;

            zcl_SendRead(
                8, 
                &zclSample_DstAddr, 
                ZCL_CLUSTER_ID_GEN_BASIC, 
                readCmd, 
                ZCL_FRAME_CLIENT_SERVER_DIR,
                0, 
                peripheralSeqNum++);
            osal_mem_free(readCmd);
                
        }
        
    }
    else if(msg_ptr->cmdID == 0x13){
        // set HEARTBEAT_PERIOD uint8 to 30
        uint8 newPeriod = 30;
        writeCmd = osal_mem_alloc(sizeof(zclWriteCmd_t) + sizeof(zclWriteRec_t));
        
        if(writeCmd){
          
            writeCmd->numAttr = 1;
            writeCmd->attrList[0].attrID = ATTRID_BASIC_SMARTGARDEN_HEARTBEAT_PERIOD;
            writeCmd->attrList[0].dataType = ZCL_DATATYPE_UINT8;
            writeCmd->attrList[0].attrData = &newPeriod;

            zcl_SendWrite(
                8, 
                &zclSample_DstAddr, 
                ZCL_CLUSTER_ID_GEN_BASIC, 
                writeCmd, 
                ZCL_FRAME_CLIENT_SERVER_DIR,
                0, 
                peripheralSeqNum++);
            
            osal_mem_free(writeCmd);
                
        }
    }
    else if(msg_ptr->cmdID == 0x14){
        // set HEARTBEAT_PERIOD uint8 to 15
        uint8 newPeriod = 15;
        writeCmd = osal_mem_alloc(sizeof(zclWriteCmd_t) + sizeof(zclWriteRec_t));
        
        if(writeCmd){
          
            writeCmd->numAttr = 1;
            writeCmd->attrList[0].attrID = ATTRID_BASIC_SMARTGARDEN_HEARTBEAT_PERIOD;
            writeCmd->attrList[0].dataType = ZCL_DATATYPE_UINT8;
            writeCmd->attrList[0].attrData = &newPeriod;

            zcl_SendWrite(
                8, 
                &zclSample_DstAddr, 
                ZCL_CLUSTER_ID_GEN_BASIC, 
                writeCmd, 
                ZCL_FRAME_CLIENT_SERVER_DIR,
                0, 
                peripheralSeqNum++);
            
            osal_mem_free(writeCmd);
                
        }
    }
    else if(msg_ptr->cmdID == 0x20){
        // send heartbeat report
        
        reportCmd = osal_mem_alloc(sizeof(zclReportCmd_t ) + sizeof(zclReport_t));
        
        if(reportCmd){
          
            reportCmd->numAttr = 1;
            reportCmd->attrList[0].attrID = ATTRID_BASIC_SMARTGARDEN_HEARTBEAT_PERIOD;
            reportCmd->attrList[0].dataType = ZCL_DATATYPE_UINT8;
            reportCmd->attrList[0].attrData = &zclSmartGarden_HeartbeatPeriod;

            zcl_SendReportCmd(
                8, 
                &zclSample_DstAddr, 
                ZCL_CLUSTER_ID_GEN_BASIC, 
                reportCmd, 
                ZCL_FRAME_CLIENT_SERVER_DIR,
                0, 
                peripheralSeqNum++);
            
            osal_mem_free(reportCmd);
                
        }
    }
    
}


void peripheralRouter_ProcessIncomingCommand(peripheralCmd_t *msg_ptr)
{

    ZStatus_t status;
    debug_str("Test event received by router");

    if(msg_ptr->srcEP != 8)
    {
        debug_str("Unrecognized EP");
        return;
    }
   

}
