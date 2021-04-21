/*************************************************************************
  \file				AtTerm.h
  \brief
		Header file of the UART interface for AT command module.

  \author
		Johnason
  \internal
  	  Revision History:
  		DEC. 18, 2017		Johnason - Created

**************************************************************************/

#ifndef __AT_TERM__
#define __AT_TERM__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

//*****************************************************************************
//
//! \brief Initializes the AT terminal
//!
//! This function configures the UART for use as an AT Terminal
//!
//! \param none
//!
//! \return UART specific status, 0 for success -1 for error
//
//*****************************************************************************
extern int32_t AtTerm_init(uint8_t mode);

//*****************************************************************************
//
//! \brief Puts a char to the AT Terminal
//!
//! This function writes a character to the AT Terminal
//!
//! \param ch - the char to be written
//!
//! \return bytes read (0 or 1)
//
//*****************************************************************************
extern void AtTerm_putChar( char ch );

//*****************************************************************************
//
//! \brief Sends a string to the AT Terminal
//!
//! This function writes a string to the AT Terminal
//!
//! \param string - the null terminated string to be written
//!
//! \return none
//
//*****************************************************************************
extern void AtTerm_sendString( char* string);

//*****************************************************************************
//
//! \brief Sends a string and a Uint8 to the AT Terminal
//!
//! This function writes a string to the AT Terminal
//!
//! \param string - the null terminated string to be written
//! \param value - the Uint8 value to be written
//! \param format - base 10 or 16 (Dec or Hex)
//!
//! \return none
//
//*****************************************************************************
extern void AtTerm_sendStringUi8Value( char *string, uint8_t value, uint8_t
        format);

//*****************************************************************************
//
//! \brief Sends a string and a Int8 to the AT Terminal
//!
//! This function writes a string to the AT Terminal
//!
//! \param string - the null terminated string to be written
//! \param value - the Int8 value to be written
//! \param format - base 10 or 16 (Dec or Hex)
//!
//! \return none
//
//*****************************************************************************
extern void AtTerm_sendStringI8Value(char *string, int8_t value,
        uint8_t format);

//*****************************************************************************
//
//! \brief Sends a string and a Uint16 to the AT Terminal
//!
//! This function writes a string to the AT Terminal
//!
//! \param string - the null terminated string to be written
//! \param value - the Uint16 value to be written
//! \param format - base 10 or 16 (Dec or Hex)
//!
//! \return none
//
//*****************************************************************************
extern void AtTerm_sendStringUi16Value( char* string, uint16_t value, uint8_t
        format);

//*****************************************************************************
//
//! \brief Sends a string and a Int16 to the AT Terminal
//!
//! This function writes a string to the AT Terminal
//!
//! \param string - the null terminated string to be written
//! \param value - the Int16 value to be written
//! \param format - base 10 or 16 (Dec or Hex)
//!
//! \return none
//
//*****************************************************************************
extern void AtTerm_sendStringI16Value(char *string, int16_t value, uint8_t
        format);

//*****************************************************************************
//
//! \brief Sends a string and a Uint32 to the AT Terminal
//!
//! This function writes a string to the AT Terminal
//!
//! \param string - the null terminated string to be written
//! \param value - the Uint32 value to be written
//! \param format - base 10 or 16 (Dec or Hex)
//!
//! \return none
//
//*****************************************************************************
extern void AtTerm_sendStringUi32Value( char *string, uint32_t value, uint8_t
        format);

//*****************************************************************************
//
//! \brief Sends a string and a Int32 to the AT Terminal
//!
//! This function writes a string to the AT Terminal
//!
//! \param string - the null terminated string to be written
//! \param value - the Int32 value to be written
//! \param format - base 10 or 16 (Dec or Hex)
//!
//! \return none
//
//*****************************************************************************
extern void AtTerm_sendStringI32Value(char *string, int32_t value, uint8_t
        format);

#ifdef __cplusplus
}
#endif

#endif /* __AT_TERM__ */
