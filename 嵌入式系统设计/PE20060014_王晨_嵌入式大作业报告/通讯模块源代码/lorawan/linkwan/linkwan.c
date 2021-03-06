/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "commissioning.h"
#include "utilities.h"
#include "LoRaMacCrypto.h"
#include "LoRaMac.h"
#include "LoRaMacClassB.h"
#include "timeServer.h"
#include "hw.h"
#include "low_power.h"
#include "radio.h"
#include "linkwan_ica_at.h"
#include <uart_port.h>
#include "hal/soc/flash.h"
#include "hal/soc/uart.h"
#include <aos/aos.h>
#ifdef AOS_KV
#include <assert.h>
#include "kvmgr.h"
#endif
#include "lwan_config.h"  
#include "linkwan.h"
#include "uart_port.h"
#include "lorawan_port.h"

#define MAX_BEACON_RETRY_TIMES 2
#define LORA_KEYS_MAGIC_NUM 0xABABBABA 

#define LORA_DEV_PARA_FLAG  0xEEFF
#define LORA_DEV_PARA_ADDR  0xFFFFFFFB
#define LORA_DEV_PARA_ADDR3  0xFFFFFFFA

#define LORA_DEV_PARA_APPSKEY   {0xdd,0x98,0x92,0x9b,0x92,0xf0,0x9e,0x2d,0xaf,0x67,0x6d,0x64,0x6d,0x0f,0xff,0xf1}
#define LORA_DEV_PARA_NWKSKEY   {0xdd,0x98,0x92,0x9b,0x92,0xf0,0x9e,0x2d,0xaf,0x67,0x6d,0x64,0x6d,0x0f,0xff,0xf1}

#define LORA_DEV_PARA_GFLAG  0xAAFF
#define LORA_DEV_PARA_GADDR  0xFFFFFFEE
#define LORA_DEV_PARA_GAPPSKEY   {0xdd,0x98,0x92,0x9b,0x92,0xf0,0x9e,0x2d,0xaf,0x67,0x6d,0x64,0x6d,0x0f,0xff,0x11}
#define LORA_DEV_PARA_GNWKSKEY   {0xdd,0x98,0x92,0x9b,0x92,0xf0,0x9e,0x2d,0xaf,0x67,0x6d,0x64,0x6d,0x0f,0xff,0x11}
#define LORA_DEV_PARA_MASK {0x00FF,0x0000, 0x0000, 0x0000, 0x0000, 0x0000}

#define WDG_CONFIG        1
#define Default_HeartBeat_Time  3600000

extern uint8_t lorabeat_inquiry_voltage[L_BEAT_LEN];
extern uint8_t lorabeat_inquiry_current[L_BEAT_LEN];
extern uint8_t lorabeat_inquiry_power[L_BEAT_LEN];
extern uint8_t lorabeat[L_BEAT_LEN];

static uint8_t tx_buf[LORAWAN_APP_DATA_BUFF_SIZE];
static lora_AppData_t tx_data = {tx_buf, 1, 10};
static uint8_t rx_buf[LORAWAN_APP_DATA_BUFF_SIZE];
static lora_AppData_t rx_data = {rx_buf, 0, 0};


static LoRaMacPrimitives_t LoRaMacPrimitives;
static LoRaMacCallback_t LoRaMacCallbacks;
static LoRaMainCallback_t *app_callbacks;

static volatile bool next_tx = true;
static volatile bool rejoin_flag = true;

static uint8_t gGatewayID[3] ={0};
static uint8_t g_beacon_retry_times = 0;

static uint32_t g_ack_index = 0;
static uint8_t g_data_send_nbtrials = 0;
static int8_t g_data_send_msg_type = -1;
#ifdef CONFIG_LINKWAN
static uint8_t g_freqband_num = 0;
#endif    

uint16_t gateway_delay = 0;
uint8_t upchannel = 0;
uint8_t sendmsgflag = false;
uint8_t switchfalg = 0;
uint8_t broad_test = false;
TimerEvent_t LedTimer;
TimerEvent_t WdgTimer;
TimerEvent_t txtimer;
TimerEvent_t LorabeatStart;
TimerEvent_t delay_6s;

extern TimerEvent_t MacStateCheckTimer;
extern uint8_t WorkModeFlag;

TimerEvent_t TxNextPacketTimer;
volatile DeviceState_t g_lwan_device_state = DEVICE_STATE_INIT;
static DeviceStatus_t g_lwan_device_status = DEVICE_STATUS_IDLE;

bool g_lora_debug = false;
LWanDevConfig_t *g_lwan_dev_config_p = NULL;
LWanMacConfig_t *g_lwan_mac_config_p = NULL;
LWanDevKeys_t *g_lwan_dev_keys_p = NULL;
LWanProdctConfig_t *g_lwan_prodct_config_p = NULL;

lora_dev_group lora_dev_para_g = {LORA_DEV_PARA_GADDR ,LORA_DEV_PARA_GNWKSKEY,LORA_DEV_PARA_GAPPSKEY,  \
	                                LORA_DEV_PARA_ADDR,LORA_DEV_PARA_GAPPSKEY,LORA_DEV_PARA_GNWKSKEY, \
	                                LORA_DEV_PARA_ADDR3,LORA_DEV_PARA_GNWKSKEY,LORA_DEV_PARA_GNWKSKEY,LORA_DEV_PARA_GFLAG};

lora_dev_para_t lora_dev_para ={19200,22,0,0,2,1,0,10,3,LORA_DEV_PARA_MASK,LORA_DEV_PARA_FLAG,1,3600000,0x00,0x0001};
lora_dev_para_t lora_dev_para_factory = {19200,22,0,0,2,0,0,10,3,LORA_DEV_PARA_MASK,LORA_DEV_PARA_FLAG,1,3600000,0x00,0x0001};

uint8_t Sendheartbeat=true;
uint8_t SendBean = false;
uint8_t Inquiry[L_BEAT_LEN];
uint8_t inquiry_num=0;
uint8_t led_value = 0;

extern void FeedWdt(void);
static void start_dutycycle_timer(void);

#ifdef CONFIG_PROJECT_CAINIAO
static void notify_host()
{
    pin_wakeup_SetDriveMode(pin_wakeup_DM_STRONG);
    pin_wakeup_Write(1);
    CyDelay(5);
    pin_wakeup_Write(0);
}
#else
static void notify_host()
{
}  
#endif    



static void WdgEvent(void)
{
	FeedWdt();
	TimerSetValue( &WdgTimer, 1200 ); 
    TimerStart( &WdgTimer );

}


void stop_join(void)
{
   TimerStop( &MacStateCheckTimer );
   TimerStop(&LedTimer);
   LED_Write(1);
}


static void Led_tollge_put(void)
{
    if(led_value == 1)
	{
       LED_Write(led_value);        
	   led_value = 0;
	}
	else
    {
		LED_Write(led_value); 
		led_value = 1;
	}
}


static void ledEvent(void)
{
	Led_tollge_put();
	TimerSetValue( &LedTimer, 500 ); 
    TimerStart( &LedTimer );

}

uint8_t set_lora_freq(void)
{
//	lora_dev_para.freqBand= freq;
	lora_dev_para.flag = LORA_DEV_PARA_FLAG;
	aos_kv_set("lora_sz", &lora_dev_para, sizeof(lora_dev_para),1);
	return 0;

}

uint8_t set_lora_certiti(uint8_t certifi)
{
	lora_dev_para.certifi= certifi;
	lora_dev_para.flag = LORA_DEV_PARA_FLAG;
	aos_kv_set("lora_sz", &lora_dev_para, sizeof(lora_dev_para),1);
	return 0;

}

uint8_t set_lora_confirm(uint8_t confirm)
{
	lora_dev_para.confirm= confirm;
	lora_dev_para.flag = LORA_DEV_PARA_FLAG;
	aos_kv_set("lora_sz", &lora_dev_para, sizeof(lora_dev_para),1);
	return 0;

}

