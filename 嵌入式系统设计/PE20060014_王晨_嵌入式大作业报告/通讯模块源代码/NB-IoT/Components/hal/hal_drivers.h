/*************************************************************************
  \file					hal_drivers.h
  \brief
	This file contains the interface to the drivers service.

  \author
	www.shuncom.com
  \internal
  Revision History:
  	May. 14, 2018		Johnason - Created

**************************************************************************/

#ifndef __HAL_DRIVERS_H__
#define __HAL_DRIVERS_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include <stdint.h>
#include <stdlib.h>

/*********************************************************************
 * CONSTANTS
 */
#define HAL_TIMER_EVENT               0x0001
#define HAL_UART_CFG_EVENT			  0x0002
#define HAL_GPIO_EVENT				  0x0004
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * 		GLOBAL VARIABLES
 */
/* record the task id */
extern uint8_t hal_taskID;
/*********************************************************************
 * 		FUNCTIONS
 */
extern void hal_init(uint8_t taskid);

/*
 * Process the hardware abstract layer event
 */
extern uint16_t hal_processEvent( uint8_t task_id, uint16_t events );
/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* __HAL_DRIVERS_H__ */
