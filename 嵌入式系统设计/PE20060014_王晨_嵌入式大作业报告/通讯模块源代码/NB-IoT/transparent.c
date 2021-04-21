/*************************************************************************
  \file				transparent.c
  \brief
	This file implements transparent transport application.

  \author
	www.shuncom.com
  \internal
  Revision History:
  	May. 11, 2018		Johnason - Created

**************************************************************************/
/*********************************************************************
 * 					INCLUDES
 */
#include <string.h>
#include "ZComDef.h"
#include "board-config.h"
#include "board.h"
#include "transparent.h"
#include "uart.h"
#include "osal.h"
#include "gpio.h"
#include "OSAL_Nv.h"
#include "timer.h"
#include "ZGlobals.h"
#include "AT_ProcessFunc.h"
#include "ATCommand.h"
#include "CRC.h"
#include "szconfig.h"

/*********************************************************************
 * 							 MACROS
 */
#define AT_AUTO_MODE				1

/*********************************************************************
 * 							CONSTANTS
 */


uint8_t timeOutFlg = false;
uint8_t Uart_TaskID;
uint8_t appConcentrator_TaskID;

static TimerEvent_t app_wdgTimer;/* wdg every second */
static TimerEvent_t app_RecvTimeout;/* Uart Timeout every second */
TimerEvent_t app_waitingconfig;



/* The run led, 0:open */
Gpio_t gpio_ledRun;
/* The power led, 0:open */
Gpio_t gpio_enPower;

Gpio_t gpio_reset;


uint8_t TimeOutFlag = false;

/*********************************************************************
* 							GLOBAL VARIABLES
*/
/*
 * UART objects
 */
Uart_t appConcentrator_Uart;
Uart_t appConcentrator_Uart2;

extern uint32_t waiting_delay;

TimerEvent_t NextCMDdelay;
TimerEvent_t Heartbeat;
TimerEvent_t CCLKbeat;
TimerEvent_t app_ledRunTimer; /* toggle every second */
TimerEvent_t app_nextcfg_msg;

TimerEvent_t Quary_device;
uint8_t hour_times = 0;

extern bool uart2_send_flag;
extern bool AT_cmd_send;

extern DeviceState dev_state;

uint32_t Hearttime = 0;
uint8_t quary_status[10] = { 0x10, 0x09, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0xd8 };
bool start_quary = false;
uint8_t led_cnt = 0;
/*********************************************************************
* 			      	 		    FUNCTIONS
*/
/*
 * @fn          App_UartRecvTimeOutEvent
 *
 * @brief       Receive the uart data timeout
 *
 * @param       none
 *
 * @return      none
 */
static void App_UartRecvTimeOutEvent(void)
{
	TimerStop( &app_RecvTimeout );

	osal_set_event(appConcentrator_TaskID, TRANSPARENT_MAIN_LOOP_EVT);
}

/*
 * @fn          Next_CMD_EVENT
 *
 * @brief       Delay time of the next instruction
 *
 * @param       none
 *
 * @return      none
 */
void Next_CMD_EVENT(void)
{
   TimerStop( &NextCMDdelay );
   if( true == start_quary )
   {
	   start_quary = false;
	   TimerStart( &Quary_device );
	   Uart2PutBuffer( &appConcentrator_Uart2, quary_status, 9 );
   }

   osal_set_event(appConcentrator_TaskID, TRANSPARENT_MAIN_LOOP_EVT);
}

/*
 * @fn          Heart_Beat_EVENT
 *
 * @brief       Delay time of the next instruction
 *
 * @param       none
 *
 * @return      none
 */
void Heart_Beat_EVENT(void)
{
   TimerStop( &Heartbeat );

   AT_cmd_send = true;
   dev_state = DEVICE_STATE_NUESTATS;

//   	Uart2PutBuffer( &appConcentrator_Uart2,"1001\r\n", 6 );
   if( TRUE == RegisterPacketEnable )
   {
	   TimerSetValue( &Heartbeat, (Hearttime * 1000) );
	   TimerStart( &Heartbeat );
   }
   osal_set_event(appConcentrator_TaskID, TRANSPARENT_MAIN_LOOP_EVT);

}

