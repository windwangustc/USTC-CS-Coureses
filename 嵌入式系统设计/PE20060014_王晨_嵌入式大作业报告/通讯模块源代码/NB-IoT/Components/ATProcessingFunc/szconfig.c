/*!
 * \file      delay.c
 *
 * \brief     Delay implementation
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \author    6646
 *
 * \time      2019/4/22 10:01
 */
#include "szconfig.h"
#include "ZGlobals.h"
#include "uart-board.h"
#include "uart.h"
#include "delay.h"
#include "OSAL_Nv.h"
#include "osal.h"
#include "transparent.h"
#include "timer.h"
#include "ZComDef.h"

extern uint8_t appConcentrator_TaskID;
extern TimerEvent_t app_waitingconfig;

//static  uint8_t DEVICE[]  = "LTE-MOD";         //设备信息
//static  uint8_t HW_VER[]  = "SZ11-08_V1.0";        //硬件版本--对应PCB版本
//static  uint8_t SHUNCOM[] = "WWW.SHUNCOM.COM";    //公司信息

uint8_t SW_VER[]  = "BC28";
uint8_t UpdataConfig_PERIOD = FALSE;
//uint8_t uartcount = 0;
//volatile uint8_t cfg_enter = true;
uint8_t  gUserLoginPassFlag = FALSE;
volatile uint8_t UartInitFLAG = FALSE;
volatile uint8_t start_cfg = 0;
uint8_t RETURN_OK[]  = "\r\nOK\r\n";
uint8_t RETURN_ERROR[]  = "\r\nERROR\r\n";
uint8_t Cfg_mode = CFGSTART;
ProcessState_t UpdataSysConfigInfoPro = {STEP0,AT_INIT,3};
//uint8_t gUserLoginPassFlag = FALSE;
//uint32_t quitconfig_count = 0 ;
//uint32_t soctimeout_count = 0 ;
volatile uint8_t CfgSave = FALSE ;
//uint8_t data_count =0;
//uint8_t ConfigOut = false;
//uint8_t flag_time = true;
//uint8_t ResetPoweOn = 0;
//oa_soc_set_parameter g_soc_param[0] = {0};
//uint8_t ConfigUart =true;
//uint8_t Cfg_out = false;
//uint8_t RecvDataFlag = false;
uint8_t CfgOutFlag = false;
//uint8_t CheckCount = 0;
//uint8_t StringUpdata[200] = {0};

extern TimerEvent_t app_nextcfg_msg;


//oa_soc_set_parameter g_soc_param[1] =
//{
//    CUR_PAGE,
//    {"[SZ-LTE]"},    //dtuid
//    {"120.26.43.233"},   //serve_ipaddr 114.80.253.25
//    {"180.101.147.115"},   //serve_host_name 1
//    {"5683"}, //port:21
//
//    {"shuncom"},   ///ftp user name
//    {"12345"},     ///ftp password
//
//    {"psm0.eDRX0.ctnb"},{""},{""},//apninfo
//    FALSE,//rssi_disp
//
//	/*{"cmt.3322.org"},	//serve_host_name 2
//	{"2012"}, //port:21*/
//
//    //{0x00, 0x00}, //checktail
//    1,  //debug
//    0, //device
//
//	0,                    //SMS off
//    {"863703030000012"},//13816590761
//    {"861530000000012"},
//
//    Baudrate_38400,             //默认9600，配置38400
//    oa_uart_len_8,
//    oa_uart_pa_none,           //ParityBit_NONE
//    oa_uart_sb_1,
//    1,                            //on  注册包使能
//    {"DC41888842FF"},     //注册包等不能为SZ开头
//    0,
//	{"20"},                              //1-65535s
//    {"DC41888842FF"},
////    {"1"},
////    {"12348"},
//    {"121.41.24.34"},   //serve_host_name sz
//    {"6008"} //port  -  sz
//    //{"shuncom.3322.org"},   //serve_host_name sz
//    //{"12346"}, //port  -  sz
//};

#if 1




void Search_Flag_info(void)
{

	if(UpdataConfig_PERIOD)
	{
		TimerSetValue( &app_nextcfg_msg, 10 );
		TimerStart( &app_nextcfg_msg );
		UpdataSysConfigParaPro();
		CfgOutFlag = true;
	// UpdataConfig_PERIOD = false;
	}
}




void ClearUserSetBufferTail(uint8_t *pUserSetBuf)
{
  uint8_t UserBuffIndex = 0;
  uint8_t UserBuffLength = 0;
  UserBuffLength = (uint8_t)strlen((char *)pUserSetBuf);
  for(UserBuffIndex = 0;UserBuffIndex < UserBuffLength;UserBuffIndex++)
	  pUserSetBuf[UserBuffIndex] = 0x00;
}


uint8_t StringSearch(uint8_t *pMajString, uint8_t MajStringLength, uint8_t *pSubString, uint8_t SubStringLength)
{

  uint8_t majcount,subcount,intcount,istrlength;

  if((MajStringLength == 0) || (SubStringLength == 0)) return(0);

  if(MajStringLength < SubStringLength) return(0);

  istrlength = SubStringLength;

  for(majcount=0,subcount=0;majcount<MajStringLength;majcount++)
  {
    /* find the string */
    if(pMajString[majcount] == pSubString[subcount])
    {
      majcount++;
      subcount++;
      --istrlength;        //指针顺移

      for(intcount=majcount;intcount<MajStringLength;intcount++,subcount++)
      {
        if(pMajString[intcount] == pSubString[subcount]) --istrlength;
        else break;

        if(istrlength == 0) return(majcount);   //注意顺序 查找成功 返回永远>0
      }
      subcount = 0;        //重新查找
      majcount--;          //还原位置   !!!NOTE BUG
      istrlength = SubStringLength;
    }
  }
  return(0);

}

void uart_Senddata( uint8_t *data )
{
	UART_DRV_StartSendData(2, data, strlen((const char *)data));
	DelayMs(15);
}

//void uart_SendNumMsg( uint8_t *data, uint8_t len)
//{
//	UART_DRV_StartSendData( 2, data, len );
//	DelayMs(45);
//}

static void SysConfigParaDefault(void)
{
   /*reset the para*/
//     g_soc_param[0] = soc_param;
//	 zgSetItem( ZCD_NV_NB_CONFIG, sizeof(soc_param), &soc_param );
//  	 BoardDisableIrq();
//	 osal_nv_write( ZCD_NV_NB_CONFIG, 0, sizeof(soc_param), &soc_param );
//	 BoardEnableIrq();

}

void FlashWrite(void)
{
	uint8_t setDefault = TRUE;

	zgInitItems( setDefault );

	return ;

}

static void WelcomeConfigDisp(void)
{
   uart_Senddata((uint8_t *)"\r\nWelcome to set mode!\r\n");
   uart_Senddata((uint8_t *)"Please connect config assist to continue...\r\n");
   uart_Senddata((uint8_t *)"\r\n+LOGIN IN\r\n");
}



void Cfg_hisr_auto(void)
{

	Cfg_mode = CFGSTART;
  if(DebugInfoEnable)
  {
    WelcomeConfigDisp();
  }

}


static void SYSTEM_RESET(void)
{
	 while(1);
}


