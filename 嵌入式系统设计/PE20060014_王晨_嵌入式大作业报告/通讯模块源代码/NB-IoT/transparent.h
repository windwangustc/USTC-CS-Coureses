/*************************************************************************
  \file				transparent.h
  \brief
	This file implements transparent transport application.

  \author
	www.shuncom.com
  \internal
  Revision History:
  	May. 11, 2018		Johnason - Created

**************************************************************************/
#ifndef __TRANSPARENT_H__
#define __TRANSPARENT_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * 						INCLUDES
 */
#include "uart.h"
/*********************************************************************
 * 						CONSTANTS
 */
#define  TX_RX_MSG_NUM  256
#define  APN_LEN        100
#define  IP_LEN         80
#define  IMEI_LEN       15

#define  STATE_ERROR   (-1)
#define  STATE_SUCCESS (0)
// Application Events
// Application Events
#define TRANSPARENT_UART_RX_EVT        0x0001
#define TRANSPARENT_MAIN_LOOP_EVT		0x0002
//#define CONCENTRATOR_SPI_NOEMPTY		0x0004
#define TRANSPARENT_CONFIG_EVT		   0x0008
#define TRANSPARENT_UART2_RX_EVT       0x0010
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * 								VARIABLES
 */


/*********************************************************************
 * FUNCTIONS
 */

/*test info */

//typedef enum eDeviceState
//{
//    DEVICE_STATE_CHECK = 0,
//    DEVICE_STATE_CFUN,
//    DEVICE_STATE_IMEI,
//    DEVICE_STATE_IMSI,
//    DEVICE_STATE_REBOOT,
//	DEVICE_STATE_IDLE
//}DeviceState;
 /*
  * Initialization for the application task
  */
extern void transparent_Init( uint8_t task_id );

/*
 *  Event Process for the application task
 */
extern uint16_t transparent_event_loop( uint8_t task_id, uint16_t events );

void Next_CMD_EVENT(void);
void Heart_Beat_EVENT(void);
void CCLK_Beat_EVENT(void);
void OnAppLedimerEvent(void);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* __TRANSPARENT_H__ */
