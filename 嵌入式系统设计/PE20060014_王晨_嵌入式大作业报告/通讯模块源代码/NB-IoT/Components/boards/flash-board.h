/*************************************************************************
  \file				flash-board.h
  \brief
		Target board flash driver implementation.

  \author
		Johnason
  \internal
  	  Revision History:
  		MAY 22, 2018		Johnason - Created

**************************************************************************/
#ifndef __FLASH_BOARD_H__
#define __FLASH_BOARD_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * 							INCLUDES
 */
#include "utilities.h"
#include "SSD_FTFx.h"

/*********************************************************************
 * 							CONSTANTS
 */

#define FTFx_REG_BASE           0x40020000
#define P_FLASH_BASE            0x00000000

/* Program Flash block information */
#define P_FLASH_SIZE            (FSL_FEATURE_FLASH_PFLASH_BLOCK_SIZE * FSL_FEATURE_FLASH_PFLASH_BLOCK_COUNT)
#define P_BLOCK_NUM             FSL_FEATURE_FLASH_PFLASH_BLOCK_COUNT
#define P_SECTOR_SIZE           FTFx_PSECTOR_SIZE
#define P_PAGE_NUM				(P_FLASH_SIZE / P_SECTOR_SIZE)
#define P_WORD_SIZE       		PGM_SIZE_BYTE
// Data Flash block information
#define FLEXNVM_BASE            FSL_FEATURE_FLASH_FLEX_NVM_START_ADDRESS

// Flex Ram block information
#define EERAM_BASE              FSL_FEATURE_FLASH_FLEX_RAM_START_ADDRESS

/* Destination to program security key back to flash location */
#if (FSL_FEATURE_FLASH_PFLASH_BLOCK_WRITE_UNIT_SIZE == 8)
    #define SECURITY_LOCATION         0x408
#else /* FSL_FEATURE_FLASH_PFLASH_BLOCK_WRITE_UNIT_SIZE == 4 */
    #define SECURITY_LOCATION         0x40C
#endif

#define BACKDOOR_KEY_LOCATION     0x400

/* Other defines */
#define DEBUGENABLE               0x00

#define READ_NORMAL_MARGIN        0x00
#define READ_USER_MARGIN          0x01
#define READ_FACTORY_MARGIN       0x02

#define ONE_KB                  1024                        //0x400:  10 zeros
#define TWO_KB                  (2*ONE_KB)
#define THREE_KB                (3*ONE_KB)
#define FOUR_KB                 (4*ONE_KB)
#define FIVE_KB                 (5*ONE_KB)
#define SIX_KB                  (6*ONE_KB)
#define SEVEN_KB                (7*ONE_KB)
#define EIGHT_KB                (8*ONE_KB)
#define NINE_KB                 (9*ONE_KB)
#define TEN_KB                  (10*ONE_KB)
#define ONE_MB                  (ONE_KB*ONE_KB)             //0x100000:     20 zeros
#define ONE_GB                  (ONE_KB*ONE_KB*ONE_KB)      //0x40000000:   30 zeros

/*********************************************************************
 * 							GLOBAL FUNCTIONS
 */
/*
 * @fn      BOARD_flashInit
 *
 * @brief   Initialize the flash driver.
 *
 * @param   none
 *
 * @return  TRUE
 */
uint8_t BOARD_flashInit( void );

/*
 * @fn      BOARD_flashRead
 *
 * @brief   This function reads 'cnt' bytes from the internal flash.
 *
 * @param   pg	-	A valid flash page number.
 * @param	offset	-	A valid offset into the page.
 * @param	buf	-	A valid buffer space at least as big as the 'cnt' parameter.
 * @param	cnt	-	A valid number of bytes to read.
 *
 * @return  none
 */
void BOARD_flashRead(uint8_t pg, uint16_t offset, uint8_t *buf, uint16_t cnt);

/*
 * @fn          BOARD_flashWrite
 *
 * @brief       This function writes 'cnt' bytes to the internal flash.
 *
 * @param   pg	-	A valid flash page number.
 * @param	offset	-	A valid offset into the page, 4-aligned.
 * @param	buf	-	Valid buffer space at least as big as 'cnt' X 4.
 * @param	cnt	-	Number of 4-byte blocks to write.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 */
void BOARD_flashWrite(uint8_t pg, uint16_t offset, uint8_t *buf, uint16_t cnt);

/*
 * @fn          BOARD_flashErase
 *
 * @brief       This function erases the specified page of the internal flash.
 *
 * @param       pg - A valid flash page number to erase.
 *
 * None.
 *
 * @return      None.
 */
void BOARD_flashErase(uint8_t pg);

#ifdef __cplusplus
}
#endif

#endif // __FLASH_BOARD_H__
