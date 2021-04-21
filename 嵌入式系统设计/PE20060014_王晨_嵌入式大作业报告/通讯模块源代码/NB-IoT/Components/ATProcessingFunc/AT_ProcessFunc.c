/*
 * ATCMDJudgement.c
 *
 *  Created on: 2019Äê4ÔÂ16ÈÕ
 *      Author: 8033
 */

/*********************************************************************
 * 							INCLUDES
 */
#include "stdio.h"
#include "string.h"
#include "stdbool.h"
#include "stdint.h"
#include "osal.h"
#include "OSAL_Nv.h"
#include "transparent.h"
#include "ZGlobals.h"
#include "ATCommand.h"
#include "AT_ProcessFunc.h"
#include "CRC.h"
#include "szconfig.h"

/*********************************************************************
 * 							 MACROS
 */

/*********************************************************************
 * 							CONSTANTS
 */

/*********************************************************************
 * 							TYPEDEFS
 */

/*********************************************************************
 * 						GLOBAL VARIABLES
 */

extern Uart_t appConcentrator_Uart;
extern Uart_t appConcentrator_Uart2;

extern uint8_t appConcentrator_TaskID;

extern TimerEvent_t NextCMDdelay;
extern TimerEvent_t Heartbeat;
extern TimerEvent_t CCLKbeat;
extern TimerEvent_t app_ledRunTimer;
extern TimerEvent_t Quary_device;
extern Gpio_t gpio_ledRun;
extern Gpio_t gpio_enPower;

DeviceState dev_state = DEVICE_STATE_CHECK;
uint32_t waiting_delay = DEF_DELAY_TIME;

bool uart2_send_flag = true;
bool AT_cmd_send = true;
uint8_t AT_REGISTER_packet[TX_RX_MSG_NUM] = { 0 };
uint8_t AT_APN_PACKET[APN_LEN] = { 0 };
uint8_t AT_IP_PACKET[IP_LEN] = { 0 };
uint8_t AT_IMEI[IMEI_LEN] = { 0 };
uint8_t AT_IMSI[IMEI_LEN] = { 0 };

bool NB_registered = false;
uint16_t search_cnt = 0;
uint8_t error_cnt = 0;
uint8_t delay_basic_num = 0;
//extern uint8_t heart_cnt;
extern uint32_t Hearttime;
extern uint8_t quary_status[10];
extern bool start_quary;
/*********************************************************************
 *						 LOCAL VARIABLES
 */
uint32_t next_cmd_delay = NEXT_CMD_TIME;
bool peak_start = false;
bool once_operate = true;
/*********************************************************************
* 			      	 SEND AT COMMAND FUNCTIONS
*/

/*
 * @fn
 *
 * @brief
 *
 * @param       none
 *
 * @return      none
 */

uint8_t SendATCmd(uint8_t *data, uint8_t len )
{
	/*debug infomation*/
	if( TRUE == DebugInfoEnable )
	Uart2PutBuffer( &appConcentrator_Uart2, data, len );

	UartPutBuffer( &appConcentrator_Uart, data, len );

	return 0;
}

void Staggered_peak_time( uint8_t num )
{
	TimerStop( &NextCMDdelay );
	uint16_t imei_param = atoi( (const char *)(AT_IMEI + 11) );

	if( 0 == delay_basic_num )
	{
		delay_basic_num = 3;
	}
	else if( 3 == delay_basic_num )
	{
		delay_basic_num = 5;
	}
	else if( 5 == delay_basic_num )
	{
		delay_basic_num = 7;
	}
	else if( 7 == delay_basic_num )
	{
		delay_basic_num = 11;
	}
	else if( 11 == delay_basic_num )
	{
		delay_basic_num = 13;
	}
	else
	{
		while(1);
	}

	next_cmd_delay = ( imei_param % ( delay_basic_num * 60 + 1) ) * 1000 ;
	search_cnt = 0;
	dev_state = DEVICE_OPEN_CFUN;
	TimerSetValue( &NextCMDdelay, next_cmd_delay );
	TimerStart( &NextCMDdelay );
}

