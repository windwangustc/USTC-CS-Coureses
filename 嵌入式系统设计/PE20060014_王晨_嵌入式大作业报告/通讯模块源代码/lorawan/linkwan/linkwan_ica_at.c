/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdlib.h>
#include "hw.h"
#include "LoRaMac.h"
#include "Region.h"
#include "board_test.h"
#include "lwan_config.h"
#include "linkwan.h"
#include "low_power.h"
#include "linkwan_ica_at.h"
#include "lorawan_port.h"
#include "uart_port.h"


#define ARGC_LIMIT 16
#define ATCMD_SIZE (LORAWAN_APP_DATA_BUFF_SIZE * 2 + 18)
#define PORT_LEN 4

#define QUERY_CMD		0x01
#define EXECUTE_CMD		0x02
#define DESC_CMD        0x03
#define SET_CMD			0x04

extern LWanDevConfig_t *g_lwan_dev_config_p;
extern lora_dev_para_t lora_dev_para;
extern lora_dev_group lora_dev_para_g;
extern LWanDevKeys_t *g_lwan_dev_keys_p;
extern lora_dev_para_t lora_dev_para_factory;
extern uint16_t CRC16Calc(uint8_t *puchMsg, uint8_t usDataLen) ;

//extern lora_heartbeat_para_t lora_heartbeat_para;
//uint8_t PowerIndex[] = {22,}
uint8_t atcmd[ATCMD_SIZE];
uint16_t atcmd_index = 0;
volatile bool g_atcmd_processing = false;
uint8_t g_default_key[LORA_KEY_LENGTH] = {0x41, 0x53, 0x52, 0x36, 0x35, 0x30, 0x58, 0x2D, 
                                          0x32, 0x30, 0x31, 0x38, 0x31, 0x30, 0x33, 0x30};
uint8_t WorkModeFlag = false;

typedef struct {
	char *cmd;
	int (*fn)(int opt, int argc, char *argv[]);	
}at_cmd_t;

//AT functions
static int at_cjoinmode_func(int opt, int argc, char *argv[]);
static int at_cdeveui_func(int opt, int argc, char *argv[]);
static int at_cappeui_func(int opt, int argc, char *argv[]);
static int at_cappkey_func(int opt, int argc, char *argv[]);
static int at_cdevaddr_func(int opt, int argc, char *argv[]);
static int at_cappskey_func(int opt, int argc, char *argv[]);
static int at_cnwkskey_func(int opt, int argc, char *argv[]);
static int at_caddmulticast_func(int opt, int argc, char *argv[]);
static int at_cdelmulticast_func(int opt, int argc, char *argv[]);
static int at_cnummulticast_func(int opt, int argc, char *argv[]);
static int at_cfreqbandmask_func(int opt, int argc, char *argv[]);
static int at_culdlmode_func(int opt, int argc, char *argv[]);
static int at_cworkmode_func(int opt, int argc, char *argv[]);
static int at_cclass_func(int opt, int argc, char *argv[]);
static int at_cbl_func(int opt, int argc, char *argv[]);
static int at_cstatus_func(int opt, int argc, char *argv[]);
static int at_cjoin_func(int opt, int argc, char *argv[]);
static int at_dtrx_func(int opt, int argc, char *argv[]);
static int at_drx_func(int opt, int argc, char *argv[]);
static int at_cconfirm_func(int opt, int argc, char *argv[]);
static int at_cappport_func(int opt, int argc, char *argv[]);
static int at_cdatarate_func(int opt, int argc, char *argv[]);
static int at_crssi_func(int opt, int argc, char *argv[]);
static int at_cnbtrials_func(int opt, int argc, char *argv[]);
static int at_crm_func(int opt, int argc, char *argv[]);
static int at_ctxp_func(int opt, int argc, char *argv[]);
static int at_clinkcheck_func(int opt, int argc, char *argv[]);
static int at_cadr_func(int opt, int argc, char *argv[]);
static int at_crxp_func(int opt, int argc, char *argv[]);
static int at_crx1delay_func(int opt, int argc, char *argv[]);
static int at_csave_func(int opt, int argc, char *argv[]);
static int at_crestore_func(int opt, int argc, char *argv[]);
static int at_cpslotinforeq_func(int opt, int argc, char *argv[]);
static int at_ckeysprotect_func(int opt, int argc, char *argv[]);
static int at_clpm_func(int opt, int argc, char *argv[]);
static int at_cgmi_func(int opt, int argc, char *argv[]);
static int at_cgmm_func(int opt, int argc, char *argv[]);
static int at_cgmr_func(int opt, int argc, char *argv[]);
static int at_cgsn_func(int opt, int argc, char *argv[]);
static int at_cgbr_func(int opt, int argc, char *argv[]);
static int at_iloglvl_func(int opt, int argc, char *argv[]);
static int at_ireboot_func(int opt, int argc, char *argv[]);
static int at_Freq_func(int opt, int argc, char *argv[]);
static int at_Certifi_func(int opt, int argc, char *argv[]);
static int at_Debug_func(int opt, int argc, char *argv[]);
static int at_szLight_func (int opt, int argc, char *argv[]);
static int at_beattime_func (int opt, int argc, char *argv[]);
static int at_endaddr_func (int opt,int argc, char *argv[]);

//test AT functions
#ifndef CONFIG_ASR650X_TEST_DISABLE
static int at_csleep_func(int opt, int argc, char *argv[]);
static int at_cmcu_func(int opt, int argc, char *argv[]);
static int at_ctxcw_func(int opt, int argc, char *argv[]);
static int at_crxs_func(int opt, int argc, char *argv[]);
static int at_crx_func(int opt, int argc, char *argv[]);
static int at_ctx_func(int opt, int argc, char *argv[]);
static int at_cstdby_func(int opt, int argc, char *argv[]);
#endif

static at_cmd_t g_at_table[] = {
    {LORA_AT_CJOINMODE, at_cjoinmode_func},
    {LORA_AT_CDEVEUI,  at_cdeveui_func},
    {LORA_AT_CAPPEUI,  at_cappeui_func},
    {LORA_AT_CAPPKEY,  at_cappkey_func},
    {LORA_AT_CDEVADDR,  at_cdevaddr_func},
    {LORA_AT_CAPPSKEY,  at_cappskey_func},
    {LORA_AT_CNWKSKEY,  at_cnwkskey_func},
    {LORA_AT_CADDMUTICAST,  at_caddmulticast_func},
    {LORA_AT_CDELMUTICAST,  at_cdelmulticast_func},
    {LORA_AT_CNUMMUTICAST,  at_cnummulticast_func},
    {LORA_AT_CFREQBANDMASK,  at_cfreqbandmask_func},
    {LORA_AT_CULDLMODE,  at_culdlmode_func},
    {LORA_AT_CWORKMODE,  at_cworkmode_func},
    {LORA_AT_CCLASS,  at_cclass_func},
    {LORA_AT_CBL,  at_cbl_func},
    {LORA_AT_CSTATUS,  at_cstatus_func},
    {LORA_AT_CJOIN, at_cjoin_func},
    {LORA_AT_DTRX,  at_dtrx_func},
    {LORA_AT_DRX,  at_drx_func},
    {LORA_AT_CCONFIRM,  at_cconfirm_func},
    {LORA_AT_CAPPPORT,  at_cappport_func},
    {LORA_AT_CDATARATE,  at_cdatarate_func},
    {LORA_AT_CRSSI,  at_crssi_func},
    {LORA_AT_CNBTRIALS,  at_cnbtrials_func},
    {LORA_AT_CRM,  at_crm_func},
    {LORA_AT_CTXP,  at_ctxp_func},
    {LORA_AT_CLINKCHECK,  at_clinkcheck_func},
    {LORA_AT_CADR,  at_cadr_func},
    {LORA_AT_CRXP,  at_crxp_func},
    {LORA_AT_CRX1DELAY,  at_crx1delay_func},
    {LORA_AT_CSAVE,  at_csave_func},
    {LORA_AT_CRESTORE,  at_crestore_func},
    {LORA_AT_PINGSLOTINFOREQ,  at_cpslotinforeq_func},
    {LORA_AT_CKEYSPROTECT,  at_ckeysprotect_func},
    {LORA_AT_CLPM,  at_clpm_func},
    {LORA_AT_CGMI,  at_cgmi_func},
    {LORA_AT_CGMM,  at_cgmm_func},
    {LORA_AT_CGMR,  at_cgmr_func},
    {LORA_AT_CGSN,  at_cgsn_func},
    {LORA_AT_CGBR,  at_cgbr_func},
    {LORA_AT_ILOGLVL,  at_iloglvl_func},
    {LORA_AT_IREBOOT,  at_ireboot_func},
    {LORA_AT_FREQ,  at_Freq_func},
    {LORA_AT_CERTIFI,  at_Certifi_func},
    {LORA_AT_DEBUG,  at_Debug_func},
    {LORA_AT_LIGHT,  at_szLight_func},
    {LORA_AT_BEATTIME,  at_beattime_func},
    {LORA_AT_ENDADDR,  at_endaddr_func},
    
#ifndef CONFIG_ASR650X_TEST_DISABLE    
    {LORA_AT_CSLEEP,  at_csleep_func},
    {LORA_AT_CMCU,  at_cmcu_func},
    {LORA_AT_CTXCW,  at_ctxcw_func},
    {LORA_AT_CRXS,  at_crxs_func},
    {LORA_AT_CRX,  at_crx_func},
    {LORA_AT_CTX,  at_ctx_func},
    {LORA_AT_CSTDBY,  at_cstdby_func},
#endif    
};

#define AT_TABLE_SIZE	(sizeof(g_at_table) / sizeof(at_cmd_t))

static int hex2bin(const char *hex, uint8_t *bin, uint16_t bin_length)
{
    uint16_t hex_length = strlen(hex);
    const char *hex_end = hex + hex_length;
    uint8_t *cur = bin;
    uint8_t num_chars = hex_length & 1;
    uint8_t byte = 0;

    if ((hex_length + 1) / 2 > bin_length) {
        return -1;
    }

    while (hex < hex_end) {
        if ('A' <= *hex && *hex <= 'F') {
            byte |= 10 + (*hex - 'A');
        } else if ('a' <= *hex && *hex <= 'f') {
            byte |= 10 + (*hex - 'a');
        } else if ('0' <= *hex && *hex <= '9') {
            byte |= *hex - '0';
        } else {
            return -1;
        }
        hex++;
        num_chars++;

        if (num_chars >= 2) {
            num_chars = 0;
            *cur++ = byte;
            byte = 0;
        } else {
            byte <<= 4;
        }
    }
    return cur - bin;
}

// this can be in intrpt context
void linkwan_serial_input(uint8_t cmd)
{
#if 0
	if(g_atcmd_processing) 
    {
        printf("start\r\n");
        return;
    }
#endif
    if ((cmd >= '0' && cmd <= '9') || (cmd >= 'a' && cmd <= 'z') ||
        (cmd >= 'A' && cmd <= 'Z') || cmd == '?' || cmd == '+' ||
        cmd == ':' || cmd == '=' || cmd == ' ' || cmd == ',') {
        if (atcmd_index >= ATCMD_SIZE) {
            memset(atcmd, 0xff, ATCMD_SIZE);
            atcmd_index = 0;
            return;
        }
        atcmd[atcmd_index++] = cmd;
    } else if (cmd == '\r' || cmd == '\n') {
        if (atcmd_index >= ATCMD_SIZE) {
            memset(atcmd, 0xff, ATCMD_SIZE);
            atcmd_index = 0;
            return;
        }
        atcmd[atcmd_index] = '\0';
    }
	else
	{
        if (atcmd_index >= ATCMD_SIZE) {
            memset(atcmd, 0xff, ATCMD_SIZE);
            atcmd_index = 0;
            return;
        }
        atcmd[atcmd_index++] = cmd;
	}
	//printf("%02x",atcmd[atcmd_index++]);
}

