#include "main.h"
#include "Inputs.h"
#include "Settings.h"
#include "cmsis_os.h"
#include "Iec104.h"
#include "Events.h"
#include "Protection.h"

unsigned short int Control_VV_On_state = 0, Control_VV_Off_state = 0;
uint8_t VV_ReadState_timer = 0;

void VV_on_event(void)
{
	VV_Ready_Timer = 10000;
	VV_Faults &= (255 - 1);
	if(APVvv)
	{
		ChasGotovn_timer = APV_Tgot * 1000;
	}
	OZZ_state = 0;
	Timer_OZZ = 0;
	ZMN_Lvl_blnk = false;
	VV_On_cnt++;
	IEC104_BuildDetectF_Spont(1, (float)VV_On_cnt, IEC104_INFOADDR_VALUE_HXGF+4);//1005 VV On Cnt
	VV_On_Sourse = VV_On_Sourse_tmp;
	VV_On_Sourse_tmp = 0;
	VV_Off_Sourse = 0;
	VV_Off_Sourse_tmp = 0;
	Save_State_f = true;
	LogWrite(0x08000000 + VV_On_Sourse);
	IEC104_BuildSignal_Spon(1, 1, IEC104_INFOADDR_STATE_HXGF+30);//131 VV On
	IEC104_BuildDetectF_Spont(1, (float)VV_Off_Sourse, IEC104_INFOADDR_VALUE_HXGF+20);//1021 Last Off Cmd
	IEC104_BuildDetectF_Spont(1, (float)VV_On_Sourse, IEC104_INFOADDR_VALUE_HXGF+21);//1022 Last On Cmd
}

void VV_off_event(void)
{
	ChasGotovn_timer = 0;
	VV_Off_Sourse = VV_Off_Sourse_tmp;
	VV_Off_Sourse_tmp = 0;
	VV_On_Sourse = 0;
	VV_On_Sourse_tmp = 0;
	Save_State_f = true;
	LogWrite(0x08010000 + VV_Off_Sourse);
	IEC104_BuildSignal_Spon(1, 0, IEC104_INFOADDR_STATE_HXGF+30);//131 VV Off
	IEC104_BuildDetectF_Spont(1, (float)VV_Off_Sourse, IEC104_INFOADDR_VALUE_HXGF+20);//1021  Last Off Cmd
	IEC104_BuildDetectF_Spont(1, (float)VV_On_Sourse, IEC104_INFOADDR_VALUE_HXGF+21);//1022 Last On Cmd
}