void oa_app_config_entry(uint8_t * param, uint32_t len)
{
	  uint32_t  iter = 0;
	  uint8_t param_len=0;
	  uint8_t param_buf[100]={0};
//	  uint8_t send_buf[100]={0};
	  uint8_t pStartPointer = 0;
	  uint8_t pEndPointer = 0;
	  uint8_t param_item = UndefineItem;
//	  char imei[20] = {0};
//	  uint32_t im = 0;

	  if(len>100) param_len = 100;
	  else param_len = len;

	  for(iter=0;iter<param_len;iter++)
	  {
		 param_buf[iter] = param[iter];
	  }

	  if(StringSearch(param,len,(uint8_t *)"SZ+READ",7))
	  {
		param_item=Assist_read;
	  }
	  else if(StringSearch(param,len,(uint8_t *)"SZ+RESET",8) )
	  {
		param_item=Assist_reset;
	  }
	  else if(StringSearch(param,len,(uint8_t *)"SZ+SAVE",7))
	  {
		param_item=Assist_save;
	  }

	  else if(StringSearch(param,len,(uint8_t *)"SZ+QUIT",7))
	  {
		param_item=Assist_quit;
	  }

	  //get Dtu IP address
//	  else if(StringSearch(param,len,(uint8_t *)"SZ+IPADD",8) == 1)
//	  {
//		param_item=Assist_ipadd;
//	  }

	  //SZ+DUSER=admin
	  else if(StringSearch(param,len,(uint8_t *)"SZ+DEBUG",8))
	  {
		param_item=Assist_debug;
	  }

	//++
//	  else if(StringSearch(param,len,(uint8_t *)"SZ+DEVSHOW",10))	 //SZ+DEVSHOW=ON/OFF
//	  {
//		param_item=Assist_devshow;
//	  }
//	  else if(StringSearch(param,len,(uint8_t *)"SZ+DEVICE",9))
//	  {
//		param_item=Assist_device;
//	  }
//	  else if(StringSearch(param,len,(uint8_t *)"SZ+SMSCOM",9))	 //SZ+SMSCOM=ON/OFF
//	  {
//		param_item=Assist_smscom;
//	  }
//	  else if(StringSearch(param,len,(uint8_t *)"SZ+TEL",6))	  //get user number
//	  {
//		param_item=Assist_tel;
//	  }
//	  else if(StringSearch(param,len,(uint8_t *)"SZ+DUSER",8))
//	  {
//		param_item=Assist_duser;
//	  }
//	  else if(StringSearch(param,len,(uint8_t *)"SZ+DPSWD",8)) 	//pasword
//	  {
//		param_item=Assist_dpswd;
//	  }
//	  else if(StringSearch(param,len,(uint8_t *)"SZ+SMSCOM",9))	 //SZ+SMSCOM=ON/OFF
//	  {
//		param_item=Assist_smscom;
//	  }
	  else if(StringSearch(param,len,(uint8_t *)"SZ+BAUDRATE",11))
	  {
		param_item=Assist_baudrate;
	  }
	  else if(StringSearch(param,len,(uint8_t *)"SZ+CHECKBIT",11))
	  {
		param_item=Assist_checkbit;
	  }
	  else if(StringSearch(param,len,(uint8_t *)"SZ+DATABIT",10))
	  {
		param_item=Assist_databit;
	  }
	  else if(StringSearch(param,len,(uint8_t *)"SZ+BREAKBIT",11))
	  {
		param_item=Assist_breakbit;
	  }

	  else if(StringSearch(param,len,(uint8_t *)"SZ+GNOD",7))	//APN
	  {
		param_item=Assist_gnod;
	  }
	  else if(StringSearch(param,len,(uint8_t *)"SZ+REGENABLE",12))
	  {
		param_item=Assist_regenable;
	  }
	  else if(StringSearch(param,len,(uint8_t *)"SZ+REGDATA",10))
	  {
		param_item=Assist_regdata;
	  }
//	  else if(StringSearch(param,len,(uint8_t *)"SZ+HEARTENABLE",14))
//	  {
//		param_item=Assist_heartenable;
//	  }
	  else if(StringSearch(param,len,(uint8_t *)"SZ+HEARTTIME",12))
	  {
		param_item=Assist_heartime;
	  }
//	  else if(StringSearch(param,len,(uint8_t *)"SZ+HEARTDATA",12))
//	  {
//		param_item=Assist_heartdata;
//	  }
	  else if(StringSearch(param,len,(uint8_t *)"SZ+PCENTERADDR",14))
	  {
		param_item=Assist_pcenteraddr;
	  }
	  else if(StringSearch(param,len,(uint8_t *)"SZ+PCENTERPORT",14))
	  {
		param_item=Assist_pcenterport;
	  }

//	  else if(StringSearch(param,len,(uint8_t *)"SZ+THIRDADDR",12))
//	  {
//		param_item=Assist_thirdaddr;
//	  }
//	  else if(StringSearch(param,len,(uint8_t *)"SZ+THIRDPORT",12))
//	  {
//		param_item=Assist_thirdport;
//	  }
//	  else if(StringSearch(param,len,(uint8_t *)"SZ+FOURTHADDR",13))
//	  {
//		param_item=Assist_fourthaddr;
//	  }
//	  else if(StringSearch(param,len,(uint8_t *)"SZ+FOURTHPORT",13))
//	  {
//		param_item=Assist_fourthport;
//	  }
//	  else if(StringSearch(param,len,(uint8_t *)"SZ+FIFTHADDR",12))
//	  {
//		param_item=Assist_fifthaddr;
//	  }
//	  else if(StringSearch(param,len,(uint8_t *)"SZ+FIFTHPORT",12))
//	  {
//		param_item=Assist_fifthport;
//	  }
//	  else if(StringSearch(param,len,(uint8_t *)"SZ+VER",6))
//	  {
//		param_item=Assist_ver;
//	  }
//	  else if(StringSearch(param,len,(uint8_t *)"SZ+IMEI",7))
//	  {
//		param_item=Assist_imei;//imei = oa_get_imei();
//	  }
//	  else if(StringSearch(param,len,(uint8_t *)"SZ+RSSI",7))
//	  {
//		param_item=Assist_rssi;
//	  }
//	  else if(StringSearch(param,len,(uint8_t *)"SZ+SETIMEI",10))
//	  {
//		param_item=Assist_set_imei;
//	  }
//	  else if(StringSearch(param,len,(uint8_t *)"SZ+PDNSIP",9))
//	  {
//		param_item = Assist_pdnsip;
//	  }
//	  else if(StringSearch(param,len,(uint8_t *)"SZ+SDNSIP",9))
//	  {
//		param_item = Assist_sdnsip;
//	  }
//	  else if(StringSearch(param,len,(uint8_t *)"SZ+SCENTERADDR",14))// FOAT 用户名
//	  {
//		param_item = Assist_fotauser;   //Ql_UART_Write(UART_PORT3, param_buf, param_len);//uart_Senddata("SCENTERADDR--OK");
//	  }
//	  else if(StringSearch(param,len,(uint8_t *)"SZ+SCENTERPORT",14))  //FOTA密码
//	  {
//		param_item = Assist_fotapassword;  //uart_Senddata("SCENTERPORT--OK");
//	  }
	  else if(StringSearch(param,len,(uint8_t *)"SZ+REBOOT",9))
	  {


	  }
//	  else if(StringSearch(param,len,(uint8_t *)"SZ+GUSER",8))
//	  {
//		param_item = Assist_guser;
//	  }
//	  else if(StringSearch(param,len,(uint8_t *)"SZ+GPSWD",8))
//	  {
//		param_item = Assist_gpswd;
//	  }
//	  else if(StringSearch(param,len,(uint8_t *)"SZ+FACTORYTIME",14))
//	  {
//		  param_item=Assist_FactorybeatTime;
//	  }
//	  else if(StringSearch(param,len,(uint8_t *)"SZ+FACTORYPORT",14))
//	  {
//		  param_item=Assist_Factoryport;
//	  }


	  pStartPointer = 0;
	  pEndPointer = 0;

	  for(iter=0;iter<param_len;iter++)
	  {
		if((pStartPointer == 0) && (param_buf[iter] == '=')) pStartPointer = iter+1;
		if(param_buf[iter] == '\r')  pEndPointer = iter+1;
	  }
	  param_len = pEndPointer-pStartPointer-1;



	  if(param_len > 0) 							  //DEBUG 2
	  {
		if(pStartPointer > 3)
		{
		  for(iter=0;iter<param_len;iter++,pStartPointer++)
		  {
			param_buf[iter] = param_buf[pStartPointer];
		  }
		  param_buf[iter] = 0x00;				  //All input data add zero
		}
		else param_len = 0;
	  }
	  else param_len = 0;


	  switch(param_item)
	  {
		case UndefineItem:
			///uart_Senddata(RETURN_ERROR);
			break;

		case Assist_read:
			if(AT_INIT==UpdataSysConfigInfoPro.State)
             UpdataConfig_PERIOD = TRUE;
			break;
		case Assist_reset:
			SysConfigParaDefault();
			uart_Senddata((uint8_t *)"+RESET:OK\r\n");
			break;
		case Assist_save:
		    quit_config();
//		    SysConfigParaDefault();
			FlashWrite();
		 //SaveCurSetSysNvmInfo();
		   SYSTEM_RESET();


		//	SaveCurSetSysNvmInfo();   ///msw save not create
			//m_tcp_state = STATE_GPRS_AGAIN_CONNECT;
			CfgSave = TRUE;
			//UserUartInit();
			break;
		case Assist_quit:

			CfgSave = TRUE;

			quit_config();

			break;

		case Assist_debug:
			if(StringSearch(param_buf,param_len+1,(uint8_t *)"ON",2)){
			  DebugInfoEnable= TRUE;
			  uart_Senddata((uint8_t *)"\r\nOK\r\n");
			}
			else if(StringSearch(param_buf,param_len+1,(uint8_t *)"OFF",3)){
			  DebugInfoEnable = FALSE;
			  uart_Senddata((uint8_t *)"\r\nOK\r\n");
			}
			else if(param_buf[0] == '?')	//help
			{
			  uart_Senddata((uint8_t *)"+HELP:ON/OFF\r\n");			 //HELP INFO
			  return;
			}
			else{

			  uart_Senddata((uint8_t *)"+DEBUG:");
			  if(DebugInfoEnable == 1) uart_Senddata((uint8_t *)"ON\r\n");
			  else uart_Senddata((uint8_t *)"OFF\r\n");

			}
			break;
		case Assist_devshow:
//			if(StringSearch(param_buf,param_len+1,(uint8_t *)"ON",2)){
//			  DeviceIdentEnable = TRUE;
//			  uart_Senddata((uint8_t *)"\r\nOK\r\n");
//			}
//			else if(StringSearch(param_buf,param_len+1,(uint8_t *)"OFF",3)){
//			  DeviceIdentEnable = FALSE;
//			  uart_Senddata((uint8_t *)"\r\nOK\r\n");
//			}
//			else if(param_buf[0] == '?')	//help
//			{
//			  uart_Senddata((uint8_t *)"+HELP:ON/OFF\r\n");			 //HELP INFO
//			  return;
//			}
//			else{
//
//			  //uart_uart_Senddata(RETURN_ERROR);
//			  uart_Senddata((uint8_t *)"+DEVSHOW:");
//			  if(DeviceIdentEnable == 1) uart_Senddata((uint8_t *)"ON\r\n");
//			  else uart_Senddata((uint8_t *)"OFF\r\n");
//
//			}
			break;
		case Assist_device:
//			if(param_len==0){
//			  uart_Senddata((uint8_t *)"+DEVICE:");
//			  uart_Senddata((unsigned char *)dtuid);
//			  uart_Senddata((uint8_t *)"\r\n");
//			}
//			else if(param_len>16)
//			{
//			  uart_Senddata((uint8_t *)"\r\nERROR\r\n");
//			}
//			else
//			{
//			  ClearUserSetBufferTail((unsigned char *)dtuid);
//			  strcpy(( char *)dtuid,(char *)param_buf);
//			  uart_Senddata((uint8_t *)"\r\nOK\r\n");
//			}
			break;
		case Assist_smscom:
//			if(StringSearch(param_buf,param_len+1,(uint8_t *)"ON",2)){
//			  SMSComEnable = TRUE;
//			  uart_Senddata((uint8_t *)"\r\nOK\r\n");
//			}
//			else if(StringSearch(param_buf,param_len+1,"OFF",3)){
//			  SMSComEnable = FALSE;
//			  uart_Senddata((uint8_t *)"\r\nOK\r\n");
//			}
//			else if(param_buf[0] == '?')	//help
//			{
//			  uart_Senddata((uint8_t *)"+HELP:ON/OFF\r\n");			 //HELP INFO
//			  return;
//			}
//			else{
//			  uart_Senddata((uint8_t *)"+SMSCOM::");
//			  if(SMSComEnable == 1) uart_Senddata((uint8_t *)"ON\r\n");
//			  else uart_Senddata((uint8_t *)"OFF\r\n");
//			  //uart_uart_Senddata(RETURN_ERROR);
//			}
			break;
		case Assist_tel:	//send to user message

		    //SmsSendPro(NULL,dtuid,strlen(dtuid));
//			 uart_Senddata((uint8_t *)"\r\nOK\r\n");
			break;
		case Assist_duser:	//11 user tel number
//			if(param_len==0)
//			{
//			  uart_Senddata((uint8_t *)"+DUSER:");
//			  uart_Senddata((unsigned char *)UserName);
//			  uart_Senddata((uint8_t *)"\r\n");
//			}
//			else if(param_len>16){
//			   uart_Senddata((uint8_t *)"\r\nERROR\r\n");
//			}
//			else{
//			  ClearUserSetBufferTail((unsigned char *)UserName);
//			  param_buf[15]=0;
////			  uart_Senddata((uint8_t *)param_buf);
//			  strcpy(( char *)UserName, (char *)param_buf);
////			  uart_Senddata((uint8_t *)UserName);
//			  uart_Senddata((uint8_t *)"\r\nOK\r\n");
//			}
			break;
		case Assist_dpswd:
//			if(param_len==0){
//			  uart_Senddata((uint8_t *)"+DPSWD:");
//			  uart_Senddata((unsigned char *)PassWord);
//			  uart_Senddata((uint8_t *)"\r\n");
//			}
//			else if(param_len>6){
//			uart_Senddata((uint8_t *)"\r\nERROR\r\n");
//			}
//			else{
//			  ClearUserSetBufferTail((unsigned char *)PassWord);
//			  strcpy(( char *)PassWord,(char *)param_buf);
//			  uart_Senddata((uint8_t *)"\r\nOK\r\n");
//			}
			break;
		case Assist_baudrate:
			if(StringSearch(param_buf,param_len+1,(uint8_t *)"1200",4)) BaudRateType = Baudrate_1200;
				else if(StringSearch(param_buf,param_len+1,(uint8_t *)"2400",4)) BaudRateType = Baudrate_2400;
				else if(StringSearch(param_buf,param_len+1,(uint8_t *)"4800",4)) BaudRateType = Baudrate_4800;
				else if(StringSearch(param_buf,param_len+1,(uint8_t *)"9600",4)) BaudRateType = Baudrate_9600;
				else if(StringSearch(param_buf,param_len+1,(uint8_t *)"19200",5)) BaudRateType = Baudrate_19200;
				else if(StringSearch(param_buf,param_len+1,(uint8_t *)"38400",5)) BaudRateType = Baudrate_38400;
				else if(StringSearch(param_buf,param_len+1,(uint8_t *)"57600",5)) BaudRateType = Baudrate_57600;
				else if(StringSearch(param_buf,param_len+1,(uint8_t *)"115200",6)) BaudRateType = Baudrate_115200;
				else if(param_buf[0] == '?')	//help
				{
				  uart_Senddata((uint8_t *)"+HELP:1200,2400,4800,9600,19200,38400,57600\r\n"); 		   //HELP INFO
				  return;
				}
				else{

				  uart_Senddata((uint8_t *)"+BAUDRATE:");
				  if(BaudRateType == Baudrate_1200) uart_Senddata((uint8_t *)"1200");
				  else if(BaudRateType == Baudrate_2400) uart_Senddata((uint8_t *)"2400");
				  else if(BaudRateType == Baudrate_4800) uart_Senddata((uint8_t *)"4800");
				  else if(BaudRateType == Baudrate_9600) uart_Senddata((uint8_t *)"9600");
				  else if(BaudRateType == Baudrate_19200) uart_Senddata((uint8_t *)"19200");
				  else if(BaudRateType == Baudrate_38400) uart_Senddata((uint8_t *)"38400");
				  else if(BaudRateType == Baudrate_57600) uart_Senddata((uint8_t *)"57600");
				 else if(BaudRateType == Baudrate_115200) uart_Senddata((uint8_t *)"115200");
				  else uart_Senddata((uint8_t *)" ");
				  uart_Senddata((uint8_t *)"\r\n");

				  //uart_uart_Senddata(RETURN_ERROR);
				  return;
				}

			uart_Senddata(RETURN_OK);

			break;

		case Assist_checkbit:
			if(StringSearch(param_buf,param_len+1,(uint8_t *)NONE,4)) CheckBitType = oa_uart_pa_none;
			else if(StringSearch(param_buf,param_len+1,(uint8_t *)"ODD",3)) CheckBitType = oa_uart_pa_odd;
			else if(StringSearch(param_buf,param_len+1,(uint8_t *)"EVEN",4)) CheckBitType = oa_uart_pa_even;
			else if(param_buf[0] == '?')	//help
			{
			  uart_Senddata((uint8_t *)"+HELP:NONE,ODD,EVEN\r\n");			//HELP INFO
			  return;
			}
			else{

			  uart_Senddata((uint8_t *)"+CHECKBIT:");
			  if(CheckBitType == oa_uart_pa_none) uart_Senddata((uint8_t *)NONE);
			  else if(CheckBitType == oa_uart_pa_odd) uart_Senddata((uint8_t *)"ODD");
			  else if(CheckBitType == oa_uart_pa_even) uart_Senddata((uint8_t *)"EVEN");
			  else uart_Senddata((uint8_t *)" ");
			  uart_Senddata((uint8_t *)"\r\n");

			  //uart_uart_Senddata(RETURN_ERROR);
			  return;
			}
			uart_Senddata((uint8_t *)"\r\nOK\r\n");
			break;

		case Assist_databit:
			if(StringSearch(param_buf,param_len+1,(uint8_t *)"8bit",4)) DataBitType = oa_uart_len_8;
			else if(StringSearch(param_buf,param_len+1,(uint8_t *)"9bit",4)) DataBitType = oa_uart_len_7;
			else if(param_buf[0] == '?')	//help
			{
			  uart_Senddata((uint8_t *)"+HELP:8bit,9bit\r\n");			//HELP INFO
			  return;
			}
			else{

			  uart_Senddata((uint8_t *)"+DATABIT:");
			  if(DataBitType == oa_uart_len_8) uart_Senddata((uint8_t *)"8");
			  else if(DataBitType == oa_uart_len_7) uart_Senddata((uint8_t *)"9");
			  else uart_Senddata(" ");
			  uart_Senddata((uint8_t *)"\r\n");

			  //uart_uart_Senddata(RETURN_ERROR);
			  return;
			}
			uart_Senddata((uint8_t *)"\r\nOK\r\n");
			break;

		case Assist_breakbit:
			if(StringSearch(param_buf,param_len+1,(uint8_t *)"1bit",4)) StopBitType = oa_uart_sb_1;
			else if(StringSearch(param_buf,param_len+1,(uint8_t *)"1.5bit",6)) StopBitType = oa_uart_sb_1_5;
			else if(StringSearch(param_buf,param_len+1,(uint8_t *)"2bit",4)) StopBitType = oa_uart_sb_2;
			else if(param_buf[0] == '?')	//help
			{
			  uart_Senddata((uint8_t *)"+HELP:1bit,1.5bit,2bit\r\n"); 		   //HELP INFO
			  return;
			}
			else{
			  uart_Senddata((uint8_t *)"+BREAKBIT:");
			  if(StopBitType == oa_uart_sb_1) uart_Senddata((uint8_t *)"1");
			  else if(StopBitType == oa_uart_sb_1_5) uart_Senddata((uint8_t *)"1.5");
			  else if(StopBitType == oa_uart_sb_2) uart_Senddata((uint8_t *)"2");
			  else uart_Senddata((uint8_t *)" ");
			  uart_Senddata((uint8_t *)"\r\n");

			  //uart_uart_Senddata(RETURN_ERROR);
			  return;
			}
			uart_Senddata((uint8_t *)"\r\nOK\r\n");
			break;



		case Assist_gnod:
			if(param_len==0){
			  uart_Senddata((uint8_t *)"+GNOD:");
			  uart_Senddata((unsigned char *)apninfo);
			  uart_Senddata((uint8_t *)"\r\n");
			}
			else if(param_len>40){
				uart_Senddata((uint8_t *)"\r\nERROR\r\n");
			}
			else{
			  ClearUserSetBufferTail((unsigned char *)apninfo);
			  param_buf[SET_APNINFO_LEN]=0;
			  strcpy(( char *)apninfo,(char *)param_buf);
			  uart_Senddata((uint8_t *)"\r\nOK\r\n");
			}
			break;
		case Assist_regenable:
			if(StringSearch(param_buf,param_len+1,(uint8_t *)"ON",2)){
			  RegisterPacketEnable = TRUE;
			  uart_Senddata((uint8_t *)"\r\nOK\r\n");
			}
			else if(StringSearch(param_buf,param_len+1,(uint8_t *)"OFF",3)){
			  RegisterPacketEnable = FALSE;
			  uart_Senddata((uint8_t *)"\r\nOK\r\n");
			}
			else if(param_buf[0] == '?')	//help
			{
			  uart_Senddata((uint8_t *)"+HELP:ON/OFF\r\n");			 //HELP INFO
			  return;
			}
			else{
			  uart_Senddata((uint8_t *)"+REGENABLE::");
			  if(RegisterPacketEnable == 1) uart_Senddata((uint8_t *)"ON\r\n");
			  else uart_Senddata((uint8_t *)"OFF\r\n");

			  //uart_Senddata(RETURN_ERROR);
			}
			break;
		case Assist_regdata:
//			if(param_len==0){
//			  uart_Senddata((uint8_t *)"+REGDATA:");
//			  uart_Senddata((unsigned char *)RegisterPacketContent);
//			  uart_Senddata((uint8_t *)"\r\n");
//			}
//			else if(param_len>120){
//			  uart_Senddata(RETURN_ERROR);
//			}
//			else
//			{
//			  ClearUserSetBufferTail((unsigned char *)RegisterPacketContent);
//			  param_buf[60]=0;
//			  strcpy(( char *)RegisterPacketContent,(char *)param_buf);	//发送时从第二个开始取值
//			  uart_Senddata(RETURN_OK);
//			}
			break;
		case Assist_heartenable:
//			if(StringSearch(param_buf,param_len+1,(uint8_t *)"ON",2)){
//			  HeartbeatTimeEnable = TRUE;
//			  uart_Senddata(RETURN_OK);
//			}
//			else if(StringSearch(param_buf,param_len+1,(uint8_t *)"OFF",3)){
//			  HeartbeatTimeEnable = FALSE;
//			  uart_Senddata(RETURN_OK);
//			}
//			else if(param_buf[0] == '?')	//help
//			{
//			  uart_Senddata((uint8_t *)"+HELP:ON/OFF\r\n");			 //HELP INFO
//			  return;
//			}
//			else{
//			  uart_Senddata((uint8_t *)"+REGENABLE::");
//			  if(HeartbeatTimeEnable == 1) uart_Senddata((uint8_t *)"ON\r\n");
//			  else uart_Senddata((uint8_t *)"OFF\r\n");
//
//			  //uart_Senddata(RETURN_ERROR);
//			}
//			break;
			break;

		case Assist_heartime:
			if(param_len==0){
			  uart_Senddata((uint8_t *)"+HEARTTIME:");
			  uart_Senddata((unsigned char *)HeartbeatTime);
			  uart_Senddata((uint8_t *)"\r\n");
			}
			else if(param_len>5){
			  uart_Senddata(RETURN_ERROR);
			}
			else
			{
			  ClearUserSetBufferTail((unsigned char *)HeartbeatTime);
			  param_buf[6]=0;
			  strcpy(( char *)HeartbeatTime,(char *)param_buf);
			  uart_Senddata(RETURN_OK);
			}
			break;
		case Assist_heartdata:
//			if(param_len==0){
//			  uart_Senddata((uint8_t *)"+HEARTDATA:");
//			  uart_Senddata((unsigned char *)HeartbeatData);
//			  uart_Senddata((uint8_t *)"\r\n");
//			}
//			else if(param_len>120){
//			  uart_Senddata(RETURN_ERROR);
//			}
//			else
//			{
//			  ClearUserSetBufferTail((unsigned char *)HeartbeatData);
//			  param_buf[60]=0;
//			  strcpy(( char *)HeartbeatData,(char *)param_buf);
//			  uart_Senddata(RETURN_OK);
//			}
			break;

		case Assist_pcenteraddr:
			if(param_len==0){
			  uart_Senddata((uint8_t *)"+PCENTERADDR:");
			  uart_Senddata((unsigned char *)serve_host_name);
			  uart_Senddata((uint8_t *)"\r\n");
			}
			else if(param_len>50){
			  uart_Senddata(RETURN_ERROR);
			}
			else{
			  ClearUserSetBufferTail((unsigned char *)serve_host_name);
			  strcpy(( char *)serve_host_name,(char *)param_buf);
			  uart_Senddata(RETURN_OK);
			}
			break;

		case Assist_pcenterport:
			if(param_len==0){
			  uart_Senddata((uint8_t *)"+PCENTERPORT:");
			  uart_Senddata((unsigned char *)port);
			  uart_Senddata((uint8_t *)"\r\n");
			}
			else if(param_len>5){
			  uart_Senddata(RETURN_ERROR);
			}
			else{
			  ClearUserSetBufferTail((unsigned char *)port);
			  strcpy(( char *)port,(char *)param_buf);
			  uart_Senddata(RETURN_OK);
			}
			break;

		case Assist_FactorybeatTime:
//			if(param_len==0){
//			  uart_Senddata((uint8_t *)"+FactorybeatTime:");
//			  uart_Senddata((unsigned char *)FactorybeatTime);
//			  uart_Senddata((uint8_t *)"\r\n");
//			}
//			else if(param_len>5){
//			  uart_Senddata(RETURN_ERROR);
//			}
//			else{
//			  ClearUserSetBufferTail((unsigned char *)FactorybeatTime);
//			  strcpy(( char *)FactorybeatTime,(char *)param_buf);
//			  uart_Senddata(RETURN_OK);
//			}
			break;

		case Assist_Factoryport:
//			if(param_len==0){
//			  uart_Senddata((uint8_t *)"+Factoryport:");
//			  uart_Senddata((unsigned char *)Factoryport);
//			  uart_Senddata((uint8_t *)"\r\n");
//			}
//			else if(param_len>5){
//			  uart_Senddata(RETURN_ERROR);
//			}
//			else{
//			  ClearUserSetBufferTail((unsigned char *)Factoryport);
//			  strcpy(( char *)Factoryport,(char *)param_buf);
//			  uart_Senddata(RETURN_OK);
//			}
			break;

		case Assist_scenteraddr:

			break;
		case Assist_scenterport:

			break;
		case Assist_thirdaddr:

			break;
		case Assist_thirdport:

			break;
		case Assist_fourthaddr:

			break;
		case Assist_fourthport:

			break;
		case Assist_fifthaddr:

			break;
		case Assist_fifthport:

			break;
		case Assist_ver:
//			uart_Senddata((uint8_t *)"\r\n+VER:");		   //保存失败处理
//			uart_Senddata(DEVICE);
//			uart_Senddata((uint8_t *)",");
//			uart_Senddata(HW_VER);
//			uart_Senddata((uint8_t *)",");
//			uart_Senddata(SW_VER);
//			uart_Senddata((uint8_t *)",");
//			uart_Senddata(SHUNCOM);
//			uart_Senddata((uint8_t *)"\r\n");
			break;
		case Assist_imei:

			//im = GetIMEI(imei);
//			if(im)
//			{
//			  uart_Senddata((uint8_t *)"+IMEI:");
//			  uart_Senddata((uint8_t *)imei);
//			  uart_Senddata((uint8_t *)"\r\n");
//			}
//			else uart_Senddata((uint8_t *)"NONE\r\n");


			break;
		case Assist_rssi:
		  // GPRS drops down
          //RIL_NW_GetSignalQuality(&im, &iter);
//		  ClearUserSetBufferTail(send_buf);
//
//		  send_buf[0] ='+';
//		  send_buf[1] = 'R';send_buf[2] = 'S';
//		  send_buf[3] = 'S';send_buf[4] = 'I';
//		  send_buf[5] = ':';
//		  send_buf[6] = im/10 + '0' ;
//		  send_buf[7] = im%10 + '0' ;
//		  send_buf[8] ='\r';
//		  send_buf[9] ='\n';
//		  send_buf[10] = 0;
//		  uart_Senddata((uint8_t *)imei);

			break;

		case Assist_set_imei:
//			///oa_strcpy(send_buf,"AT+EGMR=1,7,");
//			send_buf[12] = '\"' ;
//			send_buf[13] = 0;
//
//			///oa_strcpy(send_buf+13,param_buf);
//			send_buf[28] = '\"';
//			send_buf[29] = '\r';
//			send_buf[30] = '\n';
//			///oa_uart_send_at_cmd_req(send_buf,30);
//			///uart_uart_Senddata(RETURN_OK);
			break;

		case Assist_pdnsip:
//			if(param_len==0){
//			  uart_Senddata((uint8_t *)"+SDNSIP:");
//			  //uart_uart_Senddata((unsigned char *)serve_ipaddr);
//			  uart_Senddata((unsigned char *)"0.0.0.0");
//			  uart_Senddata((uint8_t *)"\r\n");
//			}
//			else if(param_len>15){
//			  uart_Senddata(RETURN_ERROR);
//			}
//			else{
//			  //ClearUserSetBufferTail((unsigned char *)serve_ipaddr);
//			  //strcpy((unsigned char *)serve_ipaddr,param_buf);
//			  uart_Senddata(RETURN_OK);
//			}
			break;

		 case Assist_sdnsip:
//			if(param_len==0){
//			  uart_Senddata((uint8_t *)"+SDNSIP:");
//			  uart_Senddata((unsigned char *)serve_ipaddr); //fota address
//			  uart_Senddata((uint8_t *)"\r\n");
//			}
//			else if(param_len>15){
//			  uart_Senddata(RETURN_ERROR);
//			}
//			else{
//			  ClearUserSetBufferTail((unsigned char *)serve_ipaddr);
//			  strcpy(( char *)serve_ipaddr,(char *)param_buf);
//			  uart_Senddata(RETURN_OK);
//			}
			break;

        case Assist_fotauser:
//			if(param_len==0){
//		      uart_Senddata((uint8_t *)"+SCENTERADDR:");
//		      uart_Senddata((unsigned char *)fota_user_name);
//		      uart_Senddata((uint8_t *)"\r\n");
//		    }
//		    else if(param_len>50){
//		      uart_Senddata(RETURN_ERROR);
//		    }
//		    else{
//		      ClearUserSetBufferTail((unsigned char *)fota_user_name);
//		      (void)strcpy((char *)fota_user_name,(char *)param_buf);
//		      uart_Senddata(RETURN_OK);
//		    }
			break;

       case Assist_fotapassword:

//		 if(param_len==0){
//			   uart_Senddata((uint8_t *)"+SCENTERPORT:");
//			   uart_Senddata((unsigned char *)fota_password);
//			   uart_Senddata((uint8_t *)"\r\n");
//		 }
//		 else if(param_len>5){
//		   uart_Senddata(RETURN_ERROR);
//		 }
//		 else{
//		   ClearUserSetBufferTail((unsigned char *)fota_password);
//		   (void)strcpy(( char *)fota_password,(char *)param_buf);
//		   uart_Senddata(RETURN_OK);
//		 }

          break;

		 case Assist_guser:
//			if(param_len==0){
//			  uart_Senddata((uint8_t *)"+GUSER:");
//			  uart_Senddata((unsigned char *)apnname);
//			  uart_Senddata((uint8_t *)"\r\n");
//			}
//			else if(param_len>16){
//			  uart_Senddata(RETURN_ERROR);
//			}
//			else{
//			  ClearUserSetBufferTail((unsigned char *)apnname);
//			  strcpy(( char *)apnname,(char *)param_buf);
//			  uart_Senddata(RETURN_OK);
//			}
			break;
		 case Assist_gpswd:
//			if(param_len==0){
//			  uart_Senddata((uint8_t *)"+GPSWD:");
//			  uart_Senddata((unsigned char *)apnpassword);
//			  uart_Senddata((uint8_t *)"\r\n");
//			}
//			else if(param_len>16){
//			  uart_Senddata(RETURN_ERROR);
//			}
//			else{
//			  ClearUserSetBufferTail((unsigned char *)apnpassword);
//			  strcpy((char *)apnpassword,(char *)param_buf);
//			  uart_Senddata(RETURN_OK);
//			}
			break;
		default:
			break;

	  }



	}