uint8_t set_lora_group(void)
{
	lora_dev_para_g.flag= LORA_DEV_PARA_GFLAG;
	aos_kv_set("lora_g", &lora_dev_para_g, sizeof(lora_dev_para_g),1);
	return 0;
}


uint8_t set_lora_para(void)
{
	lora_dev_para.flag = LORA_DEV_PARA_FLAG;
	aos_kv_set("lora_sz", &lora_dev_para, sizeof(lora_dev_para),1);
	return 0;
}

uint8_t set_lora_port(uint8_t port)
{
	lora_dev_para.port= port;
	lora_dev_para.flag = LORA_DEV_PARA_FLAG;
	aos_kv_set("lora_sz", &lora_dev_para, sizeof(lora_dev_para),1);
	return 0;

}



uint8_t set_lora_band(uint8_t band)
{
	lora_dev_para.band = band;
	lora_dev_para.flag = LORA_DEV_PARA_FLAG;
	aos_kv_set("lora_sz", &lora_dev_para, sizeof(lora_dev_para),1);
	return 0;

}

static void Command_package(uint8_t code)
{
    uint16_t crc;
    Inquiry[0]=0x01;
    Inquiry[1]=0x03;
    Inquiry[2] = code;
    Inquiry[3] = lora_dev_para.Beat_startAddr;
    Inquiry[4] = lora_dev_para.Beat_endAddr >> 8;
    Inquiry[5] = lora_dev_para.Beat_endAddr;
    crc = CRC16Calc( Inquiry, 6 );
    Inquiry[6] = crc >> 8;
    Inquiry[7] = crc;
}


static bool send_frame(void)
{
    McpsReq_t mcpsReq;
    LoRaMacTxInfo_t txInfo;
    uint8_t send_msg_type;

    if (LoRaMacQueryTxPossible(tx_data.BuffSize, &txInfo) != LORAMAC_STATUS_OK) {
        return true;
    }

    if(g_lwan_mac_config_p->modes.linkcheck_mode == 2) {
        MlmeReq_t mlmeReq;
        mlmeReq.Type = MLME_LINK_CHECK;
        LoRaMacMlmeRequest(&mlmeReq);
    }
    
    send_msg_type = g_data_send_msg_type>=0?g_data_send_msg_type:lora_dev_para.confirm;
    if (send_msg_type == LORAWAN_UNCONFIRMED_MSG) {
        MibRequestConfirm_t mibReq;
#if 0
        mibReq.Type = MIB_CHANNELS_NB_REP;
        mibReq.Param.ChannelNbRep = g_data_send_nbtrials?g_data_send_nbtrials:
                                                    g_lwan_mac_config_p->nbtrials.unconf + 1;
        LoRaMacMibSetRequestConfirm(&mibReq);
#endif
        mcpsReq.Type = MCPS_UNCONFIRMED;
        mcpsReq.Req.Unconfirmed.fPort = lora_dev_para.port;
        mcpsReq.Req.Unconfirmed.fBuffer = UartPayload;
        mcpsReq.Req.Unconfirmed.fBufferSize = UartPayloadLen;
        mcpsReq.Req.Unconfirmed.Datarate = lora_dev_para.datarate;
		
    } else {
        mcpsReq.Type = MCPS_CONFIRMED;
        mcpsReq.Req.Confirmed.fPort = lora_dev_para.port;
        mcpsReq.Req.Confirmed.fBuffer = UartPayload;
        mcpsReq.Req.Confirmed.fBufferSize = UartPayloadLen;
        mcpsReq.Req.Confirmed.NbTrials = g_data_send_nbtrials?g_data_send_nbtrials:
                                                    g_lwan_mac_config_p->nbtrials.conf+1;
        mcpsReq.Req.Confirmed.Datarate = lora_dev_para.datarate; 
    }

    g_data_send_nbtrials = 0;
    g_data_send_msg_type = -1;
	UartPayloadLen = 0;
    
    if (LoRaMacMcpsRequest(&mcpsReq) == LORAMAC_STATUS_OK) {
        return false;
    }

    return true;
}

static void prepare_tx_frame(void)
{
    if (g_lwan_mac_config_p->modes.report_mode == TX_ON_TIMER) {
      //  app_callbacks->LoraTxData(&tx_data);
    }
}


static void send_group_rsp(void)
{
    uint16_t crcdata;
	//UartPayloadLen = L_BEAT_LEN;
	UartPayloadLen = 0;
	UartPayload[UartPayloadLen++] = 0xAF;
	UartPayloadLen += 1;
	UartPayload[UartPayloadLen++] = 0x4F;
	UartPayload[UartPayloadLen++] = 0x4B;
	UartPayload[1] = UartPayloadLen;
	crcdata = CRC16Calc(UartPayload,UartPayloadLen);
	UartPayload[UartPayloadLen++] = (crcdata >> 8 ) & 0xFF;
	UartPayload[UartPayloadLen++] = crcdata & 0xFF;
	send_frame();
}



#ifdef CONFIG_LINKWAN
static uint8_t get_freqband_num(void)
{
    uint8_t num = 0;
    uint16_t mask = g_lwan_dev_config_p->freqband_mask;

    for (uint8_t i = 0; i < 16; i++) {
        if ((mask & (1 << i)) && i != 1) {
            num++;
        }
    }
    return num;
}
static uint8_t get_next_freqband(void)
{
    uint8_t freqband[16];
    uint8_t freqnum = 0;
    uint16_t mask = g_lwan_dev_config_p->freqband_mask;

    freqband[freqnum++] = 1; //1A2
    for (uint8_t i = 0; i < 16; i++) {
        if ((mask & (1 << i)) && i != 1) {
            freqband[freqnum++] = i;
        }
    }
    
    return freqband[randr(0,freqnum-1)];
}
#endif




static void SendEvent(void)
{
    TimerStop(&txtimer);
	if(lora_dev_para.debug){
		printf("sendevent\r\n");
	}


	if(SendBean == true){
        Command_package(inquiry_num);
		UartPayloadLen = L_BEAT_LEN;
		memcpy(UartPayload,Inquiry,L_BEAT_LEN);
		next_tx = send_frame();
		SendBean = false;	
	}else{
	    prepare_tx_frame();
		next_tx = send_frame();
	}
}

static void reset_join_state(void)
{
    g_lwan_device_state = DEVICE_STATE_JOIN;
}
static void on_tx_next_packet_timer_event(void)
{
    MibRequestConfirm_t mib_req;
    LoRaMacStatus_t status;

    TimerStop(&TxNextPacketTimer);
 //   printf("on_tx_next_packet_timer_event\r\n");
    mib_req.Type = MIB_NETWORK_JOINED;
    status = LoRaMacMibGetRequestConfirm(&mib_req);

    if (status == LORAMAC_STATUS_OK) {
		// printf("%d\r\n",mib_req.Param.IsNetworkJoined);
        if (mib_req.Param.IsNetworkJoined == true) {
            g_lwan_device_state = DEVICE_STATE_SEND;
        } else {
            rejoin_flag = true;
            g_lwan_device_state = DEVICE_STATE_JOIN;
        }
    }
}