void FOTACommandJudgeFunc( uint8_t *recvbuff )
{
//	char *str = NULL;

//	if( NULL != strstr( (const char *)recvbuff, AT_FOTA_UP_MSG ) )
	{
	    TimerStop(  &app_ledRunTimer );
	    GpioWrite( &gpio_ledRun, 0 );
//		dev_state = DEVICE_STATE_CREATE;
		osal_set_event(appConcentrator_TaskID, TRANSPARENT_MAIN_LOOP_EVT);
	}
//	else if( ( NULL != strstr( (const char *)recvbuff, AT_FOTA_UPDATA_FAIL ) ) ||
//			( NULL != strstr( (const char *)recvbuff, AT_FOTA_END ) ) ||
//			( NULL != strstr( (const char *)recvbuff, AT_FOTA_DISCONNECT ) ) )
	{
		FOTA_mode = 0;
		FlashWrite( );
//		GpioWrite( &gpio_enPower, 0 );
		while(1);
	}
}

bool ATCommandJudgeFunc( uint8_t *recvbuff, uint8_t *cmd )
{
	char *str = NULL;
    uint16_t recvLen = 0;

	str = strstr( (const char *)recvbuff, (const char *)cmd );

	if( str != NULL )
	{
		if( DEVICE_STATE_CCLK == dev_state )
		{
			Take_CCLK_fromAT( (uint8_t *)recvbuff );
		}
		else if( DEVICE_STATE_IMEI == dev_state )
		{
			Take_IMEI_fromAT( (uint8_t *)recvbuff );
		}
		else if( DEVICE_STATE_IMSI == dev_state )
		{
			Take_IMSI_fromAT( (uint8_t *)recvbuff );
		}
		else if( DEVICE_CLOSE_CFUN == dev_state )
		{
			TimerSetValue( &app_ledRunTimer, 1000 );
			TimerStart( &app_ledRunTimer );
		}

		if( true == Take_Nuestats_fromAT( (uint8_t *)recvbuff, &recvLen ) )
		{
			Data_Set_AT( (uint8_t *)recvbuff, &recvLen );
			strncpy( (char *)AT_REGISTER_packet, (const char *)recvbuff, recvLen );
		}

		if( DEVICE_STATE_REBOOT != dev_state )
		{
			TimerSetValue( &NextCMDdelay, next_cmd_delay );
			TimerStart( &NextCMDdelay );
		}

		error_cnt = 0;
		return true;
	}
	else
	{
		if( DEVICE_STATE_CHECK_CSCON == dev_state )
		{
			search_cnt++;
			if( 0 == FOTA_mode )
			{
				TimerSetValue( &app_ledRunTimer, 500 );
				TimerStart( &app_ledRunTimer );
			}

			if( search_cnt > 50 )
			{
//				peak_start = true;
				search_cnt = 0;
				dev_state = DEVICE_CLOSE_CFUN;
			}

			TimerSetValue( &NextCMDdelay, next_cmd_delay );
			TimerStart( &NextCMDdelay );
		}
		else
		{
			if( error_cnt < 50 )
			{
				if( DEVICE_WAIT_OPEN != dev_state )
				{
					error_cnt ++;
					TimerSetValue( &NextCMDdelay, next_cmd_delay );
					TimerStart( &NextCMDdelay );
				}
			}
			else
			{
				while(1);
			}
		}

		return false;
	}
}

static void Composition_APN(void)
{
	strcpy( (char *)AT_APN_PACKET, "AT+CGDCONT=1,\"IP\",\"" );
	strncat( (char *)AT_APN_PACKET, (const char *)apninfo, strlen((const char *)apninfo) );
	strcat( (char *)AT_APN_PACKET, "\"\r\n");
}

static void Composition_IP(void)
{
	strcpy( (char *)AT_IP_PACKET, "AT+NCDP=" );
	strncat( (char *)AT_IP_PACKET, (const char *)serve_host_name, strlen((const char *)serve_host_name) );
	strcat( (char *)AT_IP_PACKET, ",");
	strncat( (char *)AT_IP_PACKET, (const char *)port, strlen((const char *)port) );
	strcat( (char *)AT_IP_PACKET, "\r\n" );
}

