/*************************************************************************
  \file				osal_transparent.c
  \brief
	This file implements transparent transport application, and contains
		all the settings and other functions that the user should set
		and change.

  \author
	www.shuncom.com
  \internal
  Revision History:
  	May. 11, 2018		Johnason - Created

**************************************************************************/
/*********************************************************************
 * 					INCLUDES
 */

#include "ZComDef.h"
#include "OSAL.h"

#include "transparent.h"
#include "hal_drivers.h"
/*********************************************************************
 * 							CONSTANTS
 */
/* The count of the tasks, must equal to the count of the tasksArr */
#define APP_TASK_NUM	3
/*********************************************************************
 * 					GLOBAL VARIABLES
 */

/* The order in this table must be identical to the task initialization
 * 		calls below in osalInitTask.
 */
const pTaskEventHandlerFn tasksArr[APP_TASK_NUM] = {
//	macTask_processEvent,
	hal_processEvent,
	transparent_event_loop
};

const uint8_t tasksCnt = APP_TASK_NUM;
uint16_t tasksEventsIns[APP_TASK_NUM] = {0};
uint16_t *tasksEvents;

/*********************************************************************
 * 						FUNCTIONS
 *********************************************************************/

/*********************************************************************
 * @fn      osalInitTasks
 *
 * @brief   This function invokes the initialization function for each task.
 *
 * @param   void
 *
 * @return  none
 */
void osalInitTasks( void )
{
	uint8_t taskID = 0;

	tasksEvents = tasksEventsIns;
	hal_init( taskID++ );
	transparent_Init( taskID );
}

/*********************************************************************
*********************************************************************/
