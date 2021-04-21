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

#ifndef __FSL_LPUART_DRIVER_H__
#define __FSL_LPUART_DRIVER_H__

#include <stdint.h>
#include <stdbool.h>
#include "fsl_os_abstraction.h"
#include "fsl_lpuart_hal.h"
#include "fsl_clock_manager.h"
#include "RingBuf.h"

/*!
 * @addtogroup lpuart_driver
 * @{
 */

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief Table of base addresses for LPUART instances. */
extern LPUART_Type * const g_lpuartBase[LPUART_INSTANCE_COUNT];

/*! @brief Table to save LPUART IRQ enumeration numbers defined in the CMSIS header file */
extern const IRQn_Type g_lpuartRxTxIrqId[LPUART_INSTANCE_COUNT];

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief LPUART receive callback function type. */
typedef void (* lpuart_rx_callback_t)(uint32_t instance, void * lpuartState);

/*! @brief UART transmit callback function type */
typedef void (* lpuart_tx_callback_t)(uint32_t instance, void * lpuartState);

/*!
 * @brief Runtime state of the LPUART driver.
 *
 * Note that the caller must provide memory for the driver state structures during
 * initialization because the driver does not statically allocate memory.
 */
typedef struct LpuartState {
    uint8_t txFifoEntryCount;        /*!< Number of data word entries in transmit FIFO. */
    const uint8_t * txBuff;          /*!< The buffer of data being sent.*/
    RingBuf_Object txRingBuffer;   /*! local circular tx buffer object */
    uint8_t * rxBuff;                /*!< The buffer of received data.*/
    RingBuf_Object rxRingBuffer;   /*! local circular rx buffer object */
    volatile size_t txSize;          /*!< The remaining number of bytes to be transmitted. */
    volatile size_t rxSize;          /*!< The remaining number of bytes to be received. */
    lpuart_rx_callback_t rxCallback; /*!< Callback to invoke after receiving byte.*/
    void * rxCallbackParam;          /*!< Receive callback parameter pointer.*/
    lpuart_tx_callback_t txCallback; /*!< Callback to invoke after transmitting byte.*/
    void * txCallbackParam;          /*!< Transmit callback parameter pointer.*/
} lpuart_state_t;

/*! @brief LPUART configuration structure
 * @internal gui name="Configuration" id="Configuration"
 */
typedef struct LpuartUserConfig {
    clock_lpuart_src_t clockSource;      /*!< LPUART clock source @internal gui name="Clock source" id="ClockSource" */
    uint32_t baudRate;                   /*!< LPUART baud rate @internal gui name="Baud rate" id="BaudRate" */
    lpuart_parity_mode_t parityMode;     /*!< parity mode, disabled (default), even, odd @internal gui name="Parity mode" id="Parity" */
    lpuart_stop_bit_count_t stopBitCount;/*!< number of stop bits, 1 stop bit (default) or 2 stop bits @internal gui name="Stop bits" id="StopBits" */
    lpuart_bit_count_per_char_t bitCountPerChar; /*!< number of bits, 8-bit (default) or 9-bit in a
                                                   char (up to 10-bits in some LPUART instances. @internal gui name="Bits per char" id="DataBits" */
} lpuart_user_config_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name LPUART Driver
 * @{
 */

/*!
 * @brief Initializes an LPUART operation instance.
 *
 * The caller provides memory for the driver state structures during initialization.
 * The user must select the LPUART clock source in the application to initialize the LPUART.
 *
 * @param instance  LPUART instance number
 * @param lpuartUserConfig user configuration structure of type #lpuart_user_config_t
 * @param lpuartStatePtr pointer to the LPUART driver state structure
 * @return An error code or kStatus_LPUART_Success
 */
lpuart_status_t LPUART_DRV_Init(uint32_t instance, lpuart_state_t * lpuartStatePtr,
                                const lpuart_user_config_t * lpuartUserConfig);

/*!
 * @brief Shuts down the LPUART by disabling interrupts and transmitter/receiver.
 *
 * This function disables the LPUART interrupts, the transmitter and receiver, and
 * flushes the FIFOs (for modules that support FIFOs).
 *
 * @param instance  LPUART instance number
 * @return An error code or kStatus_LPUART_Success
 */
lpuart_status_t LPUART_DRV_Deinit(uint32_t instance);

/*!
 * @brief Installs callback function for the LPUART receive.
 *
 * @note After a callback is installed, it bypasses part of the LPUART IRQHandler logic.
 * Therefore, the callback needs to handle the indexes of txBuff and txSize.
 *
 * @param instance The LPUART instance number.
 * @param function The LPUART receive callback function.
 * @param rxBuff The receive buffer used inside IRQHandler. This buffer must be kept
 * 					as long as the callback is alive.
 * @param rxBuffLen The length of the receive buffer.
 * @param callbackParam The LPUART receive callback parameter pointer.
 * @return Former LPUART receive callback function pointer.
 */
lpuart_rx_callback_t LPUART_DRV_InstallRxCallback(uint32_t instance,
                                                lpuart_rx_callback_t function,
                                                uint8_t * rxBuff,
												size_t rxBuffLen,
                                                void * callbackParam);
/*!
 * @brief Installs callback function for the LPUART transmit.
 *
 * @note After a callback is installed, it bypasses part of the LPUART IRQHandler logic.
 * Therefore, the callback needs to handle the indexes of txBuff and txSize.
 *
 * @param instance The LPUART instance number.
 * @param function The LPUART transmit callback function.
 * @param txBuff The transmit buffer used inside IRQHandler. This buffer must be
 * 					kept as long as the callback is alive.
 * @param txBuffLen The length of the transmit buffer.
 * @param callbackParam The LPUART transmit callback parameter pointer.
 * @return Former LPUART transmit callback function pointer.
 */
lpuart_tx_callback_t LPUART_DRV_InstallTxCallback(uint32_t instance,
                                                  lpuart_tx_callback_t function,
                                                  uint8_t * txBuff,
												  size_t txBuffLen,
                                                  void * callbackParam);

/*!
 * @brief Sends data out through the LPUART module using a non-blocking method.
 *
 * A non-blocking (also known as asynchronous) function means that the function returns
 * immediately after initiating the transmit function.
 * The asynchronous method of transmitting and receiving allows the LPUART to perform a full duplex
 * operation (simultaneously transmit and receive).
 *
 * @param instance  LPUART instance number
 * @param txBuff  source buffer containing 8-bit data characters to send
 * @param txSize  the number of bytes to send
 * @return An error code or kStatus_LPUART_Success
 */
lpuart_status_t LPUART_DRV_StartSendData(uint32_t instance, const uint8_t * txBuff,
                                    uint32_t txSize);

/*!
 * @brief Gets data from the LPUART module by using a non-blocking method.
 *
 * A non-blocking (also known as synchronous) function means that the function returns
 * immediately after initiating the receive function.
 * The asynchronous method of transmitting and receiving allows the UART to perform a full duplex
 * operation (simultaneously transmit and receive).
 *
 * @param instance  LPUART instance number

 * @return An error code or kStatus_LPUART_Success
 */
lpuart_status_t LPUART_DRV_StartReceiveData(uint32_t instance);

/*@}*/

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* __FSL_LPUART_DRIVER_H__ */
/******************************************************************************/
/* EOF */
/******************************************************************************/
