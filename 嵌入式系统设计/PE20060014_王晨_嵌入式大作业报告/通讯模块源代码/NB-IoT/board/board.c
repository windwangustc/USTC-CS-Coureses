/*!
 * \file      board.c
 *
 * \brief     Target board general functions implementation
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

#include "utilities.h"
#include "gpio.h"
#include "adc.h"
#include "spi.h"
#include "i2c.h"
#include "timer.h"
#include "board-config.h"
#include "rtc-board.h"

#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#include "fsl_os_abstraction.h"
#include "fsl_smc_hal.h"
//#include "fsl_tpm.h"
#include "fsl_port_hal.h"
#include "fsl_sim_hal.h"
#include "fsl_spi_master_driver.h"
#include "fsl_interrupt_manager.h"
#include "fsl_cop_driver.h"
#include "fsl_rcm_hal.h"
#include "board.h"

/*!
 * Unique Devices IDs register set ( STM32L0xxx )
 */
#define         ID1                                 ( 0x1FF80050 )
#define         ID2                                 ( 0x1FF80054 )
#define         ID3                                 ( 0x1FF80064 )
#define COP_INSTANCE        0
/* Configuration for enter VLPR mode. Core clock = 2MHz. */
const clock_manager_user_config_t g_defaultClockConfigVlpr =
{
    .mcgliteConfig =
    {
        .mcglite_mode       = kMcgliteModeLirc8M,   // Work in LIRC_8M mode.
        .irclkEnable        = true,  // MCGIRCLK enable.
        .irclkEnableInStop  = false, // MCGIRCLK disable in STOP mode.
        .ircs               = kMcgliteLircSel2M, // Select LIRC_2M.
        .fcrdiv             = kMcgliteLircDivBy1,    // FCRDIV is 0.
        .lircDiv2           = kMcgliteLircDivBy1,    // LIRC_DIV2 is 0.
        .hircEnableInNotHircMode         = false, // HIRC disable.
    },
    .simConfig =
    {
        .er32kSrc  = kClockEr32kSrcOsc0,   // ERCLK32K selection, use OSC.
        .outdiv1   = 0U,
        .outdiv4   = 1U,
    },
    .oscerConfig =
    {
        .enable       = false, // OSCERCLK disable.
        .enableInStop = false, // OSCERCLK disable in STOP mode.
    }
};

/* Configuration for enter RUN mode. Core clock = 48MHz. */
const clock_manager_user_config_t g_defaultClockConfigRun =
{
    .mcgliteConfig =
    {
        .mcglite_mode        = kMcgliteModeHirc48M,   // Work in HIRC mode.
        .irclkEnable        = false, // MCGIRCLK disable.
        .irclkEnableInStop  = false, // MCGIRCLK disable in STOP mode.
        .ircs               = kMcgliteLircSel2M, // Select LIRC_2M.
        .fcrdiv             = kMcgliteLircDivBy1,    // FCRDIV is 0.
        .lircDiv2           = kMcgliteLircDivBy1,    // LIRC_DIV2 is 0.
        .hircEnableInNotHircMode         = true,  // HIRC disable.
    },
    .simConfig =
    {
        .er32kSrc  = kClockEr32kSrcOsc0,  // ERCLK32K selection, use OSC.
        .outdiv1   = 0U,
        .outdiv4   = 1U,
    },
    .oscerConfig =
    {
        .enable       = false, // OSCERCLK disable.
        .enableInStop = false, // OSCERCLK disable in STOP mode.
    }
};

static void CLOCK_SetBootConfig(clock_manager_user_config_t const* config)
{
    CLOCK_SYS_SetSimConfigration(&config->simConfig);

    CLOCK_SYS_SetOscerConfigration(0, &config->oscerConfig);

    CLOCK_SYS_SetMcgliteMode(&config->mcgliteConfig);

    SystemCoreClock = CORE_CLOCK_FREQ;
}