int linkwan_serial_output(uint8_t *buffer, int len)
{
    PRINTF_AT("%s", buffer);
    linkwan_at_prompt_print();
    return 0;
}

void linkwan_at_prompt_print()
{
    //PRINTF_RAW("\r\n%s%s:~# ", CONFIG_MANUFACTURER, CONFIG_DEVICE_MODEL);
}

static int at_cjoinmode_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
    uint8_t join_mode;
        
    switch(opt) {
        case QUERY_CMD: {
            ret = LWAN_SUCCESS;
            lwan_dev_config_get(DEV_CONFIG_JOIN_MODE, &join_mode);  
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:%d\r\nOK\r\n", LORA_AT_CJOINMODE, join_mode);
            break;
        }
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:\"mode\"\r\nOK\r\n", LORA_AT_CJOINMODE);
            break;
        }
        case SET_CMD: {
            if(argc < 1) break;
            
            join_mode = strtol((const char *)argv[0], NULL, 0);
            int res = lwan_dev_config_set(DEV_CONFIG_JOIN_MODE, (void *)&join_mode);
            if (res == LWAN_SUCCESS) {
                ret = LWAN_SUCCESS;
                snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }
            break;
        }
        default: break;
    }
    
    return ret;
}

static int at_cdeveui_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
    uint8_t length;
    uint8_t buf[16];
        
    switch(opt) {
        case QUERY_CMD: {
            ret = LWAN_SUCCESS;
            lwan_dev_keys_get(DEV_KEYS_OTA_DEVEUI, buf);
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:%02X%02X%02X%02X%02X%02X%02X%02X\r\nOK\r\n", \
                     LORA_AT_CDEVEUI, buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
            break;
        }
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s=\"DevEUI:length is 16\"\r\n", LORA_AT_CDEVEUI);
            break;
        }
        case SET_CMD: {
            if(argc < 1) break;
            
            length = hex2bin((const char *)argv[0], buf, LORA_EUI_LENGTH);
            if (length == LORA_EUI_LENGTH) {
                if(lwan_dev_keys_set(DEV_KEYS_OTA_DEVEUI, buf) == LWAN_SUCCESS) {
                    snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
                    ret = LWAN_SUCCESS;
                }
            }
            break;
        }
        default: break;
    }
    
    return ret;
}

static int at_cappeui_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
    uint8_t length;
    uint8_t buf[16];
        
    switch(opt) {
        case QUERY_CMD: {
            ret = LWAN_SUCCESS;
            lwan_dev_keys_get(DEV_KEYS_OTA_APPEUI, buf);
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:%02X%02X%02X%02X%02X%02X%02X%02X\r\nOK\r\n", \
                     LORA_AT_CAPPEUI, buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
            break;
        }
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s=\"AppEUI:length is 16\"\r\n", LORA_AT_CAPPEUI);
            break;
        }
        case SET_CMD: {
            if(argc < 1) break;
            length = hex2bin((const char *)argv[0], buf, LORA_EUI_LENGTH);
            if (length == LORA_EUI_LENGTH && lwan_dev_keys_set(DEV_KEYS_OTA_APPEUI, buf) == LWAN_SUCCESS) {
                snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
                ret = LWAN_SUCCESS;
            }
            break;
        }
        default: break;
    }

    return ret;
}

static int at_cappkey_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
    uint8_t length;
    uint8_t buf[16];
        
    switch(opt) {
        case QUERY_CMD: {
            ret = LWAN_SUCCESS;
            lwan_dev_keys_get(DEV_KEYS_OTA_APPKEY, buf);   
            snprintf((char *)atcmd, ATCMD_SIZE,
                     "\r\n%s:%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\r\nOK\r\n", \
                     LORA_AT_CAPPKEY, buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7],
                                      buf[8], buf[9], buf[10], buf[11], buf[12], buf[13], buf[14], buf[15]);
            break;
        }
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s=\"AppKey:length is 32\"\r\n", LORA_AT_CAPPKEY);
            break;
        }
        case SET_CMD: {
            if(argc < 1) break;
            length = hex2bin((const char *)argv[0], buf, LORA_KEY_LENGTH);
            if (length == LORA_KEY_LENGTH && lwan_dev_keys_set(DEV_KEYS_OTA_APPKEY, buf) == LWAN_SUCCESS) {
                snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
                ret = LWAN_SUCCESS;
            }
            break;
        }
        default: break;
    }

    return ret;
}

static int at_cdevaddr_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
    uint8_t length;
    uint8_t buf[16];
        
    switch(opt) {
        case QUERY_CMD: {
            ret = LWAN_SUCCESS;
            uint32_t devaddr;
            lwan_dev_keys_get(DEV_KEYS_ABP_DEVADDR, &devaddr);
            length = snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:%08X\r\nOK\r\n", LORA_AT_CDEVADDR, (unsigned int)devaddr);
            break;
        }
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s=<DevAddr:length is 8, Device address of ABP mode>\r\n", LORA_AT_CDEVADDR);
            break;
        }
        case SET_CMD: {
            if(argc < 1) break;
            
            length = hex2bin((const char *)argv[0], buf, 4);
            if (length == 4) {
                uint32_t devaddr = buf[0] << 24 | buf[1] << 16 | buf[2] <<8 | buf[3];
                if(lwan_dev_keys_set(DEV_KEYS_ABP_DEVADDR, &devaddr) == LWAN_SUCCESS) {
                    snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
                    ret = LWAN_SUCCESS;
                }
            }
            break;
        }
        default: break;
    }

    return ret;
}

static int at_cappskey_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
    uint8_t length;
    uint8_t buf[16];
        
    switch(opt) {
        case QUERY_CMD: {
            ret = LWAN_SUCCESS;
            lwan_dev_keys_get(DEV_KEYS_ABP_APPSKEY, buf); 
            length = snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:", LORA_AT_CAPPSKEY);
            for (int i = 0; i < LORA_KEY_LENGTH; i++) {
                length += snprintf((char *)(atcmd + length), ATCMD_SIZE, "%02X", buf[i]);
            }
            snprintf((char *)(atcmd + length), ATCMD_SIZE, "\r\nOK\r\n");
            break;
        }
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s=<AppSKey: length is 32>\r\n", LORA_AT_CAPPSKEY);
            break;
        }
        case SET_CMD: {
            if(argc < 1) break;
            length = hex2bin((const char *)argv[0], buf, LORA_KEY_LENGTH);
            if (length == LORA_KEY_LENGTH) {
                if(lwan_dev_keys_set(DEV_KEYS_ABP_APPSKEY, buf) == LWAN_SUCCESS) {
                    snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
                    ret = true;
                }
            }
            break;
        }
        default: break;
    }

    return ret;
}


static int at_cnwkskey_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
    uint8_t length;
    uint8_t buf[16];
        
    switch(opt) {
        case QUERY_CMD: {
            ret = LWAN_SUCCESS;
            lwan_dev_keys_get(DEV_KEYS_ABP_NWKSKEY, buf); 
            length = snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:", LORA_AT_CNWKSKEY);
            for (int i = 0; i < LORA_KEY_LENGTH; i++) {
                length += snprintf((char *)(atcmd + length), ATCMD_SIZE, "%02X", buf[i]);
            }
            snprintf((char *)(atcmd + length), ATCMD_SIZE, "\r\nOK\r\n");
            break;
        }
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s=<NwkSKey:length is 32>\r\n", LORA_AT_CNWKSKEY);
            break;
        }
        case SET_CMD: {
            if(argc < 1) break;
            length = hex2bin((const char *)argv[0], buf, LORA_KEY_LENGTH);
            if (length == LORA_KEY_LENGTH) {
                if(lwan_dev_keys_set(DEV_KEYS_ABP_NWKSKEY, buf) == LWAN_SUCCESS) {
                    snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
                    ret = true;
                }
            }
            break;
        }
        default: break;
    }
 
    return ret;
}

static int at_caddmulticast_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
        
    switch(opt) {

		case QUERY_CMD: {
            ret = LWAN_SUCCESS;
          //  lwan_dev_keys_get(DEV_KEYS_ABP_NWKSKEY, buf); 
           // length = snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:\r\n", LORA_AT_CNWKSKEY);
            printf("\r\n%s:\r\n",LORA_AT_CNWKSKEY);
            for (int i = 0; i < 3; i++) 
			{
			   printf("DevAddr[%d]:%02x%02x%02x%02x\r\n",i,(uint8_t)(lora_dev_para_g.lora_group[i].devaddr>>24),(uint8_t)(lora_dev_para_g.lora_group[i].devaddr>>16),(uint8_t)(lora_dev_para_g.lora_group[i].devaddr>>8),(uint8_t)lora_dev_para_g.lora_group[i].devaddr);
			   printf("AppSkey[%d]:%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\r\n",i, lora_dev_para_g.lora_group[i].appskey[0], lora_dev_para_g.lora_group[i].appskey[1], lora_dev_para_g.lora_group[i].appskey[2], lora_dev_para_g.lora_group[i].appskey[3],	 \
								 lora_dev_para_g.lora_group[i].appskey[4], lora_dev_para_g.lora_group[i].appskey[5], lora_dev_para_g.lora_group[i].appskey[6], lora_dev_para_g.lora_group[i].appskey[7],								 \
								lora_dev_para_g.lora_group[i].appskey[8], lora_dev_para_g.lora_group[i].appskey[9], lora_dev_para_g.lora_group[i].appskey[10],	lora_dev_para_g.lora_group[i].appskey[11], \
								 lora_dev_para_g.lora_group[i].appskey[12], lora_dev_para_g.lora_group[i].appskey[13], lora_dev_para_g.lora_group[i].appskey[14],
								 lora_dev_para_g.lora_group[i].appskey[15]);
			   printf("NwkSkey[%d]:%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\r\n",i,lora_dev_para_g.lora_group[i].nwkskey[0], lora_dev_para_g.lora_group[i].nwkskey[1], lora_dev_para_g.lora_group[i].nwkskey[2], lora_dev_para_g.lora_group[i].nwkskey[3],	 \
								 lora_dev_para_g.lora_group[i].nwkskey[4], lora_dev_para_g.lora_group[i].nwkskey[5], lora_dev_para_g.lora_group[i].nwkskey[6], lora_dev_para_g.lora_group[i].nwkskey[7],								 \
								lora_dev_para_g.lora_group[i].nwkskey[8], lora_dev_para_g.lora_group[i].nwkskey[9], lora_dev_para_g.lora_group[i].nwkskey[10],	lora_dev_para_g.lora_group[i].nwkskey[11], \
								 lora_dev_para_g.lora_group[i].nwkskey[12], lora_dev_para_g.lora_group[i].nwkskey[13], lora_dev_para_g.lora_group[i].nwkskey[14],
								 lora_dev_para_g.lora_group[i].nwkskey[15]);
            }
            snprintf((char *)(atcmd), ATCMD_SIZE, "\r\nOK\r\n");
            break;
        }
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:\"DevAddr\",\"AppSKey\",\"NwkSKey\"\r\nOK\r\n", LORA_AT_CADDMUTICAST);
            break;
        }
        case SET_CMD: {
            if(argc < 4) break;

            
          //  MulticastParams_t curMulticastInfo;
            uint8_t num =strtoul(argv[3], NULL, 16);
            lora_dev_para_g.lora_group[num-1].devaddr= (uint32_t)strtoul(argv[0], NULL, 16);
            uint8_t appskey_len = hex2bin((const char *)argv[1], lora_dev_para_g.lora_group[num-1].appskey, 16);
            uint8_t nwkskey_len = hex2bin((const char *)argv[2], lora_dev_para_g.lora_group[num-1].nwkskey, 16);
            
            if (appskey_len == 16 && nwkskey_len == 16) {
				
				if(set_lora_group() == LWAN_SUCCESS)
				{
				    ret = LWAN_SUCCESS;
					snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
				}
            }
            
            break;
        }
        default: break;
    }

    return ret;
}

