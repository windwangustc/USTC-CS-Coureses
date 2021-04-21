/*************************************************************************
  \file				gpio-board.c
  \brief
		Target board GPIO driver implementation.

  \author
		Johnason
  \internal
  	  Revision History:
  		MAY 19, 2018		Johnason - Created

**************************************************************************/
/*********************************************************************
 * 							INCLUDES
 */
#include "utilities.h"
#include "board-config.h"
#include "rtc-board.h"
#include "osal.h"
#include "gpio-board.h"

#include "hal_drivers.h"
#include "ZGlobals.h"

// the MCU dependency
#include "fsl_gpio_driver.h"
#include "fsl_flexio_driver.h"
#include "fsl_interrupt_manager.h"
/*********************************************************************
 * 							CONSTANTS
 */
#define GPIO_MODE_KEY		GPIO_KEY_1
#define GPIO_RADIO_DIO0		GPIO_KEY_2

#define GPIO_MODE_KEY_IRQ_MASK		0x10
#define GPIO_RADIO_DIO0_IRQ_MASK	0x10
#define GPIO_1301_DIO0_IRQ_MASK		0x02
/*********************************************************************
 * 							TYPEDEFINES
 */

/*********************************************************************
 * 							LOCAL VAIRABLES
 */
static uint16_t gpioBoard_saveKEY = 0;
/*********************************************************************
 * 							API FUNCTIONS
 */
void GpioMcuInit( Gpio_t *obj, PinNames pin, PinModes mode, PinConfigs config, PinTypes type, uint32_t value )
{
    if( pin < IOE_0 )
    {
    	gpio_input_pin_user_config_t inputPin;
    	gpio_output_pin_user_config_t outputPin;

        obj->pin = pin;

        obj->pinIndex = obj->pin % 32;
        obj->portIndex = obj->pin / 32;

        BoardInitGpio(obj->portIndex);

        if(PIN_INPUT == mode)
        {
        	inputPin.pinName = GPIO_MAKE_PIN(obj->portIndex, obj->pinIndex);
        	if(PIN_PULL_UP == type)
        	{
        		inputPin.config.isPullEnable = true;
#if FSL_FEATURE_PORT_HAS_PULL_SELECTION
       			inputPin.config.pullSelect   = kPortPullUp;
#endif
       		}
       		else if (PIN_PULL_DOWN == type)
       		{
       			inputPin.config.isPullEnable = true;
#if FSL_FEATURE_PORT_HAS_PULL_SELECTION
       			inputPin.config.pullSelect   = kPortPullDown;
#endif
       		}
       		else
       		{
       			inputPin.config.isPullEnable = false;
       		}

       		inputPin.config.interrupt = kPortIntDisabled;

       		GPIO_DRV_InputPinInit(&inputPin);
       	}
       	else
       	{
       		outputPin.pinName = GPIO_MAKE_PIN(obj->portIndex, obj->pinIndex);
       		outputPin.config.outputLogic = value;
       		if(PIN_OPEN_DRAIN == config)
       		{
#if FSL_FEATURE_PORT_HAS_OPEN_DRAIN
       			outputPin.config.isOpenDrainEnabled = true;  /*!< Enable or disable open drain.*/
#endif
       		}
       		GPIO_DRV_OutputPinInit(&outputPin);
       	}
    }
    else
    {
#if defined( BOARD_IOE_EXT )
        // IOExt Pin
        GpioIoeInit( obj, pin, mode, config, type, value );
#endif
    }
}

void GpioMcuSetInterrupt( Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority, GpioIrqHandler *irqHandler )
{
    if( obj->pin < IOE_0 )
    {
        uint32_t interruptEdge = kPortIntRisingEdge;
        PORT_Type * portBase = g_portBase[obj->portIndex];

        obj->irqHand = irqHandler;

        if(IRQ_RISING_EDGE == irqMode)
        {
        	interruptEdge = kPortIntRisingEdge;
        }
        else if(IRQ_FALLING_EDGE == irqMode)
        {
        	interruptEdge = kPortIntFallingEdge;
        }
        else if(IRQ_RISING_FALLING_EDGE == irqMode)
        {
        	interruptEdge = kPortIntEitherEdge;
        }
        if(0 == obj->portIndex)
        {
        	INT_SYS_EnableIRQ(PORTA_IRQn);
        }
        else
        {
#if defined(SX1278SZ)
        	INT_SYS_EnableIRQ(PORTBCDE_IRQn);
#elif defined(SX1301SZ)
        	INT_SYS_EnableIRQ(PORTCD_IRQn);
#endif
        }

        PORT_HAL_SetPinIntMode(portBase, obj->pinIndex, interruptEdge);

        /* Configure NVIC */
        if (g_portIrqId[obj->portIndex])
        {
            /* Enable GPIO interrupt.*/
            INT_SYS_EnableIRQ(g_portIrqId[obj->portIndex]);
        }
    }
    else
    {
#if defined( BOARD_IOE_EXT )
        // IOExt Pin
        GpioIoeSetInterrupt( obj, irqMode, irqPriority, irqHandler );
#endif
    }
}