void AT_CMD_LoopMainEvent(void)
{
    switch(dev_state)
    {
	   case DEVICE_STATE_INIT:
		   SendATCmd( (uint8_t *)AT_CMD_TEST, (uint8_t)strlen(AT_CMD_TEST) );
		   break;

	   case DEVICE_CLOSE_ECHO:
		   SendATCmd( (uint8_t *)AT_CMD_ECHO, (uint8_t)strlen(AT_CMD_ECHO) );
		   break;

	   case DEVICE_CLOSE_CFUN:
		   SendATCmd( (uint8_t *)AT_CLOSE_CFUN, (uint8_t)strlen(AT_CLOSE_CFUN) );
		   break;

	   case DEVICE_STATE_CGMR:
		   SendATCmd( (uint8_t *)AT_CHECK_CGMR, (uint8_t)strlen(AT_CHECK_CGMR) );
		   break;

	   case DEVICE_STATE_IMEI:
		   SendATCmd( (uint8_t *)AT_CHECK_CGSN, (uint8_t)strlen(AT_CHECK_CGSN) );
		   break;

	   case DEVICE_OPEN_ERROR_MSG:
		   SendATCmd( (uint8_t *)AT_CMD_ERROR_MSG, (uint8_t)strlen(AT_CMD_ERROR_MSG) );
	   	   break;

	   case DEVICE_STATE_PSM:
		   SendATCmd( (uint8_t *)AT_CMD_PSM, (uint8_t)strlen(AT_CMD_PSM) );
		   break;

	   case DEVICE_STATE_EDRX:
		   SendATCmd( (uint8_t *)AT_CMD_EDRX, (uint8_t)strlen(AT_CMD_EDRX) );
		   break;

	   case DEVICE_STATE_IOT:
		   SendATCmd( (uint8_t *)AT_CMD_REGISTER_IOT, (uint8_t)strlen(AT_CMD_REGISTER_IOT) );
		   break;

	   case DEVICE_STATE_AUTO:
		   SendATCmd( (uint8_t *)AT_CMD_AUTO, (uint8_t)strlen(AT_CMD_AUTO) );
		   break;


	   case DEVICE_SET_IP:
		   Composition_IP( );
		   SendATCmd( (uint8_t *)AT_IP_PACKET, strlen((const char *)AT_IP_PACKET) );
		   break;

	   case DEVICE_OPEN_CFUN:
		   SendATCmd( (uint8_t *)AT_OPEN_CFUN, (uint8_t)strlen(AT_OPEN_CFUN) );
		   break;


	   case DEVICE_STATE_REBOOT:
		   SendATCmd( (uint8_t *)AT_CMD_REBOOT, (uint8_t)strlen(AT_CMD_REBOOT) );
		   break;

	   case DEVICE_STATE_IMSI:
		   SendATCmd( (uint8_t *)AT_CHECK_CIMI, (uint8_t)strlen(AT_CHECK_CIMI) );
		   break;

	   case DEVICE_STATE_APN:
		   Composition_APN( );
		   SendATCmd( (uint8_t *)AT_APN_PACKET, (uint8_t)strlen((const char *)AT_APN_PACKET) );
		   break;

	   case DEVICE_STATE_CHECK_CSQ:
		   SendATCmd( (uint8_t *)AT_CHECK_CSQ, (uint8_t)strlen(AT_CHECK_CSQ) );
		   break;

	   case DEVICE_STATE_CSCON:
		   SendATCmd( (uint8_t *)AT_CMD_CGATT, (uint8_t)strlen(AT_CMD_CGATT) );
		   break;

	   case DEVICE_STATE_CHECK_CSCON:
		   SendATCmd( (uint8_t *)AT_CHECK_CGATT, (uint8_t)strlen(AT_CHECK_CGATT) );
		   break;

	   case DEVICE_CHECK_ADDR:
		   SendATCmd( (uint8_t *)AT_CHECK_ADDR, (uint8_t)strlen(AT_CHECK_ADDR) );
//		   waiting_delay = CREAT_DELAY_TIME;
		   break;

	   case DEVICE_OPEN_RECVMSG:
		   SendATCmd( (uint8_t *)AT_OPEN_RECV_MSG, (uint8_t)strlen((const char *)AT_OPEN_RECV_MSG) );
		   break;

	   case DEVICE_STATE_NUESTATS:
		   SendATCmd( (uint8_t *)AT_CHECK_STATUS, (uint8_t)strlen((const char *)AT_CHECK_STATUS) );
		   break;

	   case DEVICE_STATE_SEND:
		   SendATCmd( (uint8_t *)AT_REGISTER_packet, (uint8_t)strlen((const char *)AT_REGISTER_packet) );
		   break;

	   case DEVICE_STATE_CCLK:
		   SendATCmd( (uint8_t *)AT_CHECK_CCLK, (uint8_t)strlen(AT_CHECK_CCLK) );
		   break;


   default:
	   dev_state = DEVICE_STATE_OVER;
	   break;

    }
}