static void mcps_confirm(McpsConfirm_t *mcpsConfirm)
{
    if (mcpsConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK) {
#ifdef CONFIG_LINKWAN_AT   
        notify_host();
		if(lora_dev_para.debug)
        PRINTF_AT("\r\nOK+SENT:%02X\r\n", mcpsConfirm->NbRetries);
#endif        
    } else {
#ifdef CONFIG_LINKWAN_AT   
        notify_host();
		if(lora_dev_para.debug)
        PRINTF_AT("\r\nERR+SENT:%02X\r\n", mcpsConfirm->NbRetries);
#endif  
        if(mcpsConfirm->McpsRequest ==MCPS_CONFIRMED) {
            if(g_lwan_dev_config_p->modes.join_mode == JOIN_MODE_OTAA) {
                reset_join_state();
#ifdef CONFIG_LINKWAN            
                MibRequestConfirm_t mibReq;
                uint16_t channelsMaskTemp[8] = {0};

                g_lwan_dev_config_p->join_settings.join_method = JOIN_METHOD_DEF;
				#if 0
                for (uint8_t i = 0; i < 16; i++) {
                    if ((g_lwan_dev_config_p->freqband_mask & (1 << i)) != 0) {
                        channelsMaskTemp[i / 2] |= (0xFF << ((i % 2) * 8));
                    }
                }
				#endif
			
                channelsMaskTemp[0] |= 0XFF00;
                mibReq.Type = MIB_CHANNELS_MASK;
                mibReq.Param.ChannelsMask = lora_dev_para.freqBand;
                LoRaMacMibSetRequestConfirm(&mibReq);
#endif    
				if(lora_dev_para.debug)
                DBG_LINKWAN("Not receive Ack,Start to Join...\r\n");
            }else{
#ifdef CONFIG_LINKWAN
                MibRequestConfirm_t mibReq;
                mibReq.Type = MIB_FREQ_BAND;
                mibReq.Param.freqband = get_next_freqband();
                LoRaMacMibSetRequestConfirm(&mibReq);
#endif  
            }
        }
    }
    next_tx = true;
}

static void mcps_indication(McpsIndication_t *mcpsIndication)
{
    uint16_t crcdata = 0;
	uint16_t gatewaynum = 0;
	uint16_t rcnt = 0;
	uint8_t i;

    if ( mcpsIndication->Status != LORAMAC_EVENT_INFO_STATUS_OK ) {
        return;
    }
    // Check Multicast
    // Check Port
    // Check Datarate
    // Check FramePending
    // Check Buffer
    // Check BufferSize
    // Check Rssi
    // Check Snr
    // Check RxSlot
    if(lora_dev_para.debug)
    DBG_PRINTF( "receive data: rssi = %d, snr = %d, datarate = %d\r\n", mcpsIndication->Rssi, mcpsIndication->Snr,
                 mcpsIndication->RxDatarate);
    lwan_dev_status_set(DEVICE_STATUS_SEND_PASS_WITH_DL);
    if (mcpsIndication->RxData == true) {
        switch ( mcpsIndication->Port ) {
            case 224:
                break;
            default: {            
                rx_data.Port = mcpsIndication->Port;
                rx_data.BuffSize = mcpsIndication->BufferSize;
                memcpy1(rx_data.Buff, mcpsIndication->Buffer, rx_data.BuffSize);
                //app_callbacks->LoraRxData(&rx_data);
                
        //process RX data to change TXpower,TXdatarate,heartbeat_time
              if(mcpsIndication->Buffer[0] == 0x5A && mcpsIndication->Buffer[1] == 0x5B)
            {   
                if(mcpsIndication->Buffer[2]==(mcpsIndication->BufferSize-5))
                {
                    if(lora_dev_para.debug){
				    printf("Entering data setting now!\r\n");
				    }
                    crcdata = CRC16Calc(mcpsIndication->Buffer,mcpsIndication->BufferSize-2);
                     if(( mcpsIndication->Buffer[mcpsIndication->BufferSize-2] == (crcdata >> 8))&&(mcpsIndication->Buffer[mcpsIndication->BufferSize-1] == (uint8_t)crcdata))
                         {   
                             lora_dev_para.power=mcpsIndication->Buffer[3];
                             lora_dev_para.datarate=mcpsIndication->Buffer[4];
                             lora_dev_para.Beattime=(mcpsIndication->Buffer[6]+mcpsIndication->Buffer[5]*0x100)*1000;
                             set_lora_para();
                             send_group_rsp();//response to the cloud
                         }
                }
            }

              if(lora_dev_para.szlight)
              {
                if(mcpsIndication->Buffer[0] == 0xBB && mcpsIndication->Buffer[1] == 0xAA)
				{
					crcdata = CRC16Calc(mcpsIndication->Buffer,mcpsIndication->BufferSize-2);
					if(lora_dev_para.debug){
						printf("crcdata:%d,%d,%d,%d\r\n",crcdata>>8,mcpsIndication->Buffer[mcpsIndication->BufferSize-2],(uint8_t)crcdata,mcpsIndication->Buffer[mcpsIndication->BufferSize-1]);
					}
					if(( mcpsIndication->Buffer[mcpsIndication->BufferSize-2] == (crcdata >> 8)) && (mcpsIndication->Buffer[mcpsIndication->BufferSize-1] == (uint8_t)crcdata) )
					{
						lora_dev_para_g.lora_group[2].devaddr= mcpsIndication->Buffer[3] << 24;
						lora_dev_para_g.lora_group[2].devaddr |= mcpsIndication->Buffer[4] << 16;
						lora_dev_para_g.lora_group[2].devaddr |= mcpsIndication->Buffer[5] << 8;
						lora_dev_para_g.lora_group[2].devaddr |= mcpsIndication->Buffer[6];
						memcpy(lora_dev_para_g.lora_group[2].appskey,mcpsIndication->Buffer+7,16);
						//for(i=0;i<16;i++){
							//printf("%02x",lora_dev_para.appskeycast);
						//}
						//printf("\n");
					
						memcpy(lora_dev_para_g.lora_group[2].nwkskey,mcpsIndication->Buffer+23,16);
						//for(i=0;i<16;i++){
						//	printf("%02x",lora_dev_para.nwkskeycast);
						//}
						//printf("\n");
					
						set_lora_para();
						send_group_rsp();
						//CyDelay(2000);
						//HW_Reset(0);
					}

				} else if(mcpsIndication->Buffer[0] == 0x5A && mcpsIndication->Buffer[1] == 0x5A){
					crcdata = CRC16Calc(mcpsIndication->Buffer,mcpsIndication->BufferSize-2);
					if(lora_dev_para.debug){
						printf("crcdata:%d,%d,%d,%d\r\n",crcdata>>8,mcpsIndication->Buffer[mcpsIndication->BufferSize-2],(uint8_t)crcdata,mcpsIndication->Buffer[mcpsIndication->BufferSize-1]);
					}
					if(( mcpsIndication->Buffer[mcpsIndication->BufferSize-2] == (crcdata >> 8)) && (mcpsIndication->Buffer[mcpsIndication->BufferSize-1] == (uint8_t)crcdata) )
					{
						gatewaynum = mcpsIndication->Buffer[3] >> 8;
						gatewaynum |= mcpsIndication->Buffer[4];
						gateway_delay = mcpsIndication->Buffer[5];
						rcnt = g_lwan_dev_keys_p->abp.devaddr % gatewaynum;
						upchannel = rcnt % 8;
						gateway_delay = (rcnt/8)*gateway_delay;
						if(lora_dev_para.debug){
							printf("gatapara:%d,%d,%d,%d,%d\r\n",gatewaynum,rcnt,gateway_delay,upchannel,gateway_delay);
							printf("crc is oK \r\n");
						}
						
					}
					broad_test = true;
					aos_uart_send(mcpsIndication->Buffer+6,mcpsIndication->BufferSize-8,3000);
					//sendmsgflag = true;
					}
				else
				{
					aos_uart_send(mcpsIndication->Buffer,mcpsIndication->BufferSize,3000);
					broad_test = false;

				}
              }
		      else
			  {
                  aos_uart_send(mcpsIndication->Buffer,mcpsIndication->BufferSize,3000);
			  }
                break;
            }
        }
#ifdef CONFIG_DEBUG_LINKWAN
    } else if (mcpsIndication->AckReceived) {
    if(lora_dev_para.debug)
        DBG_LINKWAN( "rx, ACK, index %u\r\n", (unsigned int)g_ack_index++);
#endif
    }
#ifdef CONFIG_LINKWAN_AT   
    uint8_t confirm = 0;
    if(mcpsIndication->McpsIndication==MCPS_UNCONFIRMED)
        confirm = 0;
    else if(mcpsIndication->McpsIndication==MCPS_CONFIRMED)
        confirm = 1;
    uint8_t type = confirm | mcpsIndication->AckReceived<<1 | 
                   mcpsIndication->LinkCheckAnsReceived<<2 | mcpsIndication->DevTimeAnsReceived<<3;
    notify_host();
	if(lora_dev_para.debug)

    PRINTF_AT("\r\nOK+RECV:%02X,%02X,%02X", type, mcpsIndication->Port, mcpsIndication->BufferSize);
	if(lora_dev_para.debug)
	{
	    if(mcpsIndication->BufferSize) {
	        PRINTF_AT(",");
	        for(int i=0; i<mcpsIndication->BufferSize; i++) {
	            PRINTF_AT("%02X", mcpsIndication->Buffer[i]);
	        }
	    }
	    PRINTF_AT("\r\n");
	}
#endif

#ifdef CONFIG_LWAN    
    if(mcpsIndication->UplinkNeeded) {
        g_lwan_device_state = DEVICE_STATE_SEND_MAC;
    }
#endif 
}

