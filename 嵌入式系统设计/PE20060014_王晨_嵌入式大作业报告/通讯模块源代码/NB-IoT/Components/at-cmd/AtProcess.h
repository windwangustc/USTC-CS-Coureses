/*************************************************************************
  \file				AtProcess.h
  \brief
		Header file of the main loop for AT command module.

  \author
		Johnason
  \internal
  	  Revision History:
  		DEC. 18, 2017		Johnason - Created

**************************************************************************/

#ifndef __AT_PROCESS__
#define __AT_PROCESS__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#define ATPROCESS_MAX_AT_MSG_LEN	64
#define ATPROCESS_MAX_AT_CMD_LEN	9

/// \brief AtProcess and error codes
typedef enum
{
    AtProcess_Status_Success       = 0, ///Success
    AtProcess_Status_CmdIdError  = 1, ///Param error
    AtProcess_Status_CmdLenError   = 2, ///Param error
    AtProcess_Status_ParamError     = 3, ///Memory Error
    AtProcess_Status_MemoryError     = 4, ///Memory Error
    AtProcess_Status_EasyLinkError      = 5, ///Tx Error
} AtProcess_Status;

/// \brief AT command processing function type
typedef AtProcess_Status (*AtCmdFnx)(char* paramStr, uint32_t pramLen);

/// \brief AT command processing type, matching command string to processing
/// function
typedef struct {
    char cmdStr[ATPROCESS_MAX_AT_CMD_LEN];
    AtCmdFnx cmdFxn;
} AtCommand_t;

//*****************************************************************************
//
//! \brief Processing loop for preading and parsing UART characters
//!
//! This function parses in coming UART characture to identify the "AT" start
//! of frame and the "/r"/CR end of frame. The command frame is then parsed to
//! identify the command type before passing to the appropriate sub module
//!
//! \param none
//!
//! \return none
//
//*****************************************************************************
extern void AtProcess_processingLoop( char ch );

//*****************************************************************************
//
//! \brief Print error code to AT terminal
//!
//! This function identifies the atProcess_Status error code and prints the
//! ASCI to the AT terminal
//!
//! \param errorCode the atProcess_Status error code
//!
//! \return none
//
//*****************************************************************************
extern void AtProcess_printErrorCode(AtProcess_Status errorCode);

//*****************************************************************************
//
//! \brief Print success to AT terminal
//!
//! This function Prints success to AT terminal
//!
//! \param none
//!
//! \return none
//
//*****************************************************************************
extern void AtProcess_printSuccess(void);

//*****************************************************************************
//
//! \brief Compare AT commands
//!
//! This function compares 2 AT commands and returns 0 for no match or index to
//! paramters if they do match
//!
//! \param cmd1 string containing first command
//! \param cmd2 string containing second command
//!
//! \return index to paramters if commands match, 0 if not
//
//*****************************************************************************
extern uint8_t AtProcess_cmdCmp( char* cmd1, char* cmd2 );

void AtProcess_process( void );

uint8_t AtProcess_getState(void);
/***************************************************************************************************
 ***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* __AT_PROCESS__ */
