/*!
 * \file      spi-board.h
 *
 * \brief     SPI driver implementation
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
#ifndef __SPI_H__
#define __SPI_H__

#include "gpio.h"

/*!
 * SPI peripheral ID
 */
typedef enum
{
    SPI_1,
    SPI_2,
}SpiId_t;

/*!
 * SPI object type definition
 */
typedef struct Spi_s
{
    SpiId_t SpiId;
    Gpio_t Nss;
}Spi_t;

/*!
 * \brief Initializes the SPI object and MCU peripheral
 *
 * \remark When NSS pin is software controlled set the pin name to NC otherwise
 *         set the pin name to be used.
 *
 * \param [IN] obj  SPI object
 * \param [IN] spiId SPI ID
 * \param [IN] slave When set the peripheral acts in slave mode
 * \param [IN] cpol  Clock polarity
 * \param [IN] cpha  Clock phase
 * \param [IN] hz  SPI clock frequency in hz
 */
void SpiInit( Spi_t *obj, SpiId_t spiId, uint8_t slave, uint8_t cpol,
	uint8_t cpha, uint32_t hz);

/*!
 * \brief De-initializes the SPI object and MCU peripheral
 *
 * \param [IN] obj SPI object
 */
void SpiDeInit( Spi_t *obj );

/*!
 * \brief Sends outData and receives inData
 *
 * \param [IN] obj     SPI object
 * \param [IN] outData Byte to be sent
 * \retval inData      Received byte.
 */
uint16_t SpiInOut( Spi_t *obj, uint16_t outData );

void SpiWriteBuffer( Spi_t * obj, uint16_t addr, uint8_t *buffer, uint16_t size );

void SpiReadBuffer( Spi_t * obj, uint16_t addr, uint8_t *buffer, uint16_t size );
#endif // __SPI_H__