static int at_cdelmulticast_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
        
    switch(opt) {
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:\"DevAddr\"\r\nOK\r\n", LORA_AT_CDELMUTICAST);
            break;
        }
        case SET_CMD: {
            if(argc < 1) break;
            uint32_t devAddr = (uint32_t)strtol((const char *)argv[0], NULL, 16);
            ret = lwan_multicast_del(devAddr);
            if (ret == true) {
                snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            } else {
                ret = false;
	        }
            break;
        }
        default: break;
    }
 
    return ret;
}

static int at_cnummulticast_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
        
    switch(opt) {
        case QUERY_CMD: {
            ret = LWAN_SUCCESS;
            uint8_t multiNum = lwan_multicast_num_get();
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:%u\r\nOK\r\n", LORA_AT_CNUMMUTICAST, multiNum);
            break;
        }
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:\"number\"\r\nOK\r\n", LORA_AT_CNUMMUTICAST);
            break;
        }
        default: break;
    }

    return ret;
}

static int at_cfreqbandmask_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
    uint8_t length;
    uint16_t freqband_mask;
        
    switch(opt) {
        case QUERY_CMD: {
            ret = LWAN_SUCCESS;
            lwan_dev_config_get(DEV_CONFIG_FREQBAND_MASK, &freqband_mask);  
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:%04X\r\nOK\r\n", LORA_AT_CFREQBANDMASK, freqband_mask);
            break;
        }
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s=\"mask\"\r\nOK\r\n", LORA_AT_CFREQBANDMASK);
            break;
        }
        case SET_CMD: {
            if(argc < 1) break;
            uint8_t mask[2];
            length = hex2bin((const char *)argv[0], (uint8_t *)mask, 2);
            if (length == 2) {
                freqband_mask = mask[1] | ((uint16_t)mask[0] << 8);
                if (lwan_dev_config_set(DEV_CONFIG_FREQBAND_MASK, (void *)&freqband_mask) == LWAN_SUCCESS) {
                    ret = LWAN_SUCCESS;
                    snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
                }
            }
            break;
        }
        default: break;
    }

    return ret;
}

static int at_culdlmode_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
    uint8_t mode;
        
    switch(opt) {
        case QUERY_CMD: {
            ret = LWAN_SUCCESS;
           // lwan_dev_config_get(DEV_CONFIG_ULDL_MODE, &mode);
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:%d\r\nOK\r\n", LORA_AT_CULDLMODE, lora_dev_para.band);
            break;
        }
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s=\"mode\"\r\nOK\r\n", LORA_AT_CULDLMODE);
            break;
        }
        case SET_CMD: {
            if(argc < 1) break;
            
            mode = strtol((const char *)argv[0], NULL, 0);
            if (lwan_dev_config_set(DEV_CONFIG_ULDL_MODE, (void *)&mode) == LWAN_SUCCESS) {
                
				if(set_lora_band(mode) == LWAN_SUCCESS)
				{
				    ret = LWAN_SUCCESS;
                    snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
				}
            }
            break;
        }
        default: break;
    }

    return ret;
}

static int at_cworkmode_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
    uint8_t mode;
        
    switch(opt) {
        case QUERY_CMD: {
            ret = LWAN_SUCCESS;
            lwan_dev_config_get(DEV_CONFIG_WORK_MODE, &mode);
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:%d\r\nOK\r\n", LORA_AT_CWORKMODE, mode);
            break;
        }
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s=\"mode\"\r\nOK\r\n", LORA_AT_CWORKMODE);
            break;
        }
        case SET_CMD: {
            if(argc < 1) break;
            
            mode = strtol((const char *)argv[0], NULL, 0);
            if (mode == 2) {
                ret = LWAN_SUCCESS;
                snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }
            
            break;
        }
        default: break;
    }

    return ret;
}

static int at_cclass_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
    uint8_t class;
        
    switch(opt) {
        case QUERY_CMD: {
            ret = LWAN_SUCCESS;
            lwan_dev_config_get(DEV_CONFIG_CLASS, &class);  
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:%d\r\nOK\r\n", LORA_AT_CCLASS, class);
            break;
        }
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE,
                     "\r\n%s:\"class\",\"branch\",\"para1\",\"para2\",\"para3\",\"para4\"\r\nOK\r\n",
                     LORA_AT_CCLASS);
            break;
        }
        case SET_CMD: {
            if(argc < 1) break;
            
            int8_t branch = -1;
            uint32_t param1 = 0;
            uint32_t param2 = 0;
            uint32_t param3 = 0;
            uint32_t param4 = 0;
            class = strtol((const char *)argv[0], NULL, 0);
            if(class == CLASS_B) {
                if(argc>1) {
                    branch = strtol((const char *)argv[1], NULL, 0);
                    param1 = strtol((const char *)argv[2], NULL, 0);
                    if(branch == 1 ){
                        param2 = strtol((const char *)argv[3], NULL, 0);
                        param3 = strtol((const char *)argv[4], NULL, 0);
                        param4 = strtol((const char *)argv[5], NULL, 0);
                    }
                }
            }

            if (lwan_dev_config_set(DEV_CONFIG_CLASS, (void *)&class) == LWAN_SUCCESS) {
                ClassBParam_t classb_param;
                lwan_dev_config_get(DEV_CONFIG_CLASSB_PARAM, (void *)&classb_param);
                if(branch==0){
                    classb_param.periodicity = param1;
                }else{
                    classb_param.beacon_freq = param1;
                    classb_param.beacon_dr = param2;
                    classb_param.pslot_freq = param3;
                    classb_param.beacon_dr = param4;
                }
                if (lwan_dev_config_set(DEV_CONFIG_CLASSB_PARAM, (void *)&classb_param) == LWAN_SUCCESS) {
                    ret = LWAN_SUCCESS;
                    snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
                }
            }
            
            break;
        }
        default: break;
    }

    return ret;
}

static int at_cbl_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
        
    switch(opt) {
        case QUERY_CMD: {
            ret = LWAN_SUCCESS;
            uint8_t batteryLevel = lwan_dev_battery_get();
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:%d\r\nOK\r\n", LORA_AT_CBL, batteryLevel);
            break;
        }
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:\"value\"\r\nOK\r\n", LORA_AT_CBL);
            break;
        }
        default: break;
    }

    return ret;
}

static int at_cstatus_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
    int status;
        
    switch(opt) {
        case QUERY_CMD: {
            ret = LWAN_SUCCESS;
            status = lwan_dev_status_get();
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:%02d\r\nOK\r\n", LORA_AT_CSTATUS, status);
            break;
        }
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:\"status\"\r\nOK\r\n", LORA_AT_CSTATUS);
            break;
        }
        default: break;
    }
 
    return ret;
}

static int at_dtrx_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
        
    switch(opt) {
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:[confirm],[nbtrials],[length],<payload>\r\nOK\r\n", LORA_AT_DTRX);
            break;
        }
        case SET_CMD: {
            if(argc < 2) break;
            
            uint8_t confirm, Nbtrials;
            uint16_t len;
            int bin_len = 0;
            uint8_t payload[LORAWAN_APP_DATA_BUFF_SIZE];
            confirm = strtol((const char *)argv[0], NULL, 0);
            Nbtrials = strtol((const char *)argv[1], NULL, 0);
            len = strtol((const char *)argv[2], NULL, 0);
            bin_len = hex2bin((const char *)argv[3], payload, len);
            
            if(bin_len>=0) {
                ret = LWAN_SUCCESS;
                if (lwan_data_send(confirm, Nbtrials, payload, bin_len) == LWAN_SUCCESS) {
                    snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK+SEND:%02X\r\n", bin_len);
                }else{
                    snprintf((char *)atcmd, ATCMD_SIZE, "\r\nERR+SEND:00\r\n");
                }
            }
            break;
        }
        default: break;
    }

    return ret;
}

static int at_drx_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
        
    switch(opt) {
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:<Length>,<Payload>\r\nOK\r\n", LORA_AT_DRX);
            break;
        }
        case QUERY_CMD: {
            uint8_t port;
            uint8_t *buf;
            uint8_t size;
                    
            if(lwan_data_recv(&port, &buf, &size) == LWAN_SUCCESS) {
                ret = LWAN_SUCCESS;
                int16_t len = snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:%d", LORA_AT_DRX, size);
                if (size > 0) {
                    len += snprintf((char *)(atcmd + len), ATCMD_SIZE, ",");
                    for(int i=0; i<size; i++) {
                        len += snprintf((char *)(atcmd + len), ATCMD_SIZE, "%02X", buf[i]);
                    }
                }
                snprintf((char *)(atcmd + len), ATCMD_SIZE, "\r\n%s\r\n", "OK");
            }
            break;
        }
        default: break;
    }

    return ret;

}


static int at_cconfirm_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
    uint8_t cfm;
        
    switch(opt) {
        case QUERY_CMD: {
            ret = LWAN_SUCCESS;
            //lwan_mac_config_get(MAC_CONFIG_CONFIRM_MSG, &cfm);
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:%d\r\nOK\r\n", LORA_AT_CCONFIRM, lora_dev_para.confirm);
            break;
        }
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:\"value\"\r\nOK\r\n", LORA_AT_CCONFIRM);
            break;
        }
        case SET_CMD: {
            if(argc < 1) break;
            
            cfm = strtol((const char *)argv[0], NULL, 0);
            if (set_lora_confirm(cfm) == LWAN_SUCCESS) {
                ret = LWAN_SUCCESS;
                snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }
            
            break;
        }
        default: break;
    }

    return ret;
}


static int at_cappport_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
    uint8_t port;
        
    switch(opt) {
        case QUERY_CMD: {
            ret = LWAN_SUCCESS;
            //lwan_mac_config_get(MAC_CONFIG_APP_PORT, &port);
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:%d\r\nOK\r\n", LORA_AT_CAPPPORT, lora_dev_para.port);
            break;
        }
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:\"value\"\r\nOK\r\n", LORA_AT_CAPPPORT);
            break;
        }
        case SET_CMD: {
            if(argc < 1) break;
            port = (uint8_t)strtol((const char *)argv[0], NULL, 0);

            if (set_lora_port(port) == LWAN_SUCCESS) {
                ret = LWAN_SUCCESS;
                snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }
            break;
        }
        default: break;
    }

    return ret;
}


