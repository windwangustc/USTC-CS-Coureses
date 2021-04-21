/*************************************************************************
  \file					uart-board.c
  \brief
	Target board UART driver implementation.

  \author
	www.shuncom.com
  \internal
  Revision History:
  	May. 19, 2018		Johnason - Created

**************************************************************************/

/*********************************************************************
 * 						INCLUDES
 */
#include "string.h"
#include "utilities.h"
#include "board-config.h"
#include "board.h"
#include "timer.h"
#include "osal.h"
#include "uart.h"
#include "RingBuf.h"
#include "uart-board.h"
#include "fsl_lpuart_driver.h"
#include "fsl_uart_driver.h"
#include "fsl_uart_hal.h"
#include "hal_drivers.h"
#include "ZGlobals.h"
/*********************************************************************
 * 						CONSTANTS
 */
#define UART_RING_BUFFER_RX		254
#define UART_RING_BUFFER_TX		254
/*!
 * Defines the application data transmission duty cycle value in [ms].
 */
#define APP_TX_DUTYCYCLE                            3
#define MSG_MAX_SIZE            255

/*********************************************************************
 * 						LOCAL VAIRABLES
 */
static lpuart_state_t lpuartStatePtr;
uart_state_t  uartStatePtr;
static uint8_t uartRingBufferRx[UART_RING_BUFFER_RX];
static uint8_t uartRingBufferTx[UART_RING_BUFFER_TX];
static uint8_t uart2RingBufferRx[UART_RING_BUFFER_RX];
static uint8_t uart2RingBufferTx[UART_RING_BUFFER_TX];

/*!
 * Timer to handle the application data transmission duty cycle
 */
static TimerEvent_t uartBoard_rxTimer;
static TimerEvent_t uart2Board_rxTimer;
/* The task and event id, which will be triggered when RX information coming */
static uint8_t uartBoard_taskID;
static uint16_t uartBoard_eventID;
static uint8_t uart2Board_taskID;
static uint16_t uart2Board_eventID;
/*********************************************************************
 * 						LOCAL FUNCTIONS
 */


/*
 * @fn      OnUartBoardRxTimerEvent
 *
 * @brief   Function executed on UART RX timeout event.
 *
 * @param   none
 *
 * @return  none.
 */
static void OnUartBoardRxTimerEvent( void )
{
    TimerStop( &uartBoard_rxTimer );
    osal_set_event(uartBoard_taskID, uartBoard_eventID);
}

/*
 * @fn      uart_rx_callback
 *
 * @brief   The UART ISR callback.
 *
 * @param   instance	-	the LPUART instance
 * @param	uartState	-	the state structure of the instance
 *
 * @return  none.
 */
static void uart_rx_callback(uint32_t instance, void * uartState)
{
	//osal_set_event(uartBoard_taskID, uartBoard_eventID);
	TimerSetValue( &uartBoard_rxTimer, 3 );
	TimerStart( &uartBoard_rxTimer );
}