void quit_config(void)
{
    uart_Senddata((uint8_t *)"\r\n+LOGIN OUT\r\n");
     uart_Senddata((uint8_t *)"\r\nExit set mode ok!\r\nPlease input your data to continue...\r\n");


     //1/print v C
     //2/enable quit timer
     //3/light led
     Cfg_mode = CFGNULL;
	// CurSysMode = NORMAL_MODE;
	 gUserLoginPassFlag = FALSE ;
	 UartInitFLAG = TRUE;

}

void StartUpQuitCfg(void)
{
	if(start_cfg<=5)
	{
	  start_cfg++ ;
	}
    else if((!gUserLoginPassFlag)&&(FALSE == CfgSave))
    {
		CfgSave = TRUE;
		if(DebugInfoEnable)
		  quit_config();

		Cfg_mode = CFGNULL;
		gUserLoginPassFlag = FALSE ;
    }

	//quit config mode timer
	if(CFGSTART==Cfg_mode)
	{
      //if(++quitconfig_count>=QUIT_CONFIG_TIME){
      //  quitconfig_count =0;
		CfgSave = TRUE;
		if(DebugInfoEnable)
		    quit_config();

		Cfg_mode = CFGNULL;
		gUserLoginPassFlag = FALSE ;
	 // }
	}
}




