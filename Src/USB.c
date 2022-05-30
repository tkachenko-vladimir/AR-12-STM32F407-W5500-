#include "main.h"
#include "Settings.h"
#include "ModBus.h"
#include "usbd_cdc_if.h"
#include "Events.h"

bool SendLog = false;
uint8_t SendLogPos = 0;

void USB_Poll(void) {
	if(SendLog) {
		sprintf(USB_Send_buf, "102,%u,%u,%u,%u ", LogBuf[SendLogPos].TimeStamp, LogBuf[SendLogPos].mSec, LogBuf[SendLogPos].Event_nbr, LogBuf[SendLogPos].Event_param);
		CDC_Transmit_FS((unsigned char*)USB_Send_buf, strlen(USB_Send_buf));
		SendLogPos++;
		if(SendLogPos == 50) {
			SendLog = false;
		}
	}
	if(USB_Read_f)
	{
		USB_Read_f = false;
		if(!memcmp(USB_Read_buf, "status", 6))
		{
			sprintf(USB_Send_buf, "201,%u,%u,%u", VrmsA, IrmsA, IrmsC);
			CDC_Transmit_FS((unsigned char*)USB_Send_buf, strlen(USB_Send_buf));
		}
		if(!memcmp(USB_Read_buf, "connect", 7))
		{
			sprintf(USB_Send_buf, "200,%u,%u,%u,%u", _PO_VERSION, _SETTINGS_VER, _SETTINGS_VER, _SERIAL_NBR);
			CDC_Transmit_FS((unsigned char*)USB_Send_buf, strlen(USB_Send_buf));
		}
		if(!memcmp(USB_Read_buf, "smb", 3))
		{
			unsigned short int Adress, Data;
			sscanf(&USB_Read_buf[4], "%hu,%hu", &Adress, &Data);
			WriteHoldingReg(Adress, Data);
		}
		if(!memcmp(USB_Read_buf, "gmb", 3))
		{
			sprintf(USB_Send_buf, "50,%u", ReadHoldingReg(atoi(&USB_Read_buf[4])));
			CDC_Transmit_FS((unsigned char*)USB_Send_buf, strlen(USB_Send_buf));
		}
		if(!memcmp(USB_Read_buf, "save", 4))
		{
			switch(USB_Read_buf[5])
			{
				case '0':
				{
					short int Imsz1_t, Tmsz1_t, Imsz2_t, Tmsz2_t, Isv_t, Tsv_t, MSZ1vv_t, SVvv_t;
					sscanf(&USB_Read_buf[7], "%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd", &Imsz1_t, &Tmsz1_t, &Imsz2_t, &Tmsz2_t, &Isv_t, &Tsv_t, &MSZ1vv_t, &SVvv_t);
					if((Imsz1_t >= Imsz1_MIN) && (Imsz1_t <= Imsz1_MAX))
					{
						Imsz1 = Imsz1_t;
					}
					if(((Tmsz1_t > Tmsz1_MIN) || (Tmsz1_t == Tmsz1_MIN)) && (Tmsz1_t <= Tmsz1_MAX))
					{
						Tmsz1 = Tmsz1_t;
					}
					if(((Imsz2_t >= Imsz2_MIN)) && (Imsz2_t <= Imsz2_MAX))
					{
						Imsz2 = Imsz2_t;
					}
					if(((Tmsz2_t > Tmsz2_MIN) || (Tmsz2_t == Tmsz2_MIN)) && (Tmsz2_t <= Tmsz2_MAX))
					{
						Tmsz2 = Tmsz2_t;
					}
					if((Isv_t >= Isv_MIN) && (Isv_t <= Isv_MAX))
					{
						Isv = Isv_t;
					}
					if(((Tsv_t > Tsv_MIN) || (Tsv_t == Tsv_MIN)) && (Tsv_t <= Tsv_MAX))
					{
						Tsv = Tsv_t;
					}
					if(MSZ1vv_t == 0)
						MSZ1vv = false;
					if(MSZ1vv_t == 1)
						MSZ1vv = true;
					if(SVvv_t == 0)
						SVvv = false;
					if(SVvv_t == 1)
						SVvv = true;
					Save_Settings2_f = true;
					break;
				}
				case '1':
				{
					short int APV_Tgot_t, APV1_T_t, APV2_T_t, APVvv_t, APV2vv_t;
					sscanf(&USB_Read_buf[7], "%hd,%hd,%hd,%hd,%hd", &APV_Tgot_t, &APV1_T_t, &APV2_T_t, &APVvv_t, &APV2vv_t);
					if(((APV_Tgot_t > APV_Tgot_MIN) || (APV_Tgot_t == APV_Tgot_MIN)) && (APV_Tgot_t <= APV_Tgot_MAX))
					{
						APV_Tgot = APV_Tgot_t;
					}
					if(((APV1_T_t > APV1_T_MIN) || (APV1_T_t == APV1_T_MIN)) && (APV1_T_t <= APV1_T_MAX))
					{
						APV_T1 = APV1_T_t;
					}
					if((APV2_T_t >= APV2_T_MIN) && (APV2_T_t <= APV2_T_MAX))
					{
						APV_T2 = APV2_T_t;
					}
					if(APVvv_t == 0)
						Set_APVvv(false);
					if(APVvv_t == 1)
						Set_APVvv(true);
					if(APV2vv_t == 0)
						APV_T2vv = false;
					if(APV2vv_t == 1)
						APV_T2vv = true;
					Save_Settings2_f = true;
					break;
				}
				case '2':
				{
					short int ZMN_level_t, ZMN_Tvidkl_t, ZMN_Tvkl_t, ZMNvv_t, ZMN_APVvv_t;
					sscanf(&USB_Read_buf[7], "%hd,%hd,%hd,%hd,%hd", &ZMN_level_t, &ZMN_Tvidkl_t, &ZMN_Tvkl_t, &ZMNvv_t, &ZMN_APVvv_t);
					if((ZMN_level_t >= ZMN_level_MIN) && (ZMN_level_t <= ZMN_level_MAX))
					{
						ZMN_Lvl = (ZMN_level_t / 10) * 10;
					}
					if((ZMN_Toff >= ZMN_Tvidkl_MIN) && (ZMN_Toff <= ZMN_Tvidkl_MAX))
					{
						ZMN_Toff = ZMN_Tvidkl_t;
					}
					if((ZMN_Ton >= ZMN_Tvkl_MIN) && (ZMN_Ton <= ZMN_Tvkl_MAX))
					{
						ZMN_Ton = ZMN_Tvkl_t;
					}
					if(ZMNvv_t == 0)
						Set_ZMNvv(false);
					if(ZMNvv_t == 1)
						Set_ZMNvv(true);
					if(ZMN_APVvv_t == 0)
						ZMN_Ton_vv = false;
					if(ZMN_APVvv_t == 1)
						ZMN_Ton_vv = true;
					Save_Settings2_f = true;
					break;
				}
				case '3':
				{
					short int MB1_adress_t, MB1_speed_t, MB2_adress_t, MB2_speed_t;
					sscanf(&USB_Read_buf[7], "%hd,%hd,%hd,%hd", &MB1_adress_t, &MB1_speed_t, &MB2_adress_t, &MB2_speed_t);
					if((MB1_adress_t >= 1) && (MB1_adress_t <= 254))
					{
						MB1_adress = MB1_adress_t;
					}
					if((MB1_speed_t >= 0) && (MB1_speed_t <= 4))
					{
						MB1_speed = MB1_speed_t;
					}
					if((MB2_adress_t >= 1) && (MB2_adress_t <= 254))
					{
						MB2_adress = MB2_adress_t;
					}
					if((MB2_speed_t >= 0) && (MB2_speed_t <= 4))
					{
						MB2_speed = MB1_speed_t;
					}
					Save_Settings2_f = true;
					ModBus_Init();
					break;
				}
				case '4':
				{
					unsigned int IEC104_IP[4], IEC104_Mask[4], IEC104_Gate[4];
					unsigned short int IEC104_Port_t, IEC104_ASDU_t;
					sscanf(&USB_Read_buf[7], "%u.%u.%u.%u,%u.%u.%u.%u,%u.%u.%u.%u,%hu,%hu", &IEC104_IP[0], &IEC104_IP[1], &IEC104_IP[2], &IEC104_IP[3], &IEC104_Mask[0], &IEC104_Mask[1], &IEC104_Mask[2], &IEC104_Mask[3], &IEC104_Gate[0], &IEC104_Gate[1], &IEC104_Gate[2], &IEC104_Gate[3], &IEC104_Port_t, &IEC104_ASDU_t);
					net_info.ip[0] = IEC104_IP[0];
					net_info.ip[1] = IEC104_IP[1];
					net_info.ip[2] = IEC104_IP[2];
					net_info.ip[3] = IEC104_IP[3];
					net_info.sn[0] = IEC104_Mask[0];
					net_info.sn[1] = IEC104_Mask[1];
					net_info.sn[2] = IEC104_Mask[2];
					net_info.sn[3] = IEC104_Mask[3];
					net_info.gw[0] = IEC104_Gate[0];
					net_info.gw[1] = IEC104_Gate[1];
					net_info.gw[2] = IEC104_Gate[2];
					net_info.gw[3] = IEC104_Gate[3];
					IEC104_Port = IEC104_Port_t;
					IEC104_ASDU = IEC104_ASDU_t;
					Save_Settings2_f = true;
/*
					W5500_Init();
					disconnect(IEC104_SOCKET);
					close(IEC104_SOCKET);
*/
					break;
				}
				case '5':
				{
					unsigned short int Hour_t, Minute_t, Second_t, Date_t, Month_t, Year_t, SW_auto_t;
					sscanf(&USB_Read_buf[7],"%hd,%hd,%hd,%hd,%hd,%hd,%hd", &Hour_t, &Minute_t, &Second_t, &Date_t, &Month_t, &Year_t, &SW_auto_t);
					Hours = Hour_t;
					Minutes = Minute_t;
					Seconds = Second_t;
					Date = Date_t;
					Month = Month_t;
					Year = Year_t;
					if(SW_auto_t == 0)
					{
						SW_auto = false;
					}
					if(SW_auto_t == 1)
					{
						SW_auto = true;
					}
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
					Save_Settings2_f = true;
					break;
				}
				case '6':
				{
					unsigned int Ktt_t, Ktn_t;
					sscanf(&USB_Read_buf[7], "%u,%u", &Ktt_t, &Ktn_t);
					if(Ktt_t == 0)
						Ktt = 10;
					if(Ktt_t == 1)
						Ktt = 20;
					if(Ktt_t == 2)
						Ktt = 30;
					if(Ktt_t == 3)
						Ktt = 40;
					if(Ktt_t == 4)
						Ktt = 80;
					if(Ktt_t == 5)
						Ktt = 120;
					if(Ktn_t == 0)
						Ktn = 27.27;
					if(Ktn_t == 1)
						Ktn = 45.45;
					Save_Settings2_f = true;
					break;
				}
				case '7':
				{
					unsigned int _K_IAt, _K_IBt, _K_UAt;
					sscanf(&USB_Read_buf[7], "%u,%u,%u,%hd,%hd,%hd,%hd", &_K_IAt, &_K_IBt, &_K_UAt, &_B_IA, &_B_IC, &_B_UA, &_SERIAL_NBR);
					_K_IA = (float)_K_IAt / 10000;
					_K_IC = (float)_K_IBt / 10000;
					_K_UA = (float)_K_UAt / 10000;
					Save_Settings1_f = true;
					break;
				}
				case '8':
				{
					unsigned int Tscf1;
					sscanf(&USB_Read_buf[7], "%d,%d,%d,%d", &Tscf1, &IrmsA_d, &IrmsC_d, &VrmsA_d);
					if(Tscf1 == 1)
					{
						Debug_mode = true;
					}
					else
					{
						Debug_mode = false;
					}
					break;
				}
				case '9':
				{
					unsigned int LogMask_t;
					sscanf(&USB_Read_buf[7], "%d", &LogMask_t);
					LogMask = LogMask_t;
					Save_Settings2_f = true;
					break;
				}
			}
		}
		if(!memcmp(USB_Read_buf, "get", 3))
		{
			uint8_t GetNbr = atoi(&USB_Read_buf[4]);
			switch(GetNbr)
			{
				case 0:
				{
					uint8_t MSZ1vvI = 0, SVvvI = 0;
					if(MSZ1vv)
						MSZ1vvI = 1;
					if(SVvv)
						SVvvI = 1;
					sprintf(USB_Send_buf, "0,%d,%d,%d,%d,%d,%d,%d,%d", Imsz1, Tmsz1, Imsz2, Tmsz2, Isv, Tsv, MSZ1vvI, SVvvI);
					CDC_Transmit_FS((unsigned char*)USB_Send_buf, strlen(USB_Send_buf));
					break;
				}
				case 1:
				{
					uint8_t APVvvI = 0, APV2vvI = 0;
					if(APVvv)
						APVvvI = 1;
					if(APV_T2vv)
						APV2vvI = 1;
					sprintf(USB_Send_buf, "1,%d,%d,%d,%d,%d", APV_Tgot, APV_T1, APV_T2, APVvvI, APV2vvI);
					CDC_Transmit_FS((unsigned char*)USB_Send_buf, strlen(USB_Send_buf));
					break;
				}
				case 2:
				{
					uint8_t ZMNvvI = 0, ZMN_APVvvI = 0;
					if(ZMNvv)
						ZMNvvI = 1;
					if(ZMN_Ton_vv)
						ZMN_APVvvI = 1;
					sprintf(USB_Send_buf, "2,%d,%d,%d,%d,%d", ZMN_Lvl, ZMN_Toff, ZMN_Ton, ZMNvvI, ZMN_APVvvI);
					CDC_Transmit_FS((unsigned char*)USB_Send_buf, strlen(USB_Send_buf));
					break;
				}
				case 3:
				{
					sprintf(USB_Send_buf, "3,%d,%d,%d,%d", MB1_adress, MB1_speed, MB2_adress, MB2_speed);
					CDC_Transmit_FS((unsigned char*)USB_Send_buf, strlen(USB_Send_buf));
					break;
				}
				case 4:
				{
					sprintf(USB_Send_buf, "4,%u.%u.%u.%u,%u.%u.%u.%u,%u.%u.%u.%u,%u,%u", net_info.ip[0], net_info.ip[1], net_info.ip[2], net_info.ip[3], net_info.sn[0], net_info.sn[1], net_info.sn[2], net_info.sn[3], net_info.gw[0], net_info.gw[1], net_info.gw[2], net_info.gw[3], IEC104_Port, IEC104_ASDU);
					CDC_Transmit_FS((unsigned char*)USB_Send_buf, strlen(USB_Send_buf));
					break;
				}
				case 5:
				{
					sprintf(USB_Send_buf, "5,%u,%u,%u,%u,%u,%u,%u", Hours, Minutes, Seconds, Date, Month, Year, SW_auto);
					CDC_Transmit_FS((unsigned char*)USB_Send_buf, strlen(USB_Send_buf));
					break;
				}
				case 6:
				{
					unsigned int Ktt_t, Ktn_t;
					if(Ktt == 10)
						Ktt_t = 0;
					if(Ktt == 20)
						Ktt_t = 1;
					if(Ktt == 30)
						Ktt_t = 2;
					if(Ktt == 40)
						Ktt_t = 3;
					if(Ktt == 80)
						Ktt_t = 4;
					if(Ktt == 120)
						Ktt_t = 5;
					if((unsigned int)Ktn == 27)
						Ktn_t = 0;
					if((unsigned int)Ktn == 45)
						Ktn_t = 1;
					sprintf(USB_Send_buf, "6,%u,%u", Ktt_t, Ktn_t);
					CDC_Transmit_FS((unsigned char*)USB_Send_buf, strlen(USB_Send_buf));
					break;
				}
				case 7:
				{
					sprintf(USB_Send_buf, "7,%u,%u,%u,%d,%d,%d,%d", (unsigned int)(_K_IA*10000), (unsigned int)(_K_IC*10000), (unsigned int)(_K_UA*10000), _B_IA, _B_IC, _B_UA, _SERIAL_NBR);
					CDC_Transmit_FS((unsigned char*)USB_Send_buf, strlen(USB_Send_buf));
					break;
				}
				case 8:
				{
					sprintf(USB_Send_buf, "8,%u,%u,%u,%u", Debug_mode, IrmsA_d, IrmsC_d, VrmsA_d);
					CDC_Transmit_FS((unsigned char*)USB_Send_buf, strlen(USB_Send_buf));
					break;
				}
				case 102:
				{
					sprintf(USB_Send_buf, "103,%u ", LogMask);
					CDC_Transmit_FS((unsigned char*)USB_Send_buf, strlen(USB_Send_buf));
					SendLog = true;
					SendLogPos = 0;
					break;
				}
			}
		}
		if(!memcmp(USB_Read_buf, "cmd", 3))
		{
			switch(USB_Read_buf[4])
			{
				case '0':
				{
					for(uint8_t i = 0; i < 50; i++) {
						LogBuf[i].TimeStamp = 0;
						LogBuf[i].mSec = 0;
						LogBuf[i].Event_nbr = 0;
						LogBuf[i].Event_param = 0;
					}
					LogPos = 0;
					break;
				}
			}
		}
	}
}
