/*
 * This is template for main module created by KSDK Project Generator. Enjoy!
 */

 /*
 * [File Name]     main.h
 * [Platform]      MKL17Z32VFM4
 * [Project]       shuncom_project
 * [Version]       1.00
 * [Author]        6646
 * [Date]          04/11/2019
 * [Language]      'C'
 * [History]       1.00 - Original Release
 *
 */

//-----------------------------------------------------------------------
// KSDK Includes
//-----------------------------------------------------------------------
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_clock_manager.h"
#include "fsl_interrupt_manager.h"
#include "fsl_power_manager.h"
#include "fsl_adc16_driver.h"
#include "fsl_cmp_driver.h"
#include "fsl_cop_driver.h"
#include "fsl_crc_driver.h"
#include "fsl_dma_driver.h"
#include "fsl_flexio_driver.h"
#include "fsl_flexio_i2c_master_driver.h"
#include "fsl_flexio_i2s_driver.h"
#include "fsl_flexio_spi_driver.h"
#include "fsl_flexio_uart_dma_driver.h"
#include "fsl_flexio_uart_driver.h"
#include "fsl_gpio_driver.h"
#include "fsl_i2c_master_driver.h"
#include "fsl_i2c_slave_driver.h"
#include "fsl_lptmr_driver.h"
#include "fsl_lpuart_dma_driver.h"
#include "fsl_lpuart_driver.h"
#include "fsl_pit_driver.h"
#include "fsl_rtc_driver.h"
#include "fsl_smartcard_driver.h"
#if defined(FSL_FEATURE_SOC_EMVSIM_COUNT)
#if (FSL_FEATURE_SOC_EMVSIM_COUNT >= 1)
#include "fsl_smartcard_emvsim_driver.h"
#endif
#endif
#if defined(FSL_FEATURE_UART_HAS_SMART_CARD_SUPPORT)
#if (FSL_FEATURE_UART_HAS_SMART_CARD_SUPPORT == 1)
#include "fsl_smartcard_uart_driver.h"
#endif
#endif
#include "fsl_spi_dma_master_driver.h"
#include "fsl_spi_dma_slave_driver.h"
#include "fsl_spi_master_driver.h"
#include "fsl_spi_slave_driver.h"
#include "fsl_tpm_driver.h"
#include "fsl_uart_dma_driver.h"
#include "fsl_uart_driver.h"
#include "fsl_vref_driver.h"
#if USING_DIRECT_INTERFACE
#include "fsl_smartcard_direct_driver.h"
#endif
#if USING_NCN8025_INTERFACE
#include "fsl_smartcard_ncn8025_driver.h"
#endif

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
