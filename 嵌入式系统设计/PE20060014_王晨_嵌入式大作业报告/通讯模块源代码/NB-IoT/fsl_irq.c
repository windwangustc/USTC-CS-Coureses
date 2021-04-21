/*************************************************************************
  \file				fsl_irq.c
  \brief
	Source file of the interrupt handler.

  \author
	www.shuncom.com
  \internal
  Revision History:
  	Dec. 13, 2017		Johnason - Created

**************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "fsl_lptmr_driver.h"
#include "fsl_spi_shared_function.h"
#include "fsl_tpm_driver.h"
#include "fsl_lpuart_driver.h"
#include "board.h"

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
extern void LPUART_DRV_IRQHandler(uint32_t instance);
extern void UART_DRV_IRQHandler(uint32_t instance);
/******************************************************************************
 * Code
 *****************************************************************************/

#if FSL_FEATURE_LPTMR_HAS_SHARED_IRQ_HANDLER
/* LPTMR IRQ handler that would cover the same name's APIs in startup code */
void LPTMR0_LPTMR1_IRQHandler(void)
{
    if (g_lptmrStatePtr[0] != NULL)
    {
        LPTMR_DRV_IRQHandler(0U);
    }
    if (g_lptmrStatePtr[1] != NULL)
    {
        LPTMR_DRV_IRQHandler(1U);
    }
}

#else

#if (FSL_FEATURE_SOC_LPTMR_COUNT > 0U)
void LPTMR0_IRQHandler(void)
{
    LPTMR_DRV_IRQHandler(0U);
}
#endif

#if (FSL_FEATURE_SOC_LPTMR_COUNT > 1U)
void LPTMR1_IRQHandler(void)
{
    LPTMR_DRV_IRQHandler(1U);
}
#endif

#endif /* FSL_FEATURE_LPTMR_HAS_SHARED_IRQ_HANDLER */

#if (SPI_INSTANCE_COUNT == 1)
/*!
 * @brief This function is the implementation of SPI0 handler named in startup code.
 *
 * It passes the instance to the shared SPI IRQ handler.
 */
void SPI0_IRQHandler(void)
{
   SPI_DRV_IRQHandler(SPI0_IDX);
}

#else
/*!
 * @brief This function is the implementation of SPI0 handler named in startup code.
 *
 * It passes the instance to the shared SPI IRQ handler.
 */
void SPI0_IRQHandler(void)
{
//   SPI_DRV_IRQHandler(SPI0_IDX);
}

/*!
 * @brief This function is the implementation of SPI1 handler named in startup code.
 *
 * It passes the instance to the shared SPI IRQ handler.
 */
void SPI1_IRQHandler(void)
{
//   SPI_DRV_IRQHandler(SPI1_IDX);
}
#endif

void UART2_FLEXIO_IRQHandler(void)
{
	UART_DRV_IRQHandler(2U);
}


#if (LPUART_INSTANCE_COUNT > 0)
/* Implementation of LPUART0 handler named in startup code. */
void LPUART0_IRQHandler(void)
{
    LPUART_DRV_IRQHandler(0);
}
#endif

#if (LPUART_INSTANCE_COUNT > 1)
/* Implementation of LPUART1 handler named in startup code. */
void LPUART1_IRQHandler(void)
{
    LPUART_DRV_IRQHandler(1);
}
#endif

/******************************************************************************
 * EOF
 *****************************************************************************/

