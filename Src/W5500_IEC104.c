#include "main.h"
#include "Settings.h"
#include "Inputs.h"
#include "cmsis_os.h"
#include "Iec104.h"
#include "usbd_cdc_if.h"
#include "math.h"
#include "Events.h"
#include "Protection.h"

#define IEC104_SOCKET     1

static int Linux_init(void)
{
	return 0;
}

void delay_ms(uint16_t ms)
{
	osDelay(ms);
}

static void *Linux_Malloc(unsigned char NumByte)
{
	return malloc(NumByte);
}

static void Linux_Free(void *pblock)
{
	free(pblock);
}

static void Linux_CloseLink(void)
{
//	GPRSFlag = SYSTEM_FLAG_GPRS_CLOSE;
	IEC104_STATE_FLAG_INIT = IEC104_FLAG_CLOSED;
}

uint8_t Linuxsend(int socketfd,char *buf, int len)
{
//	write(socketfd, buf, len))
	if(-1 == send(IEC104_SOCKET, (uint8_t*)buf, len))
	{
    return RET_ERROR;
  }
  return RET_SUCESS;
}

static uint32_t Linux_SetTime(PCP56Time2a_T time)
{
	RTC_TimeTypeDef systemTime = {0};
	RTC_DateTypeDef systemDate = {0};
	
	systemTime.Seconds = (uint8_t)(time->_milliseconds / 1000);
	systemTime.Minutes = time->_min._minutes;
	systemTime.Hours = time->_hour._hours;
	systemDate.WeekDay = time->_day._dayofweek;
	systemDate.Date = time->_day._dayofmonth;
	systemDate.Month = time->_month._month;
	systemDate.Year = time->_year._year;
	HAL_RTC_SetTime(&hrtc, &systemTime, RTC_FORMAT_BIN);
	HAL_RTC_SetDate(&hrtc, &systemDate, RTC_FORMAT_BIN);

	return RET_SUCESS;
}

static uint32_t Linux_GetTime(PCP56Time2a_T time)
{
	RTC_TimeTypeDef systemTime = {0};
	RTC_DateTypeDef systemDate = {0};
	
	HAL_RTC_GetTime(&hrtc, &systemTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &systemDate, RTC_FORMAT_BIN);
	time->_milliseconds = systemTime.Seconds * 1000;
	time->_min._minutes = systemTime.Minutes;
	time->_hour._hours = systemTime.Hours;
	time->_day._dayofweek = systemDate.WeekDay;
	time->_day._dayofmonth = systemDate.Date;
	time->_month._month = systemDate.Month;
	time->_year._year = systemDate.Year;
	return RET_SUCESS;
}

