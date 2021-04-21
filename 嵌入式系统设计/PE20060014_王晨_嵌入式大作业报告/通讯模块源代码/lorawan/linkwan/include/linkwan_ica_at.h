/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef LINKWAN_AT_H
#define LINKWAN_AT_H

#define AT_CMD "AT"

#define AT_ERROR "+CME ERROR:"

#define  CMD_READ_INFO            33
#define  CMD_FACTROY_RESET        34
#define  CMD_UPDATA_INFO          35
#define  CMD_SETUPDATA_INFO       36
#define  CMD_RETURN_INFO          37

#define  LORAWAN_MODE      0
#define  LORAWAN_OTAA      1
//#define  LORAWAN_DEVEUI    {0xdd,0x98,0x92,0x9b,0x92,0xf0,0x9e,0x2d}
//#define  LORAWAN_APPKEY    {0xdd,0x98,0x92,0x9b,0x92,0xf0,0x9e,0x2d,0xaf,0x67,0x6d,0x64,0x6d,0x0f,0xff,0xf1}
//#define  LORAWAN_APPEUI    {0xdd,0x98,0x92,0x9b,0x92,0xf0,0x9e,0x2d}
#define  LORA_HEAD        0xFA
#define  LORA_LEN         0x05
#define  LORA_CMD1         0x05

#define  LORAWAM_MSG_LEN  255


#define  LORAWAN_FREQ    470300000
#define  LORAWAN_BAUD    38400

#define LORAWAN_CALSS    2

#define  LORAWAN_VER     "V4.2_1.2"
    
// mandatory
#define LORA_AT_CJOINMODE "+OTAA"  // join mode
#define LORA_AT_CDEVEUI "+DEVEUI"  // dev eui (OTAA)
#define LORA_AT_CAPPEUI "+APPEUI"  // app eui (OTAA)
#define LORA_AT_CAPPKEY "+APPKEY"  // app key (OTAA)
#define LORA_AT_CDEVADDR "+DEVADDR"  // dev addr (ABP)
#define LORA_AT_CAPPSKEY "+APPSKEY"  // sapp key (ABP)
#define LORA_AT_CNWKSKEY "+NWKSKEY"  // nwk skey (ABP)
#define LORA_AT_CADDMUTICAST "+MUTICAST"  // add mcast
#define LORA_AT_CDELMUTICAST "+DELMUTICAST"  // del mcast
#define LORA_AT_CNUMMUTICAST "+NUMMUTICAST"  // mcast num
#define LORA_AT_CFREQBANDMASK "+FREQBANDMASK"  // freqband mask
#define LORA_AT_CULDLMODE "+BAND"  // ul and dl
#define LORA_AT_CKEYSPROTECT "+KEYSPROTECT"  // keys protect
#define LORA_AT_CLPM "+LPM"  // low power

#define LORA_AT_CWORKMODE "+WORKMODE"  // work mode
#define LORA_AT_CREPEATERFREQ "+REPEATERFREQ"  // repeater freq
#define LORA_AT_CCLASS "+CLASS"  // class
#define LORA_AT_CBL "+CBL"  // battery level
#define LORA_AT_CSTATUS "+STATUS"  // cstatus
#define LORA_AT_CJOIN "+JOIN"  // OTTA join

#define LORA_AT_CERTIFI "+CERTIFI" // ali band
#define LORA_AT_FREQ "+FREQ" // ali band
#define LORA_AT_DEBUG "+DEBUG"//log print
#define LORA_AT_LIGHT "+SZLIGHT"
#define LORA_AT_BEATTIME "+BEATTIME"  //Heartbeat time interval
#define LORA_AT_ENDADDR "+ENDADDR"    //空开命令----末端空开号

 
    
#ifdef  CONFIG_PROJECT_CAINIAO
#define LORA_AT_DTRX "+DTX"  // tx
#else
#define LORA_AT_DTRX "+DTRX"  // tx
#endif        
#define LORA_AT_DRX "+DRX"  // rx

#define LORA_AT_CCONFIRM "+CONFIRM"  //cfm or uncfm
#define LORA_AT_CAPPPORT "+PORT"  // app port
#define LORA_AT_CDATARATE "+DATARATE"  // data rate
#define LORA_AT_CRSSI "+RSSI"  // rssi
#define LORA_AT_CNBTRIALS "+CNBTRIALS"  // nb trans
#define LORA_AT_CRM "+CRM"  // report mode
#define LORA_AT_CTXP "+POWER"  // tx power
#define LORA_AT_CLINKCHECK "+LINKCHECK"  // link check
#define LORA_AT_CADR "+ADR"  // ADR
#define LORA_AT_CRXP "+CRXP"  // rx win params
#define LORA_AT_CFREQLIST "+FREQLIST"  // freq list
#define LORA_AT_CRX1DELAY "+RX1DELAY"  // rx1 win delay
#define LORA_AT_CSAVE "+SAVE"  // save cfg
#define LORA_AT_CRESTORE "+RESTORE"  // restore def cfg

//#define  LORA_AT_VERSION   "+VERSION"

#define LORA_AT_PINGSLOTINFOREQ "+PINGSLOTINFOREQ" //class B send pingSlotInfoReq  

// repeater
#define LORA_AT_CREPEATERFILTER "+REPEATERFILTER"

// optional
#define LORA_AT_CGMI "+CGMI"  // manufacture identification
#define LORA_AT_CGMM "+CGMM"  // model identification
//#define LORA_AT_CGMR "+CGMR"  // revision info
#define LORA_AT_CGMR "+DEVVER"  // revision info

#define LORA_AT_CGSN "+CGSN"  // product serial number id
#define LORA_AT_CGBR "+UARTBR"  // baud rate on UART interface

#define LORA_AT_ILOGLVL "+ILOGLVL"   // log level
#define LORA_AT_IREBOOT "+REBOOT"
#define LORA_AT_IDEFAULT "+IDEFAULT"

#define LORA_AT_CSLEEP   "+SLEEP"
#define LORA_AT_CMCU    "+CMCU"
#define LORA_AT_CRX     "+CRX"
#define LORA_AT_CRXS     "+CRXS"
#define LORA_AT_CTX     "+CTX"
#define LORA_AT_CTXCW   "+CTXCW"
#define LORA_AT_CSTDBY  "+CSTDBY"



typedef  enum
{
  BAUD_38400 = 0,
  BAUD_1200,
  BAUD_2400,
  BAUD_4800,
  BAUD_9600,
  BAUD_19200,
  BAUD_57600,
  BAUD_115200

}Baud_t;

void linkwan_at_init(void);
void linkwan_at_process(void);
void linkwan_serial_input(uint8_t cmd);
int linkwan_serial_output(uint8_t *buffer, int len);
void linkwan_at_prompt_print();
#endif
