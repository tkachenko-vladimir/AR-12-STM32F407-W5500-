//#include "stm32f4xx_hal.h"
//#include "cmsis_os.h"
#include "main.h"
#include "Settings.h"
#include "Inputs.h"
#include "mb.h"
#include "mbport.h"
#include "mbutils.h"
#include "Iec104.h"
#include "Events.h"
#include "Protection.h"

bool RegPresent;

void ModBus_Init(void) {
  huart1.Instance = USART1;
  huart1.Init.BaudRate = MB_BR[MB1_speed];
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

  huart3.Instance = USART3;
  huart3.Init.BaudRate = MB_BR[MB2_speed];
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }

  eMBErrorCode eStatus = eMBInit1(MB1_adress, 1, MB_BR[MB1_speed], MB_PAR_NONE);
  eStatus = eMBEnable1();
  eStatus = eMBInit2(MB2_adress, 1, MB_BR[MB2_speed], MB_PAR_NONE);
  eStatus = eMBEnable2();
}

void ModBus_Poll(void) {
	eMBPoll1();
	eMBPoll2();
}

uint16_t ReadInputReg(uint16_t RegAdress) {//04W
	RegPresent = true;
	switch(RegAdress)
	{
		default:
		{
			RegPresent = false;
			return 0;
		}
	}
}

