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
#include "fsl_lpuart_driver.h"
#include "fsl_interrupt_manager.h"

#if FSL_FEATURE_SOC_LPUART_COUNT

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Pointer to lpuart runtime state structure */
extern void * g_lpuartStatePtr[LPUART_INSTANCE_COUNT];

/*******************************************************************************
 * Private Functions
 ******************************************************************************/
/*******************************************************************************
 * Code
 ******************************************************************************/
/*FUNCTION**********************************************************************
 *
 * Function Name : LPUART_DRV_Init
 * Description   : This function initializes a LPUART instance for operation.
 * This function will initialize the run-time state structure to keep track of
 * the on-going transfers, ungate the clock to the LPUART module, initialize the
 * module to user defined settings and default settings, configure the IRQ state
 * structure and enable the module-level interrupt to the core, and enable the
 * LPUART module transmitter and receiver.
 * The following is an example of how to set up the lpuart_state_t and the
 * lpuart_user_config_t parameters and how to call the LPUART_DRV_Init function
 * by passing in these parameters:
 *    lpuart_user_config_t lpuartConfig;
 *    lpuartConfig.clockSource = kClockLpuartSrcPllFllSel;
 *    lpuartConfig.baudRate = 9600;
 *    lpuartConfig.bitCountPerChar = klpuart8BitsPerChar;
 *    lpuartConfig.parityMode = klpuartParityDisabled;
 *    lpuartConfig.stopBitCount = klpuartOneStopBit;
 *    lpuart_state_t lpuartState;
 *    LPUART_DRV_Init(instance, &lpuartState, &lpuartConfig);
 *
 *END**************************************************************************/
