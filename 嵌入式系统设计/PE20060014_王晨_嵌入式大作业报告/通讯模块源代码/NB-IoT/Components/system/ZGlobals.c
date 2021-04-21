/*************************************************************************
  \file				ZGlobals.c
  \brief
		User definable Stack parameters.

  \author
		Johnason
  \internal
  	  Revision History:
  		DEC. 16, 2017		Johnason - Created
**************************************************************************/

/*********************************************************************
 * 							INCLUDES
 */
#include <stdlib.h>
#include <string.h>
#include "ZGlobals.h"
#include "OSAL_Nv.h"
#include "board-config.h"
#include "szconfig.h"
/*********************************************************************
 * 							MACROS
 */

/*********************************************************************
 * 							CONSTANTS
 */

/*********************************************************************
 * 							TYPEDEFS
 */

typedef struct zgItem
{
  uint16_t id;
  uint16_t len;
  void *buf;
} zgItem_t;



uint8_t NB_version[] = "BC28 V1.0\r\n";
uint8_t dtuid[8] = "[SZ-LTE]";
uint8_t serve_ipaddr[SET_IPADDR_LEN+1] ;//= "120.26.43.233";//service ip address    //P S dns --->fota ip address
uint8_t serve_host_name[SET_HOST_NAME_LEN+1] = "123.126.34.22";//"119.39.120.165";//service host name 1
uint8_t port[6] = "5688";
//uint8_t fota_user_name[SET_HOST_NAME_LEN+1] = "shuncom";     // --->fota user name
//uint8_t fota_password[6] = "12345";                        //--->fota password
uint8_t apninfo[SET_APNINFO_LEN+1] = "NBIOT";//"psm0.eDRX0.ctnb";//
//uint8_t apnname[SET_APNNAME_LEN+1];
//uint8_t apnpassword[SET_APNPASSWORD_LEN+1];
bool rssi_disp = FALSE;  //TRUE = TCP, FALSE = UDP
bool DebugInfoEnable = TRUE;            //调试信息输出使能
bool DeviceIdentEnable = FALSE;           //设备标识使能
bool SMSComEnable = FALSE;               //短信收发功能使能
//uint8_t UserName[SET_APNNAME_LEN];           //短信操作用户电话号码11位
//uint8_t PassWord[SET_APNNAME_LEN];    /**/     //用户密码6位
uint8_t BaudRateType = Baudrate_38400;            //用户端串口比特率
uint8_t DataBitType = oa_uart_len_8;             //数据位 8/9
uint8_t CheckBitType = oa_uart_pa_none;            //校验位
uint8_t StopBitType = oa_uart_sb_1;             //停止位 本机为空
bool RegisterPacketEnable = TRUE;        //注册包使能：开启时连接成功首次发送其内容
//uint8_t RegisterPacketContent[81] = HEART_PACKET_DEF;   //注册包内容：可以为设备标识信息
bool HeartbeatTimeEnable = FALSE;    //心跳包使能
uint8_t HeartbeatTime[6] = HEART_TIME;        //心跳时间  单位s
//uint8_t HeartbeatData[81] ;       //心跳数据包内容
//uint8_t serve_host_name_sz[SET_HOST_NAME_LEN+1];//service host name sz
//uint8_t port_sz[6];

//uint8_t Heartpacket[heart_packet_len] = HEART_PACKET_DEF;
//uint8_t Heartenable = HEART_PACKET_ON;
//uint8_t NB_IP[] = NB_APN_IP;
uint8_t FOTA_mode = NB_FOTA_ON;

/*********************************************************************
 * 						GLOBAL VARIABLES
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * 					ZGlobal Item Table
 */

static const zgItem_t zgItemTable[] =
{
	{
		ZCD_NV_SERVE_NOSTNAME, SET_HOST_NAME_LEN+1, serve_host_name
	},
	{
		ZCD_NV_SERVE_PORT, 6, port
	},
	{
		ZCD_NV_APN_INFO, SET_APNINFO_LEN+1, apninfo
	},
	{
		ZCD_NV_MODE_SWITCH, sizeof(rssi_disp), &rssi_disp
	},
	{
		ZCD_NV_DEBUG_SWITCH, sizeof(DebugInfoEnable), &DebugInfoEnable
	},
	{
		ZCD_NV_BAUD_RATE, sizeof(BaudRateType), &BaudRateType
	},
	{
		ZCD_NV_DATA_BIT, sizeof(DataBitType), &DataBitType
	},
	{
		ZCD_NV_CHECK_BIT, sizeof(CheckBitType), &CheckBitType
	},
	{
		ZCD_NV_STOP_BIT, sizeof(StopBitType), &StopBitType
	},
	{
		ZCD_NV_PACKET_ENBLE, sizeof(RegisterPacketEnable), &RegisterPacketEnable
	},
	{
//		ZCD_NV_HEART_PACKET, sizeof(RegisterPacketContent), RegisterPacketContent
	},
	{
		ZCD_NV_HEART_TIME, sizeof(HeartbeatTime), HeartbeatTime
	},
	{
		ZCD_NV_FOTA_MODE, sizeof(FOTA_mode), &FOTA_mode
	},

	// Last item -- DO NOT MOVE IT!
	{
		0x00, 0, NULL
	}
};


