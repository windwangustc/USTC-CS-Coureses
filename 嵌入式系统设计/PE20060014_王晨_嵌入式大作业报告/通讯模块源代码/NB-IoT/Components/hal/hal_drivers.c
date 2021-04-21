/*************************************************************************
  \file					hal_drivers.c
  \brief
	This file contains the interface to the drivers service.

  \author
	www.shuncom.com
  \internal
  Revision History:
  	May. 14, 2018		Johnason - Created

**************************************************************************/

/*********************************************************************
 * 				INCLUDES
 */
#include "hal_drivers.h"
#include "timer.h"
#include "gpio-board.h"
/*********************************************************************
 * 				GLOBAL VARIABLES
 */
uint8_t hal_taskID;

/*********************************************************************
 * 				FUNCTIONS
 */

/*
 * @fn      hal_init
 *
 * @brief   Hal initialization function.
 *
 * @param   task_id		-	receiving tasks ID
 *
 * @return  None
 */
void hal_init( uint8_t task_id )
{
  /* Register task ID */
	hal_taskID = task_id;
}

/*
 * @fn      hal_processEvent
 *
 * @brief   Hal process event.
 *
 * @param   task_id		-	HAL tasks ID
 * @param	events		-	HAL events
 *
 * @return  The event status, maybe with event unprocessed.
 */
uint16_t hal_processEvent( uint8_t task_id, uint16_t events )
{
	(void)task_id;

	if(events & HAL_TIMER_EVENT)
	{
		TimerIrqHandler();
		return events ^ HAL_TIMER_EVENT;
	}

	if(events & HAL_GPIO_EVENT)
	{
		szPort_eventLoop();
		return events ^ HAL_GPIO_EVENT;
	}

	return 0;
}