static int8_t Linux_GetStationState(uint16_t Addr, uint8_t DevType)
{//?
	switch(DevType)
	{
		case ENDDEVICE_TYPE_HXTM:
			if(Addr == 0)
				return 1;
			break;
		case ENDDEVICE_TYPE_HXGF://IEC104_INFOADDR_STATE_HXGF(101)
			if(Addr == 0)//101
			{
				if(VV_Block_f)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			if(Addr == 1)
			{
				if(VV_Faults == 0)
				{
					return 0;
				}
				else
				{
					return 1;
				}
			}
			if(Addr == 2)
			{
				if(VV_Faults & 1)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			if(Addr == 3)
			{
				if(VV_Faults & 32)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			if(Addr == 4)
			{
				if(Inputs_State & INPUT_DOOR)
				{
					return 0;
				}
				else
				{
					return 1;
				}
			}
			if(Addr == 5)
			{
				if(Inputs_State & INPUT_POWER_FAULT)
				{
					return 0;
				}
				else
				{
					return 1;
				}
			}
			if(Addr == 6)
			{
				if(Inputs_State & INPUT_RES_POWER)
				{
					return 0;
				}
				else
				{
					return 1;
				}
			}
			if(Addr == 7)
			{
				if(Inputs_State & INPUT_DV8)
				{
					return 0;
				}
				else
				{
					return 1;
				}
			}
			if(Addr == 8)
			{
				return 0;
			}
			if(Addr == 9)
			{
				return 0;
			}
			if(Addr == 10)
			{
				if(MB_PuskMSZ1)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			if(Addr == 11)
			{
				if(MB_VozvratMSZ1)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			if(Addr == 12)
			{
				if(MB_WorkMSZ1)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			if(Addr == 13)
			{
				if(MB_PuskMSZ2)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			if(Addr == 14)
			{
				if(MB_VozvratMSZ2)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			if(Addr == 15)
			{
				if(MB_WorkMSZ2)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			if(Addr == 16)
			{
				if(MB_PuskSV)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			if(Addr == 17)
			{
				if(MB_WorkSV)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			if(Addr == 18)
			{
				if(MB_GotovAPV)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			if(Addr == 19)
			{
				if(MB_PuskAPV1)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			if(Addr == 20)
			{
				if(MB_WorkAPV1)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			if(Addr == 21)
			{
				if(MB_PuskAPV2)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			if(Addr == 22)
			{
				if(MB_WorkAPV2)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			if(Addr == 23)
			{
				if(MB_PuskZMN)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			if(Addr == 24)
			{
				if(MB_VozvratZMN)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			if(Addr == 25)
			{
				if(MB_WorkZMN)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			if(Addr == 26)
			{
				if(MB_PuskAPV_ZMN)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			if(Addr == 27)
			{
				if(MB_VozvratAPV_ZMN)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			if(Addr == 28)
			{
				if(MB_WorkAPV_ZMN)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			if(Addr == 29)
			{
				return 0;
			}
			if(Addr == 30)
			{
				if(VV_State & 1)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			if(Addr == 31)
			{
				if(VV_State & 2)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			if(Addr == 32)
			{
				if(MSZ1vv)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			if(Addr == 33)
			{
				if(SVvv)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			if(Addr == 34)
			{
				if(APVvv)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			if(Addr == 35)
			{
				if(APV_T2vv)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			if(Addr == 36)
			{
				if(ZMNvv)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			if(Addr == 37)
			{
				if(ZMN_Ton_vv)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			if(Addr == 38)
			{
				if(DUvv)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			break;
	}
	return 0;
}

static float Linux_GetStaValue(uint16_t Addr, uint8_t DevType)
{
	switch(DevType)
	{
		case ENDDEVICE_TYPE_HXTM:
			if(Addr == 0)
				return 0.11;
			break;
		case ENDDEVICE_TYPE_HXGF:
			if(Addr == 0)
				return (float)Temp;
			if(Addr == 1)
				return (float)IrmsA;
			if(Addr == 2)
				return (float)IrmsC;
			if(Addr == 3)
				return (float)VrmsA;
			if(Addr == 4)
				return (float)VV_On_cnt;
			if(Addr == 5)
				return (float)IrmsA_poshk;
			if(Addr == 6)
				return (float)IrmsC_poshk;
			if(Addr == 7)
				return (float)VrmsA_poshk;
			if(Addr == 8)
				return (float)Imsz1;
			if(Addr == 9)
				return (float)Tmsz1;
			if(Addr == 10)
				return (float)Imsz2;
			if(Addr == 11)
				return (float)Tmsz2/10;
			if(Addr == 12)
				return (float)Isv;
			if(Addr == 13)
				return (float)Tsv/100;
			if(Addr == 14)
				return (float)APV_Tgot;
			if(Addr == 15)
				return (float)APV_T1/10;
			if(Addr == 16)
				return (float)APV_T2;
			if(Addr == 17)
				return (float)ZMN_Lvl;
			if(Addr == 18)
				return (float)ZMN_Toff;
			if(Addr == 19)
				return (float)ZMN_Ton;
			if(Addr == 20)
				return (float)VV_Off_Sourse;
			if(Addr == 21)
				return (float)VV_On_Sourse;
			if(Addr == 22)
				return (float)(((uint16_t)Year << 8) + Month);
			if(Addr == 23)
				return (float)(((uint16_t)Date << 8) + Hours);
			if(Addr == 24)
				return (float)(((uint16_t)Minutes << 8) + Seconds);
			if(Addr == 25)
				return (float)Reset_cnt;
			if(Addr == 26)
				return (float)ModBus1_cnt;
			if(Addr == 27)
				return (float)ModBus2_cnt;
			if(Addr == 28)
				return (float)IEC104_cnt;
			break;
	}
	return 0;
}

static uint16_t Linux_GetLinkAddr(void)
{
	return IEC104_ASDU;
}

int8_t Linux_GetInfoNum(uint8_t *InfoNum, uint8_t DevType)
{
	switch(DevType)
	{
		case ENDDEVICE_TYPE_HXTM:
			*InfoNum = 1;
			break;
		case ENDDEVICE_TYPE_HXGF:
			*InfoNum = 39;
			break;
		default:
			*InfoNum = 1;
	}
	return 1;
}

int8_t Linux_SetConfigF(float Value, uint32_t addr)
{
	switch(addr)
	{
		case 504:
			Set_Tmsz2(ceil(Value * 10));
			break;
		case 506:
			Set_Tsv(ceil(Value * 100));
			break;
		case 508:
			Set_APVt1(ceil(Value * 10));
			break;
	}
	return 1;
}

int8_t Linux_SetConfig(long Value, uint32_t addr)
{
	RTC_TimeTypeDef systemTime = {0};
	RTC_DateTypeDef systemDate = {0};

	switch(addr)
	{
		case 301:
			if(Value & 0x01)
			{
				VV_OnCmd(2);
			} else {
				VV_OffCmd(2);
			}
			break;
		case 302:
			if(Value & 0x01)
			{
				VV_OffCmd(2);
			}
			break;
		case 303:
			if(Value & 0x01)
			{
				Set_MSZ1vv(true);
			} else {
				Set_MSZ1vv(false);
			}
			break;
		case 304:
			if(Value & 0x01)
			{
				Set_MSZ1vv(false);
			}
			break;
		case 305:
			if(Value & 0x01)
			{
				Set_SVvv(true);
			} else {
				Set_SVvv(false);
			}
			break;
		case 306:
			if(Value & 0x01)
			{
				Set_SVvv(false);
			}
			break;
		case 307:
			if(Value & 0x01)
			{
				Set_APVvv(true);
				Set_APV_T2vv(true);
			} else {
				Set_APVvv(false);
				Set_APV_T2vv(false);
			}
			break;
		case 308:
			if(Value & 0x01)
			{
				Set_APVvv(false);
				Set_APV_T2vv(false);
			}
			break;
		case 309:
			if(Value & 0x01)
			{
				Set_ZMNvv(true);
			} else {
				Set_ZMNvv(false);
				Set_ZMN_Ton_vv(false);
			}
			break;
		case 310:
			if(Value & 0x01)
			{
				Set_ZMNvv(false);
				Set_ZMN_Ton_vv(false);
			}
			break;
		case 311:
			if(Value & 0x01)
			{
				Set_ZMN_Ton_vv(true);
			} else {
				Set_ZMN_Ton_vv(false);
			}
			break;
		case 312:
			if(Value & 0x01)
			{
				Set_ZMN_Ton_vv(false);
			}
			break;
		case 313:
			if(Value & 0x01)
			{
				Set_DUvv(true);
			} else {
				Set_DUvv(false);
			}
			break;
		case 314:
			if(Value & 0x01)
			{
				Set_DUvv(false);
			}
			break;
		case 315:
			if(Value & 0x01)
			{
				VV_Unblock_f = true;
			}
			break;
		case 401:
			if(Value & 1)
			{
				VV_OffCmd(2);
			}
			if(Value & 2)
			{
				VV_OnCmd(2);
			}
			break;
		case 403:
			if(Value & 1)
			{
				Set_MSZ1vv(false);
			}
			if(Value & 2)
			{
				Set_MSZ1vv(true);
			}
			break;
		case 405:
			if(Value & 1)
			{
				Set_SVvv(false);
			}
			if(Value & 2)
			{
				Set_SVvv(true);
			}
			break;
		case 407:
			if(Value & 1)
			{
				Set_APVvv(false);
				Set_APV_T2vv(false);
			}
			if(Value & 2)
			{
				Set_APVvv(true);
				Set_APV_T2vv(true);
			}
			break;
		case 409:
			if(Value & 1)
			{
				Set_ZMNvv(false);
				Set_ZMN_Ton_vv(false);
			}
			if(Value & 2)
			{
				Set_ZMNvv(true);
			}
			break;
		case 411:
			if(Value & 1)
			{
				Set_ZMN_Ton_vv(false);
			}
			if(Value & 2)
			{
				Set_ZMN_Ton_vv(true);
			}
			break;
		case 413:
			if(Value & 1)
			{
				Set_DUvv(false);
			}
			if(Value & 2)
			{
				Set_DUvv(true);
			}
			break;
		case 415:
			if(Value & 2)
			{
				VV_Unblock_f = true;
			}
			break;
		case 501:
			Set_Imsz1(Value);
			break;
		case 502:
			Set_Tmsz1(Value);
			break;
		case 503:
			Set_Imsz2(Value);
			break;
		case 505:
			Set_Isv(Value);
			break;
		case 507:
			Set_Tgot(Value);
			break;
		case 509:
			Set_APVt2(Value);
			break;
		case 510:
			Set_ZMNlvl(Value);
			break;
		case 511:
			Set_ZMNoff(Value);
			break;
		case 512:
			Set_ZMNon(Value);
			break;
		case 513:
			break;
		case 514:
			Year = (uint8_t)(Value >> 8);
			Month = (uint8_t)Value;
						
			systemTime.Seconds = Seconds;
			systemTime.Minutes = Minutes;
			systemTime.Hours = Hours;
			systemDate.Date = Date;
			systemDate.Month = Month;
			systemDate.Year = Year;
			HAL_RTC_SetTime(&hrtc, &systemTime, RTC_FORMAT_BIN);
			HAL_RTC_SetDate(&hrtc, &systemDate, RTC_FORMAT_BIN);
			break;
		case 515:
			Date = (uint8_t)(Value >> 8);
			Hours = (uint8_t)Value;
						
			systemTime.Seconds = Seconds;
			systemTime.Minutes = Minutes;
			systemTime.Hours = Hours;
			systemDate.Date = Date;
			systemDate.Month = Month;
			systemDate.Year = Year;
			HAL_RTC_SetTime(&hrtc, &systemTime, RTC_FORMAT_BIN);
			HAL_RTC_SetDate(&hrtc, &systemDate, RTC_FORMAT_BIN);
			break;
		case 516:
			Minutes = (uint8_t)(Value >> 8);
			Seconds = (uint8_t)Value;
						
			systemTime.Seconds = Seconds;
			systemTime.Minutes = Minutes;
			systemTime.Hours = Hours;
			systemDate.Date = Date;
			systemDate.Month = Month;
			systemDate.Year = Year;
			HAL_RTC_SetTime(&hrtc, &systemTime, RTC_FORMAT_BIN);
			HAL_RTC_SetDate(&hrtc, &systemDate, RTC_FORMAT_BIN);
			break;
		case 517:
			break;
		default:
			return 0;
	}
	return 1;
}

int8_t Linux_SaveFirmware(uint8_t FirmLen, uint8_t *buf,uint32_t FirmwareType, uint32_t Iec10x_Update_SeekAddr)
{//?
	return 1;
}

int8_t Linux_CheckFirmware(uint32_t FirmwareType, uint32_t TotalLen)
{//?
	return 1;
}

int8_t Linux_UpdateFirmware(uint32_t FirmwareType)
{//?
	return 1;
}

int8_t Linux_BackoffFirmware(uint32_t FirmwareType)
{//?
	return 1;
}

void LinuxLock(void)
{
//	pthread_mutex_lock(&mutex);
}
void LinuxUnlock()
{
//	pthread_mutex_unlock(&mutex);
}

static IEC10X_T Linux = {
    "Linux",
    Linux_init,
    delay_ms,
    Linux_CloseLink,
    Linux_Malloc,
    Linux_Free,
    IEC10X_PrioEnQueue,
    IEC10X_PrioDeQueue,
    IEC10X_PrioFindQueueHead,
    IEC10X_HighestPrio,
    IEC10X_PrioInitQueue,
    IEC10X_Prio_ClearQueue,
    Linuxsend,
    Linux_SetTime,
    Linux_GetTime,
    Linux_GetStationState,
    Linux_GetStaValue,
    Linux_GetLinkAddr,
    Linux_GetInfoNum,
    Linux_SetConfig,
    Linux_SetConfigF,
    Linux_SaveFirmware,
    Linux_CheckFirmware,
    Linux_UpdateFirmware,
    Linux_BackoffFirmware,
#ifdef IEC10XLOCK
    LinuxLock,
    LinuxUnlock
#endif
};

void W5500_Select(void)
{
	HAL_GPIO_WritePin(W5500_CS_GPIO_Port, W5500_CS_Pin, GPIO_PIN_RESET);
}

void W5500_Unselect(void) 
{
	HAL_GPIO_WritePin(W5500_CS_GPIO_Port, W5500_CS_Pin, GPIO_PIN_SET);
}

void W5500_ReadBuff(uint8_t* buff, uint16_t len) 
{
	HAL_SPI_Receive(&hspi3, buff, len, HAL_MAX_DELAY);
}

void W5500_WriteBuff(uint8_t* buff, uint16_t len) 
{
	HAL_SPI_Transmit(&hspi3, buff, len, HAL_MAX_DELAY);
}

uint8_t W5500_ReadByte(void) 
{
    uint8_t byte;
    W5500_ReadBuff(&byte, sizeof(byte));
    return byte;
}

void W5500_WriteByte(uint8_t byte) 
{
    W5500_WriteBuff(&byte, sizeof(byte));
}

void IEC104_Init(void) {
	RegisterIEC10XMoudle(&Linux);
}

void W5500_Init(void)
{
	uint8_t rx_tx_buff_sizes[] = {2, 2, 2, 2, 2, 2, 2, 2};

	HAL_GPIO_WritePin(W5500_RESET_GPIO_Port, W5500_RESET_Pin, GPIO_PIN_RESET);
	osDelay(1);
	HAL_GPIO_WritePin(W5500_RESET_GPIO_Port, W5500_RESET_Pin, GPIO_PIN_SET);
	osDelay(2);
	
	reg_wizchip_cs_cbfunc(W5500_Select, W5500_Unselect);
	reg_wizchip_spi_cbfunc(W5500_ReadByte, W5500_WriteByte);
  reg_wizchip_spiburst_cbfunc(W5500_ReadBuff, W5500_WriteBuff);

  wizchip_init(rx_tx_buff_sizes, rx_tx_buff_sizes);
	osDelay(100);
	setSHAR(net_info.mac);
  wizchip_setnetinfo(&net_info);
	ctlnetwork(CN_SET_NETINFO, (void*) &net_info);

	wiz_NetTimeout new_timeout;
	new_timeout.time_100us = 40000;
	new_timeout.retry_cnt = 2;
	ctlnetwork(CN_SET_TIMEOUT, (void*) &new_timeout);
	osDelay(100);
}

void IEC104_Poll(void) {
		IEC_Connect_timer = _IEC_Connect_timer;
		W5500_Init();
		osDelay(1000);
//		socket(IEC104_SOCKET, Sn_MR_TCP, IEC104_Port, Sn_MR_ND);
		socket(IEC104_SOCKET, Sn_MR_TCP, IEC104_Port, 0);
		listen(IEC104_SOCKET);
		while(getSn_SR(IEC104_SOCKET) != SOCK_ESTABLISHED)
		{
			osDelay(100);
		}

		IEC104_STATE_FLAG_INIT = IEC104_FLAG_SEND_CLOSED;
		IEC_Connect = true;
		IEC_Connect_timer = _IEC_Connect_timer;
		while(getSn_SR(IEC104_SOCKET) == SOCK_ESTABLISHED)
		{
			uint16_t tmp = getSn_RX_RSR(IEC104_SOCKET);
			if((tmp > 0) && (tmp != 65535))
			{
				IEC_Connect_timer = _IEC_Connect_timer;
				Iec104_RecvLen = recv(IEC104_SOCKET, (uint8_t*)Iec104_RecvBuf, 512);
				if(Iec104_RecvLen < 512)
				{
					Iex104_Receive((uint8_t*)Iec104_RecvBuf, Iec104_RecvLen);
					IEC104_cnt++;
					W5500rst_cnt = 0;
					Save_Counts_Del_f = true;
				}
			}
			Iec10x_Scheduled(IEC104_SOCKET);      
			Iec104_StateMachine();

			osDelay(1);
		}
		IEC_Connect = false;
		disconnect(IEC104_SOCKET);
		close(IEC104_SOCKET);
}
