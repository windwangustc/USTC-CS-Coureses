/*!
 * \file      sx1301-board.h
 *
 * \brief     Target board SX1301 driver implementation
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
#ifndef __SX1301_BOARD_H__
#define __SX1301_BOARD_H__

#include <stdint.h>
#include <stdbool.h>
#include "gpio.h"
#include "spi.h"

extern Gpio_t SX1301_GPIO0;
extern Gpio_t SX1301_GPIO_RESET;
extern Spi_t SX1301_Spi;

/*!
 * Hardware IO IRQ callback function definition
 */
typedef void ( DioIrqHandler )( void );

void SX1301OnDio0Irq( void );

/*!
 * \brief Initializes the radio I/Os pins interface
 */
void SX1301IoInit( void );

/*!
 * \brief Initializes DIO IRQ handlers
 *
 * \param [IN] irqHandlers Array containing the IRQ callback functions
 */
void SX1301IoIrqInit( DioIrqHandler **irqHandlers );

/*!
 * \brief De-initializes the radio I/Os pins interface.
 *
 * \remark Useful when going in MCU low power modes
 */
void SX1301IoDeInit( void );

/*!
 * \brief Resets the radio
 */
void SX1301Reset( void );

#endif // __SX1301_BOARD_H__
