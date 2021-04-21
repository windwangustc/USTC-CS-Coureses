/*************************************************************************
  \file				AtTerm.c
  \brief
		Source file of the UART interface for AT command module.

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
#include <stdio.h>

#include "Board.h"

#include "AtTerm.h"
#include "AtProcess.h"

int32_t AtTerm_init(uint8_t mode)
{
	if(mode)
	{
		// switch to at command mode
	}
	else
	{
		// switch back to normal mode
	}
    return 0;
}

void AtTerm_putChar(char ch)
{
    UartSend(BOARD_DEBUG_UART_INSTANCE, (const uint8_t *)(&ch), 1);
}

void AtTerm_sendStringUi8Value(char *string, uint8_t value, uint8_t format)
{
    char strVal[128] = { 0 };

    if (format == 10)
    {
        sprintf(strVal, "%s%02d", (char*) string, value);
    } else
    {
        sprintf(strVal, "%s%02x", (char*) string, value);
    }

    UartSend(BOARD_DEBUG_UART_INSTANCE, (const uint8_t *)strVal, strlen(strVal));
}

void AtTerm_sendStringI8Value(char *string, int8_t value, uint8_t format)
{
    char strVal[128] = { 0 };

    if (format == 10)
    {
        sprintf(strVal, "%s%02d", (char*) string, value);
    } else
    {
        sprintf(strVal, "%s%02x", (char*) string, value);
    }

    UartSend(BOARD_DEBUG_UART_INSTANCE, (const uint8_t *)strVal, strlen(strVal));
}

void AtTerm_sendStringUi16Value(char *string, uint16_t value, uint8_t format)
{
    char strVal[128] = { 0 };

    if (format == 10)
    {
        sprintf(strVal, "%s%04d", (char*) string, value);
    } else
    {
        sprintf(strVal, "%s%04x", (char*) string, value);
    }

    UartSend(BOARD_DEBUG_UART_INSTANCE, (const uint8_t *)strVal, strlen(strVal));
}

void AtTerm_sendStringI16Value(char *string, int16_t value, uint8_t format)
{
    char strVal[128] = { 0 };

    if (format == 10)
    {
        sprintf(strVal, "%s%04d", (char*) string, value);
    } else
    {
        sprintf(strVal, "%s%04x", (char*) string, value);
    }

    UartSend(BOARD_DEBUG_UART_INSTANCE, (const uint8_t *)strVal, strlen(strVal));
}

void AtTerm_sendStringUi32Value(char *string, uint32_t value, uint8_t format)
{
    char strVal[128] = { 0 };

    if (format == 10)
    {
        sprintf(strVal, "%s%08u", (char*) string, (unsigned int)value);
    } else
    {
        sprintf(strVal, "%s%08x", (char*) string, (unsigned int)value);
    }

    UartSend(BOARD_DEBUG_UART_INSTANCE, (const uint8_t *)strVal, strlen(strVal));
}

void AtTerm_sendStringI32Value(char *string, int32_t value, uint8_t format)
{
    char strVal[128] = { 0 };

    if (format == 10)
    {
        sprintf(strVal, "%s%08d", (char*) string, (int)value);
    } else
    {
        sprintf(strVal, "%s%08x", (char*) string, (unsigned int)value);
    }

    UartSend(BOARD_DEBUG_UART_INSTANCE, (const uint8_t *)strVal, strlen(strVal));
}

void AtTerm_sendString(char *string)
{
    uint32_t len = strlen(string);
    UartSend(BOARD_DEBUG_UART_INSTANCE, (const uint8_t *)string, len);
}