static uint32_t generate_rejoin_delay(void)
{
    uint32_t rejoin_delay = 0;

    while (rejoin_delay < g_lwan_dev_config_p->join_settings.join_interval*1000) {
        rejoin_delay += (rand1() % 250);
    }

    return rejoin_delay;
}

static void mlme_confirm( MlmeConfirm_t *mlmeConfirm )
{
    uint32_t rejoin_delay = 8*1000;
    MibRequestConfirm_t mibReq;

    switch ( mlmeConfirm->MlmeRequest ) {
        case MLME_JOIN: {
            if (mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK) {
                // Status is OK, node has joined the network
                TimerStop(&LedTimer);
				LED_Write(1);
                g_lwan_device_state = DEVICE_STATE_JOINED;
                lwan_dev_status_set(DEVICE_STATUS_JOIN_PASS);
#ifdef CONFIG_LINKWAN_AT
                notify_host();
				if(lora_dev_para.debug)
                PRINTF_AT("%s:OK\r\n", LORA_AT_CJOIN);
#endif                
            } else {
                lwan_dev_status_set(DEVICE_STATUS_JOIN_FAIL);
                
#ifdef CONFIG_LINKWAN                
                // Join was not successful. Try to join again
                reset_join_state();
                if (g_lwan_dev_config_p->join_settings.join_method != JOIN_METHOD_SCAN) {
                    g_lwan_dev_config_p->join_settings.join_method = 
                        (g_lwan_dev_config_p->join_settings.join_method + 1) % JOIN_METHOD_NUM;
                    rejoin_delay = generate_rejoin_delay();
                    if (g_lwan_dev_config_p->join_settings.join_method == JOIN_METHOD_SCAN) {
                        g_freqband_num = get_freqband_num();
                    }
                }

                if (g_lwan_dev_config_p->join_settings.join_method == JOIN_METHOD_SCAN) {
                    if (g_freqband_num == 0) {
                        g_lwan_dev_config_p->join_settings.join_method = JOIN_METHOD_DEF;
                        rejoin_delay = 30000;  // 1 hour
#ifdef CONFIG_LINKWAN_AT      
                        notify_host();
						if(lora_dev_para.debug)
                        PRINTF_AT("%s:FAIL\r\n", LORA_AT_CJOIN);
#endif                    
					if(lora_dev_para.debug)
                        DBG_LINKWAN("Wait 30s for new round of scan\r\n");
                    } else {
                        g_freqband_num--;
                        rejoin_delay = generate_rejoin_delay();
                    }
                }
#else
#ifdef CONFIG_LINKWAN_AT    
				if(lora_dev_para.debug)

                PRINTF_AT("%s:FAIL\r\n", LORA_AT_CJOIN);
#endif          
                rejoin_delay = generate_rejoin_delay();
#endif    
                TimerSetValue(&TxNextPacketTimer, rejoin_delay);
                TimerStart(&TxNextPacketTimer);
                rejoin_flag = false;
            }
            break;
        }
        case MLME_LINK_CHECK: {
#ifdef CONFIG_LINKWAN_AT
            notify_host();
			if(lora_dev_para.debug)
            PRINTF_AT("+CLINKCHECK: %d, %d, %d, %d, %d\r\n", mlmeConfirm->Status, mlmeConfirm->DemodMargin, mlmeConfirm->NbGateways, mlmeConfirm->Rssi, mlmeConfirm->Snr);
#endif            
            if ( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK ) {
                // Check DemodMargin
                // Check NbGateways
            } else {
                lwan_dev_status_set(DEVICE_STATUS_NETWORK_ABNORMAL);
            }
            break;
        }
        case MLME_DEVICE_TIME:
        {
            if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK ){
                // Switch to the next state immediately
                g_lwan_device_state = DEVICE_STATE_BEACON_ACQUISITION;
                next_tx = true;
            } else {
                //No device time Ans
                g_lwan_device_state = DEVICE_STATE_SLEEP;
            }
            
            break;
        }
        case MLME_BEACON_ACQUISITION:
        {
            if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK ) {
                //beacon received
                g_lwan_device_state = DEVICE_STATE_REQ_PINGSLOT_ACK;
                g_beacon_retry_times = 0;
            } else {
                //beacon lost
                if(g_beacon_retry_times < MAX_BEACON_RETRY_TIMES) {
                    g_beacon_retry_times ++;
                    g_lwan_device_state = DEVICE_STATE_REQ_DEVICE_TIME;
                } else {
                    g_beacon_retry_times = 0;
                    g_lwan_device_state = DEVICE_STATE_SLEEP;
                }
            }
            break;
        }
        case MLME_PING_SLOT_INFO:
        {
            if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
            {
                mibReq.Type = MIB_DEVICE_CLASS;
                mibReq.Param.Class = CLASS_B;
                LoRaMacMibSetRequestConfirm( &mibReq );
                
                mibReq.Type = MIB_PING_SLOT_DATARATE;
                mibReq.Param.PingSlotDatarate = g_lwan_dev_config_p->classb_param.pslot_dr;
                LoRaMacMibSetRequestConfirm( &mibReq );

                g_lwan_device_state = DEVICE_STATE_SEND;
                next_tx = true;
            }
            else
            {
                g_lwan_device_state = DEVICE_STATE_REQ_PINGSLOT_ACK;
            }
            break;
        }
        default:
            break;
    }
    next_tx = true;
}

static void mlme_indication( MlmeIndication_t *mlmeIndication )
{
    MibRequestConfirm_t mibReq;

    switch( mlmeIndication->MlmeIndication )
    {
        case MLME_SCHEDULE_UPLINK:
        {// The MAC signals that we shall provide an uplink as soon as possible
            g_lwan_device_state = DEVICE_STATE_SEND_MAC;
            break;
        }
        case MLME_BEACON_LOST:
        {
            mibReq.Type = MIB_DEVICE_CLASS;
            mibReq.Param.Class = CLASS_A;
            LoRaMacMibSetRequestConfirm( &mibReq );

            // Switch to class A again
            g_lwan_device_state = DEVICE_STATE_REQ_DEVICE_TIME;
            break;
        }
        case MLME_BEACON:
        {
            if( mlmeIndication->Status == LORAMAC_EVENT_INFO_STATUS_BEACON_LOCKED )
            {
                if(mlmeIndication->BeaconInfo.GwSpecific.InfoDesc==3){ //NetID+GatewayID
                    uint8_t *info = mlmeIndication->BeaconInfo.GwSpecific.Info;
                    if((gGatewayID[0]|gGatewayID[1]|gGatewayID[2]) 
                    && (memcmp(&info[3],gGatewayID,3)!=0)){//GatewayID not 0 and changed
                        //send an uplink in [0:120] seconds
                        TimerStop(&TxNextPacketTimer);
                        TimerSetValue(&TxNextPacketTimer,randr(0,120000));
                        TimerStart(&TxNextPacketTimer);                       
                    }
                    memcpy(gGatewayID,&info[3],3);
                }
            }
            break;
        }
        default:
            break;
    }
}


