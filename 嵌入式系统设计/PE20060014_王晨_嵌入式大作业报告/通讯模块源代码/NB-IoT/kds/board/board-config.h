/*!
 * \file      board-config.h
 *
 * \brief     Board configuration
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 *
 * \author    Daniel Jaeckle ( STACKFORCE )
 *
 * \author    Johannes Bruder ( STACKFORCE )
 */
#ifndef __BOARD_CONFIG_H__
#define __BOARD_CONFIG_H__

#include "fsl_lpuart_driver.h"
#include "fsl_gpio_driver.h"
/*************************** The board clock config ***************************/
#define CLOCK_VLPR 1U
#define CLOCK_RUN  2U
#define CLOCK_NUMBER_OF_CONFIGURATIONS 3U

#ifndef CLOCK_INIT_CONFIG
#define CLOCK_INIT_CONFIG CLOCK_RUN
#endif

#if (CLOCK_INIT_CONFIG == CLOCK_RUN)
#define CORE_CLOCK_FREQ 48000000U
#else
#define CORE_CLOCK_FREQ 2000000U
#endif

/* OSC0 configuration. */
#define OSC0_XTAL_FREQ 32768U
#define OSC0_SC2P_ENABLE_CONFIG  false
#define OSC0_SC4P_ENABLE_CONFIG  false
#define OSC0_SC8P_ENABLE_CONFIG  false
#define OSC0_SC16P_ENABLE_CONFIG false
#define MCG_HGO0   kOscGainLow
#define MCG_RANGE0 kOscRangeLow
#define MCG_EREFS0 kOscSrcOsc

/* EXTAL0 PTA18 */
#define EXTAL0_PORT   PORTA
#define EXTAL0_PIN    18
#define EXTAL0_PINMUX kPortPinDisabled

/* XTAL0 PTA19 */
#define XTAL0_PORT   PORTA
#define XTAL0_PIN    19
#define XTAL0_PINMUX kPortPinDisabled

/*************************** The board module config ***************************/

/* The LPTMR instance defination */
#define BOARD_LPTMR_INSTANCE		0
/* The LPTMR back ground interval 2s */
#define BOARD_LPTMR_DEFAULT_BACKGROUD_TIMER_INTERVAL	2000

/* The UART to use for communication */
#define BOARD_LPUART_INSTANCE		0
#define BOARD_UART_INSTANCE         2

/* The UART baudrate */
#define BOARD_LPUART_BAUD			9650
#define BOARD_UART_BAUD     		38400
/* The UART clock source */
#define BOARD_LPUART_CLOCK_SOURCE	kClockLpuartSrcIrc48M

/* The SPI to use for communication with radio */
#define BOARD_SPI_INSTANCE			0
/* The SPI baudrate */
#define BOARD_SPI_INSTANCE_BAUDRATE	(1000000U)


#define  BOARD_LED_RUN		   	(GPIOC_IDX * 32 + 4)
#define  BOARD_EN_POWER         (GPIOE_IDX * 32 + 0)




#if defined(SX1301SZ)
/* The SPI CS GPIO */
#define BOARD_SPI_INSTANCE_CS		(GPIOC_IDX * 32 + 4)
#define BOARD_LED_RUN				(GPIOE_IDX * 32 + 0)

/* The Radio interrupt GPIO */
#define BOARD_GPIO_DIO0				(GPIOA_IDX * 32 + 1)
#define BOARD_GPIO_DIO1				(GPIOD_IDX * 32 + 5)
#define BOARD_GPIO_RADIO_RESET		(GPIOC_IDX * 32 + 3)

#else
/* The SPI CS GPIO */
#define BOARD_SPI_INSTANCE_CS		(GPIOC_IDX * 32 + 4)

/* The work mode GPIO pin*/
#define BOARD_GPIO_WORKMODE			(GPIOA_IDX * 32 + 4)

/* The Radio interrupt GPIO */
#define BOARD_GPIO_DIO0				(GPIOD_IDX * 32 + 4)
#define BOARD_GPIO_DIO1				(GPIOD_IDX * 32 + 5)
#define BOARD_GPIO_RADIO_RESET		(GPIOC_IDX * 32 + 2)

/* The Board led */
#define BOARD_GPIO_LEDRUN			(GPIOE_IDX * 32 + 0)
#define BOARD_GPIO_LEDBUSY			(GPIOE_IDX * 32 + 1)
#endif
/* NV HAL layer parameters */
#define HAL_NV_PAGE_CNT					2

/************** lora config the default value of factory new start **************/

/*!
 * When set to 1 uses the Over-the-Air activation procedure
 * When set to 0 uses the Personalization activation procedure
 */
#define OVER_THE_AIR_ACTIVATION         1


/*!
 * IEEE Organizationally Unique Identifier ( OUI ) (big endian)
 * \remark This is unique to a company or organization
 */
#define IEEE_OUI                        0x02, 0x00, 0x00

/*!
 * Mote device IEEE EUI (big endian)
 *
 */


#if defined( SX1262DVK1DAS )
#define BOARD_TCXO_WAKEUP_TIME                      5
#else
#define BOARD_TCXO_WAKEUP_TIME                      0
#endif



#endif // __BOARD_CONFIG_H__
