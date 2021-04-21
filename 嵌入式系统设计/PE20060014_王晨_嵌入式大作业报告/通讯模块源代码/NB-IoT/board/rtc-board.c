/*!
 * \file      rtc-board.c
 *
 * \brief     Target board RTC timer and low power modes management.
 * 				The timer is set 2 s when initialized and no timer started.
 * 				And set other value according to the application.
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
#include <math.h>
#include "utilities.h"
#include "board.h"
#include "board-config.h"
#include "osal.h"
#include "timer.h"
#include "gpio.h"
#include "uart.h"
#include "rtc-board.h"
#include "hal_drivers.h"

#include "fsl_lptmr_driver.h"

/*! Macro to convert a microsecond period to raw count value */
#define USEC_TO_COUNT(us, clockFreqInHz) (uint64_t)((uint64_t)us * clockFreqInHz / 1000000U)
/*! Macro to convert a raw count value to microsecond */
#define COUNT_TO_USEC(count, clockFreqInHz) (uint64_t)((uint64_t)count * 1000000U / clockFreqInHz)

/*! Macro to convert a millisecond period to raw count value */
#define MSEC_TO_COUNT(ms, clockFreqInHz) (uint64_t)((uint64_t)ms * clockFreqInHz / 1000U)
/*! Macro to convert a raw count value to millisecond */
#define COUNT_TO_MSEC(count, clockFreqInHz) (uint64_t)((uint64_t)count * 1000U / clockFreqInHz)

/*!
 * This variable is used to record the total time from last set the timeout
 */
volatile uint32_t Timer_currentInterval = 0;
/*!
 * This variable is used to record the total time from startup
 */
volatile uint32_t Timer_totalTimeS = 0;
volatile uint32_t Timer_totalTimeMS = 0;
/* This variable is used to record the last time to set timeout */
volatile uint32_t Timer_lastSetTimeout = 0;

lptmr_state_t lptmrState;
//extern Gpio_t gpio_ledRun;
extern Uart_t appConcentrator_Uart;
/*!
 * @brief LPTMR interrupt callback
 */
void lptmr_isr_callback(void)
{
	uint32_t result;
    /* the total time */
    Timer_totalTimeS += Timer_currentInterval / 1000;
    Timer_totalTimeMS += Timer_currentInterval % 1000;

    if(Timer_totalTimeMS >= 1000)
    {
    	Timer_totalTimeMS -= 1000;
    	Timer_totalTimeS += 1;
    }
    /* restart the default timer */
    /* record the current interval */
    Timer_currentInterval = BOARD_LPTMR_DEFAULT_BACKGROUD_TIMER_INTERVAL;
    result = (uint64_t)((uint64_t)Timer_currentInterval * 1000U);
    // Set the timer period
    LPTMR_DRV_SetTimerPeriodUs(BOARD_LPTMR_INSTANCE, result);
    // Start counting
    LPTMR_DRV_Start(BOARD_LPTMR_INSTANCE);

    osal_set_event(hal_taskID, HAL_TIMER_EVENT);
}

void RtcInit( void )
{
	// Configure LPTMR.
    lptmr_user_config_t lptmrUserConfig =
    {
        .timerMode            = kLptmrTimerModeTimeCounter, /*! Use LPTMR in Time Counter mode */
        .freeRunningEnable    = false, /*! When hit compare value, set counter back to zero */
        .prescalerEnable      = false, /*! bypass prescaler */
        .prescalerClockSource = kClockLptmrSrcLpoClk, /*! use 1kHz Low Power Clock */
        .isInterruptEnabled   = true
    };
    /* record the current interval */
    Timer_currentInterval = BOARD_LPTMR_DEFAULT_BACKGROUD_TIMER_INTERVAL;
    // Initialize LPTMR
    LPTMR_DRV_Init(BOARD_LPTMR_INSTANCE, &lptmrState, &lptmrUserConfig);
    // Set the timer period for 1 second
    LPTMR_DRV_SetTimerPeriodUs(BOARD_LPTMR_INSTANCE, Timer_currentInterval * 1000);
    // Specify the callback function when a LPTMR interrupt occurs
    LPTMR_DRV_InstallCallback(BOARD_LPTMR_INSTANCE, lptmr_isr_callback);
    // Start counting
    LPTMR_DRV_Start(BOARD_LPTMR_INSTANCE);
}