static int at_cdatarate_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
    uint8_t datarate;
        
    switch(opt) {
        case QUERY_CMD: {
            ret = LWAN_SUCCESS;
            //lwan_mac_config_get(MAC_CONFIG_DATARATE, &LoRaMacParams.ChannelsDatarate);
		   // snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:%d\r\nOK\r\n", LORA_AT_CDATARATE, LoRaMacParams.ChannelsDatarate);
           snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:%d\r\nOK\r\n", LORA_AT_CDATARATE, lora_dev_para.datarate);
            break;
        }
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:\"value\"\r\nOK\r\n", LORA_AT_CDATARATE);
            break;
        }
        case SET_CMD: {
            if(argc < 1) break;
            lora_dev_para.datarate = strtol((const char *)argv[0], NULL, 0);
		    //lwan_mac_config_set(MAC_CONFIG_DATARATE, (void *)&datarate);
		    if(lora_dev_para.datarate<0||lora_dev_para.datarate>5)
				{
				  ret = LWAN_ERROR;
				  break;
		    	}
            if (set_lora_para() == LWAN_SUCCESS) {
                ret = LWAN_SUCCESS;
                snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }
            break;
        }
        default: break;
    }

    return ret;
}


static int at_szLight_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
    uint8_t datarate;
        
    switch(opt) {
        case QUERY_CMD: {
            ret = LWAN_SUCCESS;
            //lwan_mac_config_get(MAC_CONFIG_DATARATE, &datarate);
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:%d\r\nOK\r\n", LORA_AT_LIGHT, lora_dev_para.datarate);
            break;
        }
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:\"value\"\r\nOK\r\n", LORA_AT_LIGHT);
            break;
        }
        case SET_CMD: {
            if(argc < 1) break;
            
            lora_dev_para.szlight= strtol((const char *)argv[0], NULL, 0);
            if (set_lora_para() == LWAN_SUCCESS) {
                ret = LWAN_SUCCESS;
                snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }
            break;
        }
        default: break;
    }

    return ret;
}

static int at_crssi_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
        
    switch(opt) {
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s\r\nOK\r\n", LORA_AT_CRSSI);
            break;
        }
        case EXECUTE_CMD: {
            if(!argv[0]) break;
            
            uint8_t argv_len = strlen(argv[0]);
            if(argv_len > 2){
                char freq_str[4];
                int16_t channel_rssi[8];
                strncpy(freq_str, (const char *)argv[0], argv_len-1);
                freq_str[argv_len-1] = '\0';
                uint8_t freq_band = strtol((const char *)freq_str, NULL, 0);
                lwan_dev_rssi_get(freq_band, channel_rssi);
                snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:\r\n0:%d\r\n1:%d\r\n2:%d\r\n3:%d\r\n4:%d\r\n5:%d\r\n6:%d\r\n7:%d\r\nOK\r\n", \
                         LORA_AT_CRSSI, channel_rssi[0], channel_rssi[1], channel_rssi[2], channel_rssi[3], channel_rssi[4], channel_rssi[5],
                         channel_rssi[6], channel_rssi[7]);
                ret = LWAN_SUCCESS;
            }
            
            break;
        }
        default: break;
    }

    return ret;
}

static int at_cnbtrials_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
    uint8_t m_type, value;
        
    switch(opt) {
        case QUERY_CMD: {
            ret = LWAN_SUCCESS;
            lwan_mac_config_get(MAC_CONFIG_CONFIRM_MSG, &m_type);
            if(m_type)
                lwan_mac_config_get(MAC_CONFIG_CONF_NBTRIALS, &value);
            else
                lwan_mac_config_get(MAC_CONFIG_UNCONF_NBTRIALS, &value);
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:%d,%d\r\nOK\r\n", LORA_AT_CNBTRIALS, m_type, value);
            break;
        }
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:\"MTypes\",\"value\"\r\nOK\r\n", LORA_AT_CNBTRIALS);
            break;
        }
        case SET_CMD: {
            if(argc < 2) break;
            
            m_type = strtol((const char *)argv[0], NULL, 0);
            value = strtol((const char *)argv[1], NULL, 0);
            int res = LWAN_SUCCESS;
            if(m_type)
                res = lwan_mac_config_set(MAC_CONFIG_CONF_NBTRIALS, (void *)&value);   
            else
                res = lwan_mac_config_set(MAC_CONFIG_UNCONF_NBTRIALS, (void *)&value);
            if (res == LWAN_SUCCESS) { 
                ret = LWAN_SUCCESS;
                snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }
            break;
        }
        default: break;
    }

    return ret;
}


static int at_crm_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
    uint8_t reportMode;
    uint16_t reportInterval;
        
    switch(opt) {
        case QUERY_CMD: {
            ret = LWAN_SUCCESS;
            lwan_mac_config_get(MAC_CONFIG_REPORT_MODE, &reportMode);
            lwan_mac_config_get(MAC_CONFIG_REPORT_INTERVAL, &reportInterval);
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:%d,%u\r\nOK\r\n", LORA_AT_CRM, reportMode, (unsigned int)reportInterval);
            break;
        }
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:\"reportMode\",\"reportInterval\"\r\nOK\r\n", LORA_AT_CRM);
            break;
        }
        case SET_CMD: {
            if(argc < 2) break;
            
            reportMode = strtol((const char *)argv[0], NULL, 0);
            reportInterval = strtol((const char *)argv[1], NULL, 0);
            lwan_mac_config_set(MAC_CONFIG_REPORT_MODE, (void *)&reportMode);
            if (lwan_mac_config_set(MAC_CONFIG_REPORT_INTERVAL, (void *)&reportInterval) == LWAN_SUCCESS) {
                ret = LWAN_SUCCESS;
                snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }

            break;
        }
        default: break;
    }
    
    return ret;
}

static int at_ctxp_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
    uint8_t tx_power;
        
    switch(opt) {
        case QUERY_CMD: {
            ret = LWAN_SUCCESS;
            //lwan_mac_config_get(MAC_CONFIG_TX_POWER, &tx_power);
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:%d\r\nOK\r\n", LORA_AT_CTXP, lora_dev_para.power);
            break;
        }
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:\"value\"\r\nOK\r\n", LORA_AT_CTXP);
            break;
        }
        case SET_CMD: {
            if(argc < 1) break;
            
            lora_dev_para.power = strtol((const char *)argv[0], NULL, 0);
            if (set_lora_para() == LWAN_SUCCESS) {
                ret = LWAN_SUCCESS;
                snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }
            break;
        }
        default: break;
    }

    return ret;
}

static int at_clinkcheck_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
    uint8_t checkValue;
        
    switch(opt) {
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:\"value\"\r\nOK\r\n", LORA_AT_CLINKCHECK);
            break;
        }
        case SET_CMD: {
            if(argc < 1) break;
            checkValue = strtol((const char *)argv[0], NULL, 0);
            if (lwan_mac_config_set(MAC_CONFIG_CHECK_MODE, (void *)&checkValue) == LWAN_SUCCESS) {
                ret = LWAN_SUCCESS;
                if(checkValue==1)
                    lwan_mac_req_send(MAC_REQ_LINKCHECK, 0);
                snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }
            break;
        }
        default: break;
    }

    return ret;
}

static int at_cadr_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
    uint8_t adr;
        
    switch(opt) {
        case QUERY_CMD: {
            ret = LWAN_SUCCESS;
            lwan_mac_config_get(MAC_CONFIG_ADR_ENABLE, &adr);
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:%d\r\nOK\r\n", LORA_AT_CADR, adr);
            break;
        }
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:\"value\"\r\nOK\r\n", LORA_AT_CADR);
            break;
        }
        case SET_CMD: {
            if(argc < 1) break;
            
            adr = strtol((const char *)argv[0], NULL, 0);
            if (lwan_mac_config_set(MAC_CONFIG_ADR_ENABLE, (void *)&adr) == LWAN_SUCCESS) {
                ret = LWAN_SUCCESS;
                snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }
            break;
        }
        default: break;
    }

    return ret;
}

static int at_crxp_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
    RxParams_t rx_params;
        
    switch(opt) {
        case QUERY_CMD: {
            ret = LWAN_SUCCESS;
            lwan_mac_config_get(MAC_CONFIG_RX_PARAM, &rx_params);  
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:%u,%u,%u\r\nOK\r\n", LORA_AT_CRXP, rx_params.rx1_dr_offset, rx_params.rx2_dr, (unsigned int)rx_params.rx2_freq);            
            break;
        }
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:\"RX1DRoffset\",\"RX2DataRate\",\"RX2Frequency\"\r\nOK\r\n", LORA_AT_CRXP);
            break;
        }
        case SET_CMD: {
            if(argc < 3) break;
            
            uint8_t rx1_dr_offset;
            uint8_t rx2_dr;
            uint32_t rx2_freq;                            
            rx1_dr_offset = strtol((const char *)argv[0], NULL, 0);
            rx2_dr = strtol((const char *)argv[1], NULL, 0);
            rx2_freq = strtol((const char *)argv[2], NULL, 0);
            rx_params.rx1_dr_offset = rx1_dr_offset;
            rx_params.rx2_dr = rx2_dr;
            rx_params.rx2_freq = rx2_freq;
            if (lwan_mac_config_set(MAC_CONFIG_RX_PARAM, (void *)&rx_params) == LWAN_SUCCESS) {
                ret = LWAN_SUCCESS;
                snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }     
            break;
        }
        default: break;
    }
 
    return ret;
}

static int at_crx1delay_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
    uint16_t rx1delay;
        
    switch(opt) {
        case QUERY_CMD: {
            ret = LWAN_SUCCESS;
            lwan_mac_config_get(MAC_CONFIG_RX1_DELAY, &rx1delay);
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:%u\r\nOK\r\n", LORA_AT_CRX1DELAY, (unsigned int)rx1delay);
            break;
        }
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:\"value\"\r\nOK\r\n", LORA_AT_CRX1DELAY);
            break;
        }
        case SET_CMD: {
            if(argc < 1) break;
            
            rx1delay = strtol((const char *)argv[0], NULL, 0);
            if (lwan_mac_config_set(MAC_CONFIG_RX1_DELAY, (void *)&rx1delay) == LWAN_SUCCESS) {
                ret = LWAN_SUCCESS;
                snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }
            break;
        }
        default: break;
    }
 
    return ret;
}

static int at_csave_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
        
    switch(opt) {
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s\r\nOK\r\n", LORA_AT_CSAVE);
            break;
        }
        case EXECUTE_CMD: {
            if (lwan_mac_config_save() == LWAN_SUCCESS) {
                ret = LWAN_SUCCESS;
                snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }
            break;
        }
        default: break;
    }

    return ret;
}

static int at_crestore_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
        
    switch(opt) {
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s\r\nOK\r\n", LORA_AT_CRESTORE);
            break;
        }
        case EXECUTE_CMD: {
            LWanMacConfig_t default_mac_config = LWAN_MAC_CONFIG_DEFAULT;
            if (lwan_mac_config_reset(&default_mac_config) == LWAN_SUCCESS) {
                ret = LWAN_SUCCESS;
                snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }
            break;
        }
        default: break;
    }
 
    return ret;
}

