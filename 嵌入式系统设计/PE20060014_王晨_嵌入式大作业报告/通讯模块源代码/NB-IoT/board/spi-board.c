/*!
 * \file      spi-board.c
 *
 * \brief     Target board SPI driver implementation
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
#include "board.h"
#include "board-config.h"
#include "gpio.h"
#include "spi-board.h"

#include "fsl_device_registers.h"
#include "fsl_port_hal.h"
#include "fsl_spi_master_driver.h"

static spi_master_state_t spiMasterState;

void SpiInit( Spi_t *obj, SpiId_t spiId, uint8_t slave, uint8_t cpol,
	uint8_t cpha, uint32_t hz)
{
	uint32_t instance = SPI0_IDX;
	spi_master_user_config_t userConfig;
	uint32_t calculatedBaudRate;

    obj->SpiId = spiId;
    /* Now only support SPI0 instance, and CS use GPIO */
    if( spiId == SPI_1 )
    {
    	instance = SPI0_IDX;
        /* Affects PORTC_PCR4 register cs*/
        PORT_HAL_SetMuxMode(PORTC,4u,kPortMuxAlt2);
        /* Affects PORTC_PCR5 register clk */
        PORT_HAL_SetMuxMode(PORTC,5u,kPortMuxAlt2);
        /* Affects PORTC_PCR6 register mosi */
        PORT_HAL_SetMuxMode(PORTC,6u,kPortMuxAlt2);
        /* Affects PORTC_PCR7 register miso */
        PORT_HAL_SetMuxMode(PORTC,7u,kPortMuxAlt2);

        GpioInit( &(obj->Nss), BOARD_SPI_INSTANCE_CS, PIN_OUTPUT, PIN_PUSH_PULL,
        	PIN_NO_PULL, 1 );
    }

    if( slave == 0 )
    {
    	// Now only support master
    	userConfig.bitCount = kSpi8BitMode;
    	userConfig.direction = kSpiMsbFirst;
    	userConfig.polarity = cpol;
    	userConfig.phase = cpha;
    	userConfig.bitsPerSec = hz;
    	// Init and setup baudrate for the master
    	SPI_DRV_MasterInit(instance, &spiMasterState);
    	SPI_DRV_MasterConfigureBus(instance, &userConfig, &calculatedBaudRate);
    }
}

void SpiDeInit( Spi_t *obj )
{
	uint32_t instance = SPI0_IDX;
	if( SPI_1 == obj->SpiId )
	{
	   	instance = SPI0_IDX;
	}
	SPI_DRV_MasterDeinit(instance);
}

uint16_t SpiInOut( Spi_t *obj, uint16_t outData )
{
    uint16_t rxData = 0;
    uint8_t buffer[2];
    uint8_t buffer1[2];
	uint32_t instance = SPI0_IDX;

	if( SPI_1 == obj->SpiId )
	{
	   	instance = SPI0_IDX;
	}

	buffer[0] = (uint8_t)(outData & 0xFF);

	// Start transfer data to slave
	SPI_DRV_MasterTransfer(instance, NULL, buffer, buffer1, 1);
	while (SPI_DRV_MasterGetTransferStatus(instance, NULL) == kStatus_SPI_Busy)
	{}

	rxData = (uint16_t)buffer1[0];

	return( rxData );
}

void SpiWriteBuffer( Spi_t * obj, uint16_t addr, uint8_t *buffer, uint16_t size )
{
    uint16_t i = 0;

    //NSS = 0;
    GpioWrite( &obj->Nss, 0 );

    SpiInOut( obj, addr | 0x80 );
    for( i = 0; i < size; i++ )
    {
        SpiInOut( obj, buffer[i] );
    }

    //NSS = 1;
    GpioWrite( &obj->Nss, 1 );
}

void SpiReadBuffer( Spi_t * obj, uint16_t addr, uint8_t *buffer, uint16_t size )
{
    uint16_t i = 0;

    //NSS = 0;
    GpioWrite( &obj->Nss, 0 );

    SpiInOut( obj, addr & 0x7F );

    for( i = 0; i < size; i++ )
    {
        buffer[i] = SpiInOut( obj, 0 );
    }

    //NSS = 1;
    GpioWrite( &obj->Nss, 1 );
}
