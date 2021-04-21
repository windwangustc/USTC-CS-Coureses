/*************************************************************************
  \file				osal.c
  \brief
	This API allows the software components to be written independently
		of the specifics of the operatng system, kernel, of tasking
		environment (including control loops or connect-to-interrupt systems).

  \author
	www.shuncom.com
  \internal
  Revision History:
  	May. 11, 2018		Johnason - Created

**************************************************************************/

/*********************************************************************
 * 							INCLUDES
 */
#include "board.h"
#include "osal.h"
#include "ZComDef.h"

/*********************************************************************
 * 							CONSTANTS
 */

/*********************************************************************
 * 							MACROS
 */

/*********************************************************************
 * 							TYPEDEFS
 */


/*********************************************************************
 * 						GLOBAL VARIABLES
 */

/*********************************************************************
 * 						LOCAL VARIABLES
 */

// Index of active task
static uint8_t activeTaskID = TASK_NO_TASK;
extern uint8_t tasksCnt;
extern uint16_t *tasksEvents;
/*********************************************************************
 * 						LOCAL FUNCTIONS
 */

/*********************************************************************
 * 						API FUNCTIONS
 */
/*
 * @fn      osal_set_event
 *
 * @brief   This function is called to set the event flags for a task.
 * 				The event passed in is OR'd into the task's event variable.
 *
 * @param   task_id		-	receiving tasks ID
 * @param   event_flag	-	what event to set
 *
 * @return  SUCCESS, MSG_BUFFER_NOT_AVAIL, FAILURE, INVALID_TASK
 */
uint8_t osal_set_event( uint8_t task_id, uint16_t event_flag )
{
	if ( task_id < tasksCnt )
	{
		BoardDisableIrq();
		tasksEvents[task_id] |= event_flag;  // Stuff the event bit(s)
		BoardEnableIrq();
		return ( SUCCESS );
	}
	else
	{
		return ( INVALID_TASK );
	}
}

/*
 * @fn      osal_clear_event
 *
 * @brief	This function is called to clear the event flags for a task.
 * 				The event passed in is masked out of the task's event variable.
 *
 * @param   task_id		-	receiving tasks ID
 * @param   event_flag	-	what event to clear
 *
 * @return  SUCCESS, INVALID_TASK
 */
uint8_t osal_clear_event( uint8_t task_id, uint16_t event_flag )
{
	if ( task_id < tasksCnt )
	{
		BoardDisableIrq();
		tasksEvents[task_id] &= ~(event_flag);   // Clear the event bit(s)
		BoardEnableIrq();
		return ( SUCCESS );
	}
	else
	{
		return ( INVALID_TASK );
	}
}

/*
 * @fn      osal_init_system
 *
 * @brief	This function initializes the "task" system by creating the
 * 				tasks defined in the task table.
 *
 * @param   void
 *
 * @return  SUCCESS
 */
uint8_t osal_init_system( void )
{
	// Initialize the system tasks.
	osalInitTasks();
	return ( SUCCESS );
}

/*
 * @fn      osal_start_system
 *
 * @brief	This function is the main loop function of the task system.
 * 				This Function doesn't return.
 *
 * @param   void
 *
 * @return  none
 */
void osal_start_system( void )
{
	for(;;)  // Forever Loop
	{
		osal_run_system();
	}
}

/*
 * @fn      osal_run_system
 *
 * @brief	This function will make one pass through the OSAL taskEvents
 * 				table and call the task_event_processor() function for
 * 				the first task that is found with at least one event
 * 				pending. If there are no pending events (all tasks), this
 * 				function puts the processor into Sleep.
 *
 * @param   void
 *
 * @return  none
 */
void osal_run_system( void )
{
	uint8_t idx = 0;

	do {
		if (tasksEvents[idx])  // Task is highest priority that is ready.
		{
			break;
		}
	} while (++idx < tasksCnt);

	if (idx < tasksCnt)
	{
		uint16_t events;
		BoardDisableIrq();
		events = tasksEvents[idx];
		tasksEvents[idx] = 0;  // Clear the Events for this task.
		BoardEnableIrq();

		activeTaskID = idx;
		events = (tasksArr[idx])( idx, events );
		activeTaskID = TASK_NO_TASK;

		BoardDisableIrq();
		tasksEvents[idx] |= events;  // Add back unprocessed events to the current task.
		BoardEnableIrq();
	}
}
/*********************************************************************
 */