static int at_cpslotinforeq_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
        
    switch(opt) {
        case QUERY_CMD: {
            ret = LWAN_SUCCESS;
            ClassBParam_t classb_param;
            lwan_dev_config_get(DEV_CONFIG_CLASSB_PARAM, &classb_param);
            snprintf((char *)atcmd,ATCMD_SIZE,"\r\n%u\r\nOK\r\n",classb_param.periodicity);
            break;
        }
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:<periodicity>\r\nOK\r\n", LORA_AT_PINGSLOTINFOREQ);
            break;
        }
        case SET_CMD: {
            if(argc < 1) break;
            
            uint8_t periodicityVal = (uint8_t)strtol((const char *)argv[0], NULL, 0);
            if (lwan_mac_req_send(MAC_REQ_PSLOT_INFO, &periodicityVal) == LWAN_SUCCESS) {
                ret = LWAN_SUCCESS;
                snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }
            break;
        }
        default: break;
    }
 
    return ret;
}

static int at_ckeysprotect_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
    uint8_t length;
    uint8_t buf[16];
        
    switch(opt) {
        case QUERY_CMD: {
            ret = LWAN_SUCCESS;
            lwan_dev_keys_get(DEV_KEYS_PKEY, buf);
            bool protected = lwan_is_key_valid(buf, LORA_KEY_LENGTH);
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:%d\r\nOK\r\n", LORA_AT_CKEYSPROTECT, protected);
            break;
        }
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s=<ProtectKey:length is 32>\r\n", LORA_AT_CKEYSPROTECT);
            break;
        }
        case SET_CMD: {
            if(argc < 1) break;
            
            int res = LWAN_SUCCESS;
            length = hex2bin((const char *)argv[0], buf, LORA_KEY_LENGTH);
            if (length == LORA_KEY_LENGTH) {
                res = lwan_dev_keys_set(DEV_KEYS_PKEY, buf);
            } else {
                res = lwan_dev_keys_set(DEV_KEYS_PKEY, g_default_key);
            }
            
            if (res == LWAN_SUCCESS) {
                ret = LWAN_SUCCESS;
                snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }
            break;
        }
        default: break;
    }
    
    return ret;
}

static int at_clpm_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
        
    switch(opt) {
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:\"Mode\"\r\nOK\r\n", LORA_AT_CLPM);
            break;
        }
        case SET_CMD: {
            if(argc < 1) break;
            
            ret = LWAN_SUCCESS;
            int8_t mode = strtol((const char *)argv[0], NULL, 0);
            if (mode) {
                if ((LowPower_GetState() & e_LOW_POWER_UART)) {
                    LowPower_Enable(e_LOW_POWER_UART);
                }
            } else {
                if(!(LowPower_GetState() & e_LOW_POWER_UART)) {
                    LowPower_Disable(e_LOW_POWER_UART);
                }
            }
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            break;
        }
        default: break;
    }
    
    return ret;
}

static int at_cjoin_func(int opt, int argc, char *argv[])
{
    int ret = -1;
    uint8_t bJoin, bAutoJoin;
    uint16_t joinInterval, joinRetryCnt;
        
    switch(opt) {
        case QUERY_CMD: {
            ret = 0;
            JoinSettings_t join_settings;
            lwan_dev_config_get(DEV_CONFIG_JOIN_SETTINGS, &join_settings);
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:%d,%d,%d,%d\r\nOK\r\n", LORA_AT_CJOIN, join_settings.auto_join, 
                     join_settings.auto_join, join_settings.join_interval, join_settings.join_trials);
            break;
        }
        case DESC_CMD: {
            ret = 0;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:<ParaTag1>,[ParaTag2],[ParaTag3],[ParaTag4]\r\nOK\r\n", LORA_AT_CJOIN);
            break;
        }
        case SET_CMD: {
            if(argc < 4) break;
            
            bJoin = strtol((const char *)argv[0], NULL, 0);
            bAutoJoin = strtol((const char *)argv[1], NULL, 0);
            joinInterval = strtol((const char *)argv[2], NULL, 0);
            joinRetryCnt = strtol((const char *)argv[3], NULL, 0);

			snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
			 ret = 0;
            int res = lwan_join(bJoin, bAutoJoin, joinInterval, joinRetryCnt);
            if (res == LWAN_SUCCESS) {
                ret = 0;
               // snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }    
            break;
        }
        default: break;
    }
    
    return ret;
}

static int at_cgmi_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
    
    if(opt == QUERY_CMD) {
        ret = LWAN_SUCCESS;
        snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s=%s\r\nOK\r\n", LORA_AT_CGMI, aos_mft_itf.get_mft_id());
    }
    
    return ret;
}

static int at_cgmm_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
    
    if(opt == QUERY_CMD) {
        ret = LWAN_SUCCESS;
        snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s=%s\r\nOK\r\n", LORA_AT_CGMM, aos_mft_itf.get_mft_model());
    }
    return ret;
}

static int at_cgmr_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
    
    if(opt == QUERY_CMD) {
        ret = LWAN_SUCCESS;
        snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s=%s\r\nOK\r\n", LORA_AT_CGMR, LORAWAN_VER);
    }

    return ret;
}

static int at_cgsn_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
    
    if(opt == QUERY_CMD) {
        ret = LWAN_SUCCESS;
        snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s=%s\r\nOK\r\n", LORA_AT_CGSN, aos_mft_itf.get_mft_sn());
    }
    return ret;
}

static int at_cgbr_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
    uint32_t baud;
        
    switch(opt) {
        case QUERY_CMD: {
            ret = LWAN_SUCCESS;
            baud = lora_dev_para.baud;//aos_mft_itf.get_mft_baud();
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:%u\r\nOK\r\n", LORA_AT_CGBR, (unsigned int)baud);
            break;
        }
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:\"value\"\r\nOK\r\n", LORA_AT_CGBR);
            break;
        }
        case SET_CMD: {
            if(argc < 1) break;
            
            ret = LWAN_SUCCESS;
            lora_dev_para.baud = strtol((const char *)argv[0], NULL, 0);   
            
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            //linkwan_serial_output(atcmd, strlen((const char *)atcmd));
            //atcmd_index = 0;
            //memset(atcmd, 0xff, ATCMD_SIZE);
            //aos_mft_itf.set_mft_baud(baud);
#ifdef AOS_KV
          //  aos_kv_set("sys_baud", &baud, sizeof(baud), true);
#endif             
            break;
        }
        default: break;
    }
    
    return ret;
}

static int at_iloglvl_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
        
    switch(opt) {
        case QUERY_CMD: {
            ret = LWAN_SUCCESS;
            int8_t ll = DBG_LogLevelGet();
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:%d\r\nOK\r\n", LORA_AT_ILOGLVL, ll);
            break;
        }
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:\"level\"\r\nOK\r\n", LORA_AT_ILOGLVL);
            break;
        }
        case SET_CMD: {
            if(argc < 1) break;
         
            ret = LWAN_SUCCESS;
            int8_t ll = strtol((const char *)argv[0], NULL, 0);
            DBG_LogLevelSet(ll);
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            break;
        }
        default: break;
    }
    
    return ret;
}


static int at_Freq_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
    uint8_t freqNum, FreqList;
    uint32_t freq;
	uint8_t res;
	uint8_t freqN = 0;
    uint16_t tempFreq[6] = {0};
  
    switch(opt) {
        case QUERY_CMD: {
            ret = 0;
            //printf("\r\n");
			for( uint8_t i = 0; i < 6; i ++)
	    	{
	    		for( uint8_t j = 0; j < 16; j++ )
	    		{
	    			if( ( lora_dev_para.freqBand[i] & ( 1 << j ) ) != 0 )
	    			{
	    				freq = 470.3e6 + (i * 16 + j) * 200e3;
						printf("\r\nCH[%d]:%u", freqN,(unsigned int)freq);
						freqN++;
	    			}
	    		}
    	    }
			snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            break;
        }
        case DESC_CMD: {
            ret = 0;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:<ParaTag1>,[ParaTag2],[ParaTag3]\r\nOK\r\n", LORA_AT_FREQ);
            break;
        }
        case SET_CMD: {
            if(argc < 3) break;
            
            freqNum = strtol((const char *)argv[0], NULL, 0);
            FreqList = strtol((const char *)argv[1], NULL, 0);
            freq = strtol((const char *)argv[2], NULL, 0);
		    

	    	if(0 == freqNum)
    		{
    			uint16_t tempUse[6] = {0x0000, 0x0000, 0x0000, 0xFF00, 0x0000, 0x0000};
    			//statusPro = 11;
    			memcpy(lora_dev_para.freqBand, tempUse, sizeof(lora_dev_para.freqBand));
    		}

			if(freqNum < 0 && freqNum >= 2)
			{
               return ret;
			}
			
			if(FreqList < 0 && FreqList >= 16)
			{
			   return ret;
			}

			if(freq < 470.3e6 || freq > 489.3e6)
			{
	            return ret;
			}
			else
			{
				uint8_t channel = (uint8_t)((freq - 470.3e6) / 200e3);
				if(1 == freqNum)
				{
					while(FreqList > 0)
					{
						tempFreq[channel / 16] |= (1 << (channel % 16));
					    
						FreqList --;
						channel ++;
					}
					memcpy(lora_dev_para.freqBand,tempFreq,sizeof(lora_dev_para.freqBand));
				}
				else
				{
				    return ret;
				}
			}
			res = set_lora_freq();
            if (res == LWAN_SUCCESS) {
                ret = 0;
                snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }    
            break;
        }
        default: 
			break;
    }
    

    return ret;
}


static int at_Certifi_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
    uint8_t certiti;
        
    switch(opt) {
        case QUERY_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:%d\r\nOK\r\n", LORA_AT_CERTIFI, lora_dev_para.certifi);
            break;
        }
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:\"value\"\r\nOK\r\n", LORA_AT_CERTIFI);
            break;
        }
        case SET_CMD: {
            if(argc < 1) break;
            
            certiti = strtol((const char *)argv[0], NULL, 0);
            if (set_lora_certiti(certiti) == LWAN_SUCCESS) {
                ret = LWAN_SUCCESS;
                snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }
            break;
        }
        default: break;
    }

    return ret;
}


static int at_Debug_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
    uint8_t debug;
        
    switch(opt) {
        case QUERY_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:%d\r\nOK\r\n", LORA_AT_DEBUG, lora_dev_para.debug);
            break;
        }
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:\"value\"\r\nOK\r\n", LORA_AT_DEBUG);
            break;
        }
        case SET_CMD: {
            if(argc < 1) break;
            
            lora_dev_para.debug = strtol((const char *)argv[0], NULL, 0);
            if (set_lora_para() == LWAN_SUCCESS) {
                ret = LWAN_SUCCESS;
                snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            }
            break;
        }
        default: break;
    }

    return ret;
}

