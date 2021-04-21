/*!
 * \file      delay-board.c
 *
 * \brief     Target board delay implementation
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
 *
 * \author    Johannes Bruder ( STACKFORCE )
 */
#include "delay-board.h"
#include "rtc-board.h"
void delayUSMcu( uint32_t us)
{
	uint32_t loop = 0;
	uint32_t i = 8U;
	for(loop = 0; loop < us; loop ++)
	{
		i = 8U;
		while (i--)
		{
			__asm("nop");
		}
	}
	}

void delayMSMcu( uint32_t ms)
{
	uint32_t loop = 0;
	uint32_t i = 8021U;
	for(loop = 0; loop < ms; loop ++)
	{
		i = 8021U;
		while (i--)
		{
			__asm("nop");
		}
	}

}