/*
 * @fn          Inquire_Device_EVENT
 *
 * @brief       Timely query device status
 *
 * @param       none
 *
 * @return      none
 */
void Inquire_Device_EVENT(void)
{
	uint8_t quary_elec_energy[10] = { 0x25, 0x09, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7a, 0xdb };

	TimerStop( &Quary_device );
	if( hour_times < 2 )
	{
		Uart2PutBuffer( &appConcentrator_Uart2, quary_status, 9 );
		hour_times++;
	}
	else
	{
		Uart2PutBuffer( &appConcentrator_Uart2, quary_elec_energy, 9 );
		hour_times = 0;
	}
    TimerSetValue( &Quary_device, DEF_QUARY_TIME );
    TimerStart( &Quary_device );
    osal_set_event(appConcentrator_TaskID, TRANSPARENT_MAIN_LOOP_EVT);
}

/*
 * @fn          CCLK_Beat_EVENT
 *
 * @brief       Delay time of the next instruction
 *
 * @param       none
 *
 * @return      none
 */
void CCLK_Beat_EVENT(void)
{
   TimerStop( &CCLKbeat );

   uart2_send_flag = false;
   AT_cmd_send = true;
   dev_state = DEVICE_STATE_CCLK;
   osal_set_event(appConcentrator_TaskID, TRANSPARENT_MAIN_LOOP_EVT);
   TimerSetValue( &CCLKbeat, (DEF_CCLK_TIME * 1000) );
   TimerStart( &CCLKbeat );
}

void OnAppLedimerEvent( void )
{
	TimerStop( &app_ledRunTimer );
//	Uart2PutBuffer( &appConcentrator_Uart2,"1239",4);

	if( DEVICE_OPEN_CFUN == dev_state )
	{
		if( led_cnt < 3 )
		{
			led_cnt++;
			GpioWrite( &gpio_ledRun, 1 );
		}
		else
		{
			led_cnt = 0;
			GpioWrite( &gpio_ledRun, 0 );
		}
		TimerStart( &app_ledRunTimer );
	}
	else
	{
		GpioToggle( &gpio_ledRun );
		TimerSetValue( &app_ledRunTimer, 500 );
	}
    TimerStart( &app_ledRunTimer );
}

void OnAppWaitingCFGEvent( void )
{
	TimerStop( &app_waitingconfig );

	StartUpQuitCfg( );
	UART_DRV_Deinit( 2 );
	UserUartInit( );
#if AT_AUTO_MODE
	uart2_send_flag = false;
#endif
	GpioWrite( &gpio_enPower, 1 );
}

void OnAppNextCFGEvent( void )
{
//	Uart2PutBuffer( &appConcentrator_Uart2,"1239",4);
	TimerStop( &app_nextcfg_msg );

	Wod_Refresh( );
	osal_set_event( appConcentrator_TaskID, TRANSPARENT_CONFIG_EVT );
}


/*
 * @fn          OnAppWdgTimerEvent
 *
 * @brief       .
 *
 * @param       none
 *
 * @return      none
 */
static void OnAppWdgTimerEvent( void )
{
	Wod_Refresh( );
    TimerSetValue( &app_wdgTimer, 500 );
    TimerStart( &app_wdgTimer );
}

/*
 * @fn          App_LpUartRxGetMsg
 *
 * @brief       get the lpuart data
 *
 * @param       none
 *
 * @return      none
 */