static int at_beattime_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
    uint8_t beatflag;
    uint32_t beattime;//单位秒
        
    switch(opt) {
        case QUERY_CMD: {
            ret = LWAN_SUCCESS;
            //lwan_mac_config_get(MAC_CONFIG_APP_PORT, &port);
            beattime=lora_dev_para.Beattime;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:%d,%ds\r\nOK\r\n", LORA_AT_BEATTIME,lora_dev_para.Beatflag,lora_dev_para.Beattime/1000);
            break;
        }
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:\"value,value\"\r\nOK\r\n", LORA_AT_BEATTIME);
            break;
        }
        case SET_CMD: {
            if(argc < 1) break;
            lora_dev_para.Beatflag = (u_int8_t)strtol((const char *)argv[0], NULL, 0);
            beattime = (uint32_t)strtol((const char *)argv[1], NULL, 0);
            lora_dev_para.Beattime=beattime*1000;//输入秒，转化为毫秒
          // if (set_lora_freq() == LWAN_SUCCESS) {
                ret = LWAN_SUCCESS;
                snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
            //}
            break;
        }
        default: break;
    }

    return ret;
}

static int at_ireboot_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
        
    switch(opt) {
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:\"Mode\"\r\nOK\r\n", LORA_AT_IREBOOT);
            break;
        }
        case SET_CMD: {
            if(argc < 1) break;
            
            int8_t mode = strtol((const char *)argv[0], NULL, 0);
            if (mode == 0 || mode == 1 || mode == 7) {
                ret = LWAN_SUCCESS;
                snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
                PRINTF_AT("%s", atcmd);
                aos_lrwan_time_itf.delay_ms(1);
                atcmd_index = 0;
                memset(atcmd, 0xff, ATCMD_SIZE);
                lwan_sys_reboot(mode);
            }
            break;
        }
        default: break;
    }

    return ret;
}

#ifndef CONFIG_ASR650X_TEST_DISABLE
static int at_csleep_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
        
    switch(opt) {
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:\"SleepMode\"\r\nOK\r\n", LORA_AT_CSLEEP);
            break;
        }
        case SET_CMD: {
            if(argc < 1) break;
            
            uint8_t sleep_mode;
            sleep_mode = strtol((const char *)argv[0], NULL, 0);
            if(LoRaTestSleep(sleep_mode)) {
                ret = LWAN_SUCCESS;
                snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s\r\nOK\r\n", LORA_AT_CSLEEP);
            }
            break;
        }
        default: break;
    }
    
    return ret;
}

static int at_cmcu_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
        
    switch(opt) {
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:\"MCUMode\"\r\nOK\r\n", LORA_AT_CMCU);
            break;
        }
        case SET_CMD: {
            if(argc < 1) break;
            
            uint8_t mcu_mode;
            mcu_mode = strtol((const char *)argv[0], NULL, 0);
            if(LoRaTestMcu(mcu_mode)) {
                ret = LWAN_SUCCESS;
                snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s\r\nOK\r\n", LORA_AT_CMCU);
            }
            break;
        }
        default: break;
    }
    
    return ret;
}

static int at_ctxcw_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
        
    switch(opt) {
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:\"Frequency\",\"TxPower\",\"PaOpt\"\r\nOK\r\n", LORA_AT_CTXCW);
            break;
        }
        case SET_CMD: {
            if(argc < 1) break;
            
            uint32_t freq = strtol(argv[0], NULL, 0);
            uint8_t pwr = strtol(argv[1], NULL, 0);
            uint8_t opt = strtol(argv[2], NULL, 0);
            if(LoRaTestTxcw(freq, pwr, opt)) {
                ret = LWAN_SUCCESS;
                snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s\r\nOK\r\n", LORA_AT_CTXCW);
            }
            break;
        }
        default: break;
    }
    
    return ret;
}

static int at_crxs_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
        
    switch(opt) {
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:\"Frequency\",\"DataRate\",\"CodeRate\",\"ldo\"\r\nOK\r\n", LORA_AT_CRXS);
            break;
        }
        case SET_CMD: {
            if(argc < 2) break;
            uint8_t cr = 1;
            uint8_t ldo = 0;
            
            uint32_t freq = strtol(argv[0], NULL, 0);
            uint8_t dr = strtol(argv[1], NULL, 0);
            if(argc>2)
                cr = strtol(argv[2], NULL, 0);
            if(argc>3)
                ldo = strtol(argv[3], NULL, 0);
                   
            if(LoRaTestRxs(freq, dr, cr, ldo)) {
                ret = LWAN_SUCCESS;
                snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s\r\nOK\r\n", LORA_AT_CRXS);
            }
            break;
        }
        default: break;
    }

    return ret;
}

static int at_crx_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
        
    switch(opt) {
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:\"Frequency\",\"DataRate\"\r\nOK\r\n", LORA_AT_CRX);
            break;
        }
        case SET_CMD: {
            if(argc < 2) break;

            uint32_t freq = strtol(argv[0], NULL, 0);
            uint8_t dr = strtol(argv[1], NULL, 0);        
            if(LoRaTestRx(freq, dr)) {
                ret = LWAN_SUCCESS;
                snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s\r\nOK\r\n", LORA_AT_CRX);
            }
            break;
        }
        default: break;
    }

    return ret;
}

static int at_ctx_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
        
    switch(opt) {
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:\"Frequency\",\"DataRate\",\"TxPower\",\"TxLen\"\r\nOK\r\n", LORA_AT_CTX);
            break;
        }
        case SET_CMD: {
            if(argc < 3) break;

            uint8_t len = 0;
            uint32_t freq = strtol(argv[0], NULL, 0);
            uint8_t dr = strtol(argv[1], NULL, 0);  
            uint8_t pwr = strtol(argv[2], NULL, 0);
            if(argc>3)
                len = strtol(argv[3], NULL, 0);
                
            if(LoRaTestTx(freq, dr, pwr, len)) {
                ret = LWAN_SUCCESS;
                snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s\r\nOK\r\n", LORA_AT_CTX);
            }
            break;
        }
        default: break;
    }

    return ret;
}

static int at_cstdby_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
        
    switch(opt) {
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:\"StandbyMode\"\r\nOK\r\n", LORA_AT_CSTDBY);
            break;
        }
        case SET_CMD: {
            if(argc < 1) break;
            
            uint8_t stdby;
            stdby = strtol((const char *)argv[0], NULL, 0);
            if(LoRaTestStdby(stdby)) {
                ret = LWAN_SUCCESS;
                snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s\r\nOK\r\n", LORA_AT_CSTDBY);
            }
            break;
        }
        default: break;
    }
    
    return ret;
}
#endif

static uint8_t concentratorLocal_fcs(const uint8_t *hex_data, uint16_t hex_len)
{
    uint8_t fcs = 0;
    uint16_t i;
    for(i = 0;i < hex_len; i ++)
    {
        fcs += hex_data[i];
    }
    return fcs;
}

static uint32_t SetUartBaud(uint8_t baud)
{
   uint32_t Uart_baud = 0;
   switch(baud)
   {
      case BAUD_38400:
	  	Uart_baud = 38400;
	  	break;
		
	  case BAUD_1200:
	    Uart_baud = 1200;
	    break;
		
	  case BAUD_2400:
	  	Uart_baud = 2400;
	  	break;
		
      case BAUD_4800:
		Uart_baud = 4800;
		break;

      case BAUD_9600:
	    Uart_baud = 9600;
	    break;

	  case BAUD_19200:
	    Uart_baud = 19200;
	    break;


      case BAUD_57600:
	    Uart_baud = 57600;
	    break;

	  case BAUD_115200:
	    Uart_baud = 115200;
	    break;

      default:
	  	  return 0;
	  	break;

   }

   return Uart_baud;
}

static uint32_t ReadUartBaud(uint32_t baud)
{
   uint8_t Uart_baud = 0;
   switch(baud)
   {
      case 38400:
	  	Uart_baud = 0;
	  	break;
		
	  case 1200:
	    Uart_baud = 1;
	    break;
		
	  case 2400:
	  	Uart_baud = 2;
	  	break;
		
      case 4800:
		Uart_baud = 3;
		break;

      case 9600:
	    Uart_baud = 4;
	    break;

	  case 19200:
	    Uart_baud = 5;
	    break;


      case 57600:
	    Uart_baud = 6;
	    break;

	  case 115200:
	    Uart_baud = 7;
	    break;

      default:
	  	  return 0;
	  	break;

   }

   return Uart_baud;
}


static void return_lora_param_info(void)
{
   uint8_t ToolBuf[LORAWAM_MSG_LEN] = {0};
   uint8_t ToolBufLen = 0;
   uint16_t fcs = 0;

   
   ToolBuf[ToolBufLen++] = 0xFA;
   ToolBuf[ToolBufLen++] = 0x00;
   ToolBuf[ToolBufLen++] = 0x05;
   ToolBuf[ToolBufLen++] = 0x05;
   ToolBuf[ToolBufLen++] = 0x25;
   fcs = CRC16Calc(ToolBuf,ToolBufLen);
   ToolBuf[ToolBufLen++] = fcs >> 8;
   ToolBuf[ToolBufLen++] = (uint8_t)fcs ;
   aos_uart_send(ToolBuf,ToolBufLen,3000);
}

