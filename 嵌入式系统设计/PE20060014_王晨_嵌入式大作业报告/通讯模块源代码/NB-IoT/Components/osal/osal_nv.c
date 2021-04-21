/*************************************************************************
  \file				osal_nv.c
  \brief
	Source file to defines the osal non-volatile memory functions.

  \author
	www.shuncom.com
  \internal
  Revision History:
  	Dec. 13, 2017		Johnason - Created

**************************************************************************/

/*********************************************************************
 * 							INCLUDES
 */
#include "board.h"
#include "OSAL_Nv.h"
#include "ZComDef.h"
#include "board-config.h"
#include "flash-board.h"
/*********************************************************************
 * 							CONSTANTS
 */

#define OSAL_NV_ACTIVE          0x00
#define OSAL_NV_ERASED          0xFF
#define OSAL_NV_ERASED_ID       0xFFFF
#define OSAL_NV_ZEROED_ID       0x0000
// Reserve MSB of Id to signal a search for the "old" source copy (new write interrupted/failed.)
#define OSAL_NV_SOURCE_ID       0x8000

// In case pages 0-1 are ever used, define a null page value.
#define OSAL_NV_PAGE_NULL       0

// In case item Id 0 is ever used, define a null item value.
#define OSAL_NV_ITEM_NULL       0

#define OSAL_NV_WORD_SIZE       P_WORD_SIZE

#define OSAL_NV_PAGE_HDR_OFFSET 0

#define OSAL_NV_PAGE_SIZE       P_SECTOR_SIZE
#define OSAL_NV_PAGES_USED      HAL_NV_PAGE_CNT
#define OSAL_NV_PAGE_BEG        (P_PAGE_NUM - OSAL_NV_PAGES_USED)
#define OSAL_NV_PAGE_END		(OSAL_NV_PAGE_BEG + OSAL_NV_PAGES_USED - 1)

#define OSAL_NV_MAX_HOT         2

/*********************************************************************
 * 							MACROS
 */
//#define OSAL_NV_CHECK_BUS_VOLTAGE  HalAdcCheckVdd(VDD_MIN_NV)

#define OSAL_NV_DATA_SIZE( LEN )                      \
  (((LEN) >= ((uint16_t)(65536UL - OSAL_NV_WORD_SIZE))) ? \
             ((uint16_t)(65536UL - OSAL_NV_WORD_SIZE))  : \
             ((((LEN) + OSAL_NV_WORD_SIZE - 1) / OSAL_NV_WORD_SIZE) * OSAL_NV_WORD_SIZE))

#define OSAL_NV_ITEM_SIZE( LEN )                                         \
  (((LEN) >= ((uint16_t)(65536UL - OSAL_NV_WORD_SIZE - OSAL_NV_HDR_SIZE))) ? \
             ((uint16_t)(65536UL - OSAL_NV_WORD_SIZE))                     : \
  (((((LEN) + OSAL_NV_WORD_SIZE - 1) / OSAL_NV_WORD_SIZE) * OSAL_NV_WORD_SIZE) + OSAL_NV_HDR_SIZE))

/* In order to recover from a page compaction that is interrupted,
 * the logic in osal_nv_init() depends upon the following order:
 * 1. State of the target of compaction is changed to ePgInUse.
 * 2. Compacted page is erased.
 */
#define COMPACT_PAGE_CLEANUP( COM_PG ) { \
  setPageUse( pgRes, TRUE );  /* Mark the reserve page as being in use. */\
  erasePage( (COM_PG) ); \
  pgRes = (COM_PG);           /* Set the reserve page to be the newly erased page. */\
}

/*********************************************************************
 * 							TYPEDEFS
 */

typedef struct
{
  uint16_t id;
  uint16_t len;   // Enforce Flash-WORD size on len.
  uint16_t chk;   // Byte-wise checksum of the 'len' data bytes of the item.
  uint16_t stat;  // Item status.
} osalNvHdr_t;
// Struct member offsets.
#define OSAL_NV_HDR_ID    0
#define OSAL_NV_HDR_LEN   2
#define OSAL_NV_HDR_CHK   4
#define OSAL_NV_HDR_STAT  6

#define OSAL_NV_HDR_ITEM  2  // Length of any item of a header struct.
#define OSAL_NV_HDR_SIZE  8
#define OSAL_NV_HDR_HALF (OSAL_NV_HDR_SIZE / 2)

typedef struct
{
	uint16_t active;
	uint16_t inUse;
	uint16_t xfer;
	uint16_t spare;
} osalNvPgHdr_t;
// Struct member offsets.
#define OSAL_NV_PG_ACTIVE 0
#define OSAL_NV_PG_INUSE  2
#define OSAL_NV_PG_XFER   4
#define OSAL_NV_PG_SPARE  6

#define OSAL_NV_PAGE_HDR_SIZE  8
#define OSAL_NV_PAGE_HDR_HALF (OSAL_NV_PAGE_HDR_SIZE / 2)

typedef enum
{
  eNvXfer,
  eNvZero
} eNvHdrEnum;

typedef enum
{
  ePgActive,
  ePgInUse,
  ePgXfer,
  ePgSpare
} ePgHdrEnum;

/*********************************************************************
 * 						GLOBAL VARIABLES
 */

/*********************************************************************
 * 						LOCAL VARIABLES
 */

// Offset into the page of the first available erased space.
static uint16_t pgOff[OSAL_NV_PAGES_USED];

// Count of the bytes lost for the zeroed-out items.
static uint16_t pgLost[OSAL_NV_PAGES_USED];

static uint8_t pgRes;  // Page reserved for item compacting transfer.

// Saving ~100 code bytes to move a uint8_t* parameter/return value from findItem() to a global.
static uint8_t findPg;

// NV page and offsets for hot items.
static uint8_t hotPg[OSAL_NV_MAX_HOT];
static uint16_t hotOff[OSAL_NV_MAX_HOT];

/*********************************************************************
 * 						LOCAL FUNCTIONS
 */
static uint8_t  initNV( void );