void App_LpUartRxGetMsg(void)
{
    uint16_t tempLen = 0;
    uint8_t datalen = 0;
    uint16_t datamid = 0;
    uint8_t state;
	uint8_t buff[TX_RX_MSG_NUM] = { 0 };
	uint8_t strbuff[TX_RX_MSG_NUM] = { 0 };
	uint16_t hearttime = 0;
	uint16_t CRC = 0;
	uint8_t crclow[2] = { 0 };
	uint8_t crchig[2] = { 0 };

	state = UartGetBuffer( &appConcentrator_Uart, buff, TX_RX_MSG_NUM, &tempLen );
	if( state == STATE_SUCCESS )
	{
		TimerStop( &app_RecvTimeout );
		waiting_delay = DEF_DELAY_TIME;

		if( TRUE == DebugInfoEnable )
		{
			Uart2PutBuffer( &appConcentrator_Uart2, buff, tempLen );
		}

//		Take_CCLK_fromAT( buff, &tempLen );
//		Take_IMEI_fromAT( buff );
//		Take_Nuestats_fromAT( buff, &tempLen );

#if AT_AUTO_MODE
        if( 0 == FOTA_mode )
        {
			if( true == TakeMSG_fromAT( (uint8_t *)buff, &tempLen ) )
			{
				CRC = CRC16Calc( buff, tempLen - 2 );
				crclow[0] = CRC >> 8 ;
				crchig[0] = CRC ;
				if( (0xaf == buff[0]) && ( 0x5a == buff[1] ) && ( crclow[0] == buff[6] ) && ( crchig[0] == buff[7] ) )
				{
					if( 0x00 == buff[3] )
					{
						RegisterPacketEnable = FALSE;
						TimerStop( &Heartbeat );
					}
					else
					{
						RegisterPacketEnable = TRUE;
						hearttime = ( ( buff[4] << 8) | ( buff[5] ) );
						memset( HeartbeatTime, 0, 6 );
						sprintf( (char *)HeartbeatTime, "%u", hearttime );
						Hearttime = hearttime;
						TimerSetValue( &Heartbeat, (Hearttime * 1000) );
						TimerStart( &Heartbeat );
					}

					FlashWrite( );
				}
#if 1
				else if( (0xaf == buff[0]) && ( 0x5b == buff[1] ) )
				{
					datalen = buff[2];
					if( ( crclow[0] == buff[tempLen-2] ) && ( crchig[0] == buff[tempLen-1] ) )
					{
						hexTostr( buff+3, tempLen-8, strbuff);
						memset( apninfo, 0, SET_APNINFO_LEN+1 );
						strncpy( (char *)apninfo, (const char *)strbuff, datalen - 6 );

						memset( serve_host_name, 0, SET_HOST_NAME_LEN+1);
						memset( port, 0, 6 );

						for(uint8_t i = 0;i < 4;i++)
						{
							datamid = buff[(datalen-6+3)+i];
							sprintf( (char *)strbuff, "%d", datamid );
							strcat( (char *)serve_host_name, (char *)strbuff);
							if(i != 3)
							{
								strcat( (char *)serve_host_name, ".");
							}
						}
						datamid  = buff[datalen-2+3]<<8 | buff[datalen-1+3];
						sprintf( (char *)strbuff, "%d", datamid);
						strcat((char *)port , (char *)strbuff);

						FlashWrite( );
					}
				}
#else
				else if( (0xaf == buff[0]) && ( 0x5b == buff[1] ) )
				{
					datalen = buff[2];
					if( ( crclow[0] == buff[tempLen-2] ) && ( crchig[0] == buff[tempLen-1] ) )
					{
						hexTostr( buff+3, tempLen-2, strbuff);
						memset( apninfo, 0, SET_APNINFO_LEN+1 );
						strncpy( (char *)apninfo, (const char *)strbuff, datalen );
						FlashWrite( );
					}
				}
				else if( (0xaf == buff[0]) && ( 0x5c == buff[1] ) && ( crclow[0] == buff[9] ) && ( crchig[0] == buff[10] ) )
				{
					memset( serve_host_name, 0, SET_HOST_NAME_LEN+1);
					memset( port, 0, 6 );
					for(uint8_t i = 0;i < 4;i++)
					{
						datamid = buff[3+i];
						sprintf( (char *)strbuff, "%d", datamid );
						strcat( (char *)serve_host_name, (char *)strbuff);
						if(i != 3)
						{
							strcat( (char *)serve_host_name, ".");
						}
					}
					datamid  = buff[7]<<8 | buff[8];
					sprintf( (char *)strbuff, "%d", datamid);
					strcat((char *)port , (char *)strbuff);
					FlashWrite( );
				}
#endif
				else
				{
					Uart2PutBuffer( &appConcentrator_Uart2, buff, tempLen );
				}

			}
        }
        else
        {
//        	FOTACommandJudgeFunc( buff );
        }
		/*analyse the msg*/
		if( true == AT_cmd_send )
		{
			AT_CMD_LoopAnalyseRecvMsg( buff, tempLen );
		}
#endif
	}
}