static void Read_ModeFactory(void)
{
	uint8_t ToolBuf[LORAWAM_MSG_LEN] = {0};
    uint16_t tempFreq[6] = {0x00FF,0x0000, 0x0000, 0x0000, 0x0000, 0x0000};
	uint8_t ToolBufLen = 0;
	uint16_t fcs = 0;
	uint8_t i = 0;
    uint8_t ul_mode,class_mode,cjoin;
    JoinSettings_t join_settings;
	   
    uint8_t LORAWAN_DEVEUI[8] = {0xdd,0x98,0x92,0x9b,0x92,0xf0,0x9e,0x2d};
    uint8_t LORAWAN_APPKEY[16] = {0xdd,0x98,0x92,0x9b,0x92,0xf0,0x9e,0x2d,0xaf,0x67,0x6d,0x64,0x6d,0x0f,0xff,0xf1};
    uint8_t LORAWAN_APPEUI[8] = {0xdd,0x98,0x92,0x9b,0x92,0xf0,0x9e,0x2d};

	uint8_t  LORAWAN_GROUP_GADDR[4] = {0xFF,0xFF,0xFF,0xFF};
	uint8_t  LORAWAN_GROUP_APPSKEY[16] = {0xdd,0x98,0x92,0x9b,0x92,0xf0,0x9e,0x2d,0xaf,0x67,0x6d,0x64,0x6d,0x0f,0xff,0xf1};
	uint8_t  LORAWAN_GROUP_NWKSKEY[16] = {0xdd,0x98,0x92,0x9b,0x92,0xf0,0x9e,0x2d,0xaf,0x67,0x6d,0x64,0x6d,0x0f,0xff,0xf1};

	ToolBuf[ToolBufLen++] = 0xFA;
	ToolBuf[ToolBufLen++] = 0x00;
	ToolBuf[ToolBufLen++] = 0x04;
	ToolBuf[ToolBufLen++] = 0x05;
	ToolBuf[ToolBufLen++] = 0x23;
    /* module param */
    ToolBuf[ToolBufLen++] = LORAWAN_MODE;
	ToolBuf[ToolBufLen++] = LORAWAN_OTAA;
	
    memcpy(ToolBuf+ToolBufLen,LORAWAN_DEVEUI,8);
	ToolBufLen = ToolBufLen + 8;
	memcpy(ToolBuf+ToolBufLen,LORAWAN_APPKEY,16);
	ToolBufLen = ToolBufLen + 16;
	memcpy(ToolBuf+ToolBufLen,LORAWAN_APPEUI,8);
	ToolBufLen = ToolBufLen + 8;

	memcpy(ToolBuf+ToolBufLen,LORAWAN_GROUP_GADDR,4);
	ToolBufLen = ToolBufLen + 4;
	memcpy(ToolBuf+ToolBufLen,LORAWAN_GROUP_APPSKEY,16);
	ToolBufLen = ToolBufLen + 16;
	memcpy(ToolBuf+ToolBufLen,LORAWAN_GROUP_NWKSKEY,16);
	ToolBufLen = ToolBufLen + 16;
	
	ToolBuf[ToolBufLen++] = 2 - LORAWAN_CALSS;
	ToolBuf[ToolBufLen++] = 0;
	ToolBuf[ToolBufLen++] = DR_0;
	ToolBuf[ToolBufLen++] = 1;
	ToolBuf[ToolBufLen++] = 0;
	ToolBuf[ToolBufLen++] = 1;
	
	ToolBuf[ToolBufLen++] = LORAWAN_FREQ >> 24;
	ToolBuf[ToolBufLen++] = LORAWAN_FREQ >> 16;
	ToolBuf[ToolBufLen++] = LORAWAN_FREQ >> 8;
	ToolBuf[ToolBufLen++] = LORAWAN_FREQ ;
	ToolBuf[ToolBufLen++] = 7 ;
    
    
	memcpy(lora_dev_para_g.lora_group[0].appskey,LORAWAN_GROUP_APPSKEY,16);
	memcpy(lora_dev_para_g.lora_group[0].nwkskey,LORAWAN_GROUP_NWKSKEY,16);

	lora_dev_para_g.lora_group[0].devaddr = LORAWAN_GROUP_GADDR[0] << 24;
    lora_dev_para_g.lora_group[0].devaddr |= LORAWAN_GROUP_GADDR[1] << 16;
    lora_dev_para_g.lora_group[0].devaddr |= LORAWAN_GROUP_GADDR[2] << 8;
    lora_dev_para_g.lora_group[0].devaddr |= LORAWAN_GROUP_GADDR[3]; 
    
    lora_dev_para.baud = 38400;
    lora_dev_para.band = 2;
    lora_dev_para.datarate = 0;
    lora_dev_para.debug = 0;
    lora_dev_para.power = 22;
    memcpy(lora_dev_para.freqBand,tempFreq,sizeof(lora_dev_para.freqBand));
    ul_mode = 2;
    class_mode = 2;
    cjoin = 0;
    join_settings.auto_join = 0;
    
	/*report time and channel*/
	ToolBuf[ToolBufLen++] = 0x04 ;
	ToolBuf[ToolBufLen++] = 0x62 ;
	ToolBuf[ToolBufLen++] = 0xFF ;
	ToolBuf[ToolBufLen++] = 0xFF ;
	/*uart info msag*/
	ToolBuf[ToolBufLen++] = 0;   //38400
	ToolBuf[ToolBufLen++] = 0;
	ToolBuf[ToolBufLen++] = 0;
	ToolBuf[ToolBufLen++] = 0 ;
    /*debug flag */
	ToolBuf[ToolBufLen++] = 0;
	
    ToolBuf[1] = ToolBufLen >> 8;
	ToolBuf[2] = ToolBufLen;
	fcs = CRC16Calc(ToolBuf,ToolBufLen);
	ToolBuf[ToolBufLen++] = fcs >> 8;
	ToolBuf[ToolBufLen++] = (uint8_t)fcs;
	aos_uart_send(ToolBuf,ToolBufLen,3000);
	CyDelay(5);
	//g_lwan_dev_keys_p->ota.appeui = LORAWAN_APPEUI;
 	//g_lwan_dev_keys_p->ota.appkey = LORAWAN_APPKEY;
	//g_lwan_dev_keys_p->ota.deveui = LORAWAN_DEVEUI;
	lwan_dev_keys_set(DEV_KEYS_OTA_DEVEUI,LORAWAN_DEVEUI);
	lwan_dev_keys_set(DEV_KEYS_OTA_APPEUI,LORAWAN_APPEUI);
	lwan_dev_keys_set(DEV_KEYS_OTA_APPKEY,LORAWAN_APPKEY);
            
	lwan_dev_config_set(DEV_CONFIG_JOIN_MODE, (void *)&cjoin);
    lwan_dev_config_set(DEV_CONFIG_ULDL_MODE, (void *)&ul_mode);
	lwan_dev_config_set(DEV_CONFIG_CLASS, (void *)&class_mode);
    lwan_dev_config_set(DEV_CONFIG_JOIN_SETTINGS, &join_settings);

   // lora_dev_para = lora_dev_para_factory; 
	set_lora_para();
    
    set_lora_group();

}
	

static uint8_t Updata_ModuleParam(uint8_t *pbuf , uint8_t len)
{
    uint8_t ToolBufLen = 0;
	uint8_t FreqList = 0;
	uint32_t freq  = 0;
	uint32_t devaddr = 0;
	uint8_t freqNum = 1;
    uint32_t baud = 0;
    uint16_t tempFreq[6] = {0};
	uint8_t joinmode = 0;
	uint8_t ul_mode,class_mode;
    JoinSettings_t join_settings;
	uint8_t cjoin = 0;
	
	if( pbuf[ToolBufLen++] == LORAWAN_MODE )
	{ 
	    cjoin = pbuf[ToolBufLen];
       if(pbuf[ToolBufLen++] == LORAWAN_OTAA)
       {
			memcpy(g_lwan_dev_keys_p->ota.deveui,pbuf+ToolBufLen,8);
			ToolBufLen = ToolBufLen + 8;
			memcpy(g_lwan_dev_keys_p->ota.appkey,pbuf+ToolBufLen,16);
			ToolBufLen = ToolBufLen + 16;
			memcpy(g_lwan_dev_keys_p->ota.appeui,pbuf+ToolBufLen,8);
			ToolBufLen = ToolBufLen + 8;
			lwan_dev_keys_set(DEV_KEYS_OTA_DEVEUI, g_lwan_dev_keys_p->ota.deveui);
			lwan_dev_keys_set(DEV_KEYS_OTA_APPEUI, g_lwan_dev_keys_p->ota.appeui);
			lwan_dev_keys_set(DEV_KEYS_OTA_APPKEY, g_lwan_dev_keys_p->ota.appkey);
			
	   }
	   else 
	   {
		   devaddr = pbuf[ToolBufLen++] << 24;
		   devaddr |= pbuf[ToolBufLen++] << 16;
		   devaddr |= pbuf[ToolBufLen++] << 8;
		   devaddr |= pbuf[ToolBufLen++];
		   
		   memcpy(g_lwan_dev_keys_p->abp.appskey,pbuf+ToolBufLen,16);
		   ToolBufLen = ToolBufLen + 16;
		   memcpy(g_lwan_dev_keys_p->abp.nwkskey,pbuf+ToolBufLen,16);
		   ToolBufLen = ToolBufLen + 16;

		   lwan_dev_keys_set(DEV_KEYS_ABP_DEVADDR, &devaddr);
		   lwan_dev_keys_set(DEV_KEYS_ABP_APPSKEY, g_lwan_dev_keys_p->abp.appskey);
		   lwan_dev_keys_set(DEV_KEYS_ABP_NWKSKEY, g_lwan_dev_keys_p->abp.nwkskey);
	   }



		memcpy(lora_dev_para_g.lora_group[0].appskey,pbuf+ToolBufLen,16);
		ToolBufLen = ToolBufLen + 16;
		memcpy(lora_dev_para_g.lora_group[0].nwkskey,pbuf+ToolBufLen,16);
		ToolBufLen = ToolBufLen + 16;

		lora_dev_para_g.lora_group[0].devaddr = pbuf[ToolBufLen++] << 24;
 	    lora_dev_para_g.lora_group[0].devaddr |= pbuf[ToolBufLen++] << 16;
 	    lora_dev_para_g.lora_group[0].devaddr |= pbuf[ToolBufLen++] << 8;
 	    lora_dev_para_g.lora_group[0].devaddr |= pbuf[ToolBufLen++];
 	    //printf("addr:%u\r\n",lora_dev_para_g.lora_group[0].devaddr);
		//ToolBufLen = ToolBufLen + 4;
		
		class_mode = 2 - pbuf[ToolBufLen++];
		lora_dev_para.power = 22 - pbuf[ToolBufLen++];
		lora_dev_para.datarate = pbuf[ToolBufLen++];
		ul_mode = pbuf[ToolBufLen++] + 1;
		ToolBufLen++;
	    joinmode = pbuf[ToolBufLen++];
		if( joinmode > 2)
		{
           //return 0;
		}
		 
		join_settings.auto_join = joinmode;
		join_settings.join_interval = 8;
		join_settings.join_trials = 8;

		//printf("freq2:%d,%d,%d,%d\r\n",pbuf[ToolBufLen],pbuf[ToolBufLen+1],pbuf[ToolBufLen+2],pbuf[ToolBufLen+3]);
        freq = pbuf[ToolBufLen++] << 24;
		freq |= pbuf[ToolBufLen++] << 16;
		freq |= pbuf[ToolBufLen++] << 8;
		freq |= pbuf[ToolBufLen++] ;
        FreqList = pbuf[ToolBufLen++]+1;
			
	    if(FreqList < 0 && FreqList >= 16)
		{
	
		   return 0;
		}

		if(freq < 470.2e6 || freq > 489.4e6)
		{
		
			return 0;
		}
		else
		{
			uint8_t channel = (uint8_t)((freq - 470.3e6) / 200e3);
			if(1 == freqNum)
			{
				while(FreqList > 0)
				{
					tempFreq[channel / 16] |= (1 << (channel % 16));
					
					FreqList --;
					channel ++;
				}
				memcpy(lora_dev_para.freqBand,tempFreq,sizeof(lora_dev_para.freqBand));
			}
			else
			{
			 
				return 0;
			}
		}
		ToolBufLen = ToolBufLen + 4;
		lora_dev_para.baud= SetUartBaud(pbuf[ToolBufLen]);
		ToolBufLen = ToolBufLen + 4;
		lora_dev_para.debug = pbuf[ToolBufLen];
 
	    //aos_mft_itf.set_mft_baud(lora_dev_para.baud);
        CyDelay(5);
		lwan_dev_config_set(DEV_CONFIG_JOIN_MODE, (void *)&cjoin);
	    lwan_dev_config_set(DEV_CONFIG_ULDL_MODE, (void *)&ul_mode);
		lwan_dev_config_set(DEV_CONFIG_CLASS, (void *)&class_mode);
	    lwan_dev_config_set(DEV_CONFIG_JOIN_SETTINGS, &join_settings);
	    set_lora_freq();
		set_lora_group();
	}
	else
	{
       return 0;
	}
	return 1;
}