uint8_t RtcSetTimeout( uint32_t timeout )
{
	uint32_t result;
	lptmr_status_t retResult;

	//GpioWrite(&gpio_ledRun, 0);
	RtcAdjustSystemTime();
    /* record the current interval */
    Timer_currentInterval = timeout;
    /* record the time of set timeout */
    Timer_lastSetTimeout = RtcGetTimerValue();
    result = (uint64_t)((uint64_t)timeout * 1000U);
    // Set the timer period
    retResult = LPTMR_DRV_SetTimerPeriodUs(BOARD_LPTMR_INSTANCE, result);
    if(kStatus_LPTMR_TimerPeriodUsTooSmall == retResult)
    {
    	result += 3000;
    	retResult = LPTMR_DRV_SetTimerPeriodUs(BOARD_LPTMR_INSTANCE, result);
    }
    // Start counting
    LPTMR_DRV_Start(BOARD_LPTMR_INSTANCE);
    //GpioWrite(&gpio_ledRun, 1);
    return (uint8_t)kStatus_LPTMR_Success;
}

/*
 * @fn      RtcAdjustSystemTime
 *
 * @brief   Adjust the system time. Need disable interrupt.
 *
 * @param   none
 *
 * @return  none
 */
void RtcAdjustSystemTime(void)
{
	uint32_t result = LPTMR_DRV_GetCurrentTimeUs(BOARD_LPTMR_INSTANCE) / 1000U;
	LPTMR_DRV_Stop(BOARD_LPTMR_INSTANCE);
	/* the total time */
    Timer_totalTimeS += result / 1000;
    Timer_totalTimeMS += result % 1000;

    if(Timer_totalTimeMS >= 1000)
    {
    	Timer_totalTimeMS -= 1000;
    	Timer_totalTimeS += 1;
    }
}

TimerTime_t RtcGetAdjustedTimeoutValue( uint32_t timeout )
{
    return  timeout;
}

TimerTime_t RtcGetTimerValue( void )
{
	uint64_t result = Timer_totalTimeS;

	result *= 1000;
	result += Timer_totalTimeMS;

	result += LPTMR_DRV_GetCurrentTimeUs(BOARD_LPTMR_INSTANCE) / 1000U;

    return (uint32_t)(result % 0xFFFFFFFF);
}

/* Get the elapsed time from the last setTimeOut */
TimerTime_t RtcGetElapsedAlarmTime( void )
{
	uint32_t result = RtcGetTimerValue();
	if(result < Timer_lastSetTimeout)
	{
		return (result + (0xFFFFFFFF - Timer_lastSetTimeout));
	}
	else
	{
		return (result - Timer_lastSetTimeout);
	}
}

TimerTime_t RtcComputeFutureEventTime( TimerTime_t futureEventInTime )
{
	uint32_t result = RtcGetTimerValue( );
	if(result < futureEventInTime)
	{
		return( futureEventInTime - result );
	}
	else
	{
		return 0;
	}
}

TimerTime_t RtcComputeElapsedTime( TimerTime_t eventInTime )
{
	uint64_t result = RtcGetTimerValue();
	uint32_t result1;

	if(0 == eventInTime)
	{
		return result;
	}

	result1 = (uint32_t)(result % 0xFFFFFFFF);

	if(result1 < eventInTime)
	{
		return (result1 + (0xFFFFFFFF - eventInTime));
	}
	else
	{
		return (result1 - eventInTime);
	}
}