static void start_dutycycle_timer(void)
{
    MibRequestConfirm_t mib_req;
    LoRaMacStatus_t status;

    TimerStop(&TxNextPacketTimer);
    mib_req.Type = MIB_NETWORK_JOINED;
    status = LoRaMacMibGetRequestConfirm(&mib_req);
    if (status == LORAMAC_STATUS_OK) {
        if (mib_req.Param.IsNetworkJoined == true &&
            g_lwan_mac_config_p->modes.report_mode == TX_ON_TIMER && g_lwan_mac_config_p->report_interval != 0) {
            TimerSetValue(&TxNextPacketTimer, g_lwan_mac_config_p->report_interval*1000);
            TimerStart(&TxNextPacketTimer);
            return;
        }
    }
    if (g_lwan_mac_config_p->report_interval == 0 && g_lwan_mac_config_p->modes.report_mode == TX_ON_TIMER) {
        g_lwan_mac_config_p->modes.report_mode = TX_ON_NONE;
    }
}

MulticastParams_t *get_lora_cur_multicast(void)
{
    MibRequestConfirm_t mib_req;
    LoRaMacStatus_t status;

    mib_req.Type = MIB_MULTICAST_CHANNEL;
    status = LoRaMacMibGetRequestConfirm(&mib_req);
    if (status == LORAMAC_STATUS_OK) {
        return mib_req.Param.MulticastList;
    }
    return NULL;
}

static void print_dev_info(void)
{
    if(g_lwan_dev_config_p->modes.join_mode == JOIN_MODE_OTAA){
        DBG_LINKWAN("OTAA\r\n" );
        DBG_LINKWAN("DevEui= %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X\r\n",
                    g_lwan_dev_keys_p->ota.deveui[0], g_lwan_dev_keys_p->ota.deveui[1], g_lwan_dev_keys_p->ota.deveui[2], g_lwan_dev_keys_p->ota.deveui[3], \
                    g_lwan_dev_keys_p->ota.deveui[4], g_lwan_dev_keys_p->ota.deveui[5], g_lwan_dev_keys_p->ota.deveui[6], g_lwan_dev_keys_p->ota.deveui[7]);
        DBG_LINKWAN("AppEui= %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X\r\n",
                    g_lwan_dev_keys_p->ota.appeui[0], g_lwan_dev_keys_p->ota.appeui[1], g_lwan_dev_keys_p->ota.appeui[2], g_lwan_dev_keys_p->ota.appeui[3], \
                    g_lwan_dev_keys_p->ota.appeui[4], g_lwan_dev_keys_p->ota.appeui[5], g_lwan_dev_keys_p->ota.appeui[6], g_lwan_dev_keys_p->ota.appeui[7]);
        DBG_LINKWAN("AppKey= %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X\r\n",
                    g_lwan_dev_keys_p->ota.appkey[0], g_lwan_dev_keys_p->ota.appkey[1], g_lwan_dev_keys_p->ota.appkey[2], g_lwan_dev_keys_p->ota.appkey[3], \
                    g_lwan_dev_keys_p->ota.appkey[4], g_lwan_dev_keys_p->ota.appkey[5], g_lwan_dev_keys_p->ota.appkey[6], g_lwan_dev_keys_p->ota.appkey[7], \
                    g_lwan_dev_keys_p->ota.appkey[8], g_lwan_dev_keys_p->ota.appkey[9], g_lwan_dev_keys_p->ota.appkey[10], g_lwan_dev_keys_p->ota.appkey[11], \
                    g_lwan_dev_keys_p->ota.appkey[12], g_lwan_dev_keys_p->ota.appkey[13], g_lwan_dev_keys_p->ota.appkey[14], g_lwan_dev_keys_p->ota.appkey[15]);
    } else if(g_lwan_dev_config_p->modes.join_mode == JOIN_MODE_ABP){
        DBG_LINKWAN("ABP\r\n");
        DBG_LINKWAN("DevAddr= %08X\r\n", (unsigned int)g_lwan_dev_keys_p->abp.devaddr);
        DBG_LINKWAN("NwkSKey= ");
        for (int i = 0; i < LORA_KEY_LENGTH; i++) {
            PRINTF_RAW("%02X", g_lwan_dev_keys_p->abp.nwkskey[i]);
        };
        PRINTF_RAW("\r\n");
        DBG_LINKWAN("AppSKey= ");
        for (int i = 0; i < LORA_KEY_LENGTH; i++) {
            PRINTF_RAW("%02X", g_lwan_dev_keys_p->abp.appskey[i]);
        };
        PRINTF_RAW("\r\n");
    }
    DBG_LINKWAN("class type %c\r\n", 'A' + g_lwan_dev_config_p->modes.class_mode);
    DBG_LINKWAN("freq mode %s\r\n", g_lwan_dev_config_p->modes.uldl_mode == ULDL_MODE_INTER ? "inter" : "intra");
    DBG_LINKWAN("scan chn mask 0x%04x\r\n", g_lwan_dev_config_p->freqband_mask);
}

void init_lwan_configs() 
{
    LWanDevKeys_t default_keys = LWAN_DEV_KEYS_DEFAULT;
    LWanDevConfig_t default_dev_config = LWAN_DEV_CONFIG_DEFAULT;
    LWanMacConfig_t default_mac_config = LWAN_MAC_CONFIG_DEFAULT;
    default_mac_config.datarate=lora_dev_para.datarate;
    LWanProdctConfig_t default_prodct_config = LWAN_PRODCT_CONFIG_DEFAULT;
    g_lwan_dev_keys_p = lwan_dev_keys_init(&default_keys);
    g_lwan_dev_config_p = lwan_dev_config_init(&default_dev_config);
    g_lwan_mac_config_p = lwan_mac_config_init(&default_mac_config);
    g_lwan_prodct_config_p = lwan_prodct_config_init(&default_prodct_config);
}


void lora_init(LoRaMainCallback_t *callbacks)
{
    g_lwan_device_state = DEVICE_STATE_INIT;
    app_callbacks = callbacks;

#ifdef AOS_KV
    assert(aos_kv_init() == 0);
#endif

#ifdef CONFIG_LINKWAN_AT
    linkwan_at_init();
#endif
}

static void Check_Next_CMD(void)
{
   if(1 == inquiry_num)
   {
      TimerStop(&delay_6s);
      Command_package( inquiry_num );
      aos_uart_send(Inquiry, L_BEAT_LEN, 3000);
      inquiry_num++;
      TimerSetValue( &delay_6s, lora_dev_para.Beattime/3 );
      TimerStart( &delay_6s ); 
   }
   else if(2 == inquiry_num)
   {
      TimerStop(&delay_6s);
      Command_package( inquiry_num );
      aos_uart_send(Inquiry, L_BEAT_LEN, 3000);
      inquiry_num = 0;
//      TimerSetValue( &delay_6s, 15000 );
//      TimerStart( &delay_6s ); 
   }
}


static void SendBeat(void)
{           
            TimerStop(&LorabeatStart); 
            if(lora_dev_para.Beatflag==true)
            {   
                Command_package( inquiry_num ); 
                aos_uart_send( Inquiry, L_BEAT_LEN, 3000 );
                inquiry_num++;
                TimerSetValue( &delay_6s, lora_dev_para.Beattime/3 );
                TimerStart( &delay_6s );
            }
            TimerSetValue(&LorabeatStart,lora_dev_para.Beattime);
            TimerStart(&LorabeatStart);
            //UartPayloadLen = L_BEAT_LEN;
		    //memcpy(UartPayload,lorabeat,L_BEAT_LEN);
		    //next_tx = send_frame();
}