void GpioMcuRemoveInterrupt( Gpio_t *obj )
{
    if( obj->pin < IOE_0 )
    {
        uint32_t interruptEdge = kPortIntDisabled;
        PORT_Type * portBase = g_portBase[obj->portIndex];

        PORT_HAL_SetPinIntMode(portBase, obj->pinIndex, interruptEdge);
        obj->irqHand = NULL;
    }
    else
    {
#if defined( BOARD_IOE_EXT )
        // IOExt Pin
        GpioIoeRemoveInterrupt( obj );
#endif
    }
}

void GpioMcuWrite( Gpio_t *obj, uint32_t value )
{
    if( obj->pin < IOE_0 )
    {
    	GPIO_DRV_WritePinOutput(GPIO_MAKE_PIN(obj->portIndex, obj->pinIndex), value);
    }
    else
    {
#if defined( BOARD_IOE_EXT )
        // IOExt Pin
        GpioIoeWrite( obj, value );
#endif
    }
}

void GpioMcuToggle( Gpio_t *obj )
{
    if( obj->pin < IOE_0 )
    {
    	GPIO_DRV_TogglePinOutput(GPIO_MAKE_PIN(obj->portIndex, obj->pinIndex));
    }
    else
    {
#if defined( BOARD_IOE_EXT )
        // IOExt Pin
        GpioIoeToggle( obj );
#endif
    }
}

uint32_t GpioMcuRead( Gpio_t *obj )
{
    if( obj->pin < IOE_0 )
    {
    	uint32_t direct = GPIO_DRV_GetPinDir(GPIO_MAKE_PIN(obj->portIndex, obj->pinIndex));

    	if(kGpioDigitalInput == direct)
    	{
    		return GPIO_DRV_ReadPinInput(GPIO_MAKE_PIN(obj->portIndex, obj->pinIndex));
    	}
    	else
    	{
//    		return GPIO_DRV_ReadPinOutput(GPIO_MAKE_PIN(obj->portIndex, obj->pinIndex));
    	}
    }
    else
    {
#if defined( BOARD_IOE_EXT )
        // IOExt Pin
        return GpioIoeRead( obj );
#else
        return 0;
#endif
    }
}

void szPort_eventLoop(void)
{
#if defined(SX1278SZ)
	uint32_t portLevel;
#endif
	if(gpioBoard_saveKEY & GPIO_RADIO_DIO0)
	{
#if defined(SX1278SZ)
		SX1278.DIO0.irqHand();
#elif defined(SX1301SZ)
		SX1301_GPIO0.irqHand();
#endif
		gpioBoard_saveKEY &= ~(GPIO_RADIO_DIO0);
	}

#if defined(SX1278SZ)
	if(gpioBoard_saveKEY & GPIO_MODE_KEY)
	{
		portLevel = GPIO_DRV_ReadPinInput(kGpioMode);
		// process the mode logic
		if(1 == portLevel)
		{
			zgWorkMode = 1;
		}
		else
		{
			zgWorkMode = 0;
		}
		gpioBoard_saveKEY &= ~(GPIO_MODE_KEY);;
	}
#endif
}

void PORTA_IRQHandler(void)
{
	uint32_t intIRQ;

	intIRQ = PORT_HAL_GetPortIntFlag(PORTA_BASE_PTR);

	if(intIRQ & GPIO_MODE_KEY_IRQ_MASK)
	{
		// working mode irq
		gpioBoard_saveKEY |= GPIO_MODE_KEY;
	    osal_set_event(hal_taskID, HAL_GPIO_EVENT);
	}
#if defined(SX1301SZ)
	if((intIRQ & GPIO_1301_DIO0_IRQ_MASK) && (SX1301_GPIO0.irqHand))
	{
		gpioBoard_saveKEY |= GPIO_RADIO_DIO0;
	    osal_set_event(hal_taskID, HAL_GPIO_EVENT);
	}
#endif
    /* Clear interrupt flag.*/
    PORT_HAL_ClearPortIntFlag(PORTA_BASE_PTR);
}

/* gpio IRQ handler with the same name in startup code. */
#if defined(SX1278SZ)
void PORTBCDE_IRQHandler(void)
{
	uint32_t intIRQ;

	intIRQ = PORT_HAL_GetPortIntFlag(PORTD_BASE_PTR);

	if((intIRQ & GPIO_RADIO_DIO0_IRQ_MASK) && (SX1278.DIO0.irqHand))
	{
		gpioBoard_saveKEY |= GPIO_RADIO_DIO0;
	    osal_set_event(hal_taskID, HAL_GPIO_EVENT);
	}

    /* Clear interrupt flag.*/
    PORT_HAL_ClearPortIntFlag(PORTD_BASE_PTR);
}
#elif defined(SX1301SZ)
void PORTCD_IRQHandler(void)
{
}
#endif
