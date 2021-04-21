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

#ifndef __FSL_UART_DRIVER_H__
#define __FSL_UART_DRIVER_H__

#include <stdint.h>
#include <stdbool.h>
#include "fsl_os_abstraction.h"
#include "fsl_uart_hal.h"
#include "RingBuf.h"

/*!
 * @addtogroup uart_driver
 * @{
 */

/*!
 * @file
 *
 * Some devices count the UART instances with LPUART(e.g., KL27) or UART0(e.g.,
 * KL25) together. However, they are  different IPs with separate drivers:
 * LPSCI for UART0, LPUART for LPUART. In such cases, this UART driver 
 * only works with specific UART instances.
 *
 * For example, in KL27, there is LPUART0, LPUART1 and UART2. For LPUART0 and
 * LPUART1,  use the LPUART driver. For UART2, use this driver and
 * pass in 2 as the instance number. 0 and 1 are not valid.
 */

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief Table of base addresses for UART instances. */
extern UART_Type * const g_uartBase[UART_INSTANCE_COUNT];

/*! @brief Table to save UART IRQ enumeration numbers defined in the CMSIS header file */
extern const IRQn_Type g_uartRxTxIrqId[UART_INSTANCE_COUNT];

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief UART receive callback function type */
typedef void (* uart_rx_callback_t)(uint32_t instance, void * uartState);

/*! @brief UART transmit callback function type */
typedef void (* uart_tx_callback_t)(uint32_t instance, void * uartState);

/*!
 * @brief Runtime state of the UART driver.
 *
 * This structure holds data that are used by the UART peripheral driver to
 * communicate between the transfer function and the interrupt handler. The
 * interrupt handler also uses this information to keep track of its progress.
 * The user passes in the memory for the run-time state structure and the
 * UART driver fills out the members.
 */
typedef struct UartState {
    uint8_t txFifoEntryCount;      /*!< Number of data word entries in TX FIFO. */
    const uint8_t * txBuff;        /*!< The buffer of data being sent.*/
    RingBuf_Object txRingBuffer;   /*! local circular tx buffer object */
    uint8_t * rxBuff;              /*!< The buffer of received data. */
    RingBuf_Object rxRingBuffer;   /*! local circular rx buffer object */
    volatile size_t txSize;        /*!< The remaining number of bytes to be transmitted. */
    volatile size_t rxSize;        /*!< The remaining number of bytes to be received. */
    uart_rx_callback_t rxCallback; /*!< Callback to invoke after receiving byte.*/
    void * rxCallbackParam;        /*!< Receive callback parameter pointer.*/
    uart_tx_callback_t txCallback; /*!< Callback to invoke after transmitting byte.*/
    void * txCallbackParam;        /*!< Transmit callback parameter pointer.*/
} uart_state_t;

/*!
 * @brief User configuration structure for the UART driver.
 *
 * Use an instance of this structure with the UART_DRV_Init()function. This enables configuration of the
 * most common settings of the UART peripheral with a single function call. Settings include:
 * UART baud rate, UART parity mode: disabled (default), or even or odd, the number of stop bits, and 
 * the number of bits per data word.
 * @internal gui name="UART configuration" id="Configuration" 
 */