static void   setPageUse( uint8_t pg, uint8_t inUse );
static uint16_t initPage( uint8_t pg, uint16_t id, uint8_t findDups );
static void   erasePage( uint8_t pg );
static uint8_t  compactPage( uint8_t srcPg, uint16_t skipId );

static uint16_t findItem( uint16_t id );
static uint8_t  initItem( uint8_t flag, uint16_t id, uint16_t len, void *buf );
static void   setItem( uint8_t pg, uint16_t offset, eNvHdrEnum stat );
static uint16_t setChk( uint8_t pg, uint16_t offset, uint16_t chk );

static uint16_t calcChkB( uint16_t len, uint8_t *buf );
static uint16_t calcChkF( uint8_t pg, uint16_t offset, uint16_t len );

static void   writeWord( uint8_t pg, uint16_t offset, uint8_t *buf );
static void   writeWordH( uint8_t pg, uint16_t offset, uint8_t *buf );
static void   writeWordM( uint8_t pg, uint16_t offset, uint8_t *buf, uint16_t cnt );
static void   writeBuf( uint8_t pg, uint16_t offset, uint16_t len, uint8_t *buf );
static void   xferBuf( uint8_t srcPg, uint16_t srcOff, uint8_t dstPg, uint16_t dstOff, uint16_t len );

static uint8_t  writeItem( uint8_t pg, uint16_t id, uint16_t len, void *buf, uint8_t flag );
static uint8_t  hotItem(uint16_t id);
static void   hotItemUpdate(uint8_t pg, uint16_t off, uint16_t id);

/*********************************************************************
 * @fn      initNV
 *
 * @brief   Initialize the NV flash pages.
 *
 * @param   none
 *
 * @return  TRUE
 */
static uint8_t initNV( void )
{
  osalNvPgHdr_t pgHdr;
  uint8_t oldPg = OSAL_NV_PAGE_NULL;
  uint8_t findDups = FALSE;
  uint8_t pg;

  pgRes = OSAL_NV_PAGE_NULL;

  for ( pg = OSAL_NV_PAGE_BEG; pg <= OSAL_NV_PAGE_END; pg++ )
  {
    BOARD_flashRead(pg, OSAL_NV_PAGE_HDR_OFFSET, (uint8_t *)(&pgHdr), OSAL_NV_HDR_SIZE);

    if ( pgHdr.active == OSAL_NV_ERASED_ID )
    {
      if ( pgRes == OSAL_NV_PAGE_NULL )
      {
        pgRes = pg;
      }
      else
      {
        setPageUse( pg, TRUE );
      }
    }
    // An Xfer from this page was in progress.
    else if ( pgHdr.xfer != OSAL_NV_ERASED_ID )
    {
      oldPg = pg;
    }
  }

  // If a page compaction was interrupted before the old page was erased.
  if ( oldPg != OSAL_NV_PAGE_NULL )
  {
    /* Interrupted compaction before the target of compaction was put in use;
     * so erase the target of compaction and start again.
     */
    if ( pgRes != OSAL_NV_PAGE_NULL )
    {
      erasePage( pgRes );
      (void)compactPage( oldPg, OSAL_NV_ITEM_NULL );
    }
    /* Interrupted compaction after the target of compaction was put in use,
     * but before the old page was erased; so erase it now and create a new reserve page.
     */
    else
    {
      erasePage( oldPg );
      pgRes = oldPg;
    }
  }
  else if ( pgRes != OSAL_NV_PAGE_NULL )
  {
    erasePage( pgRes );  // The last page erase could have been interrupted by a power-cycle.
  }
  /* else if there is no reserve page, COMPACT_PAGE_CLEANUP() must have succeeded to put the old
   * reserve page (i.e. the target of the compacted items) into use but got interrupted by a reset
   * while trying to erase the page to be compacted. Such a page should only contain duplicate items
   * (i.e. all items will be marked 'Xfer') and thus should have the lost count equal to the page
   * size less the page header.
   */

  for ( pg = OSAL_NV_PAGE_BEG; pg <= OSAL_NV_PAGE_END; pg++ )
  {
    // Calculate page offset and lost bytes - any "old" item triggers an N^2 re-scan from start.
    if ( initPage( pg, OSAL_NV_ITEM_NULL, findDups ) != OSAL_NV_ITEM_NULL )
    {
      findDups = TRUE;
      pg = (OSAL_NV_PAGE_BEG - 1);  // Pre-decrement so that loop increment will start over at zero.
      continue;
    }
  }

  if (findDups)
  {
    // Final pass to calculate page lost after invalidating duplicate items.
    for ( pg = OSAL_NV_PAGE_BEG; pg <= OSAL_NV_PAGE_END; pg++ )
    {
      (void)initPage( pg, OSAL_NV_ITEM_NULL, FALSE );
    }
  }

  if ( pgRes == OSAL_NV_PAGE_NULL )
  {
    uint8_t idx, mostLost = 0;

    for ( idx = 0; idx < OSAL_NV_PAGES_USED; idx++ )
    {
      // Is this the page that was compacted?
      if (pgLost[idx] == (OSAL_NV_PAGE_SIZE - OSAL_NV_PAGE_HDR_SIZE))
      {
        mostLost = idx;
        break;
      }
      /* This check is not expected to be necessary because the above test should always succeed
       * with an early loop exit.
       */
      else if (pgLost[idx] > pgLost[mostLost])
      {
        mostLost = idx;
      }
    }

    pgRes = mostLost + OSAL_NV_PAGE_BEG;
    erasePage( pgRes );  // The last page erase had been interrupted by a power-cycle.
  }

  return TRUE;
}

/*********************************************************************
 * @fn      setPageUse
 *
 * @brief   Set page header active/inUse state according to 'inUse'.
 *
 * @param   pg - Valid NV page to verify and init.
 * @param   inUse - Boolean TRUE if inUse, FALSE if only active.
 *
 * @return  none
 */
static void setPageUse( uint8_t pg, uint8_t inUse )
{
  osalNvPgHdr_t pgHdr;

  pgHdr.active = OSAL_NV_ZEROED_ID;

  if ( inUse )
  {
    pgHdr.inUse = OSAL_NV_ZEROED_ID;
  }
  else
  {
    pgHdr.inUse = OSAL_NV_ERASED_ID;
  }

  writeWord( pg, OSAL_NV_PAGE_HDR_OFFSET, (uint8_t*)(&pgHdr) );
}