void UpdataSysConfigParaPro(void)
   {

		// uint8_t bActFlag = 0;

		 if(UpdataSysConfigInfoPro.State == AT_INIT)
		 {
		   //READ RAM
		   uart_Senddata((uint8_t *)"\r\n+READ:RAM\r\n");

		   UpdataSysConfigInfoPro.Step	= STEP1;
		 }
		 else if(UpdataSysConfigInfoPro.State == AT_SEND_T)
		 {
		   UpdataSysConfigInfoPro.Step++;
		   UpdataSysConfigInfoPro.State = AT_SEND_N;
		 }
		 else if(UpdataSysConfigInfoPro.State == AT_PASS)
		 {
			 UpdataSysConfigInfoPro.Step = STEP_OVER;
		 }
		 else
		 {

		 }

		 switch(UpdataSysConfigInfoPro.Step)
		 {

		   case STEP1:

		   uart_Senddata((uint8_t *)"+DEBUG:");
		   if(DebugInfoEnable == 1) uart_Senddata((uint8_t *)"ON");
		   else uart_Senddata((uint8_t *)"OFF");
		   uart_Senddata((uint8_t *)"\r\n");

		   UpdataSysConfigInfoPro.State = AT_SEND_T;

		   break;

		   //++
		   case STEP2:
	   /*
			 uart_Senddata("+DEVICE:");
			 uart_Senddata((unsigned char *)DeviceHead);
			 uart_Senddata("\r\n");
	   */
			 UpdataSysConfigInfoPro.State = AT_SEND_T;

		   break;

		   case STEP3:
	   //	 if(!SearchSysMsg(Assist_updata_msg))
		   {
			 uart_Senddata((uint8_t *)"+DUSER:");
//			 uart_Senddata(UserName);
			 uart_Senddata((uint8_t *)"\r\n");

	   //	   (void)SendSysMsg(Assist_updata_msg,10);		  //NOTE:10ms单位
			 UpdataSysConfigInfoPro.State = AT_SEND_T;
		   }
		   break;

		   case STEP4:
	   //	 if(!SearchSysMsg(Assist_updata_msg))
		   {
			 uart_Senddata((uint8_t *)"+DPSWD:");
//			 uart_Senddata(PassWord);
			 uart_Senddata((uint8_t *)"\r\n");

	   //	   (void)SendSysMsg(Assist_updata_msg,10);
			 UpdataSysConfigInfoPro.State = AT_SEND_T;
		   }

		   break;

		   case STEP5:
	   //	 if(!SearchSysMsg(Assist_updata_msg))
		   {
			 uart_Senddata((uint8_t *)"+BAUDRATE:");
			 if(BaudRateType == Baudrate_1200) uart_Senddata((uint8_t *)"1200");
			 else if(BaudRateType == Baudrate_2400) uart_Senddata((uint8_t *)"2400");
			 else if(BaudRateType == Baudrate_4800) uart_Senddata((uint8_t *)"4800");
			 else if(BaudRateType == Baudrate_9600) uart_Senddata((uint8_t *)"9600");
			 else if(BaudRateType == Baudrate_19200) uart_Senddata((uint8_t *)"19200");
			 else if(BaudRateType == Baudrate_38400) uart_Senddata((uint8_t *)"38400");
			 else if(BaudRateType == Baudrate_57600) uart_Senddata((uint8_t *)"57600");
			 else if(BaudRateType == Baudrate_115200) uart_Senddata((uint8_t *)"115200");
			 else uart_Senddata((uint8_t *)" ");
			 uart_Senddata((uint8_t *)"\r\n");

	   //	   (void)SendSysMsg(Assist_updata_msg,10);
			 UpdataSysConfigInfoPro.State = AT_SEND_T;
		   }

		   break;

		   case STEP6:
	   //	 if(!SearchSysMsg(Assist_updata_msg))
		   {
			 uart_Senddata((uint8_t *)"+CHECKBIT:");
			 if(CheckBitType == oa_uart_pa_none) uart_Senddata((uint8_t *)NONE);
			 else if(CheckBitType == oa_uart_pa_even) uart_Senddata((uint8_t *)"EVEN");
			 else if(CheckBitType == oa_uart_pa_odd) uart_Senddata((uint8_t *)"ODD");
			 else uart_Senddata((uint8_t *)" ");
			 uart_Senddata((uint8_t *)"\r\n");

	   //	   (void)SendSysMsg(Assist_updata_msg,10);
			 UpdataSysConfigInfoPro.State = AT_SEND_T;
		   }

		   break;

		   case STEP7:
	   //	 if(!SearchSysMsg(Assist_updata_msg))
		   {
			 uart_Senddata((uint8_t *)"+DATABIT:");
			 if(DataBitType == oa_uart_len_8) uart_Senddata((uint8_t *)"8");
			 else if(DataBitType == oa_uart_len_7) uart_Senddata((uint8_t *)"9");
			 else uart_Senddata((uint8_t *)" ");
			 uart_Senddata((uint8_t *)"\r\n");

	   //	   (void)SendSysMsg(Assist_updata_msg,10);
			 UpdataSysConfigInfoPro.State = AT_SEND_T;
		   }

		   break;

		   case STEP8:
	   //	 if(!SearchSysMsg(Assist_updata_msg))
		   {
			 uart_Senddata((uint8_t *)"+BREAKBIT:");
			 if(StopBitType == oa_uart_sb_1) uart_Senddata((uint8_t *)"1");
			 else if(StopBitType == oa_uart_sb_1_5) uart_Senddata((uint8_t *)"1.5");
			 else if(StopBitType == oa_uart_sb_2) uart_Senddata((uint8_t *)"2");
			 else uart_Senddata((uint8_t *)" ");
			 uart_Senddata((uint8_t *)"\r\n");

	   //	   (void)SendSysMsg(Assist_updata_msg,10);
			 UpdataSysConfigInfoPro.State = AT_SEND_T;
		   }

		   break;

		   case STEP9:
	   //	 if(!SearchSysMsg(Assist_updata_msg))
		   {
		   /**	uart_Senddata("+ITVTIME:");
			 uart_Senddata((unsigned char *)"10");
			 uart_Senddata("\r\n");
	   **/
	   //	   (void)SendSysMsg(Assist_updata_msg,10);
			 UpdataSysConfigInfoPro.State = AT_SEND_T;
		   }

		   break;

		   case STEP10:
	   //	 if(!SearchSysMsg(Assist_updata_msg))
		   {
			/** uart_Senddata("+MAXPACKET:");
			 uart_Senddata((unsigned char const*)"250");
			 uart_Senddata("\r\n");
			  **/
	   //	   (void)SendSysMsg(Assist_updata_msg,10);
			 UpdataSysConfigInfoPro.State = AT_SEND_T;
		   }

		   break;


		   case STEP11:
	   //	 if(!SearchSysMsg(Assist_updata_msg))
		   {
			 uart_Senddata((uint8_t *)"+GNOD:");
			 uart_Senddata(apninfo);
			// uart_Senddata("\r\n");
              uart_Senddata((uint8_t *)"\r\n");

	   //	   (void)SendSysMsg(Assist_updata_msg,10);
			 UpdataSysConfigInfoPro.State = AT_SEND_T;
		   }

		   break;


		   case STEP12:
	   //	 if(!SearchSysMsg(Assist_updata_msg))
		   {
			 uart_Senddata((uint8_t *)"+GUSER:");
//			 uart_Senddata(apnname);
			 uart_Senddata((uint8_t *)"\r\n");


	   //	   (void)SendSysMsg(Assist_updata_msg,10);
			 UpdataSysConfigInfoPro.State = AT_SEND_T;
		   }

		   break;

		   case STEP13:
	   //	 if(!SearchSysMsg(Assist_updata_msg))
		   {
			 uart_Senddata((uint8_t *)"+GPSWD:");
//			 uart_Senddata(apnpassword);
			 uart_Senddata((uint8_t *)"\r\n");

	   //	   (void)SendSysMsg(Assist_updata_msg,10);
			 UpdataSysConfigInfoPro.State = AT_SEND_T;
		   }

		   break;


		   case STEP14:
	   //	 if(!SearchSysMsg(Assist_updata_msg))
		   {
	   /*
			 uart_Senddata("+CENTERTYPE:");
			 if(DataCenterType == 1) uart_Senddata("DOM");
			 else if(DataCenterType == 2) uart_Senddata("IP");
			 else uart_Senddata(" ");
			 uart_Senddata("\r\n");
	   */
	   //	   (void)SendSysMsg(Assist_updata_msg,10);
			 UpdataSysConfigInfoPro.State = AT_SEND_T;
		   }

		   break;


		   case STEP15:
	   //	 if(!SearchSysMsg(Assist_updata_msg))
		   {
			 uart_Senddata((uint8_t *)"+PDNSIP:");
			 uart_Senddata((uint8_t *)"0.0.0.0");
			 uart_Senddata((uint8_t *)"\r\n");

	   //	   (void)SendSysMsg(Assist_updata_msg,10);
			 UpdataSysConfigInfoPro.State = AT_SEND_T;
		   }

		   break;


		   case STEP16:
	   //	 if(!SearchSysMsg(Assist_updata_msg))
		   {
			 uart_Senddata((uint8_t *)"+SDNSIP:");
//			 uart_Senddata(serve_ipaddr);
			 uart_Senddata((uint8_t *)"\r\n");

	   //	   (void)SendSysMsg(Assist_updata_msg,10);
			 UpdataSysConfigInfoPro.State = AT_SEND_T;
		   }

		   break;

		   case STEP17:
	   //	 if(!SearchSysMsg(Assist_updata_msg))
		   {
			 uart_Senddata((uint8_t *)"+PCENTERADDR:");
			 uart_Senddata(serve_host_name);
			 uart_Senddata((uint8_t *)"\r\n");	//uart_Senddata("\r\n");

	   //	   (void)SendSysMsg(Assist_updata_msg,10);
			 UpdataSysConfigInfoPro.State = AT_SEND_T;
		   }

		   break;


		   case STEP18:
	   //	 if(!SearchSysMsg(Assist_updata_msg))
		   {
	   /*
			 uart_Senddata("+PCENTERIP:");
			 uart_Senddata((unsigned char *)DataCenterIpAddrs);
			 uart_Senddata("\r\n");
	   */
	   //	   (void)SendSysMsg(Assist_updata_msg,10);
			 UpdataSysConfigInfoPro.State = AT_SEND_T;
		   }

		   break;


		   case STEP19:
	   //	 if(!SearchSysMsg(Assist_updata_msg))
		   {
			 uart_Senddata((uint8_t *)"+PCENTERPORT:");
			 uart_Senddata(port);
			 uart_Senddata((uint8_t *)"\r\n");

	   //	   (void)SendSysMsg(Assist_updata_msg,10);
			 UpdataSysConfigInfoPro.State = AT_SEND_T;
		   }

		   break;


		   case STEP20:
	   //	 if(!SearchSysMsg(Assist_updata_msg))
		   {
			 uart_Senddata((uint8_t *)"+PCENTERPROTOCOL:");
			 uart_Senddata((uint8_t *)"UDP");

			 uart_Senddata((uint8_t *)"\r\n");

	   //	   (void)SendSysMsg(Assist_updata_msg,10);
			 UpdataSysConfigInfoPro.State = AT_SEND_T;
		   }
		   break;



		   case STEP21:
	   //	 if(!SearchSysMsg(Assist_updata_msg))
		   {
			 uart_Senddata((uint8_t *)"+SCENTERADDR:");
//			 uart_Senddata(fota_user_name);
			 uart_Senddata((uint8_t *)"\r\n");

	   //	   (void)SendSysMsg(Assist_updata_msg,10);
			 UpdataSysConfigInfoPro.State = AT_SEND_T;
		   }

		   break;


		   case STEP22:
	   //	 if(!SearchSysMsg(Assist_updata_msg))
		   {
	   /*
			 uart_Senddata("+SCENTERIP:");
			 uart_Senddata((unsigned char *)SecDataCenterIpAddrs);
			 uart_Senddata("\r\n");
	   */
	   //	   (void)SendSysMsg(Assist_updata_msg,10);
			 UpdataSysConfigInfoPro.State = AT_SEND_T;
		   }

		   break;


		   case STEP23:
	   //	 if(!SearchSysMsg(Assist_updata_msg))
		   {
			 uart_Senddata((uint8_t *)"+SCENTERPORT:");
//			 uart_Senddata(fota_password);
			 uart_Senddata((uint8_t *)"\r\n");

	   //	   (void)SendSysMsg(Assist_updata_msg,10);
			 UpdataSysConfigInfoPro.State = AT_SEND_T;
		   }

		   break;


		   case STEP24:
	   //	 if(!SearchSysMsg(Assist_updata_msg))
		   {
			/** uart_Senddata("+SCENTERPROTOCOL:");
			 uart_Senddata("TCP");
			 uart_Senddata("\r\n");
			  **/
	   //	   (void)SendSysMsg(Assist_updata_msg,10);
			 UpdataSysConfigInfoPro.State = AT_SEND_T;
		   }
		   break;
	   //

		   case STEP25:
	   //	 if(!SearchSysMsg(Assist_updata_msg))
		   {
			/** uart_Senddata("+WORKTYPE:");


			 uart_Senddata("ONLINE");

			 uart_Senddata("\r\n");
			  **/
	   //	   (void)SendSysMsg(Assist_updata_msg,10);
			 UpdataSysConfigInfoPro.State = AT_SEND_T;
		   }

		   break;

		   case STEP26:
	   //	 if(!SearchSysMsg(Assist_updata_msg))
		   {
			 uart_Senddata((uint8_t *)"+REGENABLE:");
			 if(RegisterPacketEnable) uart_Senddata((uint8_t *)"ON");
			 else uart_Senddata((uint8_t *)"OFF");
			 uart_Senddata((uint8_t *)"\r\n");

	   //	   (void)SendSysMsg(Assist_updata_msg,10);
			 UpdataSysConfigInfoPro.State = AT_SEND_T;
		   }

		   break;

		 //AT+REGDATA=A,TEST123 "AT+REGDATA=H,01 EE FA FF"	原样打印
		   case STEP27:
	   //	 if(!SearchSysMsg(Assist_updata_msg))
		   {
			 uart_Senddata((uint8_t *)"+REGDATA:");
//			 uart_Senddata(RegisterPacketContent);
			 uart_Senddata((uint8_t *)"\r\n");

	   //	   (void)SendSysMsg(Assist_updata_msg,10);
			 UpdataSysConfigInfoPro.State = AT_SEND_T;
		   }

		   break;

		   case STEP28:
	   //	 if(!SearchSysMsg(Assist_updata_msg))
		   {
			 uart_Senddata((uint8_t *)"+HEARTTIME:");
			 uart_Senddata(HeartbeatTime);
			 uart_Senddata((uint8_t *)"\r\n");

	   //	   (void)SendSysMsg(Assist_updata_msg,10);
			 UpdataSysConfigInfoPro.State = AT_SEND_T;
		   }

		   break;

		   case STEP29:
	   //	 if(!SearchSysMsg(Assist_updata_msg))
		   {
			 uart_Senddata((uint8_t *)"+HEARTDATA:");
//			 uart_Senddata(HeartbeatData);
			 uart_Senddata((uint8_t *)"\r\n");

	   //	   (void)SendSysMsg(Assist_updata_msg,10);
			 UpdataSysConfigInfoPro.State = AT_SEND_T;
		   }

		   break;


		   case STEP30:
	   //	 if(!SearchSysMsg(Assist_updata_msg))
		   {
			/** uart_Senddata("+RECNTIME:");
			 uart_Senddata((unsigned char const*)"3");
			 uart_Senddata("\r\n");
			  **/
	   //	   (void)SendSysMsg(Assist_updata_msg,10);
			 UpdataSysConfigInfoPro.State = AT_SEND_T;
		   }

		   break;


		   case STEP31:
	   //	 if(!SearchSysMsg(Assist_updata_msg))
		   {
			/** uart_Senddata("+RECNTMAX:");
			 uart_Senddata((unsigned char const*)"4");
			 uart_Senddata("\r\n");
			  **/
	   //	   (void)SendSysMsg(Assist_updata_msg,10);
			 UpdataSysConfigInfoPro.State = AT_SEND_T;
		   }

		   break;

		   case STEP32:
	   //	 if(!SearchSysMsg(Assist_updata_msg))
		   {
			/** uart_Senddata("+IDLETIME:");
			 uart_Senddata((unsigned char const*)"180");
			 uart_Senddata("\r\n");
			  **/
	   //	   (void)SendSysMsg(Assist_updata_msg,10);
			 UpdataSysConfigInfoPro.State = AT_SEND_T;
		   }

		   break;


		   case STEP33:
	   //	 if(!SearchSysMsg(Assist_updata_msg))
		   {
			/** uart_Senddata("+SHUTENABLE:");
			  uart_Senddata("ON");

			 uart_Senddata("\r\n");
			  **/
	   //	   (void)SendSysMsg(Assist_updata_msg,10);
			 UpdataSysConfigInfoPro.State = AT_SEND_T;
		   }

		   break;


		   case STEP34:
	   //	 if(!SearchSysMsg(Assist_updata_msg))
		   {
			/** uart_Senddata("+SHUTDATA:");
			 uart_Senddata((unsigned char const*)"+SHUNCOM SHUTDOWN TEST");
			 uart_Senddata("\r\n");
			  **/
			 UpdataSysConfigInfoPro.State = AT_SEND_T;
		   }

		   break;


	   //+扩展功能

		   //++
		   case STEP35:

		   uart_Senddata((uint8_t *)"+DEVSHOW:");
		   if(DeviceIdentEnable == 1) uart_Senddata((uint8_t *)"ON");
		   else uart_Senddata((uint8_t *)"OFF");
		   uart_Senddata((uint8_t *)"\r\n");
		   UpdataSysConfigInfoPro.State = AT_SEND_T;
		   break;

		   case STEP36:

			 uart_Senddata((uint8_t *)"+DEVICE:");
			 uart_Senddata(dtuid);
			 uart_Senddata((uint8_t *)"\r\n");
			 UpdataSysConfigInfoPro.State = AT_SEND_T;

		   break;

		   case STEP37:

		   uart_Senddata((uint8_t *)"+SMSCOM:");
		   if(SMSComEnable == 1) uart_Senddata((uint8_t *)"ON");
		   else uart_Senddata((uint8_t *)"OFF");
		   uart_Senddata((uint8_t *)"\r\n");

		   UpdataSysConfigInfoPro.State = AT_SEND_T;
		   break;


		   case STEP38:

		  /*uart_Senddata("\r\n+VER:");
		   uart_Senddata(SW_VER);
		   uart_Senddata("\r\n");*/

		   UpdataSysConfigInfoPro.State = AT_PASS;
		   break;

		   //++

		   case STEP_OVER:

		   UpdataSysConfigInfoPro.Retry = 3;
		   UpdataSysConfigInfoPro.Step	= STEP0;
		   UpdataSysConfigInfoPro.State = AT_INIT;
		   UpdataConfig_PERIOD = FALSE ;
		   //(void)RemoveTaskFromRunTable(UpdataSysConfigParaPro);
		   uart_Senddata((uint8_t *)"+READ:OK\r\n");

                   //PIT_StopTimer(PIT, kPIT_Chnl_0);
		   ///bActFlag = Ql_Timer_Stop(UpdataConfig_ID);
		   ///APP_DEBUG("<--Stop ID Return=%d-->\r\n",bActFlag);

		   break;


		   default:
		   UpdataSysConfigInfoPro.State = AT_ERROR;
		  // PIT_StopTimer(PIT, kPIT_Chnl_0);
		   break;
		 }

	 }