/*
 * @fn          App_LoopAnalyseMsg
 *
 * @brief       analyse the receive data from uart
 *
 * @param       data   -   receive the
 *
 * @return      none
 */
void AT_CMD_LoopAnalyseRecvMsg(uint8_t *data, uint8_t datalen)
{
    switch(dev_state)
    {
       case DEVICE_STATE_CHECK:
    	  //analyse the msg
    	   if( true == ATCommandJudgeFunc( (uint8_t *)data, (uint8_t *)AT_RECV_OK ) )
    	   {
			   dev_state = DEVICE_STATE_INIT;
    	   }
    	   break;

	   case DEVICE_STATE_INIT:
		   if( true == ATCommandJudgeFunc( (uint8_t *)data, (uint8_t *)AT_RECV_OK ) )
		   {
			   dev_state = DEVICE_CLOSE_ECHO;
		   }
		   break;

	   case DEVICE_CLOSE_ECHO:
		   if( true == ATCommandJudgeFunc( (uint8_t *)data, (uint8_t *)AT_RECV_OK ) )
		   {
			   dev_state = DEVICE_STATE_CGMR;
		   }
		   break;

	   case DEVICE_STATE_CGMR:
		   if( true == ATCommandJudgeFunc( (uint8_t *)data, (uint8_t *)AT_RECV_OK ) )
		   {
			   dev_state = DEVICE_STATE_IMEI;
		   }
		   break;

	   case DEVICE_STATE_IMEI:
		   if( true == ATCommandJudgeFunc( (uint8_t *)data, (uint8_t *)AT_RECV_OK ) )
		   {
			   dev_state = DEVICE_STATE_IOT;
		   }
		   break;

	   case DEVICE_STATE_IOT:
		   if( true == ATCommandJudgeFunc( (uint8_t *)data, (uint8_t *)AT_RECV_OK ) )
		   {
			   dev_state = DEVICE_SET_IP;
		   }
		   break;

	   case DEVICE_SET_IP:
		   if( true == ATCommandJudgeFunc( (uint8_t *)data, (uint8_t *)AT_RECV_OK ) )
		   {
			   dev_state = DEVICE_CLOSE_CFUN;
		   }
		   break;

	   case DEVICE_CLOSE_CFUN:
		   if( true == ATCommandJudgeFunc( (uint8_t *)data, (uint8_t *)AT_RECV_OK ) )
		   {
//			   Staggered_peak_time( delay_basic_num );
			   dev_state = DEVICE_OPEN_CFUN;
		   }
		   break;

	   case DEVICE_OPEN_CFUN:
		   next_cmd_delay = NEXT_CSCON_CHECK;
		   if( true == ATCommandJudgeFunc( (uint8_t *)data, (uint8_t *)AT_RECV_OK ) )
		   {
			   next_cmd_delay = INIT_BC28_TIME;
			   dev_state = DEVICE_STATE_REBOOT;
		   }
		   break;

	   case DEVICE_STATE_REBOOT:
		   if( true == ATCommandJudgeFunc( (uint8_t *)data, (uint8_t *)AT_RECV_OK ) )
		   {
//			   TimerStop(  &app_ledRunTimer );
//			   GpioWrite( &gpio_ledRun, 0 );
//
			   dev_state = DEVICE_STATE_AUTO;
		   }
		   break;

	   case DEVICE_STATE_AUTO:
		   next_cmd_delay = NEXT_CMD_TIME;
		   if( true == ATCommandJudgeFunc( (uint8_t *)data, (uint8_t *)AT_RECV_OK ) )
		   {
			   TimerStop(  &app_ledRunTimer );
			   GpioWrite( &gpio_ledRun, 0 );
			   next_cmd_delay = NEXT_CMD_TIME;
			   dev_state = DEVICE_OPEN_ERROR_MSG;
		   }
		   break;

	   case DEVICE_WAIT_OPEN:
		   if( true == ATCommandJudgeFunc( (uint8_t *)data, (uint8_t *)AT_RECV_IOT ) )
		   {
			   TimerStop(  &app_ledRunTimer );
			   GpioWrite( &gpio_ledRun, 0 );
			   next_cmd_delay = NEXT_CMD_TIME;
			   dev_state = DEVICE_OPEN_ERROR_MSG;
		   }
		   break;

	   case DEVICE_OPEN_ERROR_MSG:
		   if( true == ATCommandJudgeFunc( (uint8_t *)data, (uint8_t *)AT_RECV_OK ) )
		   {
			   dev_state = DEVICE_STATE_PSM;
		   }
		   break;

	   case DEVICE_STATE_PSM:
		   if( true == ATCommandJudgeFunc( (uint8_t *)data, (uint8_t *)AT_RECV_OK ) )
		   {
			   dev_state = DEVICE_STATE_EDRX;
		   }
		   break;

	   case DEVICE_STATE_EDRX:
		   if( true == ATCommandJudgeFunc( (uint8_t *)data, (uint8_t *)AT_RECV_OK ) )
		   {
			   dev_state = DEVICE_STATE_IMSI;
		   }
		   break;

	   case DEVICE_STATE_IMSI:
		   if( true == ATCommandJudgeFunc( (uint8_t *)data, (uint8_t *)AT_RECV_OK ) )
		   {
			   dev_state = DEVICE_STATE_APN;
		   }
		   break;

	   case DEVICE_STATE_APN:
		   if( true == ATCommandJudgeFunc( (uint8_t *)data, (uint8_t *)AT_RECV_OK ) )
		   {
			   dev_state = DEVICE_STATE_CHECK_CSQ;
		   }
		   break;

	   case DEVICE_STATE_CHECK_CSQ:
		   if( true == ATCommandJudgeFunc( (uint8_t *)data, (uint8_t *)AT_RECV_OK ) )
		   {
			   dev_state = DEVICE_STATE_CSCON;
		   }
		   break;

	   case DEVICE_STATE_CSCON:
		   if( true == ATCommandJudgeFunc( (uint8_t *)data, (uint8_t *)AT_RECV_OK ) )
		   {
			   dev_state = DEVICE_STATE_CHECK_CSCON;

		   }
		   break;

	   case DEVICE_STATE_CHECK_CSCON:
		   next_cmd_delay = NEXT_CSCON_CHECK;
		   if( true == ATCommandJudgeFunc( (uint8_t *)data, (uint8_t *)AT_RECV_CGATT ) )
		   {
			   TimerStop(  &app_ledRunTimer );
			   GpioWrite( &gpio_ledRun, 0 );
			   next_cmd_delay = NEXT_CMD_TIME;
			   dev_state = DEVICE_CHECK_ADDR;

		   }
		   break;

	   case DEVICE_CHECK_ADDR:
		   if( true == ATCommandJudgeFunc( (uint8_t *)data, (uint8_t *)AT_RECV_OK ) )
		   {
			   dev_state = DEVICE_OPEN_RECVMSG;
		   }
		   break;

	   case DEVICE_OPEN_RECVMSG:
		   if( true == ATCommandJudgeFunc( (uint8_t *)data, (uint8_t *)AT_RECV_OK ) )
		   {
			   dev_state = DEVICE_STATE_NUESTATS;
		   }
		   break;

	   case DEVICE_STATE_NUESTATS:
		   if( true == ATCommandJudgeFunc( (uint8_t *)data, (uint8_t *)AT_RECV_OK ) )
		   {
			   dev_state = DEVICE_STATE_SEND;
		   }
		   break;

	   case DEVICE_STATE_SEND:
		   if( true == ATCommandJudgeFunc( (uint8_t *)data, (uint8_t *)AT_RECV_OK ) )
		   {
			   if( true == NB_registered )
			   {
				   dev_state = DEVICE_STATE_OVER;
				   AT_cmd_send = false;
			   }
			   else
			   {
				   if( 1 == RegisterPacketEnable )
				   {
					   TimerStart( &Heartbeat );
				   }
				   dev_state = DEVICE_STATE_CCLK;
			   }

		   }
		   break;

	   case DEVICE_STATE_CCLK:
		   if( true == ATCommandJudgeFunc( (uint8_t *)data, (uint8_t *)AT_RECV_OK ) )
		   {
			   dev_state = DEVICE_STATE_OVER;
			   uart2_send_flag = true;
			   NB_registered = true;
			   AT_cmd_send = false;
			   if( true == once_operate )
			   {
				   once_operate = false;
				   TimerStart( &CCLKbeat );
				   start_quary = true;
				   next_cmd_delay = DEF_DELAY_TIME;
				   TimerSetValue( &NextCMDdelay, next_cmd_delay );
				   TimerStart( &NextCMDdelay );
				   next_cmd_delay = NEXT_CMD_TIME;
//				   Uart2PutBuffer( &appConcentrator_Uart2, quary_status, 9 );
			   }
		   }
		   break;



       default:
    	   dev_state = DEVICE_STATE_CHECK;
    	   break;

    }
}


