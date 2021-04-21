/*************************************************************************
  \file				osal_nv.h
  \brief
	Header file to defines the osal non-volatile memory functions.

  \author
	www.shuncom.com
  \internal
  Revision History:
  	Dec. 13, 2017		Johnason - Created

**************************************************************************/

#ifndef __OSAL_NV_H__
#define __OSAL_NV_H__

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

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Initialize NV service
 */
extern void osal_nv_init( void *p );

/*
 * Initialize an item in NV
 */
extern uint8_t osal_nv_item_init( uint16_t id, uint16_t len, void *buf );

/*
 * Read an NV attribute
 */
extern uint8_t osal_nv_read( uint16_t id, uint16_t offset, uint16_t len, void *buf );

/*
 * Write an NV attribute
 */
extern uint8_t osal_nv_write( uint16_t id, uint16_t offset, uint16_t len, void *buf );

/*
 * Get the length of an NV item.
 */
extern uint16_t osal_nv_item_len( uint16_t id );

/*
 * Delete an NV item.
 */
extern uint8_t osal_nv_delete( uint16_t id, uint16_t len );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* __OSAL_NV_H__ */