typedef struct UartUserConfig {
    uint32_t baudRate;            /*!< UART baud rate @internal gui name="Baud rate" id="BaudRate" */
    uart_parity_mode_t parityMode;      /*!< parity mode, disabled (default), even, odd @internal gui name="Parity mode" id="Parity" */
    uart_stop_bit_count_t stopBitCount; /*!< number of stop bits, 1 stop bit (default) or 2 stop bits @internal gui name="Stop bits" id="StopBits" */
    uart_bit_count_per_char_t bitCountPerChar; /*!< number of bits, 8-bit (default) or 9-bit in
                                                    a word (up to 10-bits in some UART instances) @internal gui name="Bits per char" id="DataBits" */
} uart_user_config_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name UART Interrupt Driver
 * @{
 */

/*!
 * @brief Initializes an UART instance for operation.
 *
 * This function initializes the run-time state structure to keep track of the on-going
 * transfers, un-gates the clock to the UART module, initializes the module
 * to user-defined settings and default settings, configures the IRQ state structure, and enables
 * the module-level interrupt to the core, and  the UART module transmitter and receiver.
 * This example shows how to set up the uart_state_t and the
 * uart_user_config_t parameters and how to call the UART_DRV_Init function by passing
 * in these parameters:
   @code
    uart_user_config_t uartConfig;
    uartConfig.baudRate = 9600;
    uartConfig.bitCountPerChar = kUart8BitsPerChar;
    uartConfig.parityMode = kUartParityDisabled;
    uartConfig.stopBitCount = kUartOneStopBit;
    uart_state_t uartState;
    UART_DRV_Init(instance, &uartState, &uartConfig);
    @endcode
 *
 * @param instance The UART instance number.
 * @param uartStatePtr A pointer to the UART driver state structure memory. The user 
 * passes in the memory for this run-time state structure. The UART driver
 *  populates the members. The run-time state structure keeps track of the
 *  current transfer in progress.
 * @param uartUserConfig The user configuration structure of type uart_user_config_t. The user
 *  populates the members of this structure and passes the pointer of this structure
 *  to this function.
 * @return An error code or kStatus_UART_Success.
 */
uart_status_t UART_DRV_Init(uint32_t instance, uart_state_t * uartStatePtr,
                        const uart_user_config_t * uartUserConfig);

/*!
 * @brief Shuts down the UART by disabling interrupts and the transmitter/receiver.
 *
 * This function disables the UART interrupts, the transmitter and receiver, and
 * flushes the FIFOs (for modules that support FIFOs).
 *
 * @param instance The UART instance number.
 * @return An error code or kStatus_UART_Success.
 */
uart_status_t UART_DRV_Deinit(uint32_t instance);

/*!
 * @brief Installs the callback function for the UART receive.
 *
 * @note After a callback is installed, it bypasses part of the UART IRQHandler logic.
 * So, the callback needs to handle the indexes of rxBuff, rxSize.
 *
 * @param instance The UART instance number.
 * @param function The UART receive callback function.
 * @param rxBuff The receive buffer used inside IRQHandler. This buffer must be
 * 					kept as long as the callback is alive.
 * @param rxBuffLen The length of the receive buffer.
 * @param callbackParam The UART receive callback parameter pointer.
 * @return Former UART receive callback function pointer.
 */
uart_rx_callback_t UART_DRV_InstallRxCallback(uint32_t instance,
                                              uart_rx_callback_t function,
                                              uint8_t * rxBuff,
											  size_t rxBuffLen,
                                              void * callbackParam);
/*!
 * @brief Installs the callback function for the UART transmit.
 *
 * @note After a callback is installed, it bypasses part of the UART IRQHandler logic.
 * Therefore, the callback needs to handle the txBuff and txSize indexes.
 *
 * @param instance The UART instance number.
 * @param function The UART transmit callback function.
 * @param txBuff The transmit buffer used inside IRQHandler. This buffer must be
 * 					kept as long as the callback is alive.
 * @param txBuffLen The length of the transmit buffer.
 * @param callbackParam The UART transmit callback parameter pointer.
 * @return Former UART transmit callback function pointer.
 */
uart_tx_callback_t UART_DRV_InstallTxCallback(uint32_t instance,
                                              uart_tx_callback_t function,
                                              uint8_t * txBuff,
											  size_t txBuffLen,
                                              void * callbackParam);

/*!
 * @brief Sends (transmits) data through the UART module using a non-blocking method.
 *
 * A non-blocking (also known as synchronous) function means that the function returns
 * immediately after initiating the transmit function.
 * The asynchronous method of transmitting and receiving allows the UART to perform a full duplex
 * operation (simultaneously transmit and receive).
 *
 * @param instance The UART module base address.
 * @param txBuff A pointer to the source buffer containing 8-bit data chars to send.
 * @param txSize The number of bytes to send.
 * @return An error code or kStatus_UART_Success.
 */
uart_status_t UART_DRV_StartSendData(uint32_t instance, const uint8_t * txBuff, uint32_t txSize);

/*!
 * @brief Start gets (receives) data from the UART module using a non-blocking method.
 *
 * A non-blocking (also known as synchronous) function means that the function returns
 * immediately after initiating the receive function.
 * The asynchronous method of transmitting and receiving allows the UART to perform a full duplex
 * operation (simultaneously transmit and receive).
 *
 * @param instance The UART module base address.
 * @return An error code or kStatus_UART_Success.
 */
uart_status_t UART_DRV_StartReceiveData(uint32_t instance);

/*@}*/

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* __FSL_UART_DRIVER_H__*/
/*******************************************************************************
 * EOF
 ******************************************************************************/