/*
 * @fn          App_LpUartRxGetMsg
 *
 * @brief       get the lpuart data
 *
 * @param       none
 *
 * @return      none
 */
void App_UartRxGetMsg(void)
{
    uint16_t tempLen = 0;
    uint16_t strlength = 0 ;
    uint8_t state;
	uint8_t buff[TX_RX_MSG_NUM] = { 0 };
	uint8_t strbuff[TX_RX_MSG_NUM] = { 0 };
	memset( buff, 0, TX_RX_MSG_NUM );

	state = Uart2GetBuffer(&appConcentrator_Uart2, buff,TX_RX_MSG_NUM, &tempLen);
	if( state == STATE_SUCCESS)
	{
		if( CFGNULL == Cfg_mode )
		{
			if( true == uart2_send_flag )
			{
				if( TRUE == DebugInfoEnable )
				{
					Uart2PutBuffer( &appConcentrator_Uart2, buff, tempLen );
				}

#if AT_AUTO_MODE
				if( tempLen > 0 )
				{
					strlength = hexTostr( buff, tempLen, strbuff );
					Data_Set_AT( strbuff, &strlength );
				}

				if( TRUE == DebugInfoEnable )
				{
					Uart2PutBuffer( &appConcentrator_Uart2, strbuff, strlength );
				}

				UartPutBuffer( &appConcentrator_Uart, strbuff, strlength );
#else
				UartPutBuffer( &appConcentrator_Uart, buff, tempLen );
#endif
			}
		}
		else
		{
			UartRecvConfig( buff, tempLen );
		}
	}
}

/*
 * @fn          App_Local_LpUartRXProcess
 *
 * @brief       receive the lpuart data event
 *
 * @param       none
 *
 * @return      none
 */
static void App_Local_LpUartRXProcess(void)
{
	App_LpUartRxGetMsg();
}

/*
 * @fn          App_Local_uartRXProcess
 *
 * @brief       receive the uart data event
 *
 * @param       none
 *
 * @return      none
 */
static void App_Local_uartRXProcess(void)
{
	App_UartRxGetMsg();
}

/*********************************************************************
 * 						LOCAL FUNCTION PROTOTYPES
 */

/*********************************************************************
 * 							LOCAL FUNCTIONS
 */

void UserUartInit(void)
{
   uint32_t  bandr = 0;

   switch(BaudRateType)
    {
       case Baudrate_ERROR:
	   	   bandr = 38400;
	   break;

	   case Baudrate_1200:
	   	   bandr = 1200;
	   break;

	   case Baudrate_2400:
	   	   bandr = 2400;
	   break;

	   case Baudrate_4800:
	   	   bandr = 4800;
	   break;

	   case Baudrate_9600:
	   	   bandr = 9600;
	   break;

	   case Baudrate_19200:
	   	   bandr = 19200;
	   break;

	   case Baudrate_38400:
	   	   bandr = 38400;
	   break;

	   case Baudrate_57600:
	   	   bandr = 57600;
	   break;

	   case Baudrate_115200:
	   	   bandr = 115200;
	   break;

	   default:
	   	   bandr = 38400;
	   break;
	}
    Uart2Init(&appConcentrator_Uart2, UART_2, appConcentrator_TaskID,
    		TRANSPARENT_UART2_RX_EVT);
    Uart2Config( &appConcentrator_Uart2, bandr, DataBitType,
    		StopBitType, CheckBitType, 0 );
//    Uart2PutBuffer( &appConcentrator_Uart2,"Uinit\r\n", 7);
}
/*
 * @fn          concentrator_Init
 *
 * @brief       Initialization function for the application task.
 *
 * @param       task_id  -  task id
 *
 * @return      none
 */
