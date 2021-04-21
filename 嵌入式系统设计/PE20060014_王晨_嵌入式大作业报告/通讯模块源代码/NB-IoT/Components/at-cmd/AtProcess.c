/*************************************************************************
  \file				AtProcess.c
  \brief
		Source file of the main loop for AT command module.

  \author
		Johnason
  \internal
  	  Revision History:
  		DEC. 18, 2017		Johnason - Created

**************************************************************************/

/*********************************************************************
 * 							INCLUDES
 */
#include <string.h>

#include "AtTerm.h"
#include "AtProcess.h"
#include "AtParams.h"

/*********************************************************************
 * 							CONSTANTS
 */
#define ATPROCESS_STATE_A         0
#define ATPROCESS_STATE_T         1
#define ATPROCESS_STATE_CMD       2

/*********************************************************************
 * 						LOCAL VAIRABLES
 */
static char cmdBuffer[ATPROCESS_MAX_AT_MSG_LEN];
static uint8_t state = ATPROCESS_STATE_A;
static uint8_t cmdLen = 0;
volatile static uint8_t gProcessStat = 0;
/*********************************************************************
 * 						LOCAL FUNCTIONS
 */
static void processCmd( char* cmdBuffer, uint8_t cmdLen )
{
    AtProcess_Status status = AtProcess_Status_CmdIdError;

    switch(cmdBuffer[0])
    {
        case '+':
            //process command
            status = AtParams_parseIncoming(&(cmdBuffer[1]), cmdLen-1);
            break;
    }

    if(status == AtProcess_Status_Success)
    {
        AtProcess_printSuccess();
    }
    else
    {
        AtProcess_printErrorCode(status);
    }

    return;
}

/*********************************************************************
 * 						GLOBAL FUNCTIONS
 */
void AtProcess_processingLoop( char ch )
{
    //if(AtParams_echoEnabled)
    //{
    //    AtTerm_putChar(ch);
    //}

    switch( state )
    {
        case ATPROCESS_STATE_A:
            memset(cmdBuffer, 0, ATPROCESS_MAX_AT_MSG_LEN);
            cmdLen = 0;
            if( (ch == 'a') || (ch == 'A') )
            {
                state = ATPROCESS_STATE_T;
            }
            break;
        case ATPROCESS_STATE_T:
            if( (ch == 't') || (ch == 'T') )
            {
                state = ATPROCESS_STATE_CMD;
            }
            break;
        case ATPROCESS_STATE_CMD:
        	if((ch == '\n') || (ch == '\r'))
        	{
        		//command entry complete, process it
        		//processCmd(cmdBuffer, cmdLen);
        		//state = ATPROCESS_STATE_A;
        		//cmdLen = 0;
        		gProcessStat = 1;
        	}
        	else
        	{
        		if(cmdLen < ATPROCESS_MAX_AT_MSG_LEN)
        		{
        			//add this char to the command string
        			cmdBuffer[cmdLen++] = ch;
        		}
        		else
        		{
        			//The command entered was too long we have overrun the command
        			//buffer
        			AtProcess_printErrorCode(AtProcess_Status_CmdIdError);
        			state = ATPROCESS_STATE_A;
        			cmdLen = 0;
        		}
        	}
        	break;
        default:
        	state = ATPROCESS_STATE_A;
        	cmdLen = 0;
        	break;
    }
}

void AtProcess_printSuccess(void)
{
    AtTerm_sendString("OK\r");
}

void AtProcess_printErrorCode(AtProcess_Status errorCode)
{
    AtTerm_sendStringUi16Value("Error ", (uint32_t) errorCode, 16);
    AtTerm_sendString("\r");
}

void AtProcess_process( void )
{
	//command entry complete, process it
    processCmd(cmdBuffer, cmdLen);
    state = ATPROCESS_STATE_A;
    cmdLen = 0;
    gProcessStat = 0;
}

uint8_t AtProcess_getState(void)
{
	return gProcessStat;
}

uint8_t AtProcess_cmdCmp( char* cmdStr, char* inComingCmdStr )
{
    uint32_t cmdIdx;

    for(cmdIdx = 0; cmdIdx < ATPROCESS_MAX_AT_CMD_LEN; cmdIdx++)
    {
        //end of command will be signified by:
        //space (for params), = (for Register), \0, \n, \r
        if( (inComingCmdStr[cmdIdx] == ' ')  ||
                (inComingCmdStr[cmdIdx] == '>')  ||
                (inComingCmdStr[cmdIdx] == '\0') ||
                (inComingCmdStr[cmdIdx] == '\n') ||
                (inComingCmdStr[cmdIdx] == '\r') )
        {
            break;
        }

        //end reg of read or write
        if( (cmdStr[cmdIdx] == inComingCmdStr[cmdIdx]) &&
                ( (cmdStr[cmdIdx] == '=') ||
                        (cmdStr[cmdIdx] == '?') ) )
        {
            break;
        }

        if( cmdStr[cmdIdx] != inComingCmdStr[cmdIdx] )
        {
            cmdIdx = 0;
            break;
        }
    }

    if(cmdIdx == ATPROCESS_MAX_AT_CMD_LEN)
    {
        cmdIdx = 0;
    }

    //remove white spaces
    while(cmdStr[cmdIdx] == ' ')
    {
        cmdIdx++;
    }

    return cmdIdx;
}

/***************************************************************************************************
 ***************************************************************************************************/