/*********************************************************************
 * 						API FUNCTIONS
 */
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
	uint16_t event_id)
{
	uint32_t instance = 0;
	obj->UartId = uartId;
	if(UART_1 == obj->UartId)
	{
		// the lpuart 0
		instance = BOARD_LPUART_INSTANCE;
	}
	BoardInitMcuLPUART(instance);
	uartBoard_taskID = task_id;
	uartBoard_eventID = event_id;
}

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
	FlowCtrl_t flowCtrl )
{
	uint32_t instance = 0;
	lpuart_user_config_t lpuartConfig;

	/* Initialize the RX timer */
	TimerInit( &uartBoard_rxTimer, OnUartBoardRxTimerEvent );

	if(UART_1 == obj->UartId)
	{
		instance = BOARD_LPUART_INSTANCE;
	}

	lpuartConfig.clockSource = BOARD_LPUART_CLOCK_SOURCE;
	lpuartConfig.baudRate = baudrate;

	if(UART_9_BIT == wordLength)
	{
		lpuartConfig.bitCountPerChar = kLpuart9BitsPerChar;
	}
	else
	{
		// default is 8 bits
		lpuartConfig.bitCountPerChar = kLpuart8BitsPerChar;
	}

	if(UART_2_STOP_BIT == stopBits)
	{
		lpuartConfig.stopBitCount = kLpuartTwoStopBit;
	}
	else
	{
		// default is 1 bit
		lpuartConfig.stopBitCount = kLpuartOneStopBit;
	}

	if(EVEN_PARITY == parity)
	{
		lpuartConfig.parityMode = kLpuartParityEven;
	}
	else if(ODD_PARITY == parity)
	{
		lpuartConfig.parityMode = kLpuartParityOdd;
	}
	else
	{
		// default is no parity
		lpuartConfig.parityMode = kLpuartParityDisabled;
	}

	// default is no flow control
	(void)flowCtrl;

    // Initialize the lpuart module with instance number and config structure
    LPUART_DRV_Init(instance, &lpuartStatePtr, &lpuartConfig);
    LPUART_DRV_InstallRxCallback(instance, uart_rx_callback, uartRingBufferRx,
    	UART_RING_BUFFER_RX, NULL);

    LPUART_DRV_InstallTxCallback(instance, NULL, uartRingBufferTx,
    	UART_RING_BUFFER_TX, NULL);

    LPUART_DRV_StartReceiveData(instance);
}

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
uint8_t UartMcuPutBuffer( Uart_t *obj, uint8_t *buffer, uint16_t size )
{
	uint32_t instance = 0;

	if(UART_1 == obj->UartId)
	{
		instance = BOARD_LPUART_INSTANCE;
	}
	LPUART_DRV_StartSendData(instance, buffer, size);
	return 0;
}

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
	uint16_t *nbReadBytes )
{
	uint32_t instance = 0;
	lpuart_state_t * state = &lpuartStatePtr;
	uint8_t readIn;
	uint32_t dataCount = 0;
	uint32_t dataIndex = 0;

	if(UART_1 == obj->UartId)
	{
		instance = BOARD_LPUART_INSTANCE;
	}

	(void)instance;

	dataCount = RingBuf_getCount(&state->rxRingBuffer);
	if(dataCount)
	{
		BoardDisableIrq();
		while(dataCount)
		{
			if (RingBuf_get(&state->rxRingBuffer, &readIn) < 0)
			{
				break;
			}
			buffer[dataIndex ++] = readIn;
			dataCount --;

			if(dataIndex >= size - 2)
			{
				break;
			}
		}
		BoardEnableIrq();
		*nbReadBytes = dataIndex;
	}
	else
	{
		*nbReadBytes = 0;
	}

	return 0;
}

/*
 *   uart2 config init
 */



static void OnUart2BoardRxTimerEvent( void )
{
    TimerStop( &uart2Board_rxTimer);
    osal_set_event(uart2Board_taskID, uart2Board_eventID);

}

/*
 * @fn      uart2_rx_callback
 *
 * @brief   The UAR2T ISR callback.
 *
 * @param   instance	-	the UART2 instance
 * @param	uartState	-	the state structure of the instance
 *
 * @return  none.
 */

static void uart2_rx_callback(uint32_t instance, void * uartState)
{
	//osal_set_event(uart2Board_taskID, uart2Board_eventID);

	TimerSetValue( &uart2Board_rxTimer, 3 );
	TimerStart( &uart2Board_rxTimer );
}

/*********************************************************************
 * 						API FUNCTIONS
 */
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
void Uart2McuInit( Uart_t * obj, UartId_t uartId, uint8_t task_id,
	uint16_t event_id)
{
	uint32_t instance = 0;
	obj->UartId = uartId;
	if(UART_1 == obj->UartId)
	{
		// the lpuart 0
		instance = BOARD_LPUART_INSTANCE;
	}
	else
	{
		instance = BOARD_UART_INSTANCE;
	}
	BoardInitMcuLPUART(instance);
	uart2Board_taskID = task_id;
	uart2Board_eventID = event_id;
}

