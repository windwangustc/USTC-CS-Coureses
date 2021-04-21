/*
 * ATCommand.h
 *
 *  Created on: 2019Äê4ÔÂ16ÈÕ
 *      Author: 8033
 */

#ifndef _ATCOMMAND_H_
#define _ATCOMMAND_H_

/* BC28 AT Command */
#define 	AT_RECV_OK						"OK\r\n"
//#define     AT_RECV_CSCON                   "+CSCON:1,1\r\n"
//#define     AT_RECV_CSCON_IDLE              "+CSCON:1,"
#define     AT_RECV_REBOOT                  "REBOOTING\r\n"
#define     AT_RECV_CGATT                   "+CGATT:1\r\n"
#define     AT_RECV_IOT                     "+QLWEVTIND:3"

#define 	AT_CMD_TEST          			"AT\r\n"
#define     AT_CMD_ECHO						"ATE0\r\n"
#define     AT_CMD_REBOOT                   "AT+NRB\r\n"
#define		AT_CHECK_CGMR					"AT+CGMR\r\n"
#define 	AT_CHECK_CGSN   			    "AT+CGSN=1\r\n"
#define     AT_CHECK_CIMI                   "AT+CIMI\r\n"
#define     AT_CHECK_CSQ					"AT+CSQ\r\n"
#define     AT_CLOSE_CFUN                   "AT+CFUN=0\r\n"
#define     AT_OPEN_CFUN					"AT+CFUN=1\r\n"
#define     AT_CMD_CGATT					"AT+CGATT=1\r\n"
#define     AT_CLOSE_CSCON                  "AT+CSCON=0\r\n"
#define 	AT_CHECK_CGATT					"AT+CGATT?\r\n"
#define     AT_CHECK_ADDR                   "AT+CGPADDR\r\n"
#define     AT_CMD_ERROR_MSG                "AT+CMEE=1\r\n"
#define     AT_CMD_APN						"AT+CGDCONT=1,\"IP\",\"cmnbiot\"\r\n"
#define     AT_CMD_PSM						"AT+CPSMS=0\r\n"    //1,,,01000011,00100100\r\n"
#define     AT_CHECK_EDRX					"AT+CEDRXS?\r\n"  //+CEDRXS:5,"0010"
#define     AT_CMD_EDRX    					"AT+CEDRXS=0,5\r\n"
#define     AT_OPEN_RECV_MSG             	"AT+NNMI=1\r\n"
#define     AT_CHECK_STATUS              	"AT+NUESTATS\r\n"
#define     AT_CHECK_CCLK   				"AT+CCLK?\r\n"
#define     AT_CMD_REGISTER_IOT             "AT+QREGSWT=1\r\n"
#define     AT_CMD_AUTO   					"AT+NCONFIG=AUTOCONNECT,TRUE\r\n"

#endif /* _ATCOMMAND_H_ */
