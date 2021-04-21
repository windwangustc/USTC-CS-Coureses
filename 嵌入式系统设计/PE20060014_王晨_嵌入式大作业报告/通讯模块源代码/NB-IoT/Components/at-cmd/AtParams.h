/*************************************************************************
  \file				AtParams.h
  \brief
		Header file of the parameters command interface for AT command module.

  \author
		Johnason
  \internal
  	  Revision History:
  		DEC. 18, 2017		Johnason - Created

**************************************************************************/

#ifndef __AT_PARAMS__
#define __AT_PARAMS__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * 							INCLUDES
 */
#include "AtProcess.h"

/*********************************************************************
 * 							CONSTANTS
 */

/*
 * Parses incoming AT parameter commands ("ATP"), these commands
 * 		are of the format "ATPxx=yy"/"ATPxx?" respectively
 */
AtProcess_Status AtParams_parseIncoming( char *param, uint8_t len );

/**********************************************************************
 **********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* __AT_PARAMS__ */