void lora_fsm( void )
{
    int stopbeat=0;
    int len;
    while (1) {
#ifdef CONFIG_LINKWAN_AT
        linkwan_at_process();
#endif
        if (Radio.IrqProcess != NULL) {
            Radio.IrqProcess();
        }
        
        switch (g_lwan_device_state) {
            case DEVICE_STATE_INIT: { 
#if 1
				len = sizeof(lora_dev_para);
            
				aos_kv_get("lora_sz", &lora_dev_para, &len);
				 if (lora_dev_para.flag == LORA_DEV_PARA_FLAG) 
				 {
					 memcpy(&lora_dev_para, &lora_dev_para, sizeof(lora_dev_para));
				 }	

				 len = sizeof(lora_dev_para_g);
				 aos_kv_get("lora_g", &lora_dev_para_g, &len);
				 if (lora_dev_para_g.flag == LORA_DEV_PARA_FLAG) 
				 {
					 memcpy(&lora_dev_para_g, &lora_dev_para_g, sizeof(lora_dev_para_g));
				 }

				 uint32_t baudrate = lora_dev_para.baud;
                 int len;
              //    printf("baud:%d,%d",baudrate,lora_dev_para.baud);
				  aos_mft_itf.set_mft_baud(baudrate);
				  aos_kv_set("sys_baud", &baudrate, sizeof(baudrate), true);
				  if(aos_kv_get("sys_baud", &baudrate, &len) == 0) {
				      HW_Set_MFT_Baud(baudrate);
				  }

#endif
				
                LoRaMacPrimitives.MacMcpsConfirm = mcps_confirm;
                LoRaMacPrimitives.MacMcpsIndication = mcps_indication;
                LoRaMacPrimitives.MacMlmeConfirm = mlme_confirm;
                LoRaMacPrimitives.MacMlmeIndication = mlme_indication;
                LoRaMacCallbacks.GetBatteryLevel = app_callbacks->BoardGetBatteryLevel;
                LoRaMacCallbacks.GetTemperatureLevel = NULL;

				
				
#if defined(REGION_AS923)
                LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_AS923);
#elif defined(REGION_AU915)
                LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_AU915);
#elif defined(REGION_CN470)
                LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_CN470);
#elif defined(REGION_CN779)
                LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_CN779);
#elif defined(REGION_EU433)
                LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_EU433);
#elif defined(REGION_IN865)
                LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_IN865);
#elif defined(REGION_EU868)
                LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_EU868);
#elif defined(REGION_KR920)
                LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_KR920);
#elif defined(REGION_US915)
                LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_US915);
#elif defined(REGION_US915_HYBRID)
                LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_US915_HYBRID);
#elif defined( REGION_CN470A )
                LoRaMacInitialization( &LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_CN470A);
#else
#error "Please define a region in the compiler options."
#endif
               
                init_lwan_configs();//default parameters have been given here
				//print_dev_info();

                if(lora_dev_para.debug)
                {
                   if(!lwan_is_key_valid(g_lwan_dev_keys_p->pkey, LORA_KEY_LENGTH))
                      print_dev_info();
                }
#ifdef WDT_CONFIG
				TimerInit( &WdgTimer, WdgEvent );

			   //	TimerSetValue( &WdgTimer, 1000 ); 
               // TimerStart( &WdgTimer );