/*********************************************************************
 * @fn      initPage
 *
 * @brief   Walk the page items; calculate checksums, lost bytes & page offset.
 *
 * @param   pg - Valid NV page to verify and init.
 * @param   id - Valid NV item Id to use function as a "findItem".
 *               If set to NULL then just perform the page initialization.
 * @param   findDups - TRUE on recursive call from initNV() to find and zero-out duplicate items
 *                     left from a write that is interrupted by a reset/power-cycle.
 *                     FALSE otherwise.
 *
 * @return  If 'id' is non-NULL and good checksums are found, return the offset
 *          of the data corresponding to item Id; else OSAL_NV_ITEM_NULL.
 */
static uint16_t initPage( uint8_t pg, uint16_t id, uint8_t findDups )
{
  uint16_t offset = OSAL_NV_PAGE_HDR_SIZE;
  uint16_t sz, lost = 0;
  osalNvHdr_t hdr;

  do
  {
    BOARD_flashRead(pg, offset, (uint8_t *)(&hdr), OSAL_NV_HDR_SIZE);

    if ( hdr.id == OSAL_NV_ERASED_ID )
    {
      break;
    }

    // Get the actual size in bytes which is the ceiling(hdr.len)
    sz = OSAL_NV_DATA_SIZE( hdr.len );

    // A bad 'len' write has blown away the rest of the page.
    if (sz > (OSAL_NV_PAGE_SIZE - OSAL_NV_HDR_SIZE - offset))
    {
      lost += (OSAL_NV_PAGE_SIZE - offset);
      offset = OSAL_NV_PAGE_SIZE;
      break;
    }

    offset += OSAL_NV_HDR_SIZE;

    if ( hdr.id != OSAL_NV_ZEROED_ID )
    {
      /* This trick allows function to do double duty for findItem() without
       * compromising its essential functionality at powerup initialization.
       */
      if ( id != OSAL_NV_ITEM_NULL )
      {
        /* This trick allows asking to find the old/transferred item in case
         * of a successful new item write that gets interrupted before the
         * old item can be zeroed out.
         */
        if ( (id & 0x7fff) == hdr.id )
        {
          if ( (((id & OSAL_NV_SOURCE_ID) == 0) && (hdr.stat == OSAL_NV_ERASED_ID)) ||
               (((id & OSAL_NV_SOURCE_ID) != 0) && (hdr.stat != OSAL_NV_ERASED_ID)) )
          {
            return offset;
          }
        }
      }
      // When invoked from the osal_nv_init(), verify checksums and find & zero any duplicates.
      else
      {
        if ( hdr.chk == calcChkF( pg, offset, hdr.len ) )
        {
          if ( findDups )
          {
            if ( hdr.stat == OSAL_NV_ERASED_ID )
            {
              /* The trick of setting the MSB of the item Id causes the logic
               * immediately above to return a valid page only if the header 'stat'
               * indicates that it was the older item being transferred.
               */
              uint16_t off = findItem( (hdr.id | OSAL_NV_SOURCE_ID) );

              if ( off != OSAL_NV_ITEM_NULL )
              {
                setItem( findPg, off, eNvZero );  // Mark old duplicate as invalid.
              }
            }
          }
          // Any "old" item immediately exits and triggers the N^2 exhaustive initialization.
          else if ( hdr.stat != OSAL_NV_ERASED_ID )
          {
            return OSAL_NV_ERASED_ID;
          }
        }
        else
        {
          setItem( pg, offset, eNvZero );  // Mark bad checksum as invalid.
          lost += (OSAL_NV_HDR_SIZE + sz);
        }
      }
    }
    else
    {
      lost += (OSAL_NV_HDR_SIZE + sz);
    }
    offset += sz;

  } while (offset < (OSAL_NV_PAGE_SIZE - OSAL_NV_HDR_SIZE));

  pgOff[pg - OSAL_NV_PAGE_BEG] = offset;
  pgLost[pg - OSAL_NV_PAGE_BEG] = lost;

  return OSAL_NV_ITEM_NULL;
}

/*********************************************************************
 * @fn      erasePage
 *
 * @brief   Erases a page in Flash.
 *
 * @param   pg - Valid NV page to erase.
 *
 * @return  none
 */
static void erasePage( uint8_t pg )
{
  BOARD_flashErase(pg);

  pgOff[pg - OSAL_NV_PAGE_BEG] = OSAL_NV_PAGE_HDR_SIZE;
  pgLost[pg - OSAL_NV_PAGE_BEG] = 0;
}

/*********************************************************************
 * @fn      compactPage
 *
 * @brief   Compacts the page specified.
 *
 * @param   srcPg - Valid NV page to erase.
 * @param   skipId - Item Id to not compact.
 *
 * @return  TRUE if valid items from 'srcPg' are successully compacted onto the 'pgRes';
 *          FALSE otherwise.
 *          Note that on a failure, this could loop, re-erasing the 'pgRes' and re-compacting with
 *          the risk of infinitely looping on HAL flash failure.
 *          Worst case scenario: HAL flash starts failing in general, perhaps low Vdd?
 *          All page compactions will fail which will cause all osal_nv_write() calls to return
 *          NV_OPER_FAILED.
 *          Eventually, all pages in use may also be in the state of "pending compaction" where
 *          the page header member OSAL_NV_PG_XFER is zeroed out.
 *          During this "HAL flash brown-out", the code will run and OTA should work (until low Vdd
 *          causes an actual chip brown-out, of course.) Although no new NV items will be created
 *          or written, the last value written with a return value of SUCCESS can continue to be
 *          read successfully.
 *          If eventually HAL flash starts working again, all of the pages marked as
 *          "pending compaction" may or may not be eventually compacted. But, initNV() will
 *          deterministically clean-up one page pending compaction per power-cycle
 *          (if HAL flash is working.) Nevertheless, one erased reserve page will be maintained
 *          through such a scenario.
 */
