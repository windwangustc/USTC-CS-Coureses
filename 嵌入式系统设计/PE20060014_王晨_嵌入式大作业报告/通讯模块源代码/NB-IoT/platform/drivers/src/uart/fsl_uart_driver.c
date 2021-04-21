/*
 * Copyright (c) 2013 - 2014, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <assert.h>
#include <string.h>
#include "RingBuf.h"
#include "fsl_uart_driver.h"
#include "fsl_clock_manager.h"
#include "fsl_interrupt_manager.h"

#if FSL_FEATURE_SOC_UART_COUNT
/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Pointer to uart runtime state structure */
extern void * g_uartStatePtr[UART_INSTANCE_COUNT];

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_Init
 * Description   : This function initializes a UART instance for operation.
 * This function will initialize the run-time state structure to keep track of
 * the on-going transfers, ungate the clock to the UART module, initialize the
 * module to user defined settings and default settings, configure the IRQ state
 * structure and enable the module-level interrupt to the core, and enable the
 * UART module transmitter and receiver.
 * The following is an example of how to set up the uart_state_t and the
 * uart_user_config_t parameters and how to call the UART_DRV_Init function by
 * passing in these parameters:
 *    uart_user_config_t uartConfig;
 *    uartConfig.baudRate = 9600;
 *    uartConfig.bitCountPerChar = kUart8BitsPerChar;
 *    uartConfig.parityMode = kUartParityDisabled;
 *    uartConfig.stopBitCount = kUartOneStopBit;
 *    uart_state_t uartState;
 *    UART_DRV_Init(instance, &uartState, &uartConfig);
 *
 *END**************************************************************************/