#endif
				TimerInit( &txtimer, SendEvent );
                TimerInit( &TxNextPacketTimer, on_tx_next_packet_timer_event );
				TimerInit( &LedTimer, ledEvent );
				TimerSetValue( &LedTimer, 500 );
                //lorabeat 
		        TimerInit(&LorabeatStart,SendBeat);
                TimerInit(&delay_6s,Check_Next_CMD);

		       
	}
                
				if(g_lwan_dev_config_p->join_settings.auto_join == 1)
				{
				    WorkModeFlag = true;
					if(g_lwan_dev_config_p->modes.join_mode== JOIN_MODE_OTAA){
						TimerStart( &LedTimer );
					}
				}



                lwan_dev_params_update();
                
                if(g_lwan_dev_config_p->modes.join_mode == JOIN_MODE_ABP){
				
                    MibRequestConfirm_t mibReq;
                    mibReq.Type = MIB_NET_ID;
                    mibReq.Param.NetID = LORAWAN_NETWORK_ID;
                    LoRaMacMibSetRequestConfirm(&mibReq);
                    mibReq.Type = MIB_DEV_ADDR;
                    mibReq.Param.DevAddr = g_lwan_dev_keys_p->abp.devaddr;
                    LoRaMacMibSetRequestConfirm(&mibReq); 
					
                    mibReq.Type = MIB_NWK_SKEY;
                    mibReq.Param.NwkSKey = g_lwan_dev_keys_p->abp.nwkskey;
                    LoRaMacMibSetRequestConfirm(&mibReq);
                    mibReq.Type = MIB_APP_SKEY;
                    mibReq.Param.AppSKey = g_lwan_dev_keys_p->abp.appskey;
                    LoRaMacMibSetRequestConfirm(&mibReq);
			
#ifdef CONFIG_LINKWAN                    
                    mibReq.Type = MIB_FREQ_BAND;
                    mibReq.Param.freqband = get_next_freqband();
                    LoRaMacMibSetRequestConfirm(&mibReq);
#endif                    
                    mibReq.Type = MIB_NETWORK_JOINED;
                    mibReq.Param.IsNetworkJoined = true;
                    LoRaMacMibSetRequestConfirm(&mibReq);
            
                    lwan_mac_params_update();
					

					
#ifdef CONFIG_LORA_VERIFY 
                    g_lwan_device_state = DEVICE_STATE_SEND;
#else
                    g_lwan_device_state = DEVICE_STATE_SLEEP;
#endif    
		        }else if(g_lwan_dev_config_p->modes.join_mode == JOIN_MODE_OTAA) {
		   
                    if(g_lwan_dev_config_p->join_settings.auto_join){
					
                        g_lwan_device_state = DEVICE_STATE_JOIN;
                    } else {
                        g_lwan_device_state = DEVICE_STATE_SLEEP;
				
                        linkwan_at_prompt_print();
                    }
                }
                lwan_dev_status_set(DEVICE_STATUS_IDLE);
                break;
            

            case DEVICE_STATE_JOIN: {
                if(g_lwan_dev_config_p->modes.join_mode == JOIN_MODE_OTAA){
                    MlmeReq_t mlmeReq;

                    mlmeReq.Type = MLME_JOIN;
                    mlmeReq.Req.Join.DevEui = g_lwan_dev_keys_p->ota.deveui;
                    mlmeReq.Req.Join.AppEui = g_lwan_dev_keys_p->ota.appeui;
                    mlmeReq.Req.Join.AppKey = g_lwan_dev_keys_p->ota.appkey;
#ifdef CONFIG_LINKWAN    
                    mlmeReq.Req.Join.method = g_lwan_dev_config_p->join_settings.join_method;
                    if (g_lwan_dev_config_p->join_settings.join_method == JOIN_METHOD_STORED) {
                        mlmeReq.Req.Join.freqband = g_lwan_dev_config_p->join_settings.stored_freqband;
                        mlmeReq.Req.Join.datarate = g_lwan_dev_config_p->join_settings.stored_datarate;
                        mlmeReq.Req.Join.NbTrials = 3;
                    } else {
                        mlmeReq.Req.Join.NbTrials = g_lwan_dev_config_p->join_settings.join_trials;
                    }
#else
                    mlmeReq.Req.Join.NbTrials = g_lwan_dev_config_p->join_settings.join_trials;
#endif

                    if (next_tx == true && rejoin_flag == true) {
                        if (LoRaMacMlmeRequest(&mlmeReq) == LORAMAC_STATUS_OK) {
                            next_tx = false;
                        }
#ifdef CONFIG_LINKWAN       
                        if(lora_dev_para.debug)
                        DBG_LINKWAN("Start to Join, method %d, nb_trials:%d\r\n",
                                    g_lwan_dev_config_p->join_settings.join_method, mlmeReq.Req.Join.NbTrials);

#endif                
						if (lora_dev_para.debug)
                        printf("Start to Join, method %d, nb_trials:%d\r\n",
                                    g_lwan_dev_config_p->join_settings.join_method, mlmeReq.Req.Join.NbTrials);
                    }
		        }
                g_lwan_device_state = DEVICE_STATE_SLEEP;
                break;
            }
            case DEVICE_STATE_JOINED: {
				if(lora_dev_para.debug)
                DBG_LINKWAN("Joined\n\r");
#ifdef CONFIG_LINKWAN                
                JoinSettings_t join_settings;
                lwan_dev_config_get(DEV_CONFIG_JOIN_SETTINGS, &join_settings);
                    
                MibRequestConfirm_t mib_req;
                mib_req.Type = MIB_FREQ_BAND;
                LoRaMacMibGetRequestConfirm(&mib_req);
                join_settings.stored_freqband = mib_req.Param.freqband;
                mib_req.Type = MIB_CHANNELS_DATARATE;
                LoRaMacMibGetRequestConfirm(&mib_req);
                join_settings.stored_datarate = mib_req.Param.ChannelsDatarate;
                join_settings.join_method = JOIN_METHOD_STORED;
                
                lwan_dev_config_set(DEV_CONFIG_JOIN_SETTINGS, &join_settings);
#endif                
                
                lwan_mac_params_update();
                
                if(g_lwan_dev_config_p->modes.class_mode == CLASS_B) {
                    g_lwan_device_state = DEVICE_STATE_REQ_DEVICE_TIME;
                }else{
#ifdef CONFIG_LORA_VERIFY                    
                    g_lwan_device_state = DEVICE_STATE_SEND;
#else
                    g_lwan_device_state = DEVICE_STATE_SLEEP;
#endif    
                }
                break;
            }
            case DEVICE_STATE_REQ_DEVICE_TIME: {
                MlmeReq_t mlmeReq;
                MibRequestConfirm_t mib_req;

                mib_req.Type = MIB_NETWORK_JOINED;
                LoRaMacMibGetRequestConfirm(&mib_req);
                if (mib_req.Param.IsNetworkJoined == true) {
                    if( next_tx == true ) {
                        mlmeReq.Type = MLME_DEVICE_TIME;
                        LoRaMacMlmeRequest( &mlmeReq );
                    }
                    g_lwan_device_state = DEVICE_STATE_SEND_MAC;
                } else {
                    g_lwan_device_state = DEVICE_STATE_SLEEP;
                }
                
                break;
            }
            case DEVICE_STATE_BEACON_ACQUISITION: {
                MlmeReq_t mlmeReq;

                if( next_tx == true ) {
                    if(g_lwan_dev_config_p->classb_param.beacon_freq)
                        LoRaMacClassBBeaconFreqReq(g_lwan_dev_config_p->classb_param.beacon_freq);
                    if(g_lwan_dev_config_p->classb_param.pslot_freq)
                        LoRaMacClassBPingSlotChannelReq(g_lwan_dev_config_p->classb_param.pslot_dr, g_lwan_dev_config_p->classb_param.pslot_freq);
                    mlmeReq.Type = MLME_BEACON_ACQUISITION;
                    LoRaMacMlmeRequest( &mlmeReq );
                }
                g_lwan_device_state = DEVICE_STATE_SLEEP;
                break;
            }
            case DEVICE_STATE_REQ_PINGSLOT_ACK: {
                MlmeReq_t mlmeReq;
                if( next_tx == true ) {
                    mlmeReq.Type = MLME_PING_SLOT_INFO;
                    mlmeReq.Req.PingSlotInfo.PingSlot.Fields.Periodicity = g_lwan_dev_config_p->classb_param.periodicity;
                    mlmeReq.Req.PingSlotInfo.PingSlot.Fields.RFU = 0;
                    LoRaMacMlmeRequest( &mlmeReq );
                }
                g_lwan_device_state = DEVICE_STATE_SEND_MAC;
                break;
            }
            case DEVICE_STATE_SEND: {
				if(lora_dev_para.szlight)
				{
					uint32_t datatime = 0;
					uint32_t devaddr;
					extern uint8_t broad_test;
					lwan_dev_keys_get(DEV_KEYS_ABP_DEVADDR, &devaddr);
					datatime = devaddr%120+gateway_delay*2; //devaddr%120+
					
					if(lora_dev_para.debug)
					{
						printf("\r\ndatatime:%d,broad_test:%d\r\n",datatime,broad_test);
					}
					
					if(broad_test == true)
					{
						if(0 == datatime){
						  TimerSetValue( &txtimer, 50 );
						}else{
						 TimerSetValue( &txtimer, datatime*1000 ); /* 500ms */
					  }  
					  broad_test = false;
					}
					else
					{
						TimerSetValue( &txtimer, 50 );
					}
					TimerStart( &txtimer );
                    g_lwan_device_state = DEVICE_STATE_SLEEP;
                    break;          
				}
                if (next_tx == true) { 
                    prepare_tx_frame();
					next_tx = send_frame();
                    CyDelay(1500);
                }
                if(lora_dev_para.Beatflag==true&&stopbeat==0)
                {   
                    stopbeat=1;
					SendBeat();
                }
                if (g_lwan_mac_config_p->modes.report_mode == TX_ON_TIMER) {
                    if(lora_dev_para.debug)
                    printf("entering modes.report_mode == TX_ON_TIMER1227\r\n");
                    start_dutycycle_timer();
                }
                
                g_lwan_device_state = DEVICE_STATE_SLEEP;
                break;
            }
            case DEVICE_STATE_SEND_MAC: {
                if (next_tx == true) {
                    tx_data.BuffSize = 0;
                    next_tx = send_frame();
                }
                g_lwan_device_state = DEVICE_STATE_SLEEP;
                break;
            }
            case DEVICE_STATE_SLEEP: {       
#ifndef LOW_POWER_DISABLE
                LowPower_Handler( );
#endif
                break;
            }
            default: {
                g_lwan_device_state = DEVICE_STATE_INIT;
                break;
            }
        }
    }
}

DeviceState_t lwan_dev_state_get( void )
{
    return g_lwan_device_state;
}

void lwan_dev_state_set(DeviceState_t state)
{
    if (g_lwan_device_state == DEVICE_STATE_SLEEP) {
        TimerStop(&TxNextPacketTimer);
    }
    g_lwan_device_state = state;
}

bool lwan_dev_status_set(DeviceStatus_t ds)
{
    g_lwan_device_status = ds;
    return true;
}
DeviceStatus_t lwan_dev_status_get(void)
{
    return g_lwan_device_status;
}

bool lwan_is_dev_busy()
{
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_MAC_STATE;
    LoRaMacMibGetRequestConfirm(&mibReq);
    
    if(g_lwan_device_state == DEVICE_STATE_SLEEP 
        && mibReq.Param.LoRaMacState == 0)
        return false;
    
    return true;
}

