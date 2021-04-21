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

#ifndef __SZCONFIG_H__
#define __SZCONFIG_H__

#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "fsl_uart_driver.h"
#include "ZGlobals.h"



#define  CUR_PAGE    0x56
#define  AT_SEND_T    0x11
#define  AT_SEND_F    0x12
#define  AT_SEND_N    0x22
#define  AT_RESP_T    0x13
#define  AT_RESP_F    0x14
#define  AT_RESP_N    0x24
#define  AT_INIT      0x00
#define  AT_PASS      0x15
#define  AT_FAIL      0x16
#define  AT_ERROR     0x26

#define LOGIN_START   "+LOGIN:OK\r\n"
#define NONE          "NONE"

#define CLIENT_BUFF_SIZE  255
#define CLIENT_YIELD_TIMEOUT  30

typedef struct ProcessState_tag
{
  uint8_t Step;              //当前步骤
  uint8_t State;             //当前状态  指示指令收发等各种状态
  uint8_t Retry;             //当前状态  失效允许重新执行次数
} ProcessState_t;

typedef enum {
    oa_uart_len_5=5,
    oa_uart_len_6,
    oa_uart_len_7,
    oa_uart_len_8
} oa_uart_bits_per_char;

typedef enum {
    oa_uart_sb_1=1,
    oa_uart_sb_2,
    oa_uart_sb_1_5
} oa_uart_stop_bits;

typedef enum {
    oa_uart_pa_none=0,
    oa_uart_pa_odd,
    oa_uart_pa_even,
    oa_uart_pa_space
} oa_uart_parity;

typedef enum {
    oa_uart_fc_none=1,
    oa_uart_fc_hw,
    oa_uart_fc_sw
} oa_uart_flow_ctrl_mode;


enum                     //波特率设置选项
{
  Baudrate_ERROR,       //<1200 ERROR!
  Baudrate_1200,        //从1开始
  Baudrate_2400,
  Baudrate_4800,
  Baudrate_9600,
  Baudrate_19200,
  Baudrate_38400,
  Baudrate_57600,
  Baudrate_115200,
};


//typedef struct
//{
//    uint8_t CurPageIdent;
//	uint8_t dtuid[26+1];//ID
//
//    uint8_t serve_ipaddr[SET_IPADDR_LEN+1];//service ip address    //P S dns --->fota ip address
//    uint8_t serve_host_name[SET_HOST_NAME_LEN+1];//service host name 1
//    uint8_t port[6];//service port
//
//    uint8_t fota_user_name[SET_HOST_NAME_LEN+1];     // --->fota user name
//	uint8_t fota_password[6];                        //--->fota password
//
//
//    uint8_t apninfo[SET_APNINFO_LEN+1];//
//    uint8_t apnname[SET_APNNAME_LEN+1];
//    uint8_t apnpassword[SET_APNPASSWORD_LEN+1];
//    bool rssi_disp; //TRUE = TCP, FALSE = UDP
//
//    //oa_uint8 checktail[2];
//
//   /* oa_char serve_host_name2[SET_HOST_NAME_LEN+1];//service host name 2
//    oa_char port2[6];//service port*/
//
//	bool DebugInfoEnable;            //调试信息输出使能
//	bool DeviceIdentEnable;           //设备标识使能
//
//    ///+1
//    bool SMSComEnable;               //短信收发功能使能
//
//    uint8_t UserName[SET_APNNAME_LEN];           //短信操作用户电话号码11位
//    uint8_t PassWord[SET_APNNAME_LEN];    /**/     //用户密码6位
//	/* 串口信息 选择用 显示需转换 */
//  /* Support this:1200,2400,4800,9600,19200,38400,57600,115200 */
//
//
//    uint8_t BaudRateType;            //用户端串口比特率
//    uint8_t DataBitType;             //数据位 8/9
//    uint8_t CheckBitType;            //校验位
//    uint8_t StopBitType;             //停止位 本机为空
//    bool RegisterPacketEnable;        //注册包使能：开启时连接成功首次发送其内容
//    uint8_t RegisterPacketContent[81];   //注册包内容：可以为设备标识信息
//    bool HeartbeatTimeEnable;    //心跳包使能
//	uint8_t HeartbeatTime[6];        //心跳时间  单位s
//    uint8_t HeartbeatData[81];       //心跳数据包内容
//
////	uint8_t FactorybeatTime[6];        //心跳时间  单位s
////	uint8_t Factoryport[6];//service port
//
//	uint8_t serve_host_name_sz[SET_HOST_NAME_LEN+1];//service host name sz
//    uint8_t port_sz[6];//service port
//
//} oa_soc_set_parameter;

enum
{
  STEP0,
  STEP1,
  STEP2,
  STEP3,
  STEP4,
  STEP5,
  STEP6,
  STEP7,
  STEP8,
  STEP9,
  STEP10,
  STEP11,
  STEP12,
  STEP13,
  STEP14,
  STEP15,
  STEP16,
  STEP17,
  STEP18,
  STEP19,
  STEP20,
  STEP21,
  STEP22,
  STEP23,
  STEP24,
  STEP25,
  STEP26,
  STEP27,
  STEP28,
  STEP29,
  STEP30,
  STEP31,
  STEP32,
  STEP33,
  STEP34,
  STEP35,
  STEP36,
  STEP37,
  STEP38,
  STEP39,
  STEP40,
  STEP_OVER,
};


enum
{
    UART_CONFIG_WAIT = 0,
    UART_CONFIG_START,
    UART_CONFIG_RECEIVED,
    UART_CONFIG_READ
};

enum
{
  CFGNULL=0,   //not config
  CFGPREP,   //config prepare
  CFGSTART,  // configuration in
};



enum
{
  UndefineItem,
  Assist_read,
  Assist_reset,
  Assist_quit,
  Assist_save,
  Assist_ipadd,
  Assist_debug,
  Assist_devshow,
  Assist_device,
  Assist_smscom,
  Assist_tel,
  Assist_duser,
  Assist_dpswd,
  Assist_baudrate,
  Assist_checkbit,
  Assist_databit,
  Assist_breakbit,
  Assist_gnod,
  Assist_regenable,
  Assist_regdata,
  Assist_heartenable,
  Assist_heartime,
  Assist_heartdata,
  Assist_pcenteraddr,
  Assist_pcenterport,
  Assist_scenteraddr,
  Assist_scenterport,
  Assist_thirdaddr,
  Assist_thirdport,
  Assist_fourthaddr,
  Assist_fourthport,
  Assist_fifthaddr,
  Assist_fifthport,
  Assist_ver,
  Assist_imei,
  Assist_rssi,
  Assist_set_imei,
  Assist_pdnsip,
  Assist_sdnsip,
  Assist_fotauser,
  Assist_fotapassword,
  Assist_guser,
  Assist_gpswd,
  Assist_FactorybeatTime,
  Assist_Factoryport,
};


//extern oa_soc_set_parameter g_soc_param[1];
//extern oa_soc_set_parameter soc_param;
extern uint8_t CfgOutFlag;
extern uint8_t SW_VER[];
extern uint8_t Cfg_mode;

void Cfg_hisr_auto(void);
void quit_config(void);
void StartUpQuitCfg(void);
void FlashWrite(void);
void UserUartInit(void);

extern void Search_Flag_info(void);
void UpdataSysConfigParaPro(void);
void ClearUserSetBufferTail(uint8_t *pUserSetBuf);

extern void UartRecvConfig(uint8_t *recvBuffer,uint8_t data_len);


#endif