/*
 * @fn      Uart2McuConfig
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
void Uart2McuConfig( Uart_t *obj, uint32_t baudrate,
	WordLength_t wordLength, StopBits_t stopBits, Parity_t parity,
	FlowCtrl_t flowCtrl )
{
	uint32_t instance = 0;
	uart_user_config_t uartConfig;

	/* Initialize the RX timer */
	TimerInit( &uart2Board_rxTimer, OnUart2BoardRxTimerEvent );

	if(UART_2 == obj->UartId)
	{
		instance = BOARD_UART_INSTANCE;
	}

	uartConfig.baudRate = baudrate;

	if(UART_9_BIT == wordLength)
	{
		uartConfig.bitCountPerChar = kLpuart9BitsPerChar;
	}
	else
	{
		// default is 8 bits
		uartConfig.bitCountPerChar = kLpuart8BitsPerChar;
	}

	if(UART_2_STOP_BIT == stopBits)
	{
		uartConfig.stopBitCount = kLpuartTwoStopBit;
	}
	else
	{
		// default is 1 bit
		uartConfig.stopBitCount = kLpuartOneStopBit;
	}

	if(EVEN_PARITY == parity)
	{
		uartConfig.parityMode = kLpuartParityEven;
	}
	else if(ODD_PARITY == parity)
	{
		uartConfig.parityMode = kLpuartParityOdd;
	}
	else
	{
		// default is no parity
		uartConfig.parityMode = kLpuartParityDisabled;
	}

	// default is no flow control
	(void)flowCtrl;

    // Initialize the lpuart module with instance number and config structure
    UART_DRV_Init(instance, &uartStatePtr, &uartConfig);
    UART_DRV_InstallRxCallback(instance, uart2_rx_callback, uart2RingBufferRx,
    	UART_RING_BUFFER_RX, NULL);

    UART_DRV_InstallTxCallback(instance, NULL, uart2RingBufferTx,
    	UART_RING_BUFFER_TX, NULL);

    UART_DRV_StartReceiveData(instance);
}


/*
 * @fn      Uart2McuPutBuffer
 *
 * @brief   Sends a buffer to the UART2
 *
 * @param   obj		-	UART2 object
 * @param	buffer	-	Bufffer to be sent
 * @param	size	-	Buffer size
 *
 * @return  status 0:OK, other:failure.
 */
uint8_t Uart2McuPutBuffer( Uart_t *obj, uint8_t *buffer, uint16_t size )
{
	uint32_t instance = 0;

	if(UART_1 == obj->UartId)
	{
		instance = BOARD_LPUART_INSTANCE;
	}
	else
	{
		instance = BOARD_UART_INSTANCE;
	}
	UART_DRV_StartSendData(instance, buffer, size);
	return 0;
}

/*
 * @fn      Uart2McuGetBuffer
 *
 * @brief   Gets contents from the UART2
 *
 * @param   obj		-	UART2 object
 * @param	buffer	-	Received buffer
 * @param	size	-	Number of bytes to be received
 * @param	nbReadBytes	-	Number of bytes really read,
 * 								output parameter
 *
 * @return  status 0:OK, other:failure.
 */
uint8_t Uart2McuGetBuffer( Uart_t *obj, uint8_t *buffer, uint16_t size,
	uint16_t *nbReadBytes )
{
	uint32_t instance = 0;
	uart_state_t * state = &uartStatePtr;
	uint8_t readIn;
	uint32_t dataCount = 0;
	uint32_t dataIndex = 0;

	if(UART_1 == obj->UartId)
	{
		instance = BOARD_LPUART_INSTANCE;
	}
	else
	{
		instance = BOARD_UART_INSTANCE;
	}

	(void)instance;

	dataCount = RingBuf_getCount(&state->rxRingBuffer);
	if(dataCount)
	{
		BoardDisableIrq();
		while(dataCount)
		{
			if (RingBuf_get(&state->rxRingBuffer, &readIn) < 0)
			{
				break;
			}
			buffer[dataIndex ++] = readIn;
			dataCount --;

			if(dataIndex >= size - 2)
			{
				break;
			}
		}
		BoardEnableIrq();
		*nbReadBytes = dataIndex;
	}
	else
	{
		*nbReadBytes = 0;
	}

	return 0;
}


//test


void uart_senddata(uint8_t *data)
{
	UART_DRV_StartSendData(2, data, strlen(data));
}

