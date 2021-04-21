/*************************************************************************
  \file				osal.h
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

#ifndef __OSAL_H__
#define __OSAL_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include <stdlib.h>
#include <stdint.h>
/*********************************************************************
 * CONSTANTS
 */
#define TASK_NO_TASK      0xFF
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*
 * Event handler function prototype
 */
typedef uint16_t (*pTaskEventHandlerFn)( uint8_t task_id, uint16_t event );
/*********************************************************************
 * GLOBAL VARIABLES
 */
/* the global task information, need application provide */
extern const pTaskEventHandlerFn tasksArr[];

/*********************************************************************
 * FUNCTIONS
 */

/*** Task Synchronization  ***/

  /*
   * Set a Task Event
   */
  extern uint8_t osal_set_event( uint8_t task_id, uint16_t event_flag );

  /*
   * Clear a Task Event
   */
  extern uint8_t osal_clear_event( uint8_t task_id, uint16_t event_flag );

/*** Task Management  ***/

  /*
   * Initialize the Task System
   */
  extern uint8_t osal_init_system( void );

  /*
   * Call each of the tasks initialization functions.
   */
  extern void osalInitTasks( void );

  /*
   * System Processing Loop
   */
  extern void osal_start_system( void );

  /*
   * One Pass Through the OSAL Processing Loop
   */
  extern void osal_run_system( void );
/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* __OSAL_H__ */