static uint8_t szMain_strToHex(uint8_t * inStr, uint8_t size, uint8_t * outStr)
{
	uint8_t index = 0;
	char tempStr[3];
	while (index * 2 < size)
	{
		memcpy(tempStr, inStr + index * 2, 2);
		tempStr[2] = '\0';
		outStr[index] = (uint8_t)strtol(tempStr, NULL, 16);
		index ++;
	}
	return index;
}

uint8_t hexTostr(uint8_t *src, uint8_t len, uint8_t *dtn)
{
   uint16_t i;
   uint8_t midbuf[3] = { 0 };


  for (i = 0; i < len; i++)
  {
    snprintf((char *)midbuf ,sizeof(midbuf),"%02X", (uint8_t)src[i]);
    memcpy(&dtn[i*2], midbuf, 2);
  }

  return (len*2);
}

/*
 * @fn          Take_IMEI_fromAT
 *
 * @brief       Take data from AT commands
 *
 * @param       data   -   receive from AT command
 *
 * @return      true or false
 */
void Take_IMEI_fromAT( uint8_t *data )
{
	char *token = NULL;
	char *str = NULL;

	str = strstr( (char *)data, "+CGSN");

	if( NULL != str)
	{
		token = strtok( str, ":");
		if( NULL != token )
		{
			token = strtok( NULL, "\r\n");
		}

		strncpy( (char *)AT_IMEI, (const char *)token, strlen(token) );

	}
}