lpuart_status_t LPUART_DRV_Init(uint32_t instance, lpuart_state_t * lpuartStatePtr,
                                const lpuart_user_config_t * lpuartUserConfig)
{
    assert(lpuartStatePtr && lpuartUserConfig);
    assert(instance < LPUART_INSTANCE_COUNT);

    uint32_t lpuartSourceClock;
    LPUART_Type * base = g_lpuartBase[instance];

    /* Exit if current instance is already initialized. */
    if (g_lpuartStatePtr[instance])
    {
        return kStatus_LPUART_Initialized;
    }

    /* Clear the state struct for this instance. */
    memset(lpuartStatePtr, 0, sizeof(lpuart_state_t));

    /* Save runtime structure pointer.*/
    g_lpuartStatePtr[instance] = lpuartStatePtr;

    /* Set LPUART clock source */
    CLOCK_SYS_SetLpuartSrc(instance, lpuartUserConfig->clockSource);

    /* ungate lpuart module clock */
    CLOCK_SYS_EnableLpuartClock(instance);

    /* initialize the LPUART instance */
    LPUART_HAL_Init(base);

    /* LPUART clock source is either system clock or bus clock depending on the instance */
    lpuartSourceClock = CLOCK_SYS_GetLpuartFreq(instance);

    /* initialize the parameters of the LPUART config structure with desired data */
    LPUART_HAL_SetBaudRate(base, lpuartSourceClock, lpuartUserConfig->baudRate);
    LPUART_HAL_SetBitCountPerChar(base, lpuartUserConfig->bitCountPerChar);
    LPUART_HAL_SetParityMode(base, lpuartUserConfig->parityMode);
    LPUART_HAL_SetStopBitCount(base, lpuartUserConfig->stopBitCount);

#if FSL_FEATURE_LPUART_HAS_FIFO
    uint8_t fifoSize;
    /* Obtain raw TX FIFO size bit setting */
    fifoSize = LPUART_HAL_GetTxFifoSize(base);
    /* Now calculate the number of data words per given FIFO size */
    lpuartStatePtr->txFifoEntryCount = (fifoSize == 0 ? 1 : 0x1 << (fifoSize + 1));

    /* Configure the TX FIFO watermark to be 1/2 of the total entry or 0 if
     * entry count = 1 A watermark setting of 0 for TX FIFO entry count of 1
     * means that TDRE will only interrupt when the TX buffer (the one entry in
     * the TX FIFO) is empty. Otherwise, if we set the watermark to 1, the TDRE
     * will always be set regardless if the TX buffer was empty or not as the
     * spec says TDRE will set when the FIFO is at or below the configured
     * watermark. */
    if (lpuartStatePtr->txFifoEntryCount > 1)
    {
        LPUART_HAL_SetTxFifoWatermark(base, (lpuartStatePtr->txFifoEntryCount >> 1U));
    }
    else
    {
        LPUART_HAL_SetTxFifoWatermark(base, 0);
    }

    /* Configure the RX FIFO watermark to be 0.
     * Note about RX FIFO support: There is only one RX data full interrupt that
     * is associated with the RX FIFO Watermark. The watermark cannot be
     * dynamically changed. This means if the rxSize is less or equal to the programmed
     * watermark the interrupt will never occur. If we try to change the
     * watermark, this will involve shutting down the receiver first - which is
     * not a desirable operation when the LPUART is actively receiving data.
     * Hence, the best solution is to set the RX FIFO watermark to 0. */
    LPUART_HAL_SetRxFifoWatermark(base, 0);

    /* Enable and flush the FIFO prior to enabling the TX/RX */
    LPUART_HAL_SetTxFifoCmd(base, true);
    LPUART_HAL_SetRxFifoCmd(base, true);
    LPUART_HAL_FlushTxFifo(base);
    LPUART_HAL_FlushRxFifo(base);
#else
    /* For modules that do not support a FIFO, they have a data buffer that
     * essentially acts likes a one-entry FIFO, thus to make the code cleaner,
     * we'll equate txFifoEntryCount to 1. Also note that TDRE flag will set
     * only when the tx buffer is empty. */
    lpuartStatePtr->txFifoEntryCount = 1;
#endif

    /* finally, enable the LPUART transmitter and receiver */
    LPUART_HAL_SetTransmitterCmd(base, true);
    LPUART_HAL_SetReceiverCmd(base, true);

    /* Enable LPUART interrupt. */
    INT_SYS_EnableIRQ(g_lpuartRxTxIrqId[instance]);

    return kStatus_LPUART_Success;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : LPUART_DRV_Deinit
 * Description   : This function shuts down the UART by disabling interrupts,
 *                 transmitter/receiver and flushing FIFOs (if module supports FIFO).
 *
 *END**************************************************************************/
lpuart_status_t LPUART_DRV_Deinit(uint32_t instance)
{
    assert(instance < LPUART_INSTANCE_COUNT);

    /* Exit if current instance is already de-initialized or is gated.*/
    if ((!g_lpuartStatePtr[instance]) || (!CLOCK_SYS_GetLpuartGateCmd(instance)))
    {
        return kStatus_LPUART_Fail;
    }

    LPUART_Type * base = g_lpuartBase[instance];

    /* In case there is still data in the TX FIFO or shift register that is
     * being transmitted wait till transmit is complete. */
#if FSL_FEATURE_LPUART_HAS_FIFO
    /* Wait until there all of the data has been drained from the TX FIFO */
    while(LPUART_HAL_GetTxDatawordCountInFifo(base) != 0) { }
#endif
    /* Wait until the data is completely shifted out of shift register */
    while (!LPUART_BRD_STAT_TC(base)) {}

    /* Disable LPUART interrupt. */
    INT_SYS_DisableIRQ(g_lpuartRxTxIrqId[instance]);

    /* disable tx and rx */
    LPUART_HAL_SetTransmitterCmd(base, false);
    LPUART_HAL_SetReceiverCmd(base, false);

#if FSL_FEATURE_LPUART_HAS_FIFO
    /* Disable the FIFOs; should be done after disabling the TX/RX */
    LPUART_HAL_SetTxFifoCmd(base, false);
    LPUART_HAL_SetRxFifoCmd(base, false);
    LPUART_HAL_FlushTxFifo(base);
    LPUART_HAL_FlushRxFifo(base);
#endif

    /* Clear our saved pointer to the state structure */
    g_lpuartStatePtr[instance] = NULL;

    /* gate lpuart module clock */
    CLOCK_SYS_DisableLpuartClock(instance);

    return kStatus_LPUART_Success;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : LPUART_DRV_InstallRxCallback
 * Description   : Install receive data callback function.
 *
 *END**************************************************************************/
lpuart_rx_callback_t LPUART_DRV_InstallRxCallback(uint32_t instance,
                                                lpuart_rx_callback_t function,
                                                uint8_t * rxBuff,
												size_t rxBuffLen,
                                                void * callbackParam)
{
    assert(instance < LPUART_INSTANCE_COUNT);
    lpuart_state_t * lpuartState = (lpuart_state_t *)g_lpuartStatePtr[instance];

    lpuart_rx_callback_t currentCallback = lpuartState->rxCallback;
    lpuartState->rxCallback = function;
    lpuartState->rxCallbackParam = callbackParam;
    /* Create circular buffer object to be used for read buffering */
    RingBuf_construct(&lpuartState->rxRingBuffer, rxBuff, rxBuffLen);

    return currentCallback;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : LPUART_DRV_InstallTxCallback
 * Description   : Install transmit data callback function, pass in NULL pointer
 * as callback will uninstall.
 *
 *END**************************************************************************/
lpuart_tx_callback_t LPUART_DRV_InstallTxCallback(uint32_t instance,
                                                  lpuart_tx_callback_t function,
                                                  uint8_t * txBuff,
												  size_t txBuffLen,
                                                  void * callbackParam)
{
    assert(instance < LPUART_INSTANCE_COUNT);
    lpuart_state_t * lpuartState = (lpuart_state_t *)g_lpuartStatePtr[instance];

    lpuart_tx_callback_t currentCallback = lpuartState->txCallback;
    lpuartState->txCallback = function;
    lpuartState->txCallbackParam = callbackParam;
    /* Create circular buffer object to be used for read buffering */
    RingBuf_construct(&lpuartState->txRingBuffer, txBuff, txBuffLen);

    return currentCallback;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : LPUART_DRV_StartSendData
 * Description   : This function sends data out through the LPUART module using
 * non-blocking method. The function will return immediately after calling this
 * function.
 *
 *END**************************************************************************/
lpuart_status_t LPUART_DRV_StartSendData(uint32_t instance,
                                    const uint8_t * txBuff,
                                    uint32_t txSize)
{
    assert(txBuff);
    assert(instance < LPUART_INSTANCE_COUNT);

    lpuart_status_t retVal = kStatus_LPUART_Success;
    lpuart_state_t * lpuartState = (lpuart_state_t *)g_lpuartStatePtr[instance];
    LPUART_Type * base = g_lpuartBase[instance];
    int32_t bufferIndex = 0;

    /* Fill up RingBuf */
    while (txSize) {
    	if(RingBuf_put(&lpuartState->txRingBuffer, *((unsigned char *)txBuff + bufferIndex)) < 0)
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
    LPUART_BWR_CTRL_TIE(base, 1U);

    return retVal;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : LPUART_DRV_StartReceiveData
 * Description   : This function receives data from LPUART module using
 * non-blocking method.  This function returns immediately after initiating the
 * receive function. The application has to get the receive status to see when
 * the receive is complete. In other words, after calling non-blocking get
 * function, the application must get the receive status to check if receive
 * is completed or not.
 *
 *END**************************************************************************/
lpuart_status_t LPUART_DRV_StartReceiveData(uint32_t instance)
{
    assert(instance < LPUART_INSTANCE_COUNT);

    lpuart_status_t retVal = kStatus_LPUART_Success;
    LPUART_Type * base = g_lpuartBase[instance];

    /* Enable the receive data overrun interrupt */
    LPUART_HAL_SetIntMode(base, kLpuartIntRxOverrun, true);

    /* Enable receive data full interrupt */
    LPUART_BWR_CTRL_RIE(base, 1U);

    return retVal;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : LPUART_DRV_ReadData2RingBuf
 * Description   : This function save the uart receives data to ringbuffer.
 *
 *END**************************************************************************/
static int32_t LPUART_DRV_ReadData2RingBuf(uint32_t instance, int32_t size)
{
    int32_t readIn;
    lpuart_state_t * lpuartState = (lpuart_state_t *)g_lpuartStatePtr[instance];
    LPUART_Type * base = g_lpuartBase[instance];

    /* Fill up RingBuf */
    while (size && (readIn = (int32_t)LPUART_HAL_Getchar_NonBlocking(base)) != -1) {
        size--;
        RingBuf_put(&lpuartState->rxRingBuffer, (unsigned char)readIn);
    }

    return (size);
}

static void LPUART_DRV_WriteDataFromRingBuf(uint32_t instance)
{
	unsigned char readIn;
    lpuart_state_t * lpuartState = (lpuart_state_t *)g_lpuartStatePtr[instance];
    LPUART_Type * base = g_lpuartBase[instance];
	/* Send characters from ring buffer. */
	while (RingBuf_peek(&lpuartState->txRingBuffer, &readIn) > 0) {
		/* Send the next character and increment counts. */
		if (!LPUART_HAL_Putchar_NonBlocking(base, readIn)) {
			/* Character was not sent */
			break;
		}
		RingBuf_get(&lpuartState->txRingBuffer, &readIn);
	}
}

/*FUNCTION**********************************************************************
 *
 * Function Name : LPUART_DRV_IRQHandler
 * Description   : Interrupt handler for LPUART.
 * This handler uses the buffers stored in the lpuart_state_t structs to transfer
 * data. This is not a public API as it is called by IRQ whenever an interrupt
 * occurs.
 *
 *END**************************************************************************/
void LPUART_DRV_IRQHandler(uint32_t instance)
{
    lpuart_state_t * lpuartState = (lpuart_state_t *)g_lpuartStatePtr[instance];
    LPUART_Type * base = g_lpuartBase[instance];
    int32_t readSize_ringBuf;
    uint32_t writtenLast;

    /* Handle receive data full interrupt */
    if((LPUART_BRD_CTRL_RIE(base)) && (LPUART_BRD_STAT_RDRF(base)))
    {
#if FSL_FEATURE_LPUART_HAS_FIFO
        /* Read out all data from RX FIFO */
        while(LPUART_HAL_GetRxDatawordCountInFifo(base))
        {
#endif
            /* Get data and put in receive buffer  */
        	readSize_ringBuf = lpuartState->rxRingBuffer.length - lpuartState->rxRingBuffer.count;
        	LPUART_DRV_ReadData2RingBuf(instance, readSize_ringBuf);

            /* Invoke callback if there is one */
            if (lpuartState->rxCallback != NULL)
            {
               	lpuartState->rxCallback(instance, lpuartState);
            }
#if FSL_FEATURE_LPUART_HAS_FIFO
        }
#endif
    }

    /* Handle transmitter data register empty interrupt */
    if((LPUART_BRD_CTRL_TIE(base)) && (LPUART_BRD_STAT_TDRE(base)))
    {
        /* Check to see if there are any more bytes to send */
    	writtenLast = RingBuf_getCount(&lpuartState->txRingBuffer);
        if (writtenLast)
        {
        	LPUART_DRV_WriteDataFromRingBuf(instance);
        }
    	/* Disable the transmitter data register empty interrupt */
    	writtenLast = RingBuf_getCount(&lpuartState->txRingBuffer);
    	if(0 == writtenLast)
    	{
    		LPUART_BWR_CTRL_TIE(base, 0U);
    	}
    }

    /* Handle receive overrun interrupt */
    if (LPUART_HAL_GetStatusFlag(base, kLpuartRxOverrun))
    {
        /* Clear the flag, OR the rxDataRegFull will not be set any more */
		LPUART_HAL_ClearStatusFlag(base, kLpuartRxOverrun);
    }
}

#endif /* FSL_FEATURE_SOC_LPUART_COUNT */
/*******************************************************************************
 * EOF
 ******************************************************************************/

