/*!
 * \file      gpio-board.h
 *
 * \brief     Target board GPIO driver implementation
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
#ifndef __GPIO_BOARD_H__
#define __GPIO_BOARD_H__

#include "gpio.h"

#if defined ( FREEDOM )
/* Freescale MCU dependency */
#include "fsl_gpio_driver.h"

#define GPIO_KEY_1		0x0001
#define GPIO_KEY_2		0x0002

/*! @brief Pin names */
enum _gpio_pins_pinNames{
    kGpioLEDBUSY     = GPIO_MAKE_PIN(GPIOE_IDX, 1),   /* BUSY LED */
    kGpioLEDRUN      = GPIO_MAKE_PIN(GPIOE_IDX, 0),   /* RUN LED */
    kGpioLED3        = GPIO_MAKE_PIN(GPIOD_IDX, 5),    /* LED 3 */
    kGpioLORARESET   = GPIO_MAKE_PIN(GPIOC_IDX, 16),   /* The LORA reset switch */
	kGpioSPINSS		 = GPIO_MAKE_PIN(GPIOC_IDX,  4),   /* The SPI NSS switch */
	kGpioLORAInt0	 = GPIO_MAKE_PIN(GPIOD_IDX,  4),   /* The LORA DIO interrupt 0 */
	kGpioLORAInt1	 = GPIO_MAKE_PIN(GPIOD_IDX,  1),   /* The LORA DIO interrupt 1 */
	kGpioMode	 	 = GPIO_MAKE_PIN(GPIOA_IDX,  4),   /* The Mode interrupt */
	kGpioWake		 = GPIO_MAKE_PIN(GPIOD_IDX,  3),   /* The Wake interrupt */
	kGpioLORAInt4	 = GPIO_MAKE_PIN(GPIOD_IDX,  4),   /* The LORA DIO interrupt 4 */
};
#endif




/*!
 * \brief Initializes the given GPIO object
 *
 * \param [IN] obj    Pointer to the GPIO object
 * \param [IN] pin    Pin name ( please look in pinName-board.h file )
 * \param [IN] mode   Pin mode [PIN_INPUT, PIN_OUTPUT,
 *                              PIN_ALTERNATE_FCT, PIN_ANALOGIC]
 * \param [IN] config Pin config [PIN_PUSH_PULL, PIN_OPEN_DRAIN]
 * \param [IN] type   Pin type [PIN_NO_PULL, PIN_PULL_UP, PIN_PULL_DOWN]
 * \param [IN] value  Default output value at initialization
 */
void GpioMcuInit( Gpio_t *obj, PinNames pin, PinModes mode, PinConfigs config, PinTypes type, uint32_t value );

/*!
 * \brief GPIO IRQ Initialization
 *
 * \param [IN] obj         Pointer to the GPIO object
 * \param [IN] irqMode     IRQ mode [NO_IRQ, IRQ_RISING_EDGE,
 *                                   IRQ_FALLING_EDGE, IRQ_RISING_FALLING_EDGE]
 * \param [IN] irqPriority IRQ priority [IRQ_VERY_LOW_PRIORITY, IRQ_LOW_PRIORITY
 *                                       IRQ_MEDIUM_PRIORITY, IRQ_HIGH_PRIORITY
 *                                       IRQ_VERY_HIGH_PRIORITY]
 * \param [IN] irqHandler  Callback function pointer
 */
void GpioMcuSetInterrupt( Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority, GpioIrqHandler *irqHandler );

/*!
 * \brief Removes the interrupt from the object
 *
 * \param [IN] obj Pointer to the GPIO object
 */
void GpioMcuRemoveInterrupt( Gpio_t *obj );

/*!
 * \brief Writes the given value to the GPIO output
 *
 * \param [IN] obj   Pointer to the GPIO object
 * \param [IN] value New GPIO output value
 */
void GpioMcuWrite( Gpio_t *obj, uint32_t value );

/*!
 * \brief Toggle the value to the GPIO output
 *
 * \param [IN] obj   Pointer to the GPIO object
 */
void GpioMcuToggle( Gpio_t *obj );

/*!
 * \brief Reads the current GPIO input value
 *
 * \param [IN] obj Pointer to the GPIO object
 * \retval value   Current GPIO input value
 */
uint32_t GpioMcuRead( Gpio_t *obj );

/* Freescale MCU dependency */
void szPort_eventLoop(void);

#endif // __GPIO_BOARD_H__