uart_status_t UART_DRV_Init(uint32_t instance, uart_state_t * uartStatePtr,
                            const uart_user_config_t * uartUserConfig)
{
    assert(uartStatePtr && uartUserConfig);
    assert(g_uartBase[instance]);
    assert(instance < UART_INSTANCE_COUNT);

    UART_Type * base = g_uartBase[instance];
    uint32_t uartSourceClock;

    /* Exit if current instance is already initialized. */
    if (g_uartStatePtr[instance])
    {
        return kStatus_UART_Initialized;
    }

    /* Clear the state structure for this instance. */
    memset(uartStatePtr, 0, sizeof(uart_state_t));

    /* Save runtime structure pointer.*/
    g_uartStatePtr[instance] = uartStatePtr;

    /* Un-gate UART module clock */
    CLOCK_SYS_EnableUartClock(instance);

    /* Initialize UART to a known state. */
    UART_HAL_Init(base);

    /* UART clock source is either system or bus clock depending on instance */
    uartSourceClock = CLOCK_SYS_GetUartFreq(instance);

    /* Initialize UART baud rate, bit count, parity and stop bit. */
    UART_HAL_SetBaudRate(base, uartSourceClock, uartUserConfig->baudRate);
    UART_HAL_SetBitCountPerChar(base, uartUserConfig->bitCountPerChar);
    UART_HAL_SetParityMode(base, uartUserConfig->parityMode);
#if FSL_FEATURE_UART_HAS_STOP_BIT_CONFIG_SUPPORT
    UART_HAL_SetStopBitCount(base, uartUserConfig->stopBitCount);
#endif

#if FSL_FEATURE_UART_HAS_FIFO
    uint8_t fifoSize;
    /* Obtain raw TX FIFO size bit setting */
    fifoSize = UART_HAL_GetTxFifoSize(base);
    /* Now calculate the number of data words per given FIFO size */
    uartStatePtr->txFifoEntryCount = (fifoSize == 0 ? 1 : 0x1 << (fifoSize + 1));

    /* Configure the TX FIFO watermark to be 1/2 of the total entry or 0 if
     * entry count = 1 A watermark setting of 0 for TX FIFO entry count of 1
     * means that TDRE will only interrupt when the TX buffer (the one entry in
     * the TX FIFO) is empty. Otherwise, if we set the watermark to 1, the TDRE
     * will always be set regardless if the TX buffer was empty or not as the
     * spec says TDRE will set when the FIFO is at or below the configured
     * watermark. */
    if (uartStatePtr->txFifoEntryCount > 1)
    {
        UART_HAL_SetTxFifoWatermark(base, (uartStatePtr->txFifoEntryCount >> 1U));
    }
    else
    {
        UART_HAL_SetTxFifoWatermark(base, 0);
    }

    /* Configure the RX FIFO watermark to be 1.
     * Note about RX FIFO support: There is only one RX data full interrupt that
     * is associated with the RX FIFO Watermark. The watermark cannot be
     * dynamically changed. This means if the rxSize is less than the programmed
     * watermark the interrupt will never occur. If we try to change the
     * watermark, this will involve shutting down the receiver first - which is
     * not a desirable operation when the UART is actively receiving data.
     * Hence, the best solution is to set the RX FIFO watermark to 1. */
    UART_HAL_SetRxFifoWatermark(base, 1);

    /* Enable and flush the FIFO prior to enabling the TX/RX */
    UART_HAL_SetTxFifoCmd(base, true);
    UART_HAL_SetRxFifoCmd(base, true);
    UART_HAL_FlushTxFifo(base);
    UART_HAL_FlushRxFifo(base);
#else
    /* For modules that do not support a FIFO, they have a data buffer that
     * essentially acts likes a one-entry FIFO, thus to make the code cleaner,
     * we'll equate txFifoEntryCount to 1. Also note that TDRE flag will set
     * only when the tx buffer is empty. */
    uartStatePtr->txFifoEntryCount = 1;
#endif

    /* Enable UART interrupt on NVIC level. */
    INT_SYS_EnableIRQ(g_uartRxTxIrqId[instance]);

    /* Finally, enable the UART transmitter and receiver*/
    UART_HAL_EnableTransmitter(base);
    UART_HAL_EnableReceiver(base);

    return kStatus_UART_Success;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_Deinit
 * Description   : This function shuts down the UART by disabling interrupts and
 *                 the transmitter/receiver.
 * This function disables the UART interrupts, disables the transmitter and
 * receiver, and flushes the FIFOs (for modules that support FIFOs).
 *
 *END**************************************************************************/
uart_status_t UART_DRV_Deinit(uint32_t instance)
{
    assert(instance < UART_INSTANCE_COUNT);
    assert(g_uartBase[instance]);

    /* Exit if current instance is already de-initialized or is gated.*/
    if ((!g_uartStatePtr[instance]) || (!CLOCK_SYS_GetUartGateCmd(instance)))
    {
        return kStatus_UART_Fail;
    }

    UART_Type * base = g_uartBase[instance];

    /* In case there is still data in the TX FIFO or shift register that is
     * being transmitted wait till transmit is complete. */
#if FSL_FEATURE_UART_HAS_FIFO
    /* Wait until there all of the data has been drained from the TX FIFO */
    while(UART_HAL_GetTxDatawordCountInFifo(base) != 0) { }
#endif
    /* Wait until the data is completely shifted out of shift register */
    while(!(UART_BRD_S1_TC(base))) { }

    /* Disable the interrupt */
    INT_SYS_DisableIRQ(g_uartRxTxIrqId[instance]);

    /* Disable TX and RX */
    UART_HAL_DisableTransmitter(base);
    UART_HAL_DisableReceiver(base);

#if FSL_FEATURE_UART_HAS_FIFO
    /* Disable the FIFOs; should be done after disabling the TX/RX */
    UART_HAL_SetTxFifoCmd(base, false);
    UART_HAL_SetRxFifoCmd(base, false);
    UART_HAL_FlushTxFifo(base);
    UART_HAL_FlushRxFifo(base);
#endif

    /* Cleared state pointer. */
    g_uartStatePtr[instance] = NULL;

    /* Gate UART module clock */
    CLOCK_SYS_DisableUartClock(instance);

    return kStatus_UART_Success;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_InstallRxCallback
 * Description   : Install receive data callback function, pass in NULL pointer
 * as callback will unistall.
 *
 *END**************************************************************************/
uart_rx_callback_t UART_DRV_InstallRxCallback(uint32_t instance,
                                              uart_rx_callback_t function,
                                              uint8_t * rxBuff,
											  size_t rxBuffLen,
                                              void * callbackParam)
{
    assert(instance < UART_INSTANCE_COUNT);
    uart_state_t * uartState = (uart_state_t *)g_uartStatePtr[instance];

    uart_rx_callback_t currentCallback = uartState->rxCallback;
    uartState->rxCallback = function;
    uartState->rxCallbackParam = callbackParam;
    /* Create circular buffer object to be used for read buffering */
    RingBuf_construct(&uartState->rxRingBuffer, rxBuff, rxBuffLen);

    return currentCallback;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_InstallTxCallback
 * Description   : Install transmit data callback function, pass in NULL pointer
 * as callback will uninstall.
 *
 *END**************************************************************************/
uart_tx_callback_t UART_DRV_InstallTxCallback(uint32_t instance,
                                              uart_tx_callback_t function,
                                              uint8_t * txBuff,
											  size_t txBuffLen,
                                              void * callbackParam)
{
    assert(instance < UART_INSTANCE_COUNT);
    uart_state_t * uartState = (uart_state_t *)g_uartStatePtr[instance];

    uart_tx_callback_t currentCallback = uartState->txCallback;
    uartState->txCallback = function;
    uartState->txCallbackParam = callbackParam;
    /* Create circular buffer object to be used for read buffering */
    RingBuf_construct(&uartState->txRingBuffer, txBuff, txBuffLen);

    return currentCallback;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_SendData
 * Description   : This function sends (transmits) data through the UART module
 * using a non-blocking method.
 * A non-blocking (also known as asynchronous) function means that the function
 * returns immediately after initiating the transmit function. The application
 * has to get the transmit status to see when the transmit is complete. In
 * other words, after calling non-blocking (asynchronous) send function, the
 * application must get the transmit status to check if transmit is completed
 * or not. The asynchronous method of transmitting and receiving allows the UART
 * to perform a full duplex operation (simultaneously transmit and receive).
 *
 *END**************************************************************************/
uart_status_t UART_DRV_StartSendData(uint32_t instance,
                                const uint8_t * txBuff,
                                uint32_t txSize)
{
    assert(txBuff);
    assert(instance < UART_INSTANCE_COUNT);

    uart_status_t retVal = kStatus_UART_Success;
    uart_state_t * uartState = (uart_state_t *)g_uartStatePtr[instance];
    UART_Type * base = g_uartBase[instance];
    int32_t bufferIndex = 0;

    /* Fill up RingBuf */
    while (txSize) {
    	if(RingBuf_put(&uartState->txRingBuffer, *((unsigned char *)txBuff + bufferIndex)) < 0)
    	{
    		break;
    	}
    	bufferIndex ++;
    	txSize--;
    }

    /* Enable the transmitter data register empty interrupt. The TDRE flag will
     * set whenever the TX buffer is emptied into the TX shift register (for
     * non-FIFO IPs) or when the data in the TX FIFO is at or below the
     * programmed watermark (for FIFO-supported IPs). */
    UART_BWR_C2_TIE(base, 1U);

    return retVal;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_StartReceiveData
 * Description   : This function gets (receives) data from the UART module using
 * a non-blocking method.
 * A non-blocking (also known as synchronous) function means that the function
 * returns immediately after initiating the receive function.
 * The asynchronous method of transmitting and receiving allows the UART
 * to perform a full duplex operation (simultaneously transmit and receive).
 *
 *END**************************************************************************/
uart_status_t UART_DRV_StartReceiveData(uint32_t instance)
{
    assert(instance < UART_INSTANCE_COUNT);

   // uart_status_t retVal = kStatus_UART_Success;
    UART_Type * base = g_uartBase[instance];

    /* Enable the receive data overrun interrupt */
    UART_HAL_SetIntMode(base, kUartIntRxOverrun, true);

    /* Enable the receive data full interrupt */
    UART_BWR_C2_RIE(base, 1U);

    return kStatus_UART_Success;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_ReadData2RingBuf
 * Description   : This function save the uart receives data to ringbuffer.
 *
 *END**************************************************************************/
static int32_t UART_DRV_ReadData2RingBuf(uint32_t instance, int32_t size)
{
    int32_t readIn;
    uart_state_t * uartState = (uart_state_t *)g_uartStatePtr[instance];
    UART_Type * base = g_uartBase[instance];

    /* Fill up RingBuf */
    while (size && (readIn = (int32_t)UART_HAL_Getchar_NonBlocking(base)) != -1) {
        size--;
        RingBuf_put(&uartState->rxRingBuffer, (unsigned char)readIn);
    }

    return (size);
}

static void UART_DRV_WriteDataFromRingBuf(uint32_t instance)
{
	unsigned char readIn;
    uart_state_t * uartState = (uart_state_t *)g_uartStatePtr[instance];
    UART_Type * base = g_uartBase[instance];
	/* Send characters from ring buffer. */
	while (RingBuf_peek(&uartState->txRingBuffer, &readIn) > 0) {
		/* Send the next character and increment counts. */
		if (!UART_HAL_Putchar_NonBlocking(base, readIn)) {
			/* Character was not sent */
			break;
		}
		RingBuf_get(&uartState->txRingBuffer, &readIn);
	}
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_IRQHandler
 * Description   : Interrupt handler for UART.
 * This handler uses the buffers stored in the uart_state_t structs to transfer
 * data. This is not a public API as it is called whenever an interrupt occurs.
 *
 *END**************************************************************************/
void UART_DRV_IRQHandler(uint32_t instance)
{
    uart_state_t * uartState = (uart_state_t *)g_uartStatePtr[instance];
    UART_Type * base = g_uartBase[instance];
    int32_t readSize_ringBuf;
    uint32_t writtenLast;

    /* Handle receive data register full interrupt, if rx data register full
     * interrupt is enabled AND there is data available. */
    if((UART_BRD_C2_RIE(base)) && (UART_BRD_S1_RDRF(base)))
    {
#if FSL_FEATURE_UART_HAS_FIFO
        /* Read out all data from RX FIFO */
        while(UART_HAL_GetRxDatawordCountInFifo(base))
        {
#endif
            /* Get data and put into receive buffer */
        	readSize_ringBuf = uartState->rxRingBuffer.length - uartState->rxRingBuffer.count;
        	UART_DRV_ReadData2RingBuf(instance, readSize_ringBuf);

            /* Invoke callback if there is one */
            if (uartState->rxCallback != NULL)
            {
                uartState->rxCallback(instance, uartState);
            }
#if FSL_FEATURE_UART_HAS_FIFO
        }
#endif
    }

    /* Handle transmit data register empty interrupt, if tx data register empty
     * interrupt is enabled AND tx data register is currently empty. */
    if((UART_BRD_C2_TIE(base)) && (UART_BRD_S1_TDRE(base)))
    {
        /* Check to see if there are any more bytes to send */
    	writtenLast = RingBuf_getCount(&uartState->txRingBuffer);
        if (writtenLast)
        {
        	UART_DRV_WriteDataFromRingBuf(instance);
        }
    	/* Disable the transmitter data register empty interrupt */
        writtenLast = RingBuf_getCount(&uartState->txRingBuffer);

        if (!writtenLast)
        {
        	UART_BWR_C2_TIE(base, 0U);
        }

    }

    /* Handle receive overrun interrupt */
    if (UART_HAL_GetStatusFlag(base, kUartRxOverrun))
    {
        /* Clear the flag, OR the rxDataRegFull will not be set any more */
        UART_HAL_ClearStatusFlag(base, kUartRxOverrun);
    }
}

#endif /* FSL_FEATURE_SOC_UART_COUNT */
/*******************************************************************************
 * EOF
 ******************************************************************************/