uint16_t ReadHoldingReg(uint16_t RegAdress)//03W
{
	RegPresent = true;
	switch(RegAdress)
	{
		case 0x0020:
		{
			return VV_Block_f;
		}
		case 0x0021:
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
		case 0x0022:
		{
			return 0;
		}
		case 0x0023:
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
		case 0x0024:
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
		case 0x0025:
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
		case 0x0026:
		{
			if(Inputs_State & INPUT_POWER_FAULT)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		case 0x0027:
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
		case 0x0028:
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
		case 0x0029:
		{
			return 0;
		}
		case 0x002A:
		{
			return VV_Off_Sourse;
		}
		case 0x002B:
		{
			return VV_On_Sourse;
		}
		case 0x002C:
		{
			return 0;
		}
		case 0x002D:
		{
			return 0;
		}
		case 0x002E:
		{
			return 0;
		}
		case 0x002F:
		{
			return 0;
		}
		case 0x0030:
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
		case 0x0031:
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
		case 0x0032:
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
		case 0x0033:
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
		case 0x0034:
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
		case 0x0035:
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
		case 0x0036:
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
		case 0x0037:
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
		case 0x0038:
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
		case 0x0039:
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
		case 0x003A:
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
		case 0x003B:
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
		case 0x003C:
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
		case 0x003D:
		{
			return 0;
		}
		case 0x003E:
		{
			return 0;
		}
		case 0x003F:
		{
			return 0;
		}
		case 0x0040:
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
		case 0x0041:
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
		case 0x0042:
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
		case 0x0043:
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
		case 0x0044:
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
		case 0x0045:
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
		case 0x0046:
		{
			return 0;
		}
		case 0x0047:
		{
			return 0;
		}
		case 0x0048:
		{
			return 0;
		}
		case 0x0049:
		{
			return 0;
		}
		case 0x004A:
		{
			return 0;
		}
		case 0x004B:
		{
			unsigned short int TmpRet = 0;
			
			if(VV_Block_f)
			{
				TmpRet |= 1;
			}
			if(VV_Faults != 0)
			{
				TmpRet |= 2;
			}
			if(VV_Faults & 1)
			{
				TmpRet |= 4;
			}
			if(VV_Faults & 32)
			{
				TmpRet |= 8;
			}
			if(!(Inputs_State & INPUT_DOOR))
			{
				TmpRet |= 16;
			}
			if(Inputs_State & INPUT_POWER_FAULT)
			{
				TmpRet |= 32;
			}
			if(!(Inputs_State & INPUT_RES_POWER))
			{
				TmpRet |= 64;
			}
			if(!(Inputs_State & INPUT_DV8))
			{
				TmpRet |= 128;
			}
			return TmpRet;
		}
		case 0x004C:
		{
			return VV_Off_Sourse;
		}
		case 0x004D:
		{
			return VV_On_Sourse;
		}
		case 0x004E:
		{
			unsigned short int TmpRet = 0;

			if(MB_PuskMSZ1)
			{
				TmpRet |= 1;
			}
			if(MB_VozvratMSZ1)
			{
				TmpRet |= 2;
			}
			if(MB_WorkMSZ1)
			{
				TmpRet |= 4;
			}
			if(MB_PuskMSZ2)
			{
				TmpRet |= 8;
			}
			if(MB_VozvratMSZ2)
			{
				TmpRet |= 16;
			}
			if(MB_WorkMSZ2)
			{
				TmpRet |= 32;
			}
			if(MB_PuskSV)
			{
				TmpRet |= 64;
			}
			if(MB_WorkSV)
			{
				TmpRet |= 128;
			}
			if(MB_GotovAPV)
			{
				TmpRet |= 256;
			}
			if(MB_PuskAPV1)
			{
				TmpRet |= 512;
			}
			if(MB_WorkAPV1)
			{
				TmpRet |= 1024;
			}
			if(MB_PuskAPV2)
			{
				TmpRet |= 2048;
			}
			if(MB_WorkAPV2)
			{
				TmpRet |= 4096;
			}
			return TmpRet;
		}
		case 0x004F:
		{
			unsigned short int TmpRet = 0;

			if(MB_PuskZMN)
			{
				TmpRet |= 1;
			}
			if(MB_VozvratZMN)
			{
				TmpRet |= 2;
			}
			if(MB_WorkZMN)
			{
				TmpRet |= 4;
			}
			if(MB_PuskAPV_ZMN)
			{
				TmpRet |= 8;
			}
			if(MB_VozvratAPV_ZMN)
			{
				TmpRet |= 16;
			}
			if(MB_WorkAPV_ZMN)
			{
				TmpRet |= 32;
			}
			return TmpRet;
		}
		case 0x0050:
		{
			return VV_State;
		}
		case 0x0051:
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
		case 0x0052:
		{
			return Imsz1;
		}
		case 0x0053:
		{
			return Tmsz1;
		}
		case 0x0054:
		{
			return Imsz2;
		}
		case 0x0055:
		{
			return Tmsz2;
		}
		case 0x0056:
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
		case 0x0057:
		{
			return Isv;
		}
		case 0x0058:
		{
			return Tsv;
		}
		case 0x0059:
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
		case 0x005A:
		{
			return APV_Tgot;
		}
		case 0x005B:
		{
			return APV_T1;
		}
		case 0x005C:
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
		case 0x005D:
		{
			return APV_T2;
		}
		case 0x005E:
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
		case 0x005F:
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
		case 0x0060:
		{
			return ZMN_Lvl;
		}
		case 0x0061:
		{
			return ZMN_Toff;
		}
		case 0x0062:
		{
			return ZMN_Ton;
		}
		case 0x0063:
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
///////////////////////////////////////////////////////////////////////
		case 0x0080:
		{
			return VV_State;
		}
		case 0x0081:
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
		case 0x0082:
		{
			return Imsz1;
		}
		case 0x0083:
		{
			return Tmsz1;
		}
		case 0x0084:
		{
			return Imsz2;
		}
		case 0x0085:
		{
			return Tmsz2;
		}
		case 0x0086:
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
		case 0x0087:
		{
			return Isv;
		}
		case 0x0088:
		{
			return Tsv;
		}
		case 0x0089:
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
		case 0x008A:
		{
			return APV_Tgot;
		}
		case 0x008B:
		{
			return APV_T1;
		}
		case 0x008C:
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
		case 0x008D:
		{
			return APV_T2;
		}
		case 0x008E:
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
		case 0x008F:
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
		case 0x0090:
		{
			return ZMN_Lvl;
		}
		case 0x0091:
		{
			return ZMN_Toff;
		}
		case 0x0092:
		{
			return ZMN_Ton;
		}
		case 0x0093:
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
///////////////////////////////////////////////////////////////////////
		case 0x0200:
		{
			return Ktt;
		}
		case 0x0201:
		{
			return (unsigned short int)(Ktn * 100);
		}
		case 0x0202:
		{
			return (((unsigned short int)Year) << 8) + Month;
		}
		case 0x0203:
		{
			return (((unsigned short int)Date) << 8) + Hours;
		}
		case 0x0204:
		{
			return (((unsigned short int)Minutes) << 8) + Seconds;
		}
		case 0x0205:
		{
			return Reset_cnt;
		}
		case 0x0206:
		{
			return ModBus1_cnt;
		}
		case 0x0207:
		{
			return ModBus1_cnt;
		}
		case 0x0208:
		{
			return ModBus2_cnt;
		}
		case 0x0209:
		{
			return ModBus2_cnt;
		}
		case 0x020A:
		{
			return IEC104_cnt;
		}
		case 0x020B:
		{
			return IEC104_cnt;
		}
		case 0x0300:
		{
			return (unsigned short int)Temp;
		}
		case 0x0301:
		{
			return IrmsA;
		}
		case 0x0302:
		{
			return IrmsC;
		}
		case 0x0303:
		{
			return VrmsA;
		}
		case 0x0304:
		{
			return IrmsA_poshk;
		}
		case 0x0305:
		{
			return IrmsC_poshk;
		}
		case 0x0306:
		{
			return VrmsA_poshk;
		}
		case 0x0400:
		{
			return VV_On_cnt;
		}
		case 0x0401:
		{
			return _SERIAL_NBR;
		}
		case 0x0402:
		{
			return _PO_VERSION;
		}
//////////////////////////////////////////////////////////////////////////////////
		case 0xF000:
		{
			unsigned short int TmpRet = 0;
			
			if(VV_Block_f)
			{
				TmpRet |= 1;
			}
			if(VV_Faults != 0)
			{
				TmpRet |= 2;
			}
			if(VV_Faults & 1)
			{
				TmpRet |= 4;
			}
			if(VV_Faults & 32)
			{
				TmpRet |= 8;
			}
			if(!(Inputs_State & INPUT_DOOR))
			{
				TmpRet |= 16;
			}
			if(Inputs_State & INPUT_POWER_FAULT)
			{
				TmpRet |= 32;
			}
			if(!(Inputs_State & INPUT_RES_POWER))
			{
				TmpRet |= 64;
			}
			if(!(Inputs_State & INPUT_DV8))
			{
				TmpRet |= 128;
			}
			if(MB_PuskMSZ1)
			{
				TmpRet |= 256;
			}
			if(MB_VozvratMSZ1)
			{
				TmpRet |= 512;
			}
			if(MB_WorkMSZ1)
			{
				TmpRet |= 1024;
			}
			if(MB_PuskMSZ2)
			{
				TmpRet |= 2048;
			}
			if(MB_VozvratMSZ2)
			{
				TmpRet |= 4096;
			}
			if(MB_WorkMSZ2)
			{
				TmpRet |= 8192;
			}
			if(MB_PuskSV)
			{
				TmpRet |= 16384;
			}
			if(MB_WorkSV)
			{
				TmpRet |= 32768;
			}
			return TmpRet;
		}
		case 0xF001:
		{
			unsigned short int TmpRet = 0;
			
			if(MB_GotovAPV)
			{
				TmpRet |= 1;
			}
			if(MB_PuskAPV1)
			{
				TmpRet |= 2;
			}
			if(MB_WorkAPV1)
			{
				TmpRet |= 4;
			}
			if(MB_PuskAPV2)
			{
				TmpRet |= 8;
			}
			if(MB_WorkAPV2)
			{
				TmpRet |= 16;
			}
			if(MB_PuskZMN)
			{
				TmpRet |= 32;
			}
			if(MB_VozvratZMN)
			{
				TmpRet |= 64;
			}
			if(MB_WorkZMN)
			{
				TmpRet |= 128;
			}
			if(MB_PuskAPV_ZMN)
			{
				TmpRet |= 256;
			}
			if(MB_VozvratAPV_ZMN)
			{
				TmpRet |= 512;
			}
			if(MB_WorkAPV_ZMN)
			{
				TmpRet |= 1024;
			}
			if(VV_State & 1)
			{
				TmpRet |= 2048;
			}
			if(VV_State & 2)
			{
				TmpRet |= 4096;
			}
			return TmpRet;
		}
		case 0xF002:
		{
			return Internal_Faults;
		}
		case 0xF003:
		{
			return (((unsigned short int)net_info.ip[0]) << 8) + net_info.ip[1];
		}
		case 0xF004:
		{
			return (((unsigned short int)net_info.ip[2]) << 8) + net_info.ip[3];
		}
		case 0xF005:
		{
			return (((unsigned short int)net_info.sn[0]) << 8) + net_info.sn[1];
		}
		case 0xF006:
		{
			return (((unsigned short int)net_info.sn[2]) << 8) + net_info.sn[3];
		}
		case 0xF007:
		{
			return (((unsigned short int)net_info.gw[0]) << 8) + net_info.gw[1];
		}
		case 0xF008:
		{
			return (((unsigned short int)net_info.gw[2]) << 8) + net_info.gw[3];
		}
		default:
		{
			RegPresent = false;
			return 0;
		}
	}
}

bool WriteHoldingReg(unsigned short int RegAdress, unsigned short int RegData)//06W
{
	bool ret = true;
	
	switch(RegAdress)
	{
		case 0x0050:
		{
			if(RegData == 0)
			{
				VV_OffCmd(2);
			}
			if(RegData == 1)
			{
				VV_OnCmd(2);
			}
			break;
		}
		case 0x0051:
		{
			if(RegData == 0)
			{
				Set_MSZ1vv(false);
			}
			if(RegData == 1)
			{
				Set_MSZ1vv(true);
			}
			break;
		}
		case 0x0052:
		{
			Set_Imsz1(RegData);
			break;
		}
		case 0x0053:
		{
			Set_Tmsz1(RegData);
			break;
		}
		case 0x0054:
		{
			Set_Imsz2(RegData);
			break;
		}
		case 0x0055:
		{
			Set_Tmsz2(RegData);
			break;
		}
		case 0x0056:
		{
			if(RegData == 0)
			{
				Set_SVvv(false);
			}
			if(RegData == 1)
			{
				Set_SVvv(true);
			}
			break;
		}
		case 0x0057:
		{
			Set_Isv(RegData);
			break;
		}
		case 0x0058:
		{
			Set_Tsv(RegData);
			break;
		}
		case 0x0059:
		{
			if(RegData == 0)
			{
				Set_APVvv(false);
			}
			if(RegData == 1)
			{
				Set_APVvv(true);
			}
			break;
		}
		case 0x005A:
		{
			Set_Tgot(RegData);
			break;
		}
		case 0x005B:
		{
			Set_APVt1(RegData);
			break;
		}
		case 0x005C:
		{
			if(RegData == 0)
			{
				Set_APV_T2vv(false);
			}
			if(RegData == 1)
			{
				Set_APV_T2vv(true);
			}
			break;
		}
		case 0x005D:
		{
			Set_APVt2(RegData);
			break;
		}
		case 0x005E:
		{
			if(RegData == 0)
			{
				Set_ZMNvv(false);
			}
			if(RegData == 1)
			{
				Set_ZMNvv(true);
			}
			break;
		}
		case 0x005F:
		{
			if(RegData == 0)
			{
				Set_ZMN_Ton_vv(false);
			}
			if(RegData == 1)
			{
				Set_ZMN_Ton_vv(true);
			}
			break;
		}
		case 0x0060:
		{
			Set_ZMNlvl(RegData);
			break;
		}
		case 0x0061:
		{
			Set_ZMNoff(RegData);
			break;
		}
		case 0x0062:
		{
			Set_ZMNon(RegData);
			break;
		}
///////////////////////////////////////////////////////////////////////
		case 0x0080:
		{
			if(RegData == 0)
			{
				VV_OffCmd(2);
			}
			if(RegData == 1)
			{
				VV_OnCmd(2);
			}
			break;
		}
		case 0x0081:
		{
			if(RegData == 0)
			{
				Set_MSZ1vv(false);
			}
			if(RegData == 1)
			{
				Set_MSZ1vv(true);
			}
			break;
		}
		case 0x0082:
		{
			Set_Imsz1(RegData);
			break;
		}
		case 0x0083:
		{
			Set_Tmsz1(RegData);
			break;
		}
		case 0x0084:
		{
			Set_Imsz2(RegData);
			break;
		}
		case 0x0085:
		{
			Set_Tmsz2(RegData);
			break;
		}
		case 0x0086:
		{
			if(RegData == 0)
			{
				Set_SVvv(false);
			}
			if(RegData == 1)
			{
				Set_SVvv(true);
			}
			break;
		}
		case 0x0087:
		{
			Set_Isv(RegData);
			break;
		}
		case 0x0088:
		{
			Set_Tsv(RegData);
			break;
		}
		case 0x0089:
		{
			if(RegData == 0)
			{
				Set_APVvv(false);
			}
			if(RegData == 1)
			{
				Set_APVvv(true);
			}
			break;
		}
		case 0x008A:
		{
			Set_Tgot(RegData);
			break;
		}
		case 0x008B:
		{
			Set_APVt1(RegData);
			break;
		}
		case 0x008C:
		{
			if(RegData == 0)
			{
				Set_APV_T2vv(false);
			}
			if(RegData == 1)
			{
				Set_APV_T2vv(true);
			}
			break;
		}
		case 0x008D:
		{
			Set_APVt2(RegData);
			break;
		}
		case 0x008E:
		{
			if(RegData == 0)
			{
				Set_ZMNvv(false);
			}
			if(RegData == 1)
			{
				Set_ZMNvv(true);
			}
			break;
		}
		case 0x008F:
		{
			if(RegData == 0)
			{
				Set_ZMN_Ton_vv(false);
			}
			if(RegData == 1)
			{
				Set_ZMN_Ton_vv(true);
			}
			break;
		}
		case 0x0090:
		{
			Set_ZMNlvl(RegData);
			break;
		}
		case 0x0091:
		{
			Set_ZMNoff(RegData);
			break;
		}
		case 0x0092:
		{
			Set_ZMNon(RegData);
			break;
		}
		case 0x0093:
		{
			if(RegData == 0)
			{
				Set_DUvv(false);
			}
			if(RegData == 1)
			{
				Set_DUvv(true);
			}
			break;
		}
///////////////////////////////////////////////////////////////////////
		case 0x00064:
		{
			if(RegData == 1)
			{
				VV_Unblock_f = true;
			}
			break;
		}
		case 0x0200:
		{
			if((RegData == 10) || (RegData == 20) || (RegData == 30))
			{
				Ktt = RegData;
				Save_Settings_Del_f = true;
			}
			break;
		}
		case 0x0201:
		{
			if((RegData == 4546) || (RegData == 2727))
			{
				Ktn = (float)RegData / 100;
				Save_Settings_Del_f = true;
			}
			break;
		}
		case 0x0202:
		{
			Year = (uint8_t)(RegData >> 8);
			Month = (uint8_t)RegData;
			RTC_TimeTypeDef systemTime = {0};
			RTC_DateTypeDef systemDate = {0};
						
			systemTime.Seconds = Seconds;
			systemTime.Minutes = Minutes;
			systemTime.Hours = Hours;
			systemDate.Date = Date;
			systemDate.Month = Month;
			systemDate.Year = Year;
			HAL_RTC_SetTime(&hrtc, &systemTime, RTC_FORMAT_BIN);
			HAL_RTC_SetDate(&hrtc, &systemDate, RTC_FORMAT_BIN);
			break;
		}
		case 0x0203:
		{
			Date = (uint8_t)(RegData >> 8);
			Hours = (uint8_t)RegData;
			RTC_TimeTypeDef systemTime = {0};
			RTC_DateTypeDef systemDate = {0};
						
			systemTime.Seconds = Seconds;
			systemTime.Minutes = Minutes;
			systemTime.Hours = Hours;
			systemDate.Date = Date;
			systemDate.Month = Month;
			systemDate.Year = Year;
			HAL_RTC_SetTime(&hrtc, &systemTime, RTC_FORMAT_BIN);
			HAL_RTC_SetDate(&hrtc, &systemDate, RTC_FORMAT_BIN);
			break;
		}
		case 0x0204:
		{
			Minutes = (uint8_t)(RegData >> 8);
			Seconds = (uint8_t)RegData;
			RTC_TimeTypeDef systemTime = {0};
			RTC_DateTypeDef systemDate = {0};
						
			systemTime.Seconds = Seconds;
			systemTime.Minutes = Minutes;
			systemTime.Hours = Hours;
			systemDate.Date = Date;
			systemDate.Month = Month;
			systemDate.Year = Year;
			HAL_RTC_SetTime(&hrtc, &systemTime, RTC_FORMAT_BIN);
			HAL_RTC_SetDate(&hrtc, &systemDate, RTC_FORMAT_BIN);
			break;
		}
		case 0x0400:
		{
			if(RegData <= 20000)
			{
				VV_On_cnt = RegData;
				Save_State_f = true;
			}
			break;
		}
//////////////////////////////////////////////////////////////////////////////////
		case 0xF003:
		{
			net_info.ip[0] = (uint8_t)(RegData >> 8);
			net_info.ip[1] = (uint8_t)RegData;
			Save_Settings_Del_f = true;
			break;
		}
		case 0xF004:
		{
			net_info.ip[2] = (uint8_t)(RegData >> 8);
			net_info.ip[3] = (uint8_t)RegData;
			Save_Settings_Del_f = true;
			break;
		}
		case 0xF005:
		{
			net_info.sn[0] = (uint8_t)(RegData >> 8);
			net_info.sn[1] = (uint8_t)RegData;
			Save_Settings_Del_f = true;
			break;
		}
		case 0xF006:
		{
			net_info.sn[2] = (uint8_t)(RegData >> 8);
			net_info.sn[3] = (uint8_t)RegData;
			Save_Settings_Del_f = true;
			break;
		}
		case 0xF007:
		{
			net_info.gw[0] = (uint8_t)(RegData >> 8);
			net_info.gw[1] = (uint8_t)RegData;
			Save_Settings_Del_f = true;
			break;
		}
		case 0xF008:
		{
			net_info.gw[2] = (uint8_t)(RegData >> 8);
			net_info.gw[3] = (uint8_t)RegData;
			Save_Settings_Del_f = true;
			break;
		}
		case 0xF100:
		{
			if(RegData == 1)
			{
				HAL_NVIC_SystemReset();
			}
			break;
		}
		case 0xF101:
		{
			if(RegData == 1)
			{
				HAL_NVIC_SystemReset();
			}
			break;
		}
		case 0x0206:
		{
			if(RegData <= 15)
			{
				MAX7219_Brightness = RegData;
				Save_Settings_Del_f = true;
			}
			break;
		}
		default:
		{
			ret = false;
			break;
		}
	}
	return ret;
}

unsigned char ReadDiscreteInput(unsigned short int RegAdress)//02b
{
	RegPresent = true;
	switch(RegAdress)
	{
		default:
		{
			RegPresent = false;
			return 0;
		}
	}
}

unsigned char ReadCoil(unsigned short int RegAdress)//01b
{
	RegPresent = true;
	switch(RegAdress)
	{
		default:
		{
			RegPresent = false;
			return 0;
		}
	}
}

bool WriteCoil(unsigned short int RegAdress, unsigned char RegData)//05b
{
	bool ret = true;
	
	switch(RegAdress)
	{
		default:
		{
			ret = false;
			break;
		}
	}
	return ret;
}

eMBErrorCode
eMBRegInputCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs)
{
	uint8_t RegCnt;
	uint16_t RegTmp;
	eMBErrorCode  eStatus = MB_ENOERR;
	
	usAddress--;
	for(RegCnt = 0; RegCnt < usNRegs; RegCnt++)
	{
		RegTmp = ReadInputReg(usAddress + RegCnt);
		if(RegPresent)
		{
			*pucRegBuffer++ = (UCHAR)(RegTmp >> 8);
			*pucRegBuffer++ = (UCHAR)(RegTmp);
		}
		else
		{
			eStatus = MB_ENOREG;
		}
	}
  return eStatus;
}