void UartRecvConfig(uint8_t *recvBuffer,uint8_t data_len)
{
	if(StringSearch(recvBuffer, data_len, (uint8_t *)"+++SHUNCOM\r\n",12))    //is ESCAPE CHARACTER
	{
		Cfg_mode = CFGSTART;
		WelcomeConfigDisp();
	}
	else if(Cfg_mode == CFGSTART)
	{
		if(TRUE ==  gUserLoginPassFlag)
		{
//		   uart_Senddata((uint8_t *)SW_VER);
		   oa_app_config_entry( recvBuffer, data_len );
		}

		if((StringSearch(recvBuffer, data_len, (uint8_t *)"SZ+LOGIN=shuncom",16))         //Login
			 ||(StringSearch(recvBuffer, data_len, (uint8_t *)"SZ+LOGIN=SHUNCOM",16)) )
		 {
			TimerStop( &app_waitingconfig );
			gUserLoginPassFlag = TRUE;
			uart_Senddata( (uint8_t *)"+LOGIN:OK\r\n" );
//			DelayMs(40);
			if( AT_INIT == UpdataSysConfigInfoPro.State )
			{
			  UpdataConfig_PERIOD = TRUE;
			  osal_set_event( appConcentrator_TaskID, TRANSPARENT_CONFIG_EVT );
			}

				 //memset(recvBuffer,0 ,255);
				 //data_len = 0 ;

		 }
	 }
//	 else
//	 {
//		 osal_set_event( appConcentrator_TaskID, TRANSPARENT_UART2_RX_EVT );
//	 }


}



#endif
