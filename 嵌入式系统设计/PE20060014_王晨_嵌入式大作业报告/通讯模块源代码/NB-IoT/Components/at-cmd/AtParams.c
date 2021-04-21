/*************************************************************************
  \file				AtParams.c
  \brief
		Source file of the parameters command interface for AT command module.

  \author
		Johnason
  \internal
  	  Revision History:
  		DEC. 18, 2017		Johnason - Created

**************************************************************************/

/*********************************************************************
 * 							INCLUDES
 */
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include "AtParams.h"
#include "AtProcess.h"
#include "AtTerm.h"
#include "OSAL_nv.h"
#include "ZGlobals.h"

/*********************************************************************
 * 							CONSTANTS
 */
#define ENTER_STRING	"\r\n"

#define MSB_INVERT		1
#define MSB_NORMAL		0

/*********************************************************************
 *							MACROS
 */
/*********************************************************************
 * 						LOCAL FUNCTIONS
 */
static AtProcess_Status atParamWriteDevEui(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadDevEui(char* paramStr, uint32_t pramLen);
#if !defined(SZ_POTOCOL)
static AtProcess_Status atParamReadAppEui(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteAppEui(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadAppKey(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteAppKey(char* paramStr, uint32_t pramLen);
#endif
static AtProcess_Status atParamReadBeatOn(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteBeatOn(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadBeat(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteBeat(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadFreq(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteFreq(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadBand(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteBand(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadOtaa(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteOtaa(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadDevAddr(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteDevAddr(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadNwkSKey(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteNwkSKey(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadAppSKey(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteAppSKey(char* paramStr, uint32_t pramLen);
#if defined(SZ_POTOCOL)
static AtProcess_Status atParamReadDevType(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteDevType(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadRate(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteRate(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadPower(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWritePower(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadPanid(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWritePanid(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteString(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadString(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteBandRate(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadBandRate(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadVer(char* paramStr, uint32_t pramLen);
#endif

AtCommand_t atParamsters[] =
{
        {"DEVEUI=",		atParamWriteDevEui},
		{"DEVEUI?",		atParamReadDevEui},
#if !defined(SZ_POTOCOL)
        {"APPEUI=",		atParamWriteAppEui},
        {"APPEUI?",		atParamReadAppEui},
        {"APPKEY?",		atParamReadAppKey},
        {"APPKEY=",		atParamWriteAppKey},
#endif
        {"BEATON?",		atParamReadBeatOn},
        {"BEATON=",		atParamWriteBeatOn},
        {"BEAT?",		atParamReadBeat},
        {"BEAT=",		atParamWriteBeat},
        {"FREQ?",		atParamReadFreq},
        {"FREQ=",		atParamWriteFreq},
        {"BAND?",		atParamReadBand},
        {"BAND=",		atParamWriteBand},
        {"OTAA?",		atParamReadOtaa},
        {"OTAA=",		atParamWriteOtaa},
        {"DEVADDR?",	atParamReadDevAddr},
        {"DEVADDR=",	atParamWriteDevAddr},
        {"NWKSKEY?",	atParamReadNwkSKey},
        {"NWKSKEY=",	atParamWriteNwkSKey},
        {"APPSKEY?",	atParamReadAppSKey},
        {"APPSKEY=",	atParamWriteAppSKey}
#if defined(SZ_POTOCOL)
        ,
		{"DEVTYPE?",	atParamReadDevType},
        {"DEVTYPE=",	atParamWriteDevType},
		{"DATARATE?",	atParamReadRate},
        {"DATARATE=",	atParamWriteRate},
		{"POWER?",	    atParamReadPower},
        {"POWER=",	    atParamWritePower},
		{"PANID?",		atParamReadPanid},
        {"PANID=",		atParamWritePanid},
		{"DATTYPE?",	atParamReadString},
        {"DATTYPE=",	atParamWriteString},
		{"DEVBAND?",	atParamReadBandRate},
        {"DEVBAND=",	atParamWriteBandRate},
        {"DEVVER?",	    atParamReadVer}
#endif
};

static AtProcess_Status atParam_valid(char* paramStr, uint8_t count)
{
	AtProcess_Status status = AtProcess_Status_Success;
	uint8_t length = strlen(paramStr);
	uint8_t index = 0;

	if(length != (count * 3 - 1) )
	{
		status = AtProcess_Status_ParamError;
	}

	for(index = 2; index < length; index += 3)
	{
		if(' ' != paramStr[index])
		{
			status = AtProcess_Status_ParamError;
			break;
		}
	}
	return status;
}

static AtProcess_Status atParam_scanf(char* paramStr, uint8_t * outStr, uint8_t invertFlag)
{
	AtProcess_Status status = AtProcess_Status_Success;
	char pStr[3];
    uint8_t num = 0;
    uint8_t count = (strlen(paramStr) + 1) / 3;
	while(num < count)
	{
		memcpy(pStr, paramStr + num * 3, 2);
		pStr[2] = '\0';
		if(MSB_NORMAL == invertFlag)
		{
			outStr[num] = (uint8_t)strtol(pStr, NULL, 16);
		}
		else
		{
			outStr[count - num - 1] = (uint8_t)strtol(pStr, NULL, 16);
		}
		num ++;
	}
	return status;
}

static AtProcess_Status atParamWriteDevEui(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;
    uint8_t AtParams_devEui[Z_EUI_LEN] = {0};

    status = atParam_valid(paramStr, Z_EUI_LEN);
    if(AtProcess_Status_Success == status)
    {
    	status = atParam_scanf(paramStr, AtParams_devEui, MSB_INVERT);
        zgSetItem( ZCD_NV_DEVEUI, Z_EUI_LEN, AtParams_devEui );
        BoardDisableIrq();
        osal_nv_write( ZCD_NV_DEVEUI, 0, Z_EUI_LEN, AtParams_devEui );
        BoardEnableIrq();
    }
    return status;
}

static AtProcess_Status atParamReadDevEui(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_MemoryError;
    uint8_t AtParams_devEui[Z_EUI_LEN] = {0};
    uint8_t numAddrBytes = 0;

    numAddrBytes = zgGetItem( ZCD_NV_DEVEUI, Z_EUI_LEN, AtParams_devEui );

    if(SUCCESS == numAddrBytes)
    {
    	for(numAddrBytes = 0; numAddrBytes < Z_EUI_LEN; numAddrBytes ++)
    	{
    		AtTerm_sendStringUi8Value("", AtParams_devEui[Z_EUI_LEN - numAddrBytes - 1], 16);
    	}
    	AtTerm_sendString(ENTER_STRING);
    	status = AtProcess_Status_Success;
    }
    return status;
}
#if !defined(SZ_POTOCOL)
static AtProcess_Status atParamReadAppEui(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_MemoryError;
    uint8_t AtParams_devEui[Z_EUI_LEN] = {0};
    uint8_t numAddrBytes = 0;

    numAddrBytes = zgGetItem( ZCD_NV_APPEUI, Z_EUI_LEN, AtParams_devEui );

    if(SUCCESS == numAddrBytes)
    {
    	for(numAddrBytes = 0; numAddrBytes < Z_EUI_LEN; numAddrBytes ++)
    	{
    		AtTerm_sendStringUi8Value("", AtParams_devEui[Z_EUI_LEN - numAddrBytes - 1], 16);
    	}
    	AtTerm_sendString(ENTER_STRING);
    	status = AtProcess_Status_Success;
    }
    return status;
}

static AtProcess_Status atParamWriteAppEui(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;
    uint8_t AtParams_devEui[Z_EUI_LEN] = {0};

    status = atParam_valid(paramStr, Z_EUI_LEN);
    if(AtProcess_Status_Success == status)
    {
    	status = atParam_scanf(paramStr, AtParams_devEui, MSB_INVERT);
        zgSetItem( ZCD_NV_APPEUI, Z_EUI_LEN, AtParams_devEui );
        BoardDisableIrq();
        osal_nv_write( ZCD_NV_APPEUI, 0, Z_EUI_LEN, AtParams_devEui );
        BoardEnableIrq();
    }
    return status;
}

static AtProcess_Status atParamReadAppKey(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_MemoryError;
    uint8_t AtParams_devEui[Z_KEY_LEN] = {0};
    uint8_t numAddrBytes = 0;

    numAddrBytes = zgGetItem( ZCD_NV_APPKEY, Z_KEY_LEN, AtParams_devEui );

    if(SUCCESS == numAddrBytes)
    {
    	for(numAddrBytes = 0; numAddrBytes < Z_KEY_LEN; numAddrBytes ++)
    	{
    		AtTerm_sendStringUi8Value("", AtParams_devEui[numAddrBytes], 16);
    	}
    	AtTerm_sendString(ENTER_STRING);
    	status = AtProcess_Status_Success;
    }
    return status;
}

static AtProcess_Status atParamWriteAppKey(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;
    uint8_t AtParams_devEui[Z_KEY_LEN] = {0};

    status = atParam_valid(paramStr, Z_KEY_LEN);
    if(AtProcess_Status_Success == status)
    {
    	status = atParam_scanf(paramStr, AtParams_devEui, MSB_NORMAL);
    	zgSetItem( ZCD_NV_APPKEY, Z_KEY_LEN, AtParams_devEui );
    	BoardDisableIrq();
    	osal_nv_write( ZCD_NV_APPKEY, 0, Z_KEY_LEN, AtParams_devEui );
    	BoardEnableIrq();
    }
    return status;
}
#endif
static AtProcess_Status atParamReadBeatOn(char* paramStr, uint32_t pramLen)
{
	AtProcess_Status status = AtProcess_Status_MemoryError;
	uint8_t AtParams_devEui = 0;
	uint8_t numAddrBytes = 0;

	numAddrBytes = zgGetItem( ZCD_NV_BEATON, 1, &AtParams_devEui );

	if(SUCCESS == numAddrBytes)
	{
		AtTerm_sendStringUi8Value("", AtParams_devEui, 16);
		AtTerm_sendString(ENTER_STRING);
		status = AtProcess_Status_Success;
	}
	return status;
}

static AtProcess_Status atParamWriteBeatOn(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;
    uint8_t AtParams_devEui = paramStr[0] - 0x30;

    if(AtParams_devEui <= 1)
    {
    	zgSetItem( ZCD_NV_BEATON, 1, &AtParams_devEui );
    	BoardDisableIrq();
    	osal_nv_write( ZCD_NV_BEATON, 0, 1, &AtParams_devEui );
    	BoardEnableIrq();
    	status = AtProcess_Status_Success;
    }
    return status;
}

static AtProcess_Status atParamReadBeat(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_MemoryError;
    uint8_t AtParams_devEui[Z_BEAT_LEN] = {0};
    uint8_t numAddrBytes = 0;

    numAddrBytes = zgGetItem( ZCD_NV_BEAT, Z_BEAT_LEN, AtParams_devEui );

    if(SUCCESS == numAddrBytes)
    {
    	for(numAddrBytes = 0; numAddrBytes < Z_BEAT_LEN; numAddrBytes ++)
    	{
    		AtTerm_sendStringUi8Value("", AtParams_devEui[numAddrBytes], 16);
    	}
    	AtTerm_sendString(ENTER_STRING);
    	status = AtProcess_Status_Success;
    }
    return status;
}

static AtProcess_Status atParamWriteBeat(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;
    uint8_t AtParams_devEui[Z_BEAT_LEN] = {0};

    status = atParam_valid(paramStr, Z_BEAT_LEN);
    if(AtProcess_Status_Success == status)
    {
    	status = atParam_scanf(paramStr, AtParams_devEui, MSB_NORMAL);
    	zgSetItem( ZCD_NV_BEAT, Z_BEAT_LEN, AtParams_devEui );
    	BoardDisableIrq();
    	osal_nv_write( ZCD_NV_BEAT, 0, Z_BEAT_LEN, AtParams_devEui );
    	BoardEnableIrq();
    }
    return status;
}

static AtProcess_Status atParamReadFreq(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_MemoryError;
    uint16_t tempFreq[Z_FREQ_LEN];
    uint8_t numAddrBytes = 0;
    uint32_t freq;
    uint8_t count = 0;
    char strVal[60] = { 0 };

    numAddrBytes = zgGetItem( ZCD_NV_FREQ, sizeof(tempFreq), (void *)tempFreq );

    if(SUCCESS == numAddrBytes)
    {
    	for(numAddrBytes = 0; numAddrBytes < Z_FREQ_LEN; numAddrBytes ++)
    	{
    		for( uint8_t j = 0; j < 16; j++ )
    		{
    			if( ( tempFreq[numAddrBytes] & ( 1 << j ) ) != 0 )
    			{
    				freq = 470.3e6 + (numAddrBytes * 16 + j) * 200e3;
    				sprintf(strVal, "CH%d: %d", count, (int)freq);
    				AtTerm_sendString(strVal);
    		    	AtTerm_sendString(ENTER_STRING);
    				count ++;
    			}
    		}
    	}
    	status = AtProcess_Status_Success;
    }
    return status;
}

static AtProcess_Status atParamWriteFreq(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;
    uint16_t tempFreq[Z_FREQ_LEN] = {0};
    uint8_t numAddrBytes = 0;
    uint8_t index = 0;
    uint8_t statusPro = 0;
    uint8_t cmd = 0;
    uint8_t freqN = 0;
    uint32_t freq;
    char strVal[30] = { 0 };
    uint8_t count = 0;

    while(numAddrBytes < pramLen)
    {
    	if((' ' == paramStr[numAddrBytes] || '\0' == paramStr[numAddrBytes] ||
    			(numAddrBytes + 1) == pramLen) && numAddrBytes != index)
    	{
    		if((numAddrBytes + 1) == pramLen)
    		{
    			numAddrBytes ++;
    		}
    		memcpy(strVal, paramStr + index, numAddrBytes - index);
    		strVal[numAddrBytes - index] = '\0';
    		switch(statusPro)
    		{
    			case 0:
    				cmd = (uint8_t)strtol(strVal, NULL, 10);
    				if(0 == cmd)
    				{
    					uint16_t tempUse[Z_FREQ_LEN] = SZ_FREQ_DEFAULT;
    					statusPro = 11;
    					memcpy(tempFreq, tempUse, sizeof(tempFreq));
    				}
    				else if(cmd >= 0 && cmd <= 2)
    				{
    					statusPro = 1;
    				}
    				else
    				{
    					// break
    					statusPro = 10;
    				}
    				break;
    			case 1:
    				freqN = (uint8_t)strtol(strVal, NULL, 10);
    				if(freqN > 0 && freqN <= 16)
    				{
    					statusPro = 2;
    				}
    				else
    				{
    					// break
    					statusPro = 10;
    				}
    				break;
    			case 2:
    				freq = (uint32_t)strtol(strVal, NULL, 10);
					if(freq < 470.3e6 || freq > 489.3e6)
					{
    					// break
    					statusPro = 10;
					}
					else
					{
						uint8_t channel = (uint8_t)((freq - 470.3e6) / 200e3);
	    				if(1 == cmd)
	    				{
	    					while(freqN > 0)
	    					{
	    						tempFreq[channel / 16] |= (1 << (channel % 16));
	    						freqN --;
	    						channel ++;
	    					}
	    					// break
	    					statusPro = 11;
	    				}
	    				else if (2 == cmd)
	    				{
	    					tempFreq[channel / 16] |= (1 << (channel % 16));
	    					count ++;
	    					if(count == freqN || count >= 16)
	    					{
	    						statusPro = 11;
	    					}
	    				}
	    				else
	    				{
	    					// break
	    					statusPro = 10;
	    				}
					}
    				break;
    		}
    		index = numAddrBytes + 1;
    	}
        if(10 == statusPro || 11 == statusPro)
        {
        	break;
        }
    	numAddrBytes ++;
    }

    if(11 == statusPro)
    {
		zgSetItem( ZCD_NV_FREQ, sizeof(tempFreq), tempFreq );
		BoardDisableIrq();
    	osal_nv_write( ZCD_NV_FREQ, 0, sizeof(tempFreq), tempFreq );
    	BoardEnableIrq();
		status = AtProcess_Status_Success;
    }

    return status;
}



static AtProcess_Status atParamReadVer(char* paramStr, uint32_t pramLen)
{
	AtTerm_sendString("SZ_LORA_1.0.2\r\n");
}

static AtProcess_Status atParamReadBand(char* paramStr, uint32_t pramLen)
{
	AtProcess_Status status = AtProcess_Status_MemoryError;
	uint8_t AtParams = 0;
	uint8_t numAddrBytes = 0;

	numAddrBytes = zgGetItem( ZCD_NV_FREQ_SAME, 1, &AtParams );

	if(SUCCESS == numAddrBytes)
	{
		AtTerm_sendStringUi8Value("", AtParams, 16);
		AtTerm_sendString(ENTER_STRING);
		status = AtProcess_Status_Success;
	}
	return status;
}

static AtProcess_Status atParamWriteBand(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;
    uint8_t AtParams = paramStr[0] - 0x30;

    if(AtParams <= 1)
    {
    	zgSetItem( ZCD_NV_FREQ_SAME, 1, &AtParams );
    	BoardDisableIrq();
    	osal_nv_write( ZCD_NV_FREQ_SAME, 0, 1, &AtParams );
    	BoardEnableIrq();
    	status = AtProcess_Status_Success;
    }
    return status;
}


static AtProcess_Status atParamWriteBandRate(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;
    uint8_t AtParams = paramStr[0] - 0x30;

    if(AtParams <= 9)
    {
    	zgSetItem( ZCD_NV_BAND, 1, &AtParams );
    	BoardDisableIrq();
    	osal_nv_write( ZCD_NV_BAND, 0, 1, &AtParams );
    	BoardEnableIrq();
    	status = AtProcess_Status_Success;
    }
    return status;
}


static AtProcess_Status atParamReadBandRate(char* paramStr, uint32_t pramLen)
{
	AtProcess_Status status = AtProcess_Status_MemoryError;
	uint8_t AtParams = 0;
	uint8_t numAddrBytes = 0;

	numAddrBytes = zgGetItem( ZCD_NV_BAND, 1, &AtParams );

	if(SUCCESS == numAddrBytes)
	{
		AtTerm_sendStringUi8Value("", AtParams, 16);
		AtTerm_sendString(ENTER_STRING);
		status = AtProcess_Status_Success;
	}
	return status;
}


static AtProcess_Status atParamReadOtaa(char* paramStr, uint32_t pramLen)
{
	AtProcess_Status status = AtProcess_Status_MemoryError;
	uint8_t AtParams = 0;
	uint8_t numAddrBytes = 0;

	numAddrBytes = zgGetItem( ZCD_NV_OTAA, 1, &AtParams );

	if(SUCCESS == numAddrBytes)
	{
		AtTerm_sendStringUi8Value("", AtParams, 16);
		AtTerm_sendString(ENTER_STRING);
		status = AtProcess_Status_Success;
	}
	return status;
}

static AtProcess_Status atParamWriteOtaa(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;
    uint8_t AtParams = paramStr[0] - 0x30;

    if(AtParams <= 1)
    {
    	zgSetItem( ZCD_NV_OTAA, 1, &AtParams );
    	BoardDisableIrq();
    	osal_nv_write( ZCD_NV_OTAA, 0, 1, &AtParams );
    	BoardEnableIrq();
    	status = AtProcess_Status_Success;
    }
    return status;
}

static AtProcess_Status atParamReadDevAddr(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_MemoryError;
    uint8_t numAddrBytes = 0;
    char strVal[60] = { 0 };

    for(numAddrBytes = 0; numAddrBytes < Z_GROUP_NUM + 1; numAddrBytes ++)
    {
    	if(0 == numAddrBytes)
    	{
    		AtTerm_sendString("DEVADDR: ");
    		AtTerm_sendStringUi32Value("", zgDevAddr, 16);
    	    AtTerm_sendString(ENTER_STRING);
    	}
    	else
    	{
    		sprintf(strVal, "GROUP%d ADDR: ", numAddrBytes);
    		AtTerm_sendString(strVal);
    		AtTerm_sendStringUi32Value("", zgGroupParam[numAddrBytes - 1].Address, 16);
    	    AtTerm_sendString(ENTER_STRING);
    	}
    }
    status = AtProcess_Status_Success;
    return status;
}

static AtProcess_Status atParamWriteDevAddr(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;
    uint8_t AtParams_value[5] = {0};
    uint32_t addressT = 0;

    // the 1 byte index + 4 bytes address
    status = atParam_valid(paramStr, 5);
    if(AtProcess_Status_Success == status)
    {
    	status = atParam_scanf(paramStr, AtParams_value, MSB_NORMAL);
    	if(AtParams_value[0] <= Z_GROUP_NUM)
    	{
    		addressT = BUILD_UINT32(AtParams_value[4], AtParams_value[3], AtParams_value[2], AtParams_value[1]);
    		if(0 == AtParams_value[0])
    		{
    			// set the device address
				zgSetItem( ZCD_NV_DEVADDR, sizeof(addressT), &addressT );
				BoardDisableIrq();
				osal_nv_write( ZCD_NV_DEVADDR, 0, sizeof(addressT), &addressT );
				BoardEnableIrq();
    		}
    		else
    		{
    			zgGroupParam[AtParams_value[0] - 1].Address = addressT;
    			// set the group parameters
				zgSetItem( ZCD_NV_GROUP, sizeof(zgGroupParam), zgGroupParam );
				BoardDisableIrq();
				osal_nv_write( ZCD_NV_GROUP, 0, sizeof(zgGroupParam), zgGroupParam );
				BoardEnableIrq();
    		}
    	}
    	else
    	{
    		status = AtProcess_Status_ParamError;
    	}
    }
    return status;
}

static AtProcess_Status atParamReadNwkSKey(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_MemoryError;
    uint8_t numAddrBytes = 0;
    uint8_t index = 0;
    char strVal[60] = { 0 };
    uint8_t * skey = NULL;

    for(numAddrBytes = 0; numAddrBytes < Z_GROUP_NUM + 1; numAddrBytes ++)
    {
    	if(0 == numAddrBytes)
    	{
    		AtTerm_sendString("NWKSKEY: ");
    		skey = zgNwkSKey;
    	}
    	else
    	{
    		sprintf(strVal, "GROUP%d NWKSKEY: ", numAddrBytes);
    		AtTerm_sendString(strVal);
    		skey = zgGroupParam[numAddrBytes - 1].NwkSKey;
    	}
    	for(index = 0; index < Z_KEY_LEN; index ++)
    	{
    		AtTerm_sendStringUi8Value("", skey[index], 16);
    	}
	    AtTerm_sendString(ENTER_STRING);
    }
    status = AtProcess_Status_Success;
    return status;
}

static AtProcess_Status atParamWriteNwkSKey(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;
    uint8_t AtParams_value[Z_KEY_LEN + 1] = {0};

    // the 1 byte index + 4 bytes address
    status = atParam_valid(paramStr, Z_KEY_LEN + 1);
    if(AtProcess_Status_Success == status)
    {
    	status = atParam_scanf(paramStr, AtParams_value, MSB_NORMAL);
    	if(AtParams_value[0] <= Z_GROUP_NUM)
    	{
    		if(0 == AtParams_value[0])
    		{
    			// set the device network session key
            	zgSetItem( ZCD_NV_NWKSKEY, Z_KEY_LEN, AtParams_value + 1 );
            	BoardDisableIrq();
            	osal_nv_write( ZCD_NV_NWKSKEY, 0, Z_KEY_LEN, AtParams_value + 1 );
            	BoardEnableIrq();
    		}
    		else
    		{
    			memcpy(zgGroupParam[AtParams_value[0] - 1].NwkSKey, AtParams_value + 1, Z_KEY_LEN);
    			// set the group parameters
				zgSetItem( ZCD_NV_GROUP, sizeof(zgGroupParam), zgGroupParam );
				BoardDisableIrq();
				osal_nv_write( ZCD_NV_GROUP, 0, sizeof(zgGroupParam), zgGroupParam );
				BoardEnableIrq();
    		}
    	}
    	else
    	{
    		status = AtProcess_Status_ParamError;
    	}
    }
    return status;
}

static AtProcess_Status atParamReadAppSKey(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_MemoryError;
    uint8_t numAddrBytes = 0;
    uint8_t index = 0;
    char strVal[60] = { 0 };
    uint8_t * skey = NULL;

    for(numAddrBytes = 0; numAddrBytes < Z_GROUP_NUM + 1; numAddrBytes ++)
    {
    	if(0 == numAddrBytes)
    	{
    		AtTerm_sendString("APPSKEY: ");
    		skey = zgAppSKey;
    	}
    	else
    	{
    		sprintf(strVal, "GROUP%d APPSKEY: ", numAddrBytes);
    		AtTerm_sendString(strVal);
    		skey = zgGroupParam[numAddrBytes - 1].AppSKey;
    	}
    	for(index = 0; index < Z_KEY_LEN; index ++)
    	{
    		AtTerm_sendStringUi8Value("", skey[index], 16);
    	}
	    AtTerm_sendString(ENTER_STRING);
    }
    status = AtProcess_Status_Success;
    return status;
}

static AtProcess_Status atParamWriteAppSKey(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;
    uint8_t AtParams_value[Z_KEY_LEN + 1] = {0};

    // the 1 byte index + 4 bytes address
    status = atParam_valid(paramStr, Z_KEY_LEN + 1);
    if(AtProcess_Status_Success == status)
    {
    	status = atParam_scanf(paramStr, AtParams_value, MSB_NORMAL);
    	if(AtParams_value[0] <= Z_GROUP_NUM)
    	{
    		if(0 == AtParams_value[0])
    		{
    			// set the device network session key
            	zgSetItem( ZCD_NV_APPSKEY, Z_KEY_LEN, AtParams_value + 1 );
            	BoardDisableIrq();
            	osal_nv_write( ZCD_NV_APPSKEY, 0, Z_KEY_LEN, AtParams_value + 1 );
            	BoardEnableIrq();
    		}
    		else
    		{
    			memcpy(zgGroupParam[AtParams_value[0] - 1].AppSKey, AtParams_value + 1, Z_KEY_LEN);
    			// set the group parameters
				zgSetItem( ZCD_NV_GROUP, sizeof(zgGroupParam), zgGroupParam );
				BoardDisableIrq();
				osal_nv_write( ZCD_NV_GROUP, 0, sizeof(zgGroupParam), zgGroupParam );
				BoardEnableIrq();
    		}
    	}
    	else
    	{
    		status = AtProcess_Status_ParamError;
    	}
    }
    return status;
}

#if defined(SZ_POTOCOL)
static AtProcess_Status atParamReadDevType(char* paramStr, uint32_t pramLen)
{
	AtProcess_Status status = AtProcess_Status_MemoryError;
	uint8_t AtParams = 0;
	uint8_t numAddrBytes = 0;

	numAddrBytes = zgGetItem( ZCD_NV_DEVTYPE, 1, &AtParams );

	if(SUCCESS == numAddrBytes)
	{
		AtTerm_sendStringUi8Value("", AtParams, 16);
		AtTerm_sendString(ENTER_STRING);
		status = AtProcess_Status_Success;
	}
	return status;
}

static AtProcess_Status atParamWriteDevType(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;
    uint8_t AtParams = paramStr[0] - 0x30;

    if(AtParams <= 1)
    {
    	zgSetItem( ZCD_NV_DEVTYPE, 1, &AtParams );
    	BoardDisableIrq();
    	osal_nv_write( ZCD_NV_DEVTYPE, 0, 1, &AtParams );
    	BoardEnableIrq();
    	status = AtProcess_Status_Success;
    }
    return status;
}


static AtProcess_Status atParamReadPower(char* paramStr, uint32_t pramLen)
{
	AtProcess_Status status = AtProcess_Status_MemoryError;
	uint8_t AtParams = 0;
	uint8_t numAddrBytes = 0;

	numAddrBytes = zgGetItem( ZCD_NV_POWER, 1, &AtParams );

	if(SUCCESS == numAddrBytes)
	{
		AtTerm_sendStringUi8Value("", AtParams, 16);
		AtTerm_sendString(ENTER_STRING);
		status = AtProcess_Status_Success;
	}
	return status;
}

static AtProcess_Status atParamWritePower(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;
    uint8_t AtParams = paramStr[0] - 0x30;

    if(AtParams <= 9)
    {
    	zgSetItem( ZCD_NV_POWER, 1, &AtParams );
    	BoardDisableIrq();
    	osal_nv_write( ZCD_NV_POWER, 0, 1, &AtParams );
    	BoardEnableIrq();
    	status = AtProcess_Status_Success;
    }
    return status;
}


static AtProcess_Status atParamReadRate(char* paramStr, uint32_t pramLen)
{
	AtProcess_Status status = AtProcess_Status_MemoryError;
	uint8_t AtParams = 0;
	uint8_t numAddrBytes = 0;

	numAddrBytes = zgGetItem( ZCD_NV_DATARATE, 1, &AtParams );

	if(SUCCESS == numAddrBytes)
	{
		AtTerm_sendStringUi8Value("", AtParams, 16);
		AtTerm_sendString(ENTER_STRING);
		status = AtProcess_Status_Success;
	}
	return status;
}

static AtProcess_Status atParamWriteRate(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;
    uint8_t AtParams = paramStr[0] - 0x30;

    if(AtParams <= 5)
    {
    	zgSetItem( ZCD_NV_DATARATE, 1, &AtParams );
    	BoardDisableIrq();
    	osal_nv_write( ZCD_NV_DATARATE, 0, 1, &AtParams );
    	BoardEnableIrq();
    	status = AtProcess_Status_Success;
    }
    return status;
}

static AtProcess_Status atParamReadPanid(char* paramStr, uint32_t pramLen)
{
	AtProcess_Status status = AtProcess_Status_MemoryError;

    AtTerm_sendString("PANID: ");
    AtTerm_sendStringUi16Value("", zgNwkPanID, 16);
    AtTerm_sendString(ENTER_STRING);

    status = AtProcess_Status_Success;
    return status;
}


static AtProcess_Status atParamReadString(char* paramStr, uint32_t pramLen)
{
	AtProcess_Status status = AtProcess_Status_MemoryError;
	uint8_t AtParams = 0;
	uint8_t numAddrBytes = 0;

	numAddrBytes = zgGetItem( ZCD_NV_DATTYPE, 1, &AtParams );

	if(SUCCESS == numAddrBytes)
	{
		AtTerm_sendStringUi8Value("", AtParams, 16);
		AtTerm_sendString(ENTER_STRING);
		status = AtProcess_Status_Success;
	}
	return status;
}

static AtProcess_Status atParamWriteString(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;
    uint8_t AtParams = paramStr[0] - 0x30;

    if(AtParams <= 1)
    {
    	zgSetItem( ZCD_NV_DATTYPE, 1, &AtParams );
    	BoardDisableIrq();
    	osal_nv_write( ZCD_NV_DATTYPE, 0, 1, &AtParams );
    	BoardEnableIrq();
    	status = AtProcess_Status_Success;
    }
    return status;
}

static AtProcess_Status atParamWritePanid(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;
    uint8_t AtParams_value[2] = {0};
    uint16_t addressT = 0;

    // the 2 bytes pan-id
    status = atParam_valid(paramStr, 2);
    if(AtProcess_Status_Success == status)
    {
    	status = atParam_scanf(paramStr, AtParams_value, MSB_NORMAL);
    	addressT = BUILD_UINT16(AtParams_value[1], AtParams_value[0]);
		zgSetItem( ZCD_NV_NWKPANID, sizeof(addressT), &addressT );
		BoardDisableIrq();
		osal_nv_write( ZCD_NV_NWKPANID, 0, sizeof(addressT), &addressT );
		BoardEnableIrq();
    }
    return status;
}
#endif
/*********************************************************************
 * 						GLOBAL FUNCTIONS
 */

/*
 * @fn          AtParams_parseIncoming
 *
 * @brief       Parses incoming AT parameter commands ("ATP"), these commands
 * 					are of the format "ATPxx=yy"/"ATPxx?" respectively
 *
 * @param       param	-	The string containing the command to be parsed
 *              paramLen	-	The length of the command
 *
 * @return      the result of read
 */
AtProcess_Status AtParams_parseIncoming( char *param, uint8_t paramLen )
{
    uint8_t cmdIdx, paramOffSet;
    AtProcess_Status status = AtProcess_Status_CmdIdError;

    for(cmdIdx = 0; cmdIdx < (sizeof(atParamsters) / sizeof(AtCommand_t )); cmdIdx++)
    {
        if( (paramOffSet = AtProcess_cmdCmp(atParamsters[cmdIdx].cmdStr, param)) && (atParamsters[cmdIdx].cmdFxn != NULL) )
        {
            status = atParamsters[cmdIdx].cmdFxn(&(param[paramOffSet+1]), paramLen - paramOffSet - 1);
            break;
        }
    }

    return status;
}

/*********************************************************************
 *********************************************************************/