/*
 * @fn          Take_IMSI_fromAT
 *
 * @brief       Take data from AT commands
 *
 * @param       data   -   receive from AT command
 *
 * @return      true or false
 */
void Take_IMSI_fromAT( uint8_t *data )
{
	char *token = NULL;

	token = strtok( (char *)data, "\r\n");

	strncpy( (char *)AT_IMSI, (const char *)token, strlen(token) );

//    Uart2PutBuffer( &appConcentrator_Uart2, AT_IMSI, strlen(token) );
}
/*
 * @fn          TakeMSG_fromAT
 *
 * @brief       Take data from AT commands
 *
 * @param       data   -   receive from AT command
 *
 * @return      true or false
 */

uint8_t TakeMSG_fromAT( uint8_t *data, uint16_t *datalen )
{
	char *token = NULL;
	char *str = NULL;
	uint8_t MSGbuf[TX_RX_MSG_NUM] = { 0 };
	uint8_t Hexbuf[TX_RX_MSG_NUM] = { 0 };
	uint8_t len = 0;

	str = strstr( (char *)data, "+NNMI");

	if( NULL != str)
	{
		token = strtok( str, ":");
		for( uint8_t i = 0; i < 2; i++ )
		{
			if( NULL != token )
			{
				token = strtok( NULL, ",");
			}

			if( 0 == i )
			{
				len = atoi( (const char* )token );
			}
			if( 1 == i )
			{
				strcpy( (char *)MSGbuf, token );
			}
		}

		szMain_strToHex( MSGbuf, len*2, Hexbuf);
		memset( data, 0, TX_RX_MSG_NUM );
		for( uint8_t j = 0; j < (len); j++)
		{
			data[j] = Hexbuf[j];
		}
		*datalen = len;
//		Uart2PutBuffer( &appConcentrator_Uart2, Hexbuf, (len/2) );
		return true;
	}
	return false;
}

