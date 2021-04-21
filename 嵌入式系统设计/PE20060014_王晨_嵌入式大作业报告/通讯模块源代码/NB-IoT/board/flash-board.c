/*************************************************************************
  \file				flash-board.c
  \brief
		Target board flash driver implementation.

  \author
		Johnason
  \internal
  	  Revision History:
  		MAY 22, 2018		Johnason - Created

**************************************************************************/
/*********************************************************************
 * 							INCLUDES
 */
#include "board-config.h"
#include "flash-board.h"

/*********************************************************************
 * 							CONSTANTS
 */

/*********************************************************************
 * 						LOCAL VARIABLES
 */
pFLASHCOMMANDSEQUENCE g_FlashLaunchCommand = (pFLASHCOMMANDSEQUENCE)0xFFFFFFFF;

/* Flash Standard Software Driver Structure */
static FLASH_SSD_CONFIG flashSSDConfig =
{
    FTFx_REG_BASE,          /* FTFx control register base */
    P_FLASH_BASE,           /* base address of PFlash block */
    P_FLASH_SIZE,           /* size of PFlash block */
    FLEXNVM_BASE,           /* base address of DFlash block */
    0,                      /* size of DFlash block */
    EERAM_BASE,             /* base address of EERAM block */
    0,                      /* size of EEE block */
    DEBUGENABLE,            /* background debug mode enable bit */
    NULL_CALLBACK           /* pointer to callback function */
};
/*********************************************************************
 * 							TYPEDEFINES
 */

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
uint8_t BOARD_flashInit( void )
{
	uint32_t ret;
    ret = FlashInit(&flashSSDConfig);
    if (FTFx_OK != ret)
    {
        return FALSE;
    }

    g_FlashLaunchCommand = FlashCommandSequence;

    return ret;
}

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
void BOARD_flashRead(uint8_t pg, uint16_t offset, uint8_t *buf, uint16_t cnt)
{
	uint8_t * pData = (uint8_t *)(flashSSDConfig.PFlashBase +
			(uint32_t)pg * FTFx_PSECTOR_SIZE + offset);
	while (cnt --)
	{
		*buf ++ = *pData ++;
	}
}

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
void BOARD_flashWrite(uint8_t pg, uint16_t offset, uint8_t *buf, uint16_t cnt)
{
	uint32_t destination;
	/* Start address of Program Once Field */
	destination = flashSSDConfig.PFlashBase + (uint32_t)pg * FTFx_PSECTOR_SIZE + offset;

	FlashProgram(&flashSSDConfig, destination, (uint32_t)cnt * PGM_SIZE_BYTE,
			buf, g_FlashLaunchCommand);
}

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
void BOARD_flashErase(uint8_t pg)
{
	uint32_t destination;
	/* Start address of Program Once Field */
	destination = flashSSDConfig.PFlashBase + (uint32_t)pg * FTFx_PSECTOR_SIZE;

	FlashEraseSector(&flashSSDConfig, destination, FTFx_PSECTOR_SIZE, g_FlashLaunchCommand);
}