void VV_Poll(void) {
	if(VV_Block_f != IEC_VV_Block)
	{
//		IEC104_BuildSignal_Spon(1, VV_Block_f, IEC104_INFOADDR_STATE_HXGF+0);//101 VV Block
		IEC_VV_Block = VV_Block_f;
	}

	if(VV_Faults_old != VV_Faults) {
		LogWrite(0x08030000 + VV_Faults);
		VV_Faults_old = VV_Faults;
	}
	
	if(VV_Faults != 0)
	{
		if(IEC_VV_Fault_timer == 0)
		{
			IEC_VV_Fault_timer = 2000;
		}
	}
	else
	{
		if(IEC_VV_Fault)
		{
			IEC_VV_Fault = false;
//			IEC104_BuildSignal_Spon(1, 0, IEC104_INFOADDR_STATE_HXGF+1);//102 VV Fail none
		}
		IEC_VV_Fault_timer = 0;
	}
	if(MB_PuskMSZ1 != IEC_PuskMSZ1)
	{
//		IEC104_BuildSignal_Spon(1, MB_PuskMSZ1, IEC104_INFOADDR_STATE_HXGF+10);//111 MSZ-1 Pusk
		IEC_PuskMSZ1 = MB_PuskMSZ1;
	}
	if(MB_VozvratMSZ1 != IEC_VozvratMSZ1)
	{
//		IEC104_BuildSignal_Spon(1, MB_VozvratMSZ1, IEC104_INFOADDR_STATE_HXGF+11);//112 MSZ-1 Vozvrat
		IEC_VozvratMSZ1 = MB_VozvratMSZ1;
	}
	if(MB_WorkMSZ1 != IEC_WorkMSZ1)
	{
//		IEC104_BuildSignal_Spon(1, MB_WorkMSZ1, IEC104_INFOADDR_STATE_HXGF+12);//113 MSZ-1 Work
		IEC_WorkMSZ1 = MB_WorkMSZ1;
	}
	if(MB_PuskMSZ2 != IEC_PuskMSZ2)
	{
//		IEC104_BuildSignal_Spon(1, MB_PuskMSZ2, IEC104_INFOADDR_STATE_HXGF+13);//114 MSZ-2 Pusk
		IEC_PuskMSZ2 = MB_PuskMSZ2;
	}
	if(MB_VozvratMSZ2 != IEC_VozvratMSZ2)
	{
//		IEC104_BuildSignal_Spon(1, MB_VozvratMSZ2, IEC104_INFOADDR_STATE_HXGF+14);//115 MSZ-2 Vozvrat
		IEC_VozvratMSZ2 = MB_VozvratMSZ2;
	}
	if(MB_WorkMSZ2 != IEC_WorkMSZ2)
	{
//		IEC104_BuildSignal_Spon(1, MB_WorkMSZ2, IEC104_INFOADDR_STATE_HXGF+15);//116 MSZ-2 Work
		IEC_WorkMSZ2 = MB_WorkMSZ2;
	}
	if(MB_PuskSV != IEC_PuskSV)
	{
//		IEC104_BuildSignal_Spon(1, MB_PuskSV, IEC104_INFOADDR_STATE_HXGF+16);//117 SV Pusk
		IEC_PuskSV = MB_PuskSV;
	}
	if(MB_WorkSV != IEC_WorkSV)
	{
//		IEC104_BuildSignal_Spon(1, MB_WorkSV, IEC104_INFOADDR_STATE_HXGF+17);//118 SV Work
		IEC_WorkSV = MB_WorkSV;
	}
	if(MB_GotovAPV != IEC_GotovAPV)
	{
//		IEC104_BuildSignal_Spon(1, MB_GotovAPV, IEC104_INFOADDR_STATE_HXGF+18);//119 APV Gotov
		IEC_GotovAPV = MB_GotovAPV;
	}
	if(MB_PuskAPV1 != IEC_PuskAPV1)
	{
//		IEC104_BuildSignal_Spon(1, MB_PuskAPV1, IEC104_INFOADDR_STATE_HXGF+19);//120 APV-1 Pusk
		IEC_PuskAPV1 = MB_PuskAPV1;
	}
	if(MB_WorkAPV1 != IEC_WorkAPV1)
	{
//		IEC104_BuildSignal_Spon(1, MB_WorkAPV1, IEC104_INFOADDR_STATE_HXGF+20);//121 APV-1 Work
		IEC_WorkAPV1 = MB_WorkAPV1;
	}
	if(MB_PuskAPV2 != IEC_PuskAPV2)
	{
//		IEC104_BuildSignal_Spon(1, MB_PuskAPV2, IEC104_INFOADDR_STATE_HXGF+21);//122 APV-2 Pusk
		IEC_PuskAPV2 = MB_PuskAPV2;
	}
	if(MB_WorkAPV2 != IEC_WorkAPV2)
	{
//		IEC104_BuildSignal_Spon(1, MB_WorkAPV2, IEC104_INFOADDR_STATE_HXGF+22);//123 APV-2 Work
		IEC_WorkAPV2 = MB_WorkAPV2;
	}
	if(MB_PuskZMN != IEC_PuskZMN)
	{
//		IEC104_BuildSignal_Spon(1, MB_PuskZMN, IEC104_INFOADDR_STATE_HXGF+23);//124 ZMN Pusk
		IEC_PuskZMN = MB_PuskZMN;
	}
	if(MB_VozvratZMN != IEC_VozvratZMN)
	{
//		IEC104_BuildSignal_Spon(1, MB_VozvratZMN, IEC104_INFOADDR_STATE_HXGF+24);//125 ZMN Vozvrat
		IEC_VozvratZMN = MB_VozvratZMN;
	}
	if(MB_WorkZMN != IEC_WorkZMN)
	{
//		IEC104_BuildSignal_Spon(1, MB_WorkZMN, IEC104_INFOADDR_STATE_HXGF+25);//126 ZMN Work
		IEC_WorkZMN = MB_WorkZMN;
	}
	if(MB_PuskAPV_ZMN != IEC_PuskAPV_ZMN)
	{
//		IEC104_BuildSignal_Spon(1, MB_PuskAPV_ZMN, IEC104_INFOADDR_STATE_HXGF+26);//127 ZMN-APV Pusk
		IEC_PuskAPV_ZMN = MB_PuskAPV_ZMN;
	}
	if(MB_VozvratAPV_ZMN != IEC_VozvratAPV_ZMN)
	{
//		IEC104_BuildSignal_Spon(1, MB_VozvratAPV_ZMN, IEC104_INFOADDR_STATE_HXGF+27);//128 ZMN-APV Vozvrat
		IEC_VozvratAPV_ZMN = MB_VozvratAPV_ZMN;
	}
	if(MB_WorkAPV_ZMN != IEC_WorkAPV_ZMN)
	{
//		IEC104_BuildSignal_Spon(1, MB_WorkAPV_ZMN, IEC104_INFOADDR_STATE_HXGF+28);//129 APV-ZMN Work
		IEC_WorkAPV_ZMN = MB_WorkAPV_ZMN;
	}
	
	if(Inputs_State & INPUT_COIL_FAULT)
	{
		if(MB_CoilFault_timer == 0)
		{
			MB_CoilFault_timer = 1000;
		}
	}
	else
	{
		MB_CoilFault_timer = 0;
		VV_Faults &= (255 - 32);
	}
	if(ChasGotovn_timer != 0)
	{
		MB_GotovAPV = false;
	}
	else
	{
		if(!MB_GotovAPV & APVvv) {
			MB_GotovAPV = true;
		}
	}
	if(VV_Unblock_f)
	{
		if(ZMN_Lvl_blnk)
		{
			OZZ_state = 0;
			Timer_OZZ = 0;
			ZMN_Lvl_blnk = false;
			ZMN_Toff_blnk = false;
			ZMN_Ton_blnk = false;
		}
		if(VV_Block_f)
		{
			VV_Unblock();
			MSZ_state = 0;
			MSZ_APV_cikl = 0;
			Timer_OZZ = 0;
			MSZ_Worked_flags = 0;
			Imsz1_blnk = false; Tmsz1_blnk = false; Imsz2_blnk = false; Tmsz2_blnk = false; Isv_blnk = false; Tsv_blnk = false; APV_Tgot_blnk = false; APV_T1_blnk = false; APV_T2_blnk = false; ZMN_Lvl_blnk = false; ZMN_Toff_blnk = false; ZMN_Ton_blnk = false;
		}
		MB_WorkMSZ1 = false;
		MB_WorkMSZ2 = false;
		MB_WorkSV = false;
		MB_PuskMSZ1 = false;
		MB_VozvratMSZ1 = false;
		MB_PuskMSZ2 = false;
		MB_VozvratMSZ2 = false;
		MB_PuskSV = false;
		MB_PuskAPV1 = false;
		MB_WorkAPV1 = false;
		MB_PuskAPV2 = false;
		MB_WorkAPV2 = false;
		MB_PuskZMN = false;
		MB_VozvratZMN = false;
		MB_WorkZMN = false;
		MB_PuskAPV_ZMN = false;
		MB_VozvratAPV_ZMN = false;
		MB_WorkAPV_ZMN = false;
		VV_Faults &= (255 - 8);
		VV_Faults &= (255 - 16);
		VV_Unblock_f = false;
	}
	if((!VV_On_State && VV_Off_State))
	{
		if(VV_State != 0)
		{
			VV_State = 0;
//			IEC104_BuildSignal_Spon(1, 0, IEC104_INFOADDR_STATE_HXGF+30);//131 VV Off
//			IEC104_BuildSignal_Spon(1, 0, IEC104_INFOADDR_STATE_HXGF+31);//132 VV Fail None
		}
	}
	else
	{
		if((VV_On_State && !VV_Off_State))
		{
			if(VV_State != 1)
			{
				VV_State = 1;
//				IEC104_BuildSignal_Spon(1, 1, IEC104_INFOADDR_STATE_HXGF+30);//131 VV On
//				IEC104_BuildSignal_Spon(1, 0, IEC104_INFOADDR_STATE_HXGF+31);//132 VV Fail None
			}
		}
		else
		{
			if(VV_State != 2)
			{
				VV_State = 2;
//				IEC104_BuildSignal_Spon(1, 1, IEC104_INFOADDR_STATE_HXGF+31);//132 VV Fail
			}
		}
	}
	if(((VV_Ready_Timer == 0) && !VV_Ready_State))
	{
		VV_ReadyFault_Timer = 20;
	}
	else
	{
		VV_Faults &= (255 - 1);
	}
	if(((IrmsA > 3) && !VV_On_State) || ((IrmsC > 3) && !VV_On_State))
	{
		VV_Faults |= 4;
	}
	else
	{
		VV_Faults &= (255 - 4);
	}
	if(VV_Faults != 0)
	{
		MAX7219_LED_Nespr = true;
	}
	else
	{
		MAX7219_LED_Nespr = false;
	}
	if(VV_Block_f)
	{
		MAX7219_LED_Block = true;
	}
	else
	{
		MAX7219_LED_Block = false;
	}

	if(Command_VV_On)
	{
		Command_VV_On = false;
		if(!VV_On_State && (VV_Faults == 0) && (!VV_Block_f))
		{
			HAL_GPIO_WritePin(OUT_VV_ON_RELE_GPIO_Port, OUT_VV_ON_RELE_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(OUT_VV_ON_MSFT_GPIO_Port, OUT_VV_ON_MSFT_Pin, GPIO_PIN_SET);
			osDelay(300);
			HAL_GPIO_WritePin(OUT_VV_ON_RELE_GPIO_Port, OUT_VV_ON_RELE_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(OUT_VV_ON_MSFT_GPIO_Port, OUT_VV_ON_MSFT_Pin, GPIO_PIN_RESET);
			Control_VV_On_state = 1;
		}
		VV_On_Sourse_tmp = 0;
	}
	if(Command_VV_Off)
	{
		Command_VV_Off = false;
		Command_VV_On = false;
		HAL_GPIO_WritePin(OUT_VV_OFF_RELE_GPIO_Port, OUT_VV_OFF_RELE_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(OUT_VV_OFF_MSFT_GPIO_Port, OUT_VV_OFF_MSFT_Pin, GPIO_PIN_SET);
		osDelay(300);
		HAL_GPIO_WritePin(OUT_VV_OFF_RELE_GPIO_Port, OUT_VV_OFF_RELE_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(OUT_VV_OFF_MSFT_GPIO_Port, OUT_VV_OFF_MSFT_Pin, GPIO_PIN_RESET);
		Control_VV_Off_state = 1;
		MB_WorkAPV1 = false;
		MB_WorkAPV2 = false;
		MB_WorkAPV_ZMN = false;
		VV_Off_Sourse_tmp = 0;
	}
	if(!(Inputs_State & INPUT_DOOR))
	{
		if(VV_On_State)
		{
			HAL_GPIO_WritePin(LED_VV_ON_GPIO_Port, LED_VV_ON_Pin, GPIO_PIN_SET);
		}
		else
		{
			HAL_GPIO_WritePin(LED_VV_ON_GPIO_Port, LED_VV_ON_Pin, GPIO_PIN_RESET);
		}
		if(VV_Off_State)
		{
			HAL_GPIO_WritePin(LED_VV_OFF_GPIO_Port, LED_VV_OFF_Pin, GPIO_PIN_SET);
		}
		else
		{
			HAL_GPIO_WritePin(LED_VV_OFF_GPIO_Port, LED_VV_OFF_Pin, GPIO_PIN_RESET);
		}
	}
	else
	{
		HAL_GPIO_WritePin(LED_VV_ON_GPIO_Port, LED_VV_ON_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED_VV_OFF_GPIO_Port, LED_VV_OFF_Pin, GPIO_PIN_RESET);
	}
	if(MAX7219_ChkMode)
	{
		HAL_GPIO_WritePin(LED_VV_ON_GPIO_Port, LED_VV_ON_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED_VV_OFF_GPIO_Port, LED_VV_OFF_Pin, GPIO_PIN_SET);
	}
	switch(Control_VV_Off_state)
	{
		case 1:
		{
			Control_VV_On_state = 0;
			Control_VV_On_timer = 0;
			Control_VV_Off_timer = 500;
			Control_VV_Off_state++;
			break;
		}
		case 2:
		{
			if(Control_VV_Off_timer == 0)
			{
				if(!VV_On_State)
				{
					VV_Faults &= (255 - 8);
				}
				else
				{
					VV_Faults |= 8;
				}
				Control_VV_Off_state = 0;
			}
			break;
		}
	}
	switch(Control_VV_On_state)
	{
		case 1:
		{
			Control_VV_Off_state = 0;
			Control_VV_Off_timer = 0;
			Control_VV_On_timer = 500;
			Control_VV_On_state++;
			break;
		}
		case 2:
		{
			if(Control_VV_On_timer == 0)
			{
				if(VV_On_State)
				{
					VV_Faults &= (255 - 16);
				}
				else
				{
					VV_Faults |= 16;
				}
				Control_VV_On_state = 0;
			}
			break;
		}
	}
}

void VV_Timers(void) {
	if(VV_ReadState_timer != 0) {
		VV_ReadState_timer--;
		if(VV_ReadState_timer == 0) {
			if(!(Inputs_State & 1) && (Inputs_State & 2)) {
				VV_on_event();
			}
			if((Inputs_State & 1) && !(Inputs_State & 2)) {
				VV_off_event();
			}
			if((VV_On_State && VV_Off_State) || (!VV_On_State && !VV_Off_State))
			{
				VV_Faults |= 2;
			}
			else
			{
				VV_Faults &= (255 - 2);
			}
		}
	}
}