static uint8_t compactPage( uint8_t srcPg, uint16_t skipId )
{
  uint16_t srcOff;
  uint8_t rtrn;

  // To minimize code size, only check for a clean page here where it's absolutely required.
  for (srcOff = 0; srcOff < OSAL_NV_PAGE_SIZE; srcOff++)
  {
    BOARD_flashRead(pgRes, srcOff, &rtrn, 1);
    if (rtrn != OSAL_NV_ERASED)
    {
      erasePage(pgRes);
      return FALSE;
    }
  }

  srcOff = OSAL_NV_PAGE_HDR_SIZE;
  rtrn = TRUE;

  while ( srcOff < (OSAL_NV_PAGE_SIZE - OSAL_NV_HDR_SIZE ) )
  {
    osalNvHdr_t hdr;
    uint16_t sz, dstOff = pgOff[pgRes-OSAL_NV_PAGE_BEG];

    BOARD_flashRead(srcPg, srcOff, (uint8_t *)(&hdr), OSAL_NV_HDR_SIZE);

    if ( hdr.id == OSAL_NV_ERASED_ID )
    {
      break;
    }

    // Get the actual size in bytes which is the ceiling(hdr.len)
    sz = OSAL_NV_DATA_SIZE( hdr.len );

    if ( sz > (OSAL_NV_PAGE_SIZE - OSAL_NV_HDR_SIZE - srcOff) )
    {
      break;
    }

    if ( sz > (OSAL_NV_PAGE_SIZE - OSAL_NV_HDR_SIZE - dstOff) )
    {
      rtrn = FALSE;
      break;
    }

    srcOff += OSAL_NV_HDR_SIZE;

    if ( (hdr.id != OSAL_NV_ZEROED_ID) && (hdr.id != skipId) )
    {
      if ( hdr.chk == calcChkF( srcPg, srcOff, hdr.len ) )
      {
        /* Prevent excessive re-writes to item header caused by numerous, rapid, & successive
         * OSAL_Nv interruptions caused by resets.
         */
        if ( hdr.stat == OSAL_NV_ERASED_ID )
        {
          setItem( srcPg, srcOff, eNvXfer );
        }

        if ( writeItem( pgRes, hdr.id, hdr.len, NULL, FALSE ) )
        {
          dstOff += OSAL_NV_HDR_SIZE;
          xferBuf( srcPg, srcOff, pgRes, dstOff, sz );
          // Calculate and write the new checksum.
          if (hdr.chk == calcChkF(pgRes, dstOff, hdr.len))
          {
            if ( hdr.chk != setChk( pgRes, dstOff, hdr.chk ) )
            {
              rtrn = FALSE;
              break;
            }
            else
            {
              hotItemUpdate(pgRes, dstOff, hdr.id);
            }
          }
          else
          {
            rtrn = FALSE;
            break;
          }
        }
        else
        {
          rtrn = FALSE;
          break;
        }
      }
    }

    srcOff += sz;
  }

  if (rtrn == FALSE)
  {
    erasePage(pgRes);
  }
  else if (skipId == OSAL_NV_ITEM_NULL)
  {
    COMPACT_PAGE_CLEANUP(srcPg)
  }
  // else invoking function must cleanup.

  return rtrn;
}

/*********************************************************************
 * @fn      findItem
 *
 * @brief   Find an item Id in NV and return the page and offset to its data.
 *
 * @param   id - Valid NV item Id.
 *
 * @return  Offset of data corresponding to item Id, if found;
 *          otherwise OSAL_NV_ITEM_NULL.
 *
 *          The page containing the item, if found;
 *          otherwise no valid assignment made - left equal to item Id.
 *
 */
static uint16_t findItem( uint16_t id )
{
  uint16_t off;
  uint8_t pg;

  for ( pg = OSAL_NV_PAGE_BEG; pg <= OSAL_NV_PAGE_END; pg++ )
  {
    if ( (off = initPage( pg, id, FALSE )) != OSAL_NV_ITEM_NULL )
    {
      findPg = pg;
      return off;
    }
  }

  // Now attempt to find the item as the "old" item of a failed/interrupted NV write.
  if ( (id & OSAL_NV_SOURCE_ID) == 0 )
  {
    return findItem( id | OSAL_NV_SOURCE_ID );
  }
  else
  {
    findPg = OSAL_NV_PAGE_NULL;
    return OSAL_NV_ITEM_NULL;
  }
}

/*********************************************************************
 * @fn      initItem
 *
 * @brief   An NV item is created and initialized with the data passed to the function, if any.
 *
 * @param   flag - TRUE if the 'buf' parameter contains data for the call to writeItem().
 *                 (i.e. if invoked from osal_nv_item_init() ).
 *                 FALSE if writeItem() should just write the header and the 'buf' parameter
 *                 is ok to use as a return value of the page number to be cleaned with
 *                 COMPACT_PAGE_CLEANUP().
 *                 (i.e. if invoked from osal_nv_write() ).
 * @param   id  - Valid NV item Id.
 * @param   len - Item data length.
 * @param  *buf - Pointer to item initalization data. Set to NULL if none.
 *
 * @return  The OSAL Nv page number if item write and read back checksums ok;
 *          OSAL_NV_PAGE_NULL otherwise.
 */
static uint8_t initItem( uint8_t flag, uint16_t id, uint16_t len, void *buf )
{
  uint16_t sz = OSAL_NV_ITEM_SIZE( len );
  uint8_t rtrn = OSAL_NV_PAGE_NULL;
  uint8_t cnt = OSAL_NV_PAGES_USED;
  uint8_t pg = pgRes+1;  // Set to 1 after the reserve page to even wear across all available pages.

  do {
    if (pg >= OSAL_NV_PAGE_BEG+OSAL_NV_PAGES_USED)
    {
      pg = OSAL_NV_PAGE_BEG;
    }
    if ( pg != pgRes )
    {
      uint8_t idx = pg - OSAL_NV_PAGE_BEG;
      if ( sz <= (OSAL_NV_PAGE_SIZE - pgOff[idx] + pgLost[idx]) )
      {
        break;
      }
    }
    pg++;
  } while (--cnt);

  if (cnt)
  {
    // Item fits if an old page is compacted.
    if ( sz > (OSAL_NV_PAGE_SIZE - pgOff[pg - OSAL_NV_PAGE_BEG]) )
    {
      osalNvPgHdr_t pgHdr;

      /* Prevent excessive re-writes to page header caused by numerous, rapid, & successive
       * OSAL_Nv interruptions caused by resets.
       */
      BOARD_flashRead(pg, OSAL_NV_PAGE_HDR_OFFSET, (uint8_t *)(&pgHdr), OSAL_NV_PAGE_HDR_SIZE);
      if ( pgHdr.xfer == OSAL_NV_ERASED_ID )
      {
        // Mark the old page as being in process of compaction.
        sz = OSAL_NV_ZEROED_ID;
        writeWordH( pg, OSAL_NV_PG_XFER, (uint8_t*)(&sz) );
      }

      /* First the old page is compacted, then the new item will be the last one written to what
       * had been the reserved page.
       */
      if (compactPage( pg, id ))
      {
        if ( writeItem( pgRes, id, len, buf, flag ) )
        {
          rtrn = pgRes;
        }

        if ( flag == FALSE )
        {
          /* Overload 'buf' as an OUT parameter to pass back to the calling function
           * the old page to be cleaned up.
           */
          *(uint8_t *)buf = pg;
        }
        else
        {
          /* Safe to do the compacted page cleanup even if writeItem() above failed because the
           * item does not yet exist since this call with flag==TRUE is from osal_nv_item_init().
           */
          COMPACT_PAGE_CLEANUP( pg )
        }
      }
    }
    else
    {
      if ( writeItem( pg, id, len, buf, flag ) )
      {
        rtrn = pg;
      }
    }
  }

  return rtrn;
}

/*********************************************************************
 * @fn      setItem
 *
 * @brief   Set an item Id or status to mark its state.
 *
 * @param   pg - Valid NV page.
 * @param   offset - Valid offset into the page of the item data - the header
 *                   offset is calculated from this.
 * @param   stat - Valid enum value for the item status.
 *
 * @return  none
 */
static void setItem( uint8_t pg, uint16_t offset, eNvHdrEnum stat )
{
  osalNvHdr_t hdr;

  offset -= OSAL_NV_HDR_SIZE;
  BOARD_flashRead(pg, offset, (uint8_t *)(&hdr), OSAL_NV_HDR_SIZE);

  if ( stat == eNvXfer )
  {
    hdr.stat = OSAL_NV_ACTIVE;
    /* Write status to flash. Total of 4 bytes written to Flash 
     * ( 2 bytes of checksum and 2 bytes of status) 
     */
    writeWord( pg, offset+OSAL_NV_HDR_CHK, (uint8_t*)(&(hdr.chk)) );
  }
  else // if ( stat == eNvZero )
  {
    uint16_t sz = ((hdr.len + (OSAL_NV_WORD_SIZE-1)) / OSAL_NV_WORD_SIZE) * OSAL_NV_WORD_SIZE +
                                                                          OSAL_NV_HDR_SIZE;
    hdr.id = 0;
    writeWord( pg, offset, (uint8_t *)(&hdr) );
    pgLost[pg-OSAL_NV_PAGE_BEG] += sz;
  }
}

/*********************************************************************
 * @fn      setChk
 *
 * @brief   Set the item header checksum given the data buffer offset.
 *
 * @param   pg - Valid NV page.
 * @param   offset - Valid offset into the page of the item data - the header
 *                   offset is calculated from this.
 * @param   chk - The checksum to set.
 *
 * @return  The checksum read back.
 */
static uint16_t setChk( uint8_t pg, uint16_t offset, uint16_t chk )
{
  offset -= OSAL_NV_WORD_SIZE;
  writeWordH( pg, offset, (uint8_t *)&chk );
  BOARD_flashRead( pg, offset, (uint8_t *)(&chk), sizeof( chk ) );

  return chk;
}

/*********************************************************************
 * @fn      calcChkB
 *
 * @brief   Calculates the data checksum over the 'buf' parameter.
 *
 * @param   len - Byte count of the data to be checksummed.
 * @param   buf - Data buffer to be checksummed.
 *
 * @return  Calculated checksum of the data bytes.
 */
static uint16_t calcChkB( uint16_t len, uint8_t *buf )
{
  uint8_t fill = len % OSAL_NV_WORD_SIZE;
  uint16_t chk;

  if ( !buf )
  {
    chk = len * OSAL_NV_ERASED;
  }
  else
  {
    chk = 0;
    while ( len-- )
    {
      chk += *buf++;
    }
  }

  // calcChkF() will calculate over OSAL_NV_WORD_SIZE alignment.
  if ( fill )
  {
    chk += (OSAL_NV_WORD_SIZE - fill) * OSAL_NV_ERASED;
  }

  return chk;
}

/*********************************************************************
 * @fn      calcChkF
 *
 * @brief   Calculates the data checksum by reading the data bytes from NV.
 *
 * @param   pg - A valid NV Flash page.
 * @param   offset - A valid offset into the page.
 * @param   len - Byte count of the data to be checksummed.
 *
 * @return  Calculated checksum of the data bytes.
 */
static uint16_t calcChkF( uint8_t pg, uint16_t offset, uint16_t len )
{
  uint16_t chk = 0;

  len = (len + (OSAL_NV_WORD_SIZE-1)) / OSAL_NV_WORD_SIZE;

  while ( len-- )
  {
    uint8_t cnt, tmp[OSAL_NV_WORD_SIZE];

    BOARD_flashRead(pg, offset, tmp, OSAL_NV_WORD_SIZE);
    offset += OSAL_NV_WORD_SIZE;

    for ( cnt = 0; cnt < OSAL_NV_WORD_SIZE; cnt++ )
    {
      chk += tmp[cnt];
    }
  }

  return chk;
}

/*********************************************************************
 * @fn      writeWord
 *
 * @brief   Writes a Flash-WORD to NV.
 *
 * @param   pg - A valid NV Flash page.
 * @param   offset - A valid offset into the page.
 * @param   buf - Pointer to source buffer.
 *
 * @return  none
 */
static void writeWord( uint8_t pg, uint16_t offset, uint8_t *buf )
{
  BOARD_flashWrite(pg, offset, buf, 1);
}

/*********************************************************************
 * @fn      writeWordM
 *
 * @brief   Writes multiple Flash-WORDs to NV.
 *
 * @param   pg - A valid NV Flash page.
 * @param   offset - A valid offset into the page.
 * @param   buf - Pointer to source buffer.
 * @param   cnt - Number of 4-byte blocks to write.
 *
 * @return  none
 */
static void writeWordM( uint8_t pg, uint16_t offset, uint8_t *buf, uint16_t cnt )
{
  BOARD_flashWrite(pg, offset, buf, cnt);
}

/*********************************************************************
 * @fn      writeWordH
 *
 * @brief   Writes the 1st half of a Flash-WORD to NV (filling 2nd half with 0xffff).
 *
 * @param   pg - A valid NV Flash page.
 * @param   offset - A valid offset into the page.
 * @param   buf - Pointer to source buffer.
 *
 * @return  none
 */
static void writeWordH( uint8_t pg, uint16_t offset, uint8_t *buf )
{
  uint8_t tmp[4];

  tmp[0] = buf[0];
  tmp[1] = buf[1];
  tmp[2] = OSAL_NV_ERASED;
  tmp[3] = OSAL_NV_ERASED;

  writeWord( pg, offset, tmp );
}

/*********************************************************************
 * @fn      writeBuf
 *
 * @brief   Writes a data buffer to NV.
 *
 * @param   dstPg - A valid NV Flash page.
 * @param   offset - A valid offset into the page.
 * @param   len  - Byte count of the data to write.
 * @param   buf  - The data to write.
 *
 * @return  TRUE if data buf checksum matches read back checksum, else FALSE.
 */
static void writeBuf( uint8_t dstPg, uint16_t dstOff, uint16_t len, uint8_t *buf )
{
  uint8_t rem = dstOff % OSAL_NV_WORD_SIZE;
  uint8_t tmp[OSAL_NV_WORD_SIZE];

  if ( rem )
  {
    dstOff = (dstOff / OSAL_NV_WORD_SIZE) * OSAL_NV_WORD_SIZE;
    BOARD_flashRead(dstPg, dstOff, tmp, OSAL_NV_WORD_SIZE);

    while ( (rem < OSAL_NV_WORD_SIZE) && len )
    {
      tmp[rem++] = *buf++;
      len--;
    }

    writeWord( dstPg, dstOff, tmp );
    dstOff += OSAL_NV_WORD_SIZE;
  }

  rem = len % OSAL_NV_WORD_SIZE;
  len /= OSAL_NV_WORD_SIZE;

  if ( len )
  {
    writeWordM( dstPg, dstOff, buf, len );
    dstOff += OSAL_NV_WORD_SIZE * len;
    buf += OSAL_NV_WORD_SIZE * len;
  }

  if ( rem )
  {
    uint8_t idx = 0;
    BOARD_flashRead(dstPg, dstOff, tmp, OSAL_NV_WORD_SIZE);
    while ( rem-- )
    {
      tmp[idx++] = *buf++;
    }
    writeWord( dstPg, dstOff, tmp );
  }
}

/*********************************************************************
 * @fn      xferBuf
 *
 * @brief   Xfers an NV buffer from one location to another, enforcing OSAL_NV_WORD_SIZE writes.
 *
 * @return  none
 */
static void xferBuf( uint8_t srcPg, uint16_t srcOff, uint8_t dstPg, uint16_t dstOff, uint16_t len )
{
  uint8_t rem = dstOff % OSAL_NV_WORD_SIZE;
  uint8_t tmp[OSAL_NV_WORD_SIZE];

  if ( rem )
  {
    dstOff -= rem;
    BOARD_flashRead(dstPg, dstOff, tmp, OSAL_NV_WORD_SIZE);

    while ( (rem < OSAL_NV_WORD_SIZE) && len )
    {
      BOARD_flashRead(srcPg, srcOff, tmp+rem, 1);
      srcOff++;
      rem++;
      len--;
    }

    writeWord( dstPg, dstOff, tmp );
    dstOff += OSAL_NV_WORD_SIZE;
  }

  rem = len % OSAL_NV_WORD_SIZE;
  len /= OSAL_NV_WORD_SIZE;

  while ( len-- )
  {
    BOARD_flashRead(srcPg, srcOff, tmp, OSAL_NV_WORD_SIZE);
    srcOff += OSAL_NV_WORD_SIZE;
    writeWord( dstPg, dstOff, tmp );
    dstOff += OSAL_NV_WORD_SIZE;
  }

  if ( rem )
  {
    uint8_t idx = 0;
    BOARD_flashRead(dstPg, dstOff, tmp, OSAL_NV_WORD_SIZE);
    while ( rem-- )
    {
      BOARD_flashRead(srcPg, srcOff, tmp+idx, 1);
      srcOff++;
      idx++;
    }
    writeWord( dstPg, dstOff, tmp );
  }
}

/*********************************************************************
 * @fn      writeItem
 *
 * @brief   Writes an item header/data combo to the specified NV page.
 *
 * @param   pg - Valid NV Flash page.
 * @param   id - Valid NV item Id.
 * @param   len  - Byte count of the data to write.
 * @param   buf  - The data to write. If NULL, no data/checksum write.
 * @param   flag - TRUE if the checksum should be written, FALSE otherwise.
 *
 * @return  TRUE if header/data to write matches header/data read back, else FALSE.
 */
static uint8_t writeItem( uint8_t pg, uint16_t id, uint16_t len, void *buf, uint8_t flag )
{
  uint16_t offset = pgOff[pg-OSAL_NV_PAGE_BEG];
  uint8_t rtrn = FALSE;
  osalNvHdr_t hdr;

  hdr.id = id;
  hdr.len = len;

  writeWord( pg, offset, (uint8_t *)&hdr );
  BOARD_flashRead(pg, offset, (uint8_t *)(&hdr), OSAL_NV_HDR_SIZE);

  if ( (hdr.id == id) && (hdr.len == len) )
  {
    if ( flag )
    {
      hdr.chk = calcChkB( len, buf );

      offset += OSAL_NV_HDR_SIZE;
      if ( buf != NULL )
      {
        writeBuf( pg, offset, len, buf );
      }

      if ( hdr.chk == calcChkF( pg, offset, len ) )
      {
        if ( hdr.chk == setChk( pg, offset, hdr.chk ) )
        {
          hotItemUpdate(pg, offset, hdr.id);
          rtrn = TRUE;
        }
      }
    }
    else
    {
      rtrn = TRUE;
    }

    len = OSAL_NV_ITEM_SIZE( hdr.len );
  }
  else
  {
    len = OSAL_NV_ITEM_SIZE( hdr.len );

    if (len > (OSAL_NV_PAGE_SIZE - pgOff[pg - OSAL_NV_PAGE_BEG]))
    {
      len = (OSAL_NV_PAGE_SIZE - pgOff[pg - OSAL_NV_PAGE_BEG]);
    }

    pgLost[pg - OSAL_NV_PAGE_BEG] += len;
  }
  pgOff[pg - OSAL_NV_PAGE_BEG] += len;

  return rtrn;
}

/*********************************************************************
 * @fn      hotItem
 *
 * @brief   Look for the parameter 'id' in the hot items array.
 *
 * @param   id - A valid NV item Id.
 *
 * @return  A valid index into the hot items if the item is hot; OSAL_NV_MAX_HOT if not.
 */
static uint8_t hotItem(uint16_t id)
{
  uint8_t hotIdx;

  for (hotIdx = 0; hotIdx < OSAL_NV_MAX_HOT; hotIdx++)
  {
//    if (hotIds[hotIdx] == id)
//    {
//      break;
//    }
  }

  return hotIdx;
}

/*********************************************************************
 * @fn      hotItemUpdate
 *
 * @brief   If the parameter 'id' is a hot item, update the corresponding hot item data.
 *
 * @param   pg - The new NV page corresponding to the hot item.
 * @param   off - The new NV page offset corresponding to the hot item.
 * @param   id - A valid NV item Id.
 *
 * @return  none
 */
static void hotItemUpdate(uint8_t pg, uint16_t off, uint16_t id)
{
  uint8_t hotIdx = hotItem(id);

  if (hotIdx < OSAL_NV_MAX_HOT)
  {
    {
      hotPg[hotIdx] = pg;
      hotOff[hotIdx] = off;
    }
  }
}

/*
 * @fn      osal_nv_init
 *
 * @brief   Initialize NV service.
 *
 * @param   p - Not used.
 *
 * @return  none
 */
void osal_nv_init( void *p )
{
  (void)p;  // Suppress Lint warning.
  BOARD_flashInit();
  (void)initNV();  // Always returns TRUE after pages have been erased.
}

/*
 * @fn      osal_nv_item_init
 *
 * @brief   If the NV item does not already exist, it is created and
 *          initialized with the data passed to the function, if any.
 *          This function must be called before calling osal_nv_read() or
 *          osal_nv_write().
 *
 * @param   id  - Valid NV item Id.
 * @param   len - Item length.
 * @param  *buf - Pointer to item initalization data. Set to NULL if none.
 *
 * @return  NV_ITEM_UNINIT - Id did not exist and was created successfully.
 *          SUCCESS        - Id already existed, no action taken.
 *          NV_OPER_FAILED - Failure to find or create Id.
 */
uint8_t osal_nv_item_init( uint16_t id, uint16_t len, void *buf )
{
  uint16_t offset;

  /*if ( !OSAL_NV_CHECK_BUS_VOLTAGE )
  {
    return NV_OPER_FAILED;
  }
  else*/ if ((offset = findItem(id)) != OSAL_NV_ITEM_NULL)
  {
    // Re-populate the NV hot item data if the corresponding items are already established.
    hotItemUpdate(findPg, offset, id);

    return SUCCESS;
  }
  else if ( initItem( TRUE, id, len, buf ) != OSAL_NV_PAGE_NULL )
  {
    return NV_ITEM_UNINIT;
  }
  else
  {
    return NV_OPER_FAILED;
  }
}

/*********************************************************************
 * @fn      osal_nv_item_len
 *
 * @brief   Get the data length of the item stored in NV memory.
 *
 * @param   id  - Valid NV item Id.
 *
 * @return  Item length, if found; zero otherwise.
 */
uint16_t osal_nv_item_len( uint16_t id )
{
  osalNvHdr_t hdr;
  uint16_t offset;
  uint8_t hotIdx;

  if ((hotIdx = hotItem(id)) < OSAL_NV_MAX_HOT)
  {
    findPg = hotPg[hotIdx];
    offset = hotOff[hotIdx];
  }
  else if ((offset = findItem(id)) == OSAL_NV_ITEM_NULL)
  {
    return 0;
  }

  BOARD_flashRead(findPg, (offset - OSAL_NV_HDR_SIZE), (uint8_t *)(&hdr), OSAL_NV_HDR_SIZE);
  return hdr.len;
}

/*********************************************************************
 * @fn      osal_nv_write
 *
 * @brief   Write a data item to NV. Function can write an entire item to NV or
 *          an element of an item by indexing into the item with an offset.
 *
 * @param   id  - Valid NV item Id.
 * @param   ndx - Index offset into item
 * @param   len - Length of data to write.
 * @param  *buf - Data to write.
 *
 * @return  SUCCESS if successful, NV_ITEM_UNINIT if item did not
 *          exist in NV and offset is non-zero, NV_OPER_FAILED if failure.
 */