eMBErrorCode
eMBRegHoldingCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode)
{
	uint8_t RegCnt;
	uint16_t RegTmp;
	eMBErrorCode  eStatus = MB_ENOERR;
	
	usAddress--;
	for(RegCnt = 0; RegCnt < usNRegs; RegCnt++)
	{
		if(eMode == MB_REG_READ)
		{
			RegTmp = ReadHoldingReg(usAddress + RegCnt);
			if(RegPresent)
			{
				*pucRegBuffer++ = (UCHAR)(RegTmp >> 8);
				*pucRegBuffer++ = (UCHAR)(RegTmp);
			}
			else
			{
				eStatus = MB_ENOREG;
			}
		}
		else
		{
			if(DUvv)
			{
				RegTmp = (unsigned short int)*pucRegBuffer++;
				RegTmp = RegTmp << 8;
				RegTmp = RegTmp + *pucRegBuffer++;
				if(!WriteHoldingReg((usAddress + RegCnt), RegTmp))
				{
					eStatus = MB_ENOREG;
				}
			}
		}
	}
  return eStatus;
}

eMBErrorCode//01
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
	uint8_t RegCnt;
	uint8_t RegTmp;
	eMBErrorCode  eStatus = MB_ENOERR;
	
	usAddress--;
	if(eMode == MB_REG_READ)
	{
		*pucRegBuffer = 0;
		for(RegCnt = 0; RegCnt < usNCoils; RegCnt++)
		{
			RegTmp = ReadCoil(usAddress + RegCnt);
			if(RegPresent)
			{
					xMBUtilSetBits(pucRegBuffer, RegCnt, 1, RegTmp);
			}
			else
			{
				eStatus = MB_ENOREG;
			}
		}
	}
	else
	{
		if(DUvv)
		{
			for(RegCnt = 0; RegCnt < usNCoils; RegCnt++)
			{
				RegTmp = xMBUtilGetBits(pucRegBuffer, RegCnt, 1);
				if(!WriteCoil((usAddress + RegCnt), RegTmp))
				{
					eStatus = MB_ENOREG;
				}
			}
		}
	}
  return eStatus;
}

eMBErrorCode//02
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
	uint8_t RegCnt;
	unsigned char RegTmp;
	eMBErrorCode  eStatus = MB_ENOERR;
	
	*pucRegBuffer = 0;
	usAddress--;
	for(RegCnt = 0; RegCnt < usNDiscrete; RegCnt++)
	{
		RegTmp = ReadDiscreteInput(usAddress + RegCnt);
		if(RegPresent)
		{
				xMBUtilSetBits(pucRegBuffer, RegCnt, 1, RegTmp);
		}
		else
		{
			eStatus = MB_ENOREG;
		}
	}
  return eStatus;
}
