/*************************************************************************
  \file				ZComDef.h
  \brief
	Header file of defines and macros.

  \author
	www.shuncom.com
  \internal
  Revision History:
  	Dec. 13, 2017		Johnason - Created

**************************************************************************/

#ifndef __ZCOMDEF_H__
#define __ZCOMDEF_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */

/*** Generic Status Return Values ***/
#ifndef SUCCESS
#define SUCCESS                   0x00
#endif
#define FAILURE                   0x01
#define INVALID_TASK              0x03
#define NV_ITEM_UNINIT            0x09
#define NV_OPER_FAILED            0x0A
#define NV_BAD_ITEM_LEN           0x0C

#ifndef FALSE
#define FALSE 0x0U
#endif

#ifndef TRUE
#define TRUE 0x01U
#endif

#define heart_packet_len                10

// LIGHT configure
#define ZCD_NV_SERVE_NOSTNAME 			0x0001
#define ZCD_NV_SERVE_PORT				0x0002
#define ZCD_NV_APN_INFO 		        0x0003
#define ZCD_NV_MODE_SWITCH				0x0004
#define ZCD_NV_DEBUG_SWITCH				0x0005
#define ZCD_NV_BAUD_RATE				0x0006
#define ZCD_NV_DATA_BIT			    	0x0007
#define ZCD_NV_CHECK_BIT				0x0008
#define ZCD_NV_STOP_BIT					0x0009
#define ZCD_NV_PACKET_ENBLE				0x000A
#define ZCD_NV_HEART_PACKET				0x000B
#define ZCD_NV_HEART_TIME    			0x000C
#define ZCD_NV_SET_LIGHTSEN				0x000D
#define ZCD_NV_RISE_OFFESTA				0x000E
#define ZCD_NV_FSK_SYNC_WORD			0x0010
#define ZCD_NV_RF_FREQ_MIN				0x0011
#define ZCD_NV_FOTA_MODE				0x0012
/*********************************************************************
 * MACROS
 */
/* takes a byte out of a u32 : var - u32,  ByteNum - byte to take out (0 - 3) */
#define BREAK_UINT32( var, ByteNum ) \
          (uint8_t)((uint32_t)(((var) >>((ByteNum) * 8)) & 0x00FF))
#define BUILD_UINT32(Byte0, Byte1, Byte2, Byte3) \
          ((uint32_t)((uint32_t)((Byte0) & 0x00FF) \
          + ((uint32_t)((Byte1) & 0x00FF) << 8) \
          + ((uint32_t)((Byte2) & 0x00FF) << 16) \
          + ((uint32_t)((Byte3) & 0x00FF) << 24)))

#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)
#define BUILD_UINT16(loByte, hiByte) \
          ((uint16_t)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))
/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* __ZCOMDEF_H__ */