uint8_t osal_nv_write( uint16_t id, uint16_t ndx, uint16_t len, void *buf )
{
  uint8_t rtrn = SUCCESS;

  /*if ( !OSAL_NV_CHECK_BUS_VOLTAGE )
  {
    return NV_OPER_FAILED;
  }
  else */if ( len != 0 )
  {
    osalNvHdr_t hdr;
    uint16_t origOff, srcOff;
    uint16_t cnt, chk;
    uint8_t *ptr, srcPg;

    origOff = srcOff = findItem( id );
    srcPg = findPg;
    if ( srcOff == OSAL_NV_ITEM_NULL )
    {
      return NV_ITEM_UNINIT;
    }

    BOARD_flashRead(srcPg, (srcOff - OSAL_NV_HDR_SIZE), (uint8_t *)(&hdr), OSAL_NV_HDR_SIZE);
    if ( hdr.len < (ndx + len) )
    {
      return NV_OPER_FAILED;
    }

    srcOff += ndx;
    ptr = buf;
    cnt = len;
    chk = 0;
    while ( cnt-- )
    {
      uint8_t tmp;
      BOARD_flashRead(srcPg, srcOff, &tmp, 1);
      if ( tmp != *ptr )
      {
        chk = 1;  // Mark that at least one byte is different.
        // Calculate expected checksum after transferring old data and writing new data.
        hdr.chk -= tmp;
        hdr.chk += *ptr;
      }
      srcOff++;
      ptr++;
    }

    if ( chk != 0 )  // If the buffer to write is different in one or more bytes.
    {
      uint8_t comPg = OSAL_NV_PAGE_NULL;
      uint8_t dstPg = initItem( FALSE, id, hdr.len, &comPg );

      if ( dstPg != OSAL_NV_PAGE_NULL )
      {
        uint16_t tmp = OSAL_NV_DATA_SIZE( hdr.len );
        uint16_t dstOff = pgOff[dstPg-OSAL_NV_PAGE_BEG] - tmp;
        srcOff = origOff;

        /* Prevent excessive re-writes to item header caused by numerous, rapid, & successive
         * OSAL_Nv interruptions caused by resets.
         */
        if ( hdr.stat == OSAL_NV_ERASED_ID )
        {
          setItem( srcPg, srcOff, eNvXfer );
        }

        xferBuf( srcPg, srcOff, dstPg, dstOff, ndx );
        srcOff += ndx;
        dstOff += ndx;

        writeBuf( dstPg, dstOff, len, buf );
        srcOff += len;
        dstOff += len;

        xferBuf( srcPg, srcOff, dstPg, dstOff, (hdr.len-ndx-len) );

        // Calculate and write the new checksum.
        dstOff = pgOff[dstPg-OSAL_NV_PAGE_BEG] - tmp;

        if ( hdr.chk == calcChkF( dstPg, dstOff, hdr.len ) )
        {
          if ( hdr.chk != setChk( dstPg, dstOff, hdr.chk ) )
          {
            rtrn = NV_OPER_FAILED;
          }
          else
          {
            hotItemUpdate(dstPg, dstOff, hdr.id);
          }
        }
        else
        {
          rtrn = NV_OPER_FAILED;
        }
      }
      else
      {
        rtrn = NV_OPER_FAILED;
      }

      if ( comPg != OSAL_NV_PAGE_NULL )
      {
        /* Even though the page compaction succeeded, if the new item is coming from the compacted
         * page and writing the new value failed, then the compaction must be aborted.
         */
        if ( (srcPg == comPg) && (rtrn == NV_OPER_FAILED) )
        {
          erasePage( pgRes );
        }
        else
        {
          COMPACT_PAGE_CLEANUP( comPg )
        }
      }

      /* Zero of the old item must wait until after compact page cleanup has finished - if the item
       * is zeroed before and cleanup is interrupted by a power-cycle, the new item can be lost.
       */
      if ( (srcPg != comPg) && (rtrn != NV_OPER_FAILED) )
      {
        setItem( srcPg, origOff, eNvZero );
      }
    }
  }

  return rtrn;
}

/*********************************************************************
 * @fn      osal_nv_read
 *
 * @brief   Read data from NV. This function can be used to read an entire item from NV or
 *          an element of an item by indexing into the item with an offset.
 *          Read data is copied into *buf.
 *
 * @param   id  - Valid NV item Id.
 * @param   ndx - Index offset into item
 * @param   len - Length of data to read.
 * @param  *buf - Data is read into this buffer.
 *
 * @return  SUCCESS if NV data was copied to the parameter 'buf'.
 *          Otherwise, NV_OPER_FAILED for failure.
 */
uint8_t osal_nv_read( uint16_t id, uint16_t ndx, uint16_t len, void *buf )
{
  uint16_t offset;
  uint8_t hotIdx;

  if ((hotIdx = hotItem(id)) < OSAL_NV_MAX_HOT)
  {
    BOARD_flashRead(hotPg[hotIdx], hotOff[hotIdx]+ndx, buf, len);
    return SUCCESS;
  }

  if ((offset = findItem(id)) == OSAL_NV_ITEM_NULL)
  {
    return NV_OPER_FAILED;
  }
  else
  {
    BOARD_flashRead(findPg, offset+ndx, buf, len);
    return SUCCESS;
  }
}

/*********************************************************************
 * @fn      osal_nv_delete
 *
 * @brief   Delete item from NV. This function will fail if the length
 *          parameter does not match the length of the item in NV.
 *
 * @param   id  - Valid NV item Id.
 * @param   len - Length of item to delete.
 *
 * @return  SUCCESS if item was deleted,
 *          NV_ITEM_UNINIT if item did not exist in NV,
 *          NV_BAD_ITEM_LEN if length parameter not correct,
 *          NV_OPER_FAILED if attempted deletion failed.
 */
uint8_t osal_nv_delete( uint16_t id, uint16_t len )
{
  uint16_t length;
  uint16_t offset;

  offset = findItem( id );
  if ( offset == OSAL_NV_ITEM_NULL )
  {
    // NV item does not exist
    return NV_ITEM_UNINIT;
  }

  length = osal_nv_item_len( id );
  if ( length != len )
  {
    // NV item has different length
    return NV_BAD_ITEM_LEN;
  }

  // Set item header ID to zero to 'delete' the item
  setItem( findPg, offset, eNvZero );

  // Verify that item has been removed
  offset = findItem( id );
  if ( offset != OSAL_NV_ITEM_NULL )
  {
    // Still there
    return NV_OPER_FAILED;
  }
  else
  {
    // Yes, it's gone
    return SUCCESS;
  }
}

/*********************************************************************
 */
