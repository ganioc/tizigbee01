#ifndef PERIPHERAL_H
#define PERIPHERAL_H

#ifdef __cplusplus
extern "C"
{
#endif
  
 /*********************************************************************
 * INCLUDES
 */
#include "zcl.h"

#define PERIPH_TEST_EVENT   0
  
#define PERIPH_TEST_CMD1    0
#define PERIPH_TEST_CMD2    1 
   
   
typedef struct
{
  osal_event_hdr_t hdr;
  uint8            cmd;
} peripheralCmd_t;


 /*
  * Initialization for the task
  */
extern void peripheral_Init( byte task_id );

/*
 *  Event Process for the task
 */
extern UINT16 peripheral_event_loop( byte task_id, UINT16 events );
  
byte peripheral_taskId(void);

void  peripheral_TestCmd(uint8 * pBuf);

#ifdef __cplusplus
}
#endif

#endif