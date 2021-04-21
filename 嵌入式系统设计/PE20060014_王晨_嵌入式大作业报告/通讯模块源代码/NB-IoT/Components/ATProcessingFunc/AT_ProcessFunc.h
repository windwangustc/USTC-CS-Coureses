/*
 * ATCMDJudgement.h
 *
 *  Created on: 2019Äê4ÔÂ16ÈÕ
 *      Author: 8033
 */

#ifndef _ATCMDJUDGEMENT_H_
#define _ATCMDJUDGEMENT_H_

/*********************************************************************
 * 							INCLUDES
 */
#include "stdio.h"
#include "string.h"
#include "stdbool.h"
#include "stdint.h"
#include "uart.h"
#include "timer.h"
#include "delay.h"
#include "gpio.h"

/*********************************************************************
 * 							 MACROS
 */
#define DEF_DELAY_TIME				2000
#define CREAT_DELAY_TIME  			10000
#define OPEN_DELAY_TIME 			20000
#define NEXT_CMD_TIME               800
#define NEXT_CSCON_CHECK            3000
#define INIT_BC28_TIME              5000
#define DEF_CSCON_TIME              5000
#define DEF_CCLK_TIME    			43200
#define DEF_QUARY_TIME	            600000//3600000
/*********************************************************************
 * 							TYPEDEFS
 */
typedef enum eDeviceState
{
    DEVICE_STATE_CHECK = 0,
	DEVICE_STATE_INIT,
	DEVICE_STATE_CGMR,
    DEVICE_STATE_IMEI,
    DEVICE_STATE_IMSI,
    DEVICE_STATE_CHECK_CSQ,
	DEVICE_STATE_PSM,
	DEVICE_STATE_EDRX,
	DEVICE_STATE_IOT,
	DEVICE_STATE_AUTO,
	DEVICE_STATE_APN,
	DEVICE_SET_IP,
	DEVICE_CLOSE_CFUN,
	DEVICE_STATE_REBOOT,
	DEVICE_OPEN_CFUN,
	DEVICE_CLOSE_ECHO,
	DEVICE_OPEN_ERROR_MSG,
	DEVICE_STATE_CSCON,
	DEVICE_CLOSE_CSCON,
	DEVICE_STATE_CHECK_CSCON,
	DEVICE_CHECK_ADDR,
	DEVICE_STATE_SEND,

	DEVICE_WAIT_OPEN,
	DEVICE_OPEN_RECVMSG,
	DEVICE_STATE_NUESTATS,
	DEVICE_STATE_CCLK,

	DEVICE_STATE_CPSMS,
	DEVICE_STATE_IDLE,
	DEVICE_STATE_OVER
}DeviceState;


/*********************************************************************
 * 							FUNCTIONS
 */
uint8_t SendATCmd(uint8_t *data, uint8_t len );
bool ATCommandJudgeFunc( uint8_t *recvbuff, uint8_t *cmd );
void FOTACommandJudgeFunc( uint8_t *recvbuff );
void AT_CMD_LoopMainEvent(void);

void AT_CMD_LoopAnalyseRecvMsg(uint8_t *data, uint8_t datalen);

uint8_t TakeMSG_fromAT( uint8_t *data, uint16_t *len );
uint8_t Take_Nuestats_fromAT( uint8_t *data, uint16_t *datalen );
void Take_CCLK_fromAT( uint8_t *data );
void CCLKData_with_CRC( uint8_t *data, uint16_t datalen );
void Take_IMEI_fromAT( uint8_t *data );
void Take_IMSI_fromAT( uint8_t *data );

uint8_t hexTostr(uint8_t *src, uint8_t len, uint8_t *dtn);
void Data_Set_AT( uint8_t *data, uint16_t *datalen );

#endif /* _ATCMDJUDGEMENT_H_ */
