/*!
 * \file      board.h
 *
 * \brief     Target board general functions implementation
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
#ifndef __BOARD_H__
#define __BOARD_H__

#include <stdint.h>

/*!
 * \brief Disable interrupts
 *
 * \remark IRQ nesting is managed
 */
void BoardDisableIrq( void );

/*!
 * \brief Enable interrupts
 *
 * \remark IRQ nesting is managed
 */
void BoardEnableIrq( void );

/*!
 * \brief Initializes the mcu.
 */
void BoardInitMcu( void );

/*!
 * \brief Resets the mcu.
 */
void BoardResetMcu( void );

/*!
 * \brief Initializes the boards peripherals.
 */
void BoardInitPeriph( void );

/* Function to initialize clock base on board configuration. */
void BoardClockInit(void);

/*
 * @fn      BoardInitMcuLPUART
 *
 * @brief   Initializes the LPUART pin MUX.
 *
 * @param   instance	-	the instance id of the LPUART
 *
 * @return  none.
 */
void BoardInitMcuLPUART(uint32_t instance);

/*
 * @fn      BoardInitGpio
 *
 * @brief   Initializes the GPIO pin MUX.
 *
 * @param   instance	-	the instance id of the PORT
 *
 * @return  none.
 */
void BoardInitGpio(uint32_t instance);

/*!
 * \brief De-initializes the target board peripherals to decrease power
 *        consumption.
 */
void BoardDeInitMcu( void );

/*!
 * \brief Measure the Battery voltage
 *
 * \retval value  battery voltage in volts
 */
uint32_t BoardGetBatteryVoltage( void );

/*!
 * \brief Get the current battery level
 *
 * \retval value  battery level [  0: USB,
 *                                 1: Min level,
 *                                 x: level
 *                               254: fully charged,
 *                               255: Error]
 */
uint8_t BoardGetBatteryLevel( void );

/*!
 * Returns a pseudo random seed generated using the MCU Unique ID
 *
 * \retval seed Generated pseudo random seed
 */
uint32_t BoardGetRandomSeed( void );

/*!
 * \brief Gets the board 64 bits unique ID
 *
 * \param [IN] id Pointer to an array that will contain the Unique ID
 */
void BoardGetUniqueId( uint8_t *id );


void WodInit(void);

void Wod_Refresh(void);

#endif // __BOARD_H__
