/*!
 * \file      delay.c
 *
 * \brief     Delay implementation
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
#include "delay-board.h"
#include "delay.h"

void Delay( uint32_t s )
{
    DelayMs( s * 1000 );
}

void DelayMs( uint32_t ms )
{
	delayMSMcu( ms );
}

/*!
 * Blocking delay of "us" microseconds
 */
void DelayUs( uint32_t us )
{
	delayUSMcu(us);
}
