/*!
 * \file      uart.c
 *
 * \brief     UART driver implementation
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
#include "uart-board.h"
#include "uart.h"

/*
 * @fn      UartInit
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
void UartInit( Uart_t *obj, UartId_t uartId, uint8_t task_id,
		uint16_t event_id )
{
    UartMcuInit( obj, uartId, task_id, event_id );
}

/*
 * @fn      UartConfig
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
void UartConfig( Uart_t *obj, uint32_t baudrate,
	WordLength_t wordLength, StopBits_t stopBits, Parity_t parity,
	FlowCtrl_t flowCtrl )
{
    UartMcuConfig( obj, baudrate, wordLength, stopBits, parity, flowCtrl );
}

/*
 * @fn      UartPutBuffer
 *
 * @brief   Sends a buffer to the UART
 *
 * @param   obj		-	UART object
 * @param	buffer	-	Buffer to be sent
 * @param	size	-	Buffer size
 *
 * @return  status 0:OK, other:failure.
 */
uint8_t UartPutBuffer( Uart_t *obj, uint8_t *buffer, uint16_t size )
{
    return UartMcuPutBuffer( obj, buffer, size );
}

/*
 * @fn      UartGetBuffer
 *
 * @brief   Gets contents from the UART
 *
 * @param   obj		-	UART object
 * @param	buffer	-	Received to be sent
 * @param	size	-	Number of bytes to be received
 * @param	nbReadBytes	-	Number of bytes really read
 *
 * @return  status 0:OK, other:failure.
 */
uint8_t UartGetBuffer( Uart_t *obj, uint8_t *buffer, uint16_t size,
	uint16_t *nbReadBytes )
{
    return UartMcuGetBuffer( obj, buffer, size, nbReadBytes );
}


/*
 * @fn      Uart2Init
 *
 * @brief   Initializes the UART2 object and MCU peripheral.
 *
 * @param   obj		-	UART2 object
 * @param	uartId	-	UART2 ID
 * @param	task_id	-	The task id to indicate
 * @param	event_id-	The event id to indicate
 *
 * @return  none.
 */
void Uart2Init( Uart_t *obj, UartId_t uartId, uint8_t task_id,
		uint16_t event_id )
{
    Uart2McuInit( obj, uartId, task_id, event_id );
}

/*
 * @fn      Uart2Config
 *
 * @brief   Configures the UART2 object and MCU peripheral.
 *
 * @param   obj			-	UART2 object
 * @param	baudrate	-	UART2 baudrate
 * @param	wordLength	-	packet length
 * @param	stopBits	-	stop bits setup
 * @param	parity		-	packet parity
 * @param	flowCtrl	-	UART2 flow control
 *
 * @return  none.
 */
void Uart2Config( Uart_t *obj, uint32_t baudrate,
	WordLength_t wordLength, StopBits_t stopBits, Parity_t parity,
	FlowCtrl_t flowCtrl )
{
    Uart2McuConfig( obj, baudrate, wordLength, stopBits, parity, flowCtrl );
}

/*
 * @fn      UartPutBuffer
 *
 * @brief   Sends a buffer to the UART
 *
 * @param   obj		-	UART object
 * @param	buffer	-	Buffer to be sent
 * @param	size	-	Buffer size
 *
 * @return  status 0:OK, other:failure.
 */
uint8_t Uart2PutBuffer( Uart_t *obj, uint8_t *buffer, uint16_t size )
{
    return Uart2McuPutBuffer( obj, buffer, size );
}

/*
 * @fn      Uart2GetBuffer
 *
 * @brief   Gets contents from the UART
 *
 * @param   obj		-	UART2 object
 * @param	buffer	-	Received to be sent
 * @param	size	-	Number of bytes to be received
 * @param	nbReadBytes	-	Number of bytes really read
 *
 * @return  status 0:OK, other:failure.
 */
uint8_t Uart2GetBuffer( Uart_t *obj, uint8_t *buffer, uint16_t size,
	uint16_t *nbReadBytes )
{
    return Uart2McuGetBuffer( obj, buffer, size, nbReadBytes );
}