void transparent_Init( uint8_t task_id )
{
	appConcentrator_TaskID = task_id;
	/*lpuart 2 init */
    UartInit(&appConcentrator_Uart, UART_1, appConcentrator_TaskID,
    		TRANSPARENT_UART_RX_EVT);
    UartConfig( &appConcentrator_Uart, BOARD_LPUART_BAUD, UART_8_BIT,
    		UART_1_STOP_BIT, NO_PARITY, 0 );

    Uart2Init(&appConcentrator_Uart2, UART_2, appConcentrator_TaskID,
    		TRANSPARENT_UART2_RX_EVT);

    if( CFGSTART == Cfg_mode )
    {
		Uart2Config( &appConcentrator_Uart2, BOARD_UART_BAUD, UART_8_BIT,
				UART_1_STOP_BIT, NO_PARITY, 0 );
    }

    GpioInit( &gpio_ledRun, BC28_LED_RUN, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
    GpioInit( &gpio_enPower, BC28_EN_POWER, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );

    WodInit( );
    Hearttime = atoi((const char*)HeartbeatTime);

    TimerInit( &app_wdgTimer, OnAppWdgTimerEvent );
    TimerSetValue( &app_wdgTimer, 800 );
    TimerStart( &app_wdgTimer );

    if( TRUE == DebugInfoEnable )
    {
		Uart2PutBuffer( &appConcentrator_Uart2, (uint8_t *)SW_VER, strlen((const char *)SW_VER) );
    }
    Cfg_hisr_auto( );

    TimerInit( &app_waitingconfig, OnAppWaitingCFGEvent );
    TimerSetValue( &app_waitingconfig, 5000 );
    TimerStart( &app_waitingconfig );

    TimerInit( &app_nextcfg_msg, OnAppNextCFGEvent );

    /*timer*/
    TimerInit( &app_ledRunTimer, OnAppLedimerEvent );
    TimerInit( &app_RecvTimeout, App_UartRecvTimeOutEvent );
	TimerInit( &NextCMDdelay, Next_CMD_EVENT );
	TimerInit( &Heartbeat, Heart_Beat_EVENT );
	TimerSetValue( &Heartbeat, (Hearttime * 1000) );

	TimerInit( &CCLKbeat, CCLK_Beat_EVENT );
	TimerSetValue( &CCLKbeat, (DEF_CCLK_TIME * 1000) );
	TimerInit( &Quary_device, Inquire_Device_EVENT );
	TimerSetValue( &Quary_device, DEF_QUARY_TIME );
//    Uart2PutBuffer( &appConcentrator_Uart2,"AT\r\n",3);
  //  UartPutBuffer( &appConcentrator_Uart,AT_CMD_TEST,strlen(AT_CMD_TEST));

}

/*
 * @fn          transparent_event_loop
 *
 * @brief       Event Loop Processor for application.
 *
 * @param       none
 *
 * @return      none
 */
uint16_t transparent_event_loop( uint8_t task_id, uint16_t events )
{
	if ( events & TRANSPARENT_UART_RX_EVT )
	{
		App_Local_LpUartRXProcess();
		return ( events ^ TRANSPARENT_UART_RX_EVT );
	}

	if ( events & TRANSPARENT_MAIN_LOOP_EVT )
	{
		AT_CMD_LoopMainEvent();
		TimerSetValue( &app_RecvTimeout, waiting_delay );
		TimerStart( &app_RecvTimeout );

		return ( events ^ TRANSPARENT_MAIN_LOOP_EVT );
	}

	if ( events & TRANSPARENT_UART2_RX_EVT )
	{
		App_Local_uartRXProcess();
		return ( events ^ TRANSPARENT_UART2_RX_EVT );
	}

	if ( events & TRANSPARENT_CONFIG_EVT )
	{
		Search_Flag_info( );
		return ( events ^ TRANSPARENT_CONFIG_EVT );
	}

	return 0;
}