/* Function to initialize OSC0 base on board configuration. */
void BOARD_InitOsc0(void)
{
    // OSC0 configuration.
    osc_user_config_t osc0Config =
    {
        .freq                = OSC0_XTAL_FREQ,
        .hgo                 = MCG_HGO0,
        .range               = MCG_RANGE0,
        .erefs               = MCG_EREFS0,
        .enableCapacitor2p   = OSC0_SC2P_ENABLE_CONFIG,
        .enableCapacitor4p   = OSC0_SC4P_ENABLE_CONFIG,
        .enableCapacitor8p   = OSC0_SC8P_ENABLE_CONFIG,
        .enableCapacitor16p  = OSC0_SC16P_ENABLE_CONFIG,
    };

    CLOCK_SYS_OscInit(0U, &osc0Config);
}

void BoardDisableIrq( void )
{
	INT_SYS_DisableIRQGlobal();
}

void BoardEnableIrq( void )
{
	INT_SYS_EnableIRQGlobal();
}

/* Initialize clock. */
void BoardClockInit(void)
{
    /* enable clock for PORTs */
    CLOCK_SYS_EnablePortClock(PORTA_IDX);
    CLOCK_SYS_EnablePortClock(PORTB_IDX);
    CLOCK_SYS_EnablePortClock(PORTC_IDX);
    CLOCK_SYS_EnablePortClock(PORTD_IDX);
    CLOCK_SYS_EnablePortClock(PORTE_IDX);

    /* Set allowed power mode, allow all. */
    SMC_HAL_SetProtection(SMC, kAllowPowerModeAll);

    /* Setup board clock source. */
    // Setup OSC0 if used.
    // Configure OSC0 pin mux.
    PORT_HAL_SetMuxMode(EXTAL0_PORT, EXTAL0_PIN, EXTAL0_PINMUX);
    PORT_HAL_SetMuxMode(XTAL0_PORT, XTAL0_PIN, XTAL0_PINMUX);
    BOARD_InitOsc0();

    /* Set system clock configuration. */
#if (CLOCK_INIT_CONFIG == CLOCK_VLPR)
    CLOCK_SetBootConfig(&g_defaultClockConfigVlpr);
#else
    CLOCK_SetBootConfig(&g_defaultClockConfigRun);
#endif
}

void BoardInitMcu( void )
{
    RtcInit( );
}

void BoardDeInitMcu( void )
{

}

/*
 * @fn      BoardInitMcuLPUART
 *
 * @brief   Initializes the LPUART pin MUX.
 *
 * @param   instance	-	the instance id of the LPUART
 *
 * @return  none.
 */
void BoardInitMcuLPUART(uint32_t instance)
{
	switch(instance)
	{
		case LPUART0_IDX:         /* LPUART0 */
			/* Affects PORTA_PCR1 register */
			PORT_HAL_SetMuxMode(PORTD,6u,kPortMuxAlt3);
			/* Affects PORTA_PCR2 register */
			PORT_HAL_SetMuxMode(PORTD,7u,kPortMuxAlt3);
			break;
		case UART2_IDX:         /* UART2 */
			/* Affects PORTA_PCR1 register */
			PORT_HAL_SetMuxMode(PORTD,4u,kPortMuxAlt3);
			/* Affects PORTA_PCR2 register */
			PORT_HAL_SetMuxMode(PORTD,5u,kPortMuxAlt3);
			break;
		default:
			break;
	}
}

/*
 * @fn      BoardInitGpio
 *
 * @brief   Initializes the GPIO pin MUX.
 *
 * @param   instance	-	the instance id of the PORT
 *
 * @return  none.
 */