/*
 * @fn          Take_Nuestats_fromAT
 *
 * @brief       Take useful data from the AT command
 *
 * @param       data   -   receive from AT command
 *
 * @return      true or false
 */
uint8_t Take_Nuestats_fromAT( uint8_t *data, uint16_t *datalen )
{
	char *token = NULL;
	char *str = NULL;

	uint16_t sigPwrnum = 0;
	uint8_t eclnum = 0;
	uint16_t snrnum = 0;
	uint16_t pcinum = 0;
	uint8_t nuestats[TX_RX_MSG_NUM] = { 0 };
	uint8_t midbuf[TX_RX_MSG_NUM] = { 0 };
	uint8_t heartpack[TX_RX_MSG_NUM] = { 0 };
	uint16_t n = 0;
	uint16_t byte = 0;
	uint16_t CRC = 0;

	str = strstr( (char *)data, "Signal");


	if( NULL != str)
	{
//		Uart2PutBuffer( &appConcentrator_Uart2, data, *datalen );
		token = strtok( str, ":" );
		for( uint8_t i = 0; i < 16; i++ )
		{
			if( i == 1 )
			{
				sigPwrnum = atoi((const char *)token+1) / 10;
			}
			if( 8 == i )
			{
				eclnum = atoi((const char *)token);
			}
			if( i == 10 )
			{
				snrnum = atoi((const char *)token);
				if( snrnum < 0 )
				{
					snrnum = 256 - snrnum;
				}
				else
				{
					snrnum = snrnum / 10;
				}
			}
			if( i == 13 )
			{
				pcinum = atoi((const char *)token);
			}

			if ( NULL != token )
			{
				if( i == 6 || i == 8 || i == 11 )
				{
					token = strtok( NULL, ":" );
				}
				else
				{
					token = strtok( NULL, "\r\n" );
				}

			}
		}

		midbuf[n++] = 0x00;
		midbuf[n++] = 0x00;
		midbuf[n++] = 0x00;
		midbuf[n++] = 0x01;
		midbuf[n++] = 0xd0;
		midbuf[n++] = 0x01;
		for( uint8_t i = 0; i < 15; i++ )
		{
			midbuf[n++] = AT_IMEI[i];
		}
		midbuf[n++] = 0x02;
		for( uint8_t j = 0; j < 15; j++ )
		{
			midbuf[n++] = AT_IMSI[j];
		}
		midbuf[n++] = 0x03;
		midbuf[n++] = sigPwrnum;
		midbuf[n++] = 0x04;
		midbuf[n++] = snrnum;
		midbuf[n++] = 0x05;
		midbuf[n++] = pcinum >> 8;
		midbuf[n++] = pcinum;
		midbuf[n++] = 0x06;
		midbuf[n++] = eclnum;

		nuestats[byte++] = 0xaa;
		nuestats[byte++] = 0x55;
		nuestats[byte++] = n >> 8;
		nuestats[byte++] = n;
		nuestats[byte++] = 0x03;
		nuestats[byte++] = 0x00;
		nuestats[byte++] = 0x03;
		nuestats[byte++] = 0x00;
		CRC = CRC16Calc( midbuf, n );
		nuestats[byte++] = CRC >> 8 ;
		nuestats[byte++] = CRC ;
        for( uint8_t k = 0; k < n; k++ )
        {
        	nuestats[byte++] = midbuf[k];
        }

        hexTostr( nuestats, byte, heartpack);

		memset( data, 0, TX_RX_MSG_NUM );
		strncpy( (char *)data, (const char *)heartpack, strlen((const char *)heartpack) );
		*datalen = strlen((const char *)heartpack);
		return true;
	}
	return false;
}