int lwan_mac_req_send(int type, void *param)
{
    MlmeReq_t mlmeReq;
    int ret = LWAN_SUCCESS;
    
    switch(type) {
        case MAC_REQ_LINKCHECK: {
            mlmeReq.Type = MLME_LINK_CHECK;
            break;
        }
        case MAC_REQ_DEVICE_TIME: {
            break;
        }
        case MAC_REQ_PSLOT_INFO: {
            uint8_t periodicity = *(uint8_t *)param;
            if(periodicity>7) 
                return LWAN_ERROR;
            
            mlmeReq.Type = MLME_PING_SLOT_INFO;
            mlmeReq.Req.PingSlotInfo.PingSlot.Fields.Periodicity = periodicity;
            mlmeReq.Req.PingSlotInfo.PingSlot.Fields.RFU = 0;
            
            ClassBParam_t classb_param;
            lwan_dev_config_get(DEV_CONFIG_CLASSB_PARAM, &classb_param);
            classb_param.periodicity = periodicity;
            lwan_dev_config_set(DEV_CONFIG_CLASSB_PARAM, &classb_param);
            break;
        }
        default: {
            ret = LWAN_ERROR;
            break;
        }
    }
    
    if (LoRaMacMlmeRequest(&mlmeReq) == LORAMAC_STATUS_OK) {
        g_lwan_device_state = DEVICE_STATE_SEND_MAC;
    }
    

    return ret;
}

int lwan_join(uint8_t bJoin, uint8_t bAutoJoin, uint16_t joinInterval, uint16_t joinRetryCnt)
{
    int ret = LWAN_SUCCESS;
    JoinSettings_t join_settings;
    LoRaMacStatus_t status ;
    
    lwan_dev_config_get(DEV_CONFIG_JOIN_SETTINGS, &join_settings);
    join_settings.auto_join = bAutoJoin;
    if(joinInterval>=7 && joinInterval<=255)
        join_settings.join_interval = joinInterval;
    if(joinRetryCnt>=1 && joinRetryCnt<=255)
        join_settings.join_trials = joinRetryCnt;
    lwan_dev_config_set(DEV_CONFIG_JOIN_SETTINGS, &join_settings);
        
    if(bJoin == 0){//stop join
        TimerStop(&TxNextPacketTimer);
        MibRequestConfirm_t mib_req;
        LoRaMacStatus_t status;
        mib_req.Type = MIB_NETWORK_JOINED;
        mib_req.Param.IsNetworkJoined = false;
        status = LoRaMacMibSetRequestConfirm(&mib_req);

        if (status != LORAMAC_STATUS_OK)
            return LWAN_ERROR;
        g_lwan_device_state = DEVICE_STATE_SLEEP;
        rejoin_flag = bAutoJoin;
    } else if(bJoin == 1){
        MibRequestConfirm_t mib_req;
        mib_req.Type = MIB_NETWORK_JOINED;
		TimerSetValue( &LedTimer, 500 ); /* 500ms */
        TimerStart( &LedTimer );
       // LoRaMacStatus_t status = LoRaMacMibGetRequestConfirm(&mib_req);
      //  if (status != LORAMAC_STATUS_OK) 
			//printf("return 1\r\n");
        //    return LWAN_ERROR;
        
        if (mib_req.Param.IsNetworkJoined == true) {
            mib_req.Type = MIB_NETWORK_JOINED;
            mib_req.Param.IsNetworkJoined = false;
            status = LoRaMacMibSetRequestConfirm(&mib_req);
            if(status  != LORAMAC_STATUS_OK) {
				//printf("return 2\r\n");
                return LWAN_ERROR;
            }
			//if (lora_dev_para.debug)
               //printf("Rejoin again...\r");
        }
        
        TimerStop(&TxNextPacketTimer);   
        rejoin_flag = true;
        //reset_join_state();
    } else{
        ret = LWAN_ERROR;
    }
    
    return ret;
}

int lwan_data_send(uint8_t confirm, uint8_t Nbtrials, uint8_t *payload, uint8_t len)
{
    MibRequestConfirm_t mib_req;
    LoRaMacStatus_t status;

    TimerStop(&TxNextPacketTimer);

    mib_req.Type = MIB_NETWORK_JOINED;
    status = LoRaMacMibGetRequestConfirm(&mib_req);
    if (status == LORAMAC_STATUS_OK) {
        if (mib_req.Param.IsNetworkJoined == true) {
            g_data_send_msg_type = confirm;
            memcpy(tx_data.Buff, payload, len);
            tx_data.BuffSize = len;
            g_data_send_nbtrials = Nbtrials;
            g_lwan_device_state = DEVICE_STATE_SEND;
            return LWAN_SUCCESS;
        }
    }
    return LWAN_ERROR;
}

int lwan_data_recv(uint8_t *port, uint8_t **payload, uint8_t *size)
{
    if(!port || !payload || !size)
        return LWAN_ERROR;
    *port = rx_data.Port;
    *size = rx_data.BuffSize;
    *payload = rx_data.Buff;
    
    rx_data.BuffSize = 0;
    return LWAN_SUCCESS;
}

uint8_t lwan_dev_battery_get()
{
    return app_callbacks->BoardGetBatteryLevel();
}

int lwan_dev_rssi_get(uint8_t band, int16_t *channel_rssi)
{
    //CN470A Only
    uint8_t FreqBandStartChannelNum[16] = {0, 8, 16, 24, 100, 108, 116, 124, 68, 76, 84, 92, 166, 174, 182, 190};
    if(band>=16) 
        return LWAN_ERROR;

    Radio.SetModem(MODEM_LORA);
    for (uint8_t i = 0; i < 8; i++) {
        uint32_t freq = 470300000 + (FreqBandStartChannelNum[band] + i) * 200000;
        
        Radio.SetChannel(freq);
        Radio.Rx( 0 );
        CyDelay(3);
        
        channel_rssi[i] = Radio.Rssi(MODEM_LORA);
    }
    
    Radio.Sleep();
    
    return LWAN_SUCCESS;
}


bool lwan_multicast_add(void *multicastInfo )
{
    MibRequestConfirm_t mibset;
    LoRaMacStatus_t status;
    MulticastParams_t *pmulticastInfo;

    pmulticastInfo = aos_malloc(sizeof(MulticastParams_t));
    if (!pmulticastInfo)
        return false;
    memcpy(pmulticastInfo, multicastInfo, sizeof(MulticastParams_t));
    mibset.Type = MIB_MULTICAST_CHANNEL;
    mibset.Param.MulticastList = pmulticastInfo;
    status = LoRaMacMibSetRequestConfirm(&mibset);
    if (status !=  LORAMAC_STATUS_OK) {
        return false;
    }
    return true;
}

bool lwan_multicast_del(uint32_t dev_addr)
{
    MulticastParams_t *multiCastNode = get_lora_cur_multicast();
    if (multiCastNode == NULL) {
        return false;
    }
    
    while (multiCastNode != NULL) {
        if (dev_addr == multiCastNode->Address) {
            MibRequestConfirm_t mibset;
            LoRaMacStatus_t status;
            mibset.Type = MIB_MULTICAST_CHANNEL_DEL;
            mibset.Param.MulticastList = multiCastNode;
            status = LoRaMacMibSetRequestConfirm(&mibset);
            if (status !=  LORAMAC_STATUS_OK) {
                return false;
            } else {
                aos_free(mibset.Param.MulticastList);
                return true;
            }
        }
        multiCastNode = multiCastNode->Next;

    }
    return false;
}

uint8_t lwan_multicast_num_get(void)
{
    MulticastParams_t *multiCastNode = get_lora_cur_multicast();
    if (multiCastNode == NULL) {
        return 0;
    }
    uint8_t num = 0;
    while (multiCastNode != NULL) {
        num++;
        multiCastNode = multiCastNode->Next;
    }
    return num;
}

void lwan_sys_reboot(int8_t mode)
{
    if (mode == 0) {
	    HW_Reset(0);
    } else if (mode == 1) {
        if (next_tx == true) {
            prepare_tx_frame();
            next_tx = send_frame();
            HW_Reset(0);
        }
    } else if (mode == 7) {
        HW_Reset(1);
    }
}