void BoardInitGpio(uint32_t instance)
{
	switch(instance)
	{
		case PORTA_IDX:                      /* PORTA_IDX */
			/* Affects PORTA_PCR4 register */
		    PORT_HAL_SetPassiveFilterCmd(PORTA,4u,false);
		    PORT_HAL_SetMuxMode(PORTA,4u,kPortMuxAsGpio);
		    PORT_HAL_SetPullMode(PORTA,4u,kPortPullUp);
		    PORT_HAL_SetPullCmd(PORTA,4u,true);
			/* Affects PORTA_PCR1 register */
		    PORT_HAL_SetPassiveFilterCmd(PORTA,1u,false);
		    PORT_HAL_SetMuxMode(PORTA,1u,kPortMuxAsGpio);
		    PORT_HAL_SetPullMode(PORTA,1u,kPortPullUp);
		    PORT_HAL_SetPullCmd(PORTA,1u,true);
			break;
	    case PORTC_IDX:                      /* PORTC_IDX */
	    	/* PORTC_PCR1 */
	    	PORT_HAL_SetMuxMode(PORTC,1u,kPortMuxAsGpio);
	    	/* Affects PORTC_PCR2 register */
	    	PORT_HAL_SetPassiveFilterCmd(PORTC,2u,false);
	    	PORT_HAL_SetMuxMode(PORTC,2u,kPortMuxAsGpio);
	    	PORT_HAL_SetPullMode(PORTC,2u,kPortPullUp);
	    	PORT_HAL_SetPullCmd(PORTC,2u,true);
	    	/* Affects PORTC_PCR3 register */
	    	PORT_HAL_SetPassiveFilterCmd(PORTC,3u,false);
	    	PORT_HAL_SetMuxMode(PORTC,3u,kPortMuxAsGpio);
	    	PORT_HAL_SetPullMode(PORTC,3u,kPortPullUp);
	    	PORT_HAL_SetPullCmd(PORTC,3u,true);
	    	/* Affects PORTC_PCR4 register */
	    	PORT_HAL_SetPassiveFilterCmd(PORTC,4u,false);
	    	PORT_HAL_SetMuxMode(PORTC,4u,kPortMuxAsGpio);
	    	PORT_HAL_SetPullMode(PORTC,4u,kPortPullUp);
	    	PORT_HAL_SetPullCmd(PORTC,4u,true);
	      break;
	    case PORTE_IDX:
	    	/* Affects PORTE_PCR0 register */
	    	PORT_HAL_SetPassiveFilterCmd(PORTE,0u,false);
	    	PORT_HAL_SetMuxMode(PORTE,0u,kPortMuxAsGpio);
	    	PORT_HAL_SetPullMode(PORTE,0u,kPortPullUp);
	    	PORT_HAL_SetPullCmd(PORTE,0u,true);
	    	break;
		default:
			break;
	}
}
uint32_t BoardGetRandomSeed( void )
{
	uint32_t id1 = ID1;
	uint32_t id2 = ID2;
	uint32_t id3 = ID3;
    return ( ( *( uint32_t* )&id1 ) ^ ( *( uint32_t* )&id2 ) ^ ( *( uint32_t* )&id3 ) );
}

uint16_t BoardBatteryMeasureVolage( void )
{
    return 0;
}

uint32_t BoardGetBatteryVoltage( void )
{
    return 0;
}

uint8_t BoardGetBatteryLevel( void )
{
    return 0;
}

#if 1
void WodInit(void)
{
	  cop_config_t copInit =
		{
			.copWindowModeEnable = (uint8_t)false,
	#if FSL_FEATURE_COP_HAS_LONGTIME_MODE
			.copTimeoutMode      = kCopShortTimeoutMode,
			.copStopModeEnable   = (uint8_t)false,
			.copDebugModeEnable  = (uint8_t)false,
	#endif
			.copClockSource      = kCopLpoClock,
			// COP reset after about 1s.
			.copTimeout          = kCopTimeout_short_2to10_or_long_2to18
		};

	  if (RCM_HAL_GetSrcStatus(RCM, kRcmWatchDog) == kRcmWatchDog)
	 {
		// PRINTF("\r\n COP reset the chip successfully\r\n");
	 }

	 // Init COP module.
	 COP_DRV_Init(COP_INSTANCE, &copInit);
//	 COP_DRV_IsRunning(1);

}

void Wod_Refresh(void)
{
	 COP_DRV_Refresh(COP_INSTANCE);
}

#endif