/*
 * @fn          CCLKData_with_CRC
 *
 * @brief       Take time from the AT command
 *
 * @param       data   -   receive from AT command
 *
 * @return      //true or false
 */
void CCLKData_with_CRC( uint8_t *data, uint16_t datalen )
{
	uint8_t midbuf[TX_RX_MSG_NUM] = { 0 };
	uint8_t bufflen = 0;
	uint16_t CRC = 0;

	midbuf[bufflen++] = 0x24;
	midbuf[bufflen++] = 0x0e;
	midbuf[bufflen++] = 0x00;
	midbuf[bufflen++] = 0x00;
	midbuf[bufflen++] = 0x00;
	midbuf[bufflen++] = 0x01;

	for( uint8_t i = 0; i < datalen; i++ )
	{
		midbuf[bufflen++] = data[i];
	}

	CRC = CRC16Calc( midbuf, bufflen );
	midbuf[bufflen++] = CRC >> 8 ;
	midbuf[bufflen++] = CRC ;

	Uart2PutBuffer( &appConcentrator_Uart2, midbuf, bufflen );
}

/*
 * @fn          Take_CCLK_fromAT
 *
 * @brief       Take time from the AT command
 *
 * @param       data   -   receive from AT command
 *
 * @return      //true or false
 */
void Take_CCLK_fromAT( uint8_t *data )
{
	char *token = NULL;
	char *str = NULL;
	uint8_t zonenum = 0;
	uint8_t CCLK[7] = { 0 };

	str = strstr( (char *)data, "+CCLK");

	if( NULL != str)
	{
		token = strtok( str, ":/,+\r\n" );
		for( uint8_t i = 0; i < 8; i++ )
		{
			if( 7 == i)
			{
				zonenum = atoi( (const char *)token );
				CCLK[3] = CCLK[3] + (zonenum/4);
				if(CCLK[3] > 23)
				{
					CCLK[3] -= 24;
				}
				break;

			}
			if( NULL != token )
			{
				token = strtok( NULL, ":/,+\r\n" );
			}


			CCLK[i] = atoi( (const char *)token );
		}

		CCLKData_with_CRC( (uint8_t *)CCLK, 6 );
	}
}
/*
 * @fn          Data_Set_AT
 *
 * @brief       Data composition AT command
 *
 * @param       data   -   receive from uart
 *
 * @return      true or false
 */
void Data_Set_AT( uint8_t *data, uint16_t *datalen )
{
	uint8_t AT_cmd[TX_RX_MSG_NUM] = { 0 };
	uint8_t strbuf[TX_RX_MSG_NUM] = { 0 };
	uint16_t strlength = 0;
	uint8_t lenbuf[5] = { 0 };

//	strlength = *datalen;
	strlength = hexTostr( data, strlen((const char *)data), strbuf );

	strcpy( (char *)AT_cmd, "AT+NMGS=" );
	sprintf( (char *)lenbuf, "%u", strlength/2 );
	strncat( (char *)AT_cmd, (const char *)lenbuf, strlen( (const char *)lenbuf) );
	strcat( (char *)AT_cmd, "," );

	strncat( (char *)AT_cmd, (const char *)strbuf, strlength );

	strcat( (char *)AT_cmd, "\r\n");

    memset( data, 0, TX_RX_MSG_NUM );
    strncpy( (char *)data, (const char *)AT_cmd, strlen((const char *)AT_cmd) );
    *datalen = strlen((const char *)AT_cmd);
}