/*********************************************************************
 * 						LOCAL FUNCTIONS
 */

/*
 * @fn       zgItemInit()
 *
 * @brief
 *
 *   Initialize a global item. If the item doesn't exist in NV memory,
 *   write the system default (value passed in) into NV memory. But if
 *   it exists, set the item to the value stored in NV memory.
 *
 *   Also, if setDefault is TRUE and the item exists, we will write
 *   the default value to NV space.
 *
 * @param   id - item id
 * @param   len - item len
 * @param   buf - pointer to the item
 * @param   setDefault - TRUE to set default, not read
 *
 * @return  ZSUCCESS if successful, NV_ITEM_UNINIT if item did not
 *          exist in NV, NV_OPER_FAILED if failure.
 */
static uint8_t zgItemInit( uint16_t id, uint16_t len, void *buf, uint8_t setDefault )
{
  uint8_t status;

  BoardDisableIrq();
  // If the item doesn't exist in NV memory, create and initialize
  // it with the value passed in.
  status = osal_nv_item_init( id, len, buf );
  if ( status == SUCCESS )
  {
    if ( setDefault )
    {
      // Write the default value back to NV
      status = osal_nv_write( id, 0, len, buf );
    }
    else
    {
      // The item exists in NV memory, read it from NV memory
      status = osal_nv_read( id, 0, len, buf );
    }
  }
  BoardEnableIrq();
  return (status);
}

/*********************************************************************
 * 						API FUNCTIONS
 */

/*
 * @fn          zgInit
 *
 * @brief
 *
 *   Initialize the Stack Globals. If an item doesn't exist in
 *   NV memory, write the system default into NV memory. But if
 *   it exists, set the item to the value stored in NV memory.
 *
 * @param       none
 *
 * @return      SUCCESS if successful, NV_ITEM_UNINIT if item did not
 *              exist in NV, NV_OPER_FAILED if failure.
 */
uint8_t zgInit( void )
{
  uint8_t setDefault = FALSE;

  // Initialize the items table
  zgInitItems( setDefault );

  return ( SUCCESS );
}

/*
 * @fn          zgInitItems
 *
 * @brief       Initializes RAM variables from NV.  If NV items don't
 *              exist, then the NV is initialize with what is in RAM
 *              variables.
 *
 * @param       none
 *
 * @return      none
 */
void zgInitItems( uint8_t setDefault )
{
  uint8_t  i = 0;

  while ( zgItemTable[i].id != 0x00 )
  {
    // Initialize the item
    zgItemInit( zgItemTable[i].id, zgItemTable[i].len, zgItemTable[i].buf, setDefault  );

    // Move on to the next item
    i++;
  }
}

/*
 * @fn          zgSetItem
 *
 * @brief       Set RAM variables from set-NV, if it exist in the zgItemTable
 *
 * @param       id - NV ID
 *              len - NV item length
 *              buf - pointer to the input buffer
 *
 * @return      none
 */
void zgSetItem( uint16_t id, uint16_t len, void *buf )
{

  uint8_t  i = 0;

  // Look up the NV item table
  while ( zgItemTable[i].id != 0x00 )
  {
    if( zgItemTable[i].id == id )
    {
      if ( zgItemTable[i].len == len )
      {
        memcpy( zgItemTable[i].buf, buf, len );
      }
      break;
    }
    // Move on to the next item
    i++;
  }
}

/*
 * @fn          zgGetItem
 *
 * @brief       Get RAM variables from set-NV, if it exist in the zgItemTable
 *
 * @param       id - NV ID
 *              len - NV item length
 *              buf - pointer to the input buffer
 *
 * @return      the result of read
 */
uint8_t zgGetItem( uint16_t id, uint16_t len, void *buf )
{

  uint8_t  i = 0;
  uint8_t result = FAILURE;

  // Look up the NV item table
  while ( zgItemTable[i].id != 0x00 )
  {
    if( zgItemTable[i].id == id )
    {
      if ( zgItemTable[i].len == len )
      {
        memcpy( buf, zgItemTable[i].buf, len );
        result = SUCCESS;
      }
      break;
    }
    // Move on to the next item
    i++;
  }

  return result;
}
/*********************************************************************
*********************************************************************/
