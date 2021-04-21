/*!
 * \file      uart-board.h
 *
 * \brief     Target board UART driver implementation
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
#ifndef __UART_BOARD_H__
#define __UART_BOARD_H__

#include <stdint.h>
#include "uart.h"



/*
 * @fn      UartMcuInit
 *
 * @brief   Initializes the UART object and MCU peripheral.
 *
 * @param   obj		-	UART object
 * @param	uartId	-	UART ID
 * @param	task_id	-	The task id to indicate
 * @param	event_id-	The event id to indicate
 *
 * @return  none.
 */
void UartMcuInit( Uart_t * obj, UartId_t uartId, uint8_t task_id,
	uint16_t event_id);

/*
 * @fn      UartMcuConfig
 *
 * @brief   Configures the UART object and MCU peripheral.
 *
 * @param   obj			-	UART object
 * @param	baudrate	-	UART baudrate
 * @param	wordLength	-	packet length
 * @param	stopBits	-	stop bits setup
 * @param	parity		-	packet parity
 * @param	flowCtrl	-	UART flow control
 *
 * @return  none.
 */
void UartMcuConfig( Uart_t *obj, uint32_t baudrate,
	WordLength_t wordLength, StopBits_t stopBits, Parity_t parity,
	FlowCtrl_t flowCtrl );

/*
 * @fn      UartMcuPutBuffer
 *
 * @brief   Sends a buffer to the UART
 *
 * @param   obj		-	UART object
 * @param	buffer	-	Bufffer to be sent
 * @param	size	-	Buffer size
 *
 * @return  status 0:OK, other:failure.
 */
uint8_t UartMcuPutBuffer( Uart_t *obj, uint8_t *buffer, uint16_t size );

/*
 * @fn      UartMcuGetBuffer
 *
 * @brief   Gets contents from the UART
 *
 * @param   obj		-	UART object
 * @param	buffer	-	Received buffer
 * @param	size	-	Number of bytes to be received
 * @param	nbReadBytes	-	Number of bytes really read,
 * 								output parameter
 *
 * @return  status 0:OK, other:failure.
 */
uint8_t UartMcuGetBuffer( Uart_t *obj, uint8_t *buffer, uint16_t size,
	uint16_t *nbReadBytes );

/*
 * @fn      Uart2McuInit
 *
 * @brief   Initializes the UART object and MCU peripheral.
 *
 * @param   obj		-	UART object
 * @param	uartId	-	UART ID
 * @param	task_id	-	The task id to indicate
 * @param	event_id-	The event id to indicate
 *
 * @return  none.
 */
void Uart2McuInit( Uart_t * obj, UartId_t uartId, uint8_t task_id,
	uint16_t event_id);

/*
 * @fn      Uart2McuConfig
 *
 * @brief   Configures the UART object and MCU peripheral.
 *
 * @param   obj			-	UART object
 * @param	baudrate	-	UART baudrate
 * @param	wordLength	-	packet length
 * @param	stopBits	-	stop bits setup
 * @param	parity		-	packet parity
 * @param	flowCtrl	-	UART flow control
 *
 * @return  none.
 */
void Uart2McuConfig( Uart_t *obj, uint32_t baudrate,
	WordLength_t wordLength, StopBits_t stopBits, Parity_t parity,
	FlowCtrl_t flowCtrl );

/*
 * @fn      Uart2McuPutBuffer
 *
 * @brief   Sends a buffer to the UART
 *
 * @param   obj		-	UART object
 * @param	buffer	-	Bufffer to be sent
 * @param	size	-	Buffer size
 *
 * @return  status 0:OK, other:failure.
 */
uint8_t Uart2McuPutBuffer( Uart_t *obj, uint8_t *buffer, uint16_t size );

/*
 * @fn      UartMcuGetBuffer
 *
 * @brief   Gets contents from the UART
 *
 * @param   obj		-	UART object
 * @param	buffer	-	Received buffer
 * @param	size	-	Number of bytes to be received
 * @param	nbReadBytes	-	Number of bytes really read,
 * 								output parameter
 *
 * @return  status 0:OK, other:failure.
 */
uint8_t Uart2McuGetBuffer( Uart_t *obj, uint8_t *buffer, uint16_t size,
	uint16_t *nbReadBytes );


#endif // __UART_BOARD_H__
