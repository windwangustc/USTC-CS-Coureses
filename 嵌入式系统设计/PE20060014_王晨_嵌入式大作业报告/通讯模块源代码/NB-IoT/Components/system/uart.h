/*!
 * \file      uart.h
 *
 * \brief     UART driver implementation
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \author    Miguel Luis (  )
 *
 * \author    Gregory Cristian (  )
 */
#ifndef __UART_H__
#define __UART_H__


#include "stdint.h"
/*!
 * UART peripheral ID
 */
typedef enum
{
    UART_1,
    UART_2,
    UART_USB_CDC = 255,
}UartId_t;

/*!
 * UART object type definition
 */
typedef struct
{
    UartId_t UartId;
}Uart_t;

/*!
 * UART word length
 */
typedef enum
{
    UART_8_BIT = 0,
    UART_9_BIT
}WordLength_t;

/*!
 * UART stop bits
 */
typedef enum
{
    UART_1_STOP_BIT = 0,
    UART_0_5_STOP_BIT,
    UART_2_STOP_BIT,
    UART_1_5_STOP_BIT
}StopBits_t;

/*!
 * UART parity
 */
typedef enum
{
    NO_PARITY = 0,
    EVEN_PARITY,
    ODD_PARITY
}Parity_t;

/*!
 * UART flow control
 */
typedef enum
{
    NO_FLOW_CTRL = 0,
    RTS_FLOW_CTRL,
    CTS_FLOW_CTRL,
    RTS_CTS_FLOW_CTRL
}FlowCtrl_t;

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
	uint16_t event_id );

/*!
 * \brief Configures the UART object and MCU peripheral
 *
 * \remark UartInit function must be called first.
 *
 * \param [IN] obj          UART object
 * \param [IN] baudrate     UART baudrate
 * \param [IN] wordLength   packet length
 * \param [IN] stopBits     stop bits setup
 * \param [IN] parity       packet parity
 * \param [IN] flowCtrl     UART flow control
 */
void UartConfig( Uart_t *obj, uint32_t baudrate,
	WordLength_t wordLength, StopBits_t stopBits, Parity_t parity,
	FlowCtrl_t flowCtrl );

/*!
 * \brief Sends a buffer to the UART
 *
 * \param [IN] obj    UART object
 * \param [IN] buffer Buffer to be sent
 * \param [IN] size   Buffer size
 * \retval status     [0: OK, 1: Busy]
 */
uint8_t UartPutBuffer( Uart_t *obj, uint8_t *buffer, uint16_t size );

/*!
 * \brief Gets a character from the UART
 *
 * \param [IN] obj          UART object
 * \param [IN] buffer       Received buffer
 * \param [IN] size         Number of bytes to be received
 * \param [OUT] nbReadBytes Number of bytes really read
 * \retval status           [0: OK, 1: Busy]
 */
uint8_t UartGetBuffer( Uart_t *obj, uint8_t *buffer, uint16_t size,
	uint16_t *nbReadBytes );


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
	uint16_t event_id );

/*!
 * \brief Configures the UART2 object and MCU peripheral
 *
 * \remark Uart2Init function must be called first.
 *
 * \param [IN] obj          UART2 object
 * \param [IN] baudrate     UART2 baudrate
 * \param [IN] wordLength   packet length
 * \param [IN] stopBits     stop bits setup
 * \param [IN] parity       packet parity
 * \param [IN] flowCtrl     UART2 flow control
 */
void Uart2Config( Uart_t *obj, uint32_t baudrate,
	WordLength_t wordLength, StopBits_t stopBits, Parity_t parity,
	FlowCtrl_t flowCtrl );

/*!
 * \brief Sends a buffer to the UART
 *
 * \param [IN] obj    UART object
 * \param [IN] buffer Buffer to be sent
 * \param [IN] size   Buffer size
 * \retval status     [0: OK, 1: Busy]
 */
uint8_t Uart2PutBuffer( Uart_t *obj, uint8_t *buffer, uint16_t size );

/*!
 * \brief Gets a character from the UART
 *
 * \param [IN] obj          UART object
 * \param [IN] buffer       Received buffer
 * \param [IN] size         Number of bytes to be received
 * \param [OUT] nbReadBytes Number of bytes really read
 * \retval status           [0: OK, 1: Busy]
 */
uint8_t Uart2GetBuffer( Uart_t *obj, uint8_t *buffer, uint16_t size,
	uint16_t *nbReadBytes );
#endif // __UART_H__
