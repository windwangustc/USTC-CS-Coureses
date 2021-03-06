/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 * Modified by Chen Wang @ 2019-12-27
 */

#ifndef LINKWAN_AT_H
#define LINKWAN_AT_H

#define LORA_AT_HELP "AT+?"  // help
#define LORA_AT_RM "AT+RM"  // rm stored lora info

#define LORA_AT_APPEUI "AT+APPEUI"  // app eui
#define LORA_AT_APPKEY "AT+APPKEY"  // app key
#define LORA_AT_DEUI "AT+DEUI"  // device eui

#define LORA_AT_DR "AT+DR"  // datarate
#define LORA_AT_ADR "AT+ADR"  // ADR
#define LORA_AT_CLASS "AT+CLASS"  // class
#define LORA_AT_SCANMASK "AT+SMASK"  // scan mask

#define LORA_AT_CFM "AT+CFM"  // confirmation mode
#define LORA_AT_CFMTRIALS "AT+CTRIALS"  // cfm trials
#define LORA_AT_JOIN "AT+JOIN"  // join network

#define LORA_AT_DCS "AT+DCS"  // dutycycle
#define LORA_AT_TXSIZE "AT+TXSIZE"  // tx size
#define LORA_AT_LINKCHK "AT+LINKCHK"  // link check req

#define LORA_AT_TX "AT+TX"  // tx data, AT+TX port:data in hex
#define LORA_AT_RX "AT+RX" // rx data, AT+RX port:data in hex

/*extend by ASR*/
#define LORA_PROMPT "ASR6501:~# "   //type "enter" prompt
#define LORA_AT_TXPWR "AT+TXPWR"    //tx power
#define LORA_AT_DEBUG "AT+DEBUG"    //debug output
#define LORA_AT_OTAA "AT+OTAA"    //activation mode:1 for otaa, 0 for abp
#define LORA_AT_DEVADDR "AT+DEVADDR"    //ABP's DevAddr
#define LORA_AT_NWKSKEY "AT+NWKSKEY"    //ABP's NwkSKey
#define LORA_AT_APPSKEY "AT+APPSKEY"    //ABP's AppSKey
#define LORA_AT_FREQ "AT+FREQ"  //specify freq
void linkwan_serial_input(uint8_t cmd);
int linkwan_serial_output(uint8_t *buffer, int len);

#endif