static void Read_ModuleParam(void)
{
	uint8_t ToolBuf[LORAWAM_MSG_LEN] = {0};
	uint8_t ToolBufLen = 0;
	uint16_t fcs = 0;
	uint8_t i = 0;
	uint32_t freq = 0;
	uint8_t freqN = 0;
	   
	ToolBuf[ToolBufLen++] = 0xFA;
	ToolBuf[ToolBufLen++] = 0x00;
	ToolBuf[ToolBufLen++] = 0x04;
	ToolBuf[ToolBufLen++] = 0x05;
	ToolBuf[ToolBufLen++] = 0x23;
    /* module param */
    ToolBuf[ToolBufLen++] = LORAWAN_MODE;
	ToolBuf[ToolBufLen++] = g_lwan_dev_config_p->modes.join_mode;
	if(LORAWAN_OTAA == g_lwan_dev_config_p->modes.join_mode)
	{
		memcpy(ToolBuf+ToolBufLen,g_lwan_dev_keys_p->ota.deveui,8);
		ToolBufLen = ToolBufLen + 8;
		memcpy(ToolBuf+ToolBufLen,g_lwan_dev_keys_p->ota.appkey,16);
		ToolBufLen = ToolBufLen + 16;
		memcpy(ToolBuf+ToolBufLen,g_lwan_dev_keys_p->ota.appeui,8);
		ToolBufLen = ToolBufLen + 8;
	}
	else
	{
		ToolBuf[ToolBufLen++] = g_lwan_dev_keys_p->abp.devaddr >> 24;
		ToolBuf[ToolBufLen++] = g_lwan_dev_keys_p->abp.devaddr >> 16;
		ToolBuf[ToolBufLen++] = g_lwan_dev_keys_p->abp.devaddr >> 8;
		ToolBuf[ToolBufLen++] = g_lwan_dev_keys_p->abp.devaddr ;
		
		memcpy(ToolBuf+ToolBufLen,g_lwan_dev_keys_p->abp.appskey,16);
		ToolBufLen = ToolBufLen + 16;
		memcpy(ToolBuf+ToolBufLen,g_lwan_dev_keys_p->abp.nwkskey,16);
		ToolBufLen = ToolBufLen + 16;

	}
	

	//memcpy(ToolBuf+ToolBufLen,lora_dev_para_g.lora_group[0].devaddr,4);
	//ToolBufLen = ToolBufLen + 4;

	
	memcpy(ToolBuf+ToolBufLen,lora_dev_para_g.lora_group[0].appskey,16);
	ToolBufLen = ToolBufLen + 16;
	memcpy(ToolBuf+ToolBufLen,lora_dev_para_g.lora_group[0].nwkskey,16);
	ToolBufLen = ToolBufLen + 16;

	ToolBuf[ToolBufLen++] = lora_dev_para_g.lora_group[0].devaddr >> 24;
	ToolBuf[ToolBufLen++] = lora_dev_para_g.lora_group[0].devaddr >> 16;
	ToolBuf[ToolBufLen++] = lora_dev_para_g.lora_group[0].devaddr >> 8;
	ToolBuf[ToolBufLen++] = lora_dev_para_g.lora_group[0].devaddr ;

	
	ToolBuf[ToolBufLen++] = 2 - g_lwan_dev_config_p->modes.class_mode;
	ToolBuf[ToolBufLen++] = 22 - lora_dev_para.power;
	ToolBuf[ToolBufLen++] = lora_dev_para.datarate;
	ToolBuf[ToolBufLen++] = g_lwan_dev_config_p->modes.uldl_mode -1;
	ToolBuf[ToolBufLen++] = 0;
	ToolBuf[ToolBufLen++] = g_lwan_dev_config_p->join_settings.auto_join;

	for( uint8_t i = 0; i < 6; i ++)
	{
		for( uint8_t j = 0; j < 16; j++ )
		{
			if( ( lora_dev_para.freqBand[i] & ( 1 << j ) ) != 0 )
			{
				freq = 470.3e6 + (i * 16 + j) * 200e3;
				//printf("\r\nCH[%d]:%u", freqN,(unsigned int)freq);
				freqN++;
			}
		}
    }
    if(freqN == 0)
        return ;
	freq = freq - (freqN-1)*200e3;
	ToolBuf[ToolBufLen++] = freq >> 24;
	ToolBuf[ToolBufLen++] = freq >> 16;
	ToolBuf[ToolBufLen++] = freq >> 8;
	ToolBuf[ToolBufLen++] = freq ;
	ToolBuf[ToolBufLen++] = freqN -1;
	
	ToolBuf[ToolBufLen++] = 0x04 ;
	ToolBuf[ToolBufLen++] = 0x62 ;
	ToolBuf[ToolBufLen++] = 0xFF ;
	ToolBuf[ToolBufLen++] = 0xFF ;
	
	ToolBuf[ToolBufLen++] = ReadUartBaud(lora_dev_para.baud);

	ToolBuf[ToolBufLen++] = 0;
    ToolBuf[ToolBufLen++] = 0;
	ToolBuf[ToolBufLen++] = 0;
	
	ToolBuf[ToolBufLen++] = lora_dev_para.debug;
	ToolBuf[1] = ToolBufLen >> 8;
    ToolBuf[2] = (uint8_t )ToolBufLen;
	fcs = CRC16Calc(ToolBuf,ToolBufLen);
	ToolBuf[ToolBufLen++] = fcs >> 8;
	ToolBuf[ToolBufLen++] = (uint8_t)fcs;


	aos_uart_send(ToolBuf,ToolBufLen,3000);
	CyDelay(5);

}

void linkwan_at_process(void)
{
    char *ptr = NULL;
	int argc = 0;
	int index = 0;
	bool ConfigMode = false;
	uint16_t cmd = 0;
	char *argv[ARGC_LIMIT];
    int ret = LWAN_ERROR;
	uint16_t crcdata = 0;
    uint8_t *rxcmd = atcmd + 2;
    int16_t rxcmd_index = atcmd_index - 2;

    if (atcmd_index <=2 && atcmd[atcmd_index] == '\0') {
        //linkwan_at_prompt_print();
        atcmd_index = 0;
        memset(atcmd, 0xff, ATCMD_SIZE);
		
        return;
    }
   if(WorkModeFlag == false)
   {
	    if (rxcmd_index <= 0 || (rxcmd[rxcmd_index] != '\0' && atcmd[0] != LORA_HEAD)) {
	        return;
	    }
        else if( rxcmd_index > 0 && atcmd[0] == LORA_HEAD )
        {
            DelayMs(4);
        }

	    g_atcmd_processing = true;
		

#if 1
       // printf("%d",);
		if(atcmd[0] ==  LORA_HEAD && atcmd[2]  >= LORA_LEN && atcmd[3] == LORA_CMD1 )
		{
		   DelayMs(30);
		   crcdata = CRC16Calc(atcmd,atcmd_index - 2);
		  // printf("crcdata:%d,%d\r\n",crcdata,atcmd_index);
		   if((atcmd[atcmd_index-2] == crcdata >> 8) && (atcmd[atcmd_index-1] == (uint8_t)crcdata))
		   {
		       ConfigMode = true;
			   switch(atcmd[4])
			   {
				  case CMD_READ_INFO:
			         Read_ModuleParam();
					break;
			   
				  case CMD_FACTROY_RESET:
				  	Read_ModeFactory();
					//return_lora_param_info();
					 break;
			   
				  case CMD_SETUPDATA_INFO:
				  	if(atcmd[2] < LORAWAM_MSG_LEN)
				  	{
				  	   if(Updata_ModuleParam(atcmd+5,atcmd_index))
				  	   {
			              return_lora_param_info();
				  	   }
				    }
					break;
			   
				  default:
			           goto at_end;
					   break;
			   }
			   goto at_end;

		   }
		 
		}
	#endif    
	
	    if(atcmd[0] != 'A' || atcmd[1] != 'T')
	        goto at_end;
	    for (index = 0; index < AT_TABLE_SIZE; index++) {
	        int cmd_len = strlen(g_at_table[index].cmd);
	    	if (!strncmp((const char *)rxcmd, g_at_table[index].cmd, cmd_len)) {
	    		ptr = (char *)rxcmd + cmd_len;
	    		break;
	    	}
	    }
		//aos_uart_send(atcmd,atcmd_index,3000);

		if (index >= AT_TABLE_SIZE || !g_at_table[index].fn)
	        goto at_end;
		
		

	    if ((ptr[0] == '?') && (ptr[1] == '\0')) {
			DelayMs(4);
			ret = g_at_table[index].fn(QUERY_CMD, argc, argv);
		} else if (ptr[0] == '\0') {
		    DelayMs(4);
			ret = g_at_table[index].fn(EXECUTE_CMD, argc, argv);
		}  else if (ptr[0] == ' ') {
	        argv[argc++] = ptr;
			DelayMs(4);
			ret = g_at_table[index].fn(EXECUTE_CMD, argc, argv);
		} else if ((ptr[0] == '=') && (ptr[1] == '?') && (ptr[2] == '\0')) {
		   DelayMs(4); 
	        ret = g_at_table[index].fn(DESC_CMD, argc, argv);
		} else if (ptr[0] == '=') {
			ptr += 1;
	        
	        char *str = strtok_l((char *)ptr, ",");
	        while(str) {
	            argv[argc++] = str;
	            str = strtok_l((char *)NULL, ",");
	        }
			DelayMs(4);
			ret = g_at_table[index].fn(SET_CMD, argc, argv);
		} else {
			ret = LWAN_ERROR;
		}


   	}

at_end:

	if(WorkModeFlag == false)
	{
		if (LWAN_ERROR == ret)
		{
		  DelayMs(4);
		  snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s%x\r\n", AT_ERROR, 1);
		}
		if(ConfigMode == false)
		{
			linkwan_serial_output(atcmd, strlen((const char *)atcmd));
		}
		  
	}
    atcmd_index = 0;
    memset(atcmd, 0xff, ATCMD_SIZE);
    g_atcmd_processing = false;        
    return;
}

void linkwan_at_init(void)
{
    atcmd_index = 0;
    memset(atcmd, 0xff, ATCMD_SIZE);
}

static int at_endaddr_func(int opt, int argc, char *argv[])
{
    int ret = LWAN_ERROR;
    uint8_t iniaddr;
    uint16_t endaddr; 
 
    switch(opt) {
        case QUERY_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:DataAddr:%02X,DataLength:%04X\r\nOK\r\n", LORA_AT_ENDADDR,lora_dev_para.Beat_startAddr,lora_dev_para.Beat_endAddr);
            break;
        }
        case DESC_CMD: {
            ret = LWAN_SUCCESS;
            snprintf((char *)atcmd, ATCMD_SIZE, "\r\n%s:\"<DataAddr>,<DataLength>\"\r\nOK\r\n", LORA_AT_ENDADDR);
            break;
        }
        case SET_CMD: {
            if(argc < 1) break;
           lora_dev_para.Beat_startAddr =(uint8_t) strtol((const char *)argv[0], NULL, 0);
           lora_dev_para.Beat_endAddr =(uint16_t) strtol((const char *)argv[1], NULL, 0);
        if(lora_dev_para.Beat_startAddr>lora_dev_para.Beat_startAddr||lora_dev_para.Beat_startAddr>=23){
            int ret = LWAN_ERROR;
            break;
        }
        else
        {    
           if (set_lora_para()==LWAN_SUCCESS) {
                ret = LWAN_SUCCESS;
                snprintf((char *)atcmd, ATCMD_SIZE, "\r\nOK\r\n");
           }
            break;
        }
        }
        default: break;
    }
    
    return ret;
}