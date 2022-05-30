#include "main.h"
#include "math.h"
#include "Settings.h"
#include "AD7606.h"
#include "Events.h"
#include "Iec104.h"

const float MSZ_gist = 1;
unsigned long VrmsAfiltr = 0, IrmsAfiltr = 0, IrmsCfiltr = 0;
unsigned char MSZ_stage = 0;
unsigned int VrmsA_ist;
unsigned int VrmsAbuf[160], IrmsAbuf[160], IrmsCbuf[160];
unsigned short int ADC_filtr_cnt = 0;
uint16_t VV_Cmd_Timer = 0, WorkAPV_ZMN_Timer = 0;

void VV_OnCmd(uint8_t source) {
	Command_VV_On = true;
	VV_On_Sourse_tmp = source;
	LogWrite(0x08040000 + source);
}

void VV_OffCmd(uint8_t source) {
	Command_VV_Off = true;
	VV_Off_Sourse_tmp = source;
	LogWrite(0x08050000 + source);
}

void PuskMSZ(void) {
}

void PuskMSZ1(void) {
	MB_PuskMSZ1 = true;
	MB_VozvratMSZ1 = false;
	LogWrite(0x04000000);
	IEC104_BuildSignal_Spon(1, MB_PuskMSZ1, IEC104_INFOADDR_STATE_HXGF+10);//111 MSZ-1 Pusk
	osMessagePut(EventsQueueHandle, 0x04000000, osWaitForever);
	PuskMSZ();
	if(WorkAPV_ZMN_Timer != 0) {
		MSZ_state = 1;
		MSZ_Worked_flags |= 1;
		Save_State_f = true;
	}
}

void VozvratMSZ1(void) {
	MB_VozvratMSZ1 = true;
	MB_PuskMSZ1 = false;
	LogWrite(0x04010000);
	IEC104_BuildSignal_Spon(1, MB_VozvratMSZ1, IEC104_INFOADDR_STATE_HXGF+11);//112 MSZ-1 Vozvrat
	osMessagePut(EventsQueueHandle, 0x04010000, osWaitForever);
}

void WorkMSZ1(void) {
	MB_WorkMSZ1 = true;
	MB_PuskMSZ1 = false;
	MB_PuskMSZ2 = false;
	MB_PuskSV = false;
	LogWrite(0x04020000);
	IEC104_BuildSignal_Spon(1, MB_WorkMSZ1, IEC104_INFOADDR_STATE_HXGF+12);//113 MSZ-1 Work
	osMessagePut(EventsQueueHandle, 0x04020000, osWaitForever);
}

void PuskMSZ2(void) {
	MB_PuskMSZ2 = true;
	MB_PuskMSZ1 = false;
	MB_VozvratMSZ2 = false;
	LogWrite(0x04030000);
	IEC104_BuildSignal_Spon(1, MB_PuskMSZ2, IEC104_INFOADDR_STATE_HXGF+13);//114 MSZ-2 Pusk
	osMessagePut(EventsQueueHandle, 0x04030000, osWaitForever);
	PuskMSZ();
	if(WorkAPV_ZMN_Timer != 0) {
		MSZ_state = 1;
		MSZ_Worked_flags |= 2;
		Save_State_f = true;
	}
}

void VozvratMSZ2(void) {
	MB_VozvratMSZ2 = true;
	MB_PuskMSZ2 = false;
	LogWrite(0x04040000);
	IEC104_BuildSignal_Spon(1, MB_VozvratMSZ2, IEC104_INFOADDR_STATE_HXGF+14);//115 MSZ-2 Vozvrat
	osMessagePut(EventsQueueHandle, 0x04040000, osWaitForever);
}

void WorkMSZ2(void) {
	MB_WorkMSZ2 = true;
	MB_PuskMSZ1 = false;
	MB_PuskMSZ2 = false;
	MB_PuskSV = false;
	LogWrite(0x04050000);
	IEC104_BuildSignal_Spon(1, MB_WorkMSZ2, IEC104_INFOADDR_STATE_HXGF+15);//116 MSZ-2 Work
	osMessagePut(EventsQueueHandle, 0x04050000, osWaitForever);
}

void PuskSV(void) {
	MB_PuskSV = true;
	MB_PuskMSZ1 = false;
	MB_PuskMSZ2 = false;
	LogWrite(0x04060000);
	IEC104_BuildSignal_Spon(1, MB_PuskSV, IEC104_INFOADDR_STATE_HXGF+16);//117 SV Pusk
	osMessagePut(EventsQueueHandle, 0x04060000, osWaitForever);
	PuskMSZ();
	if(WorkAPV_ZMN_Timer != 0) {
		MSZ_state = 1;
		MSZ_Worked_flags |= 4;
		Save_State_f = true;
	}
}

void WorkSV(void) {
	MB_WorkSV = true;
	MB_PuskMSZ1 = false;
	MB_PuskMSZ2 = false;
	MB_PuskSV = false;
	LogWrite(0x04070000);
	IEC104_BuildSignal_Spon(1, MB_WorkSV, IEC104_INFOADDR_STATE_HXGF+17);//118 SV Work
	osMessagePut(EventsQueueHandle, 0x04070000, osWaitForever);
}

void GotovAPV(void) {
	LogWrite(0x04080000);
	IEC104_BuildSignal_Spon(1, MB_GotovAPV, IEC104_INFOADDR_STATE_HXGF+18);//119 APV Gotov
	osMessagePut(EventsQueueHandle, 0x04080000, osWaitForever);
}

void PuskAPV1(void) {
	MB_PuskAPV1 = true;
	LogWrite(0x04090000);
	IEC104_BuildSignal_Spon(1, MB_PuskAPV1, IEC104_INFOADDR_STATE_HXGF+19);//120 APV-1 Pusk
	osMessagePut(EventsQueueHandle, 0x04090000, osWaitForever);
}

void WorkAPV1(void) {
	MB_WorkAPV1 = true;
	MB_PuskAPV1 = false;
	LogWrite(0x040A0000);
	IEC104_BuildSignal_Spon(1, MB_WorkAPV1, IEC104_INFOADDR_STATE_HXGF+20);//121 APV-1 Work
	osMessagePut(EventsQueueHandle, 0x040A0000, osWaitForever);
}

void PuskAPV2(void) {
	MB_PuskAPV2 = true;
	LogWrite(0x040B0000);
	IEC104_BuildSignal_Spon(1, MB_PuskAPV2, IEC104_INFOADDR_STATE_HXGF+21);//122 APV-2 Pusk
	osMessagePut(EventsQueueHandle, 0x040B0000, osWaitForever);
}

void WorkAPV2(void) {
	MB_WorkAPV2 = true;
	MB_PuskAPV2 = false;
	LogWrite(0x040C0000);
	IEC104_BuildSignal_Spon(1, MB_WorkAPV2, IEC104_INFOADDR_STATE_HXGF+22);//123 APV-2 Work
	osMessagePut(EventsQueueHandle, 0x040C0000, osWaitForever);
}

void PuskZMN(void) {
	MB_PuskZMN = true;
	MB_VozvratZMN = false;
	LogWrite(0x040D0000);
	IEC104_BuildSignal_Spon(1, MB_PuskZMN, IEC104_INFOADDR_STATE_HXGF+23);//124 ZMN Pusk
	osMessagePut(EventsQueueHandle, 0x040D0000, osWaitForever);
}

void VozvratZMN(void) {
	MB_VozvratZMN = true;
	MB_PuskZMN = false;
	LogWrite(0x040E0000);
	IEC104_BuildSignal_Spon(1, MB_VozvratZMN, IEC104_INFOADDR_STATE_HXGF+24);//125 ZMN Vozvrat
	osMessagePut(EventsQueueHandle, 0x040E0000, osWaitForever);
}

void WorkZMN(void) {
	MB_WorkZMN = true;
	MB_VozvratAPV_ZMN = false;
	MB_PuskZMN = false;
	LogWrite(0x040F0000);
	IEC104_BuildSignal_Spon(1, MB_WorkZMN, IEC104_INFOADDR_STATE_HXGF+25);//126 ZMN Work
	osMessagePut(EventsQueueHandle, 0x040F0000, osWaitForever);
}

void PuskAPV_ZMN(void) {
	MB_PuskAPV_ZMN = true;
	MB_VozvratAPV_ZMN = false;
	LogWrite(0x04100000);
	IEC104_BuildSignal_Spon(1, MB_PuskAPV_ZMN, IEC104_INFOADDR_STATE_HXGF+26);//127 ZMN-APV Pusk
	osMessagePut(EventsQueueHandle, 0x04100000, osWaitForever);
}

void VozvratAPV_ZMN(void) {
	MB_VozvratAPV_ZMN = true;
	MB_PuskAPV_ZMN = false;
	LogWrite(0x04110000);
	IEC104_BuildSignal_Spon(1, MB_VozvratAPV_ZMN, IEC104_INFOADDR_STATE_HXGF+27);//128 ZMN-APV Vozvrat
	osMessagePut(EventsQueueHandle, 0x04110000, osWaitForever);
}

void WorkAPV_ZMN(void) {
	MB_WorkAPV_ZMN = true;
	MB_PuskAPV_ZMN = false;
	WorkAPV_ZMN_Timer = 1000;
	LogWrite(0x04120000);
	IEC104_BuildSignal_Spon(1, MB_WorkAPV_ZMN, IEC104_INFOADDR_STATE_HXGF+28);//129 APV-ZMN Work
	osMessagePut(EventsQueueHandle, 0x04120000, osWaitForever);
}

void VV_Block(void) {
	VV_Block_f = true;
	Save_State_f = true;
	LogWrite(0x08020000);
	IEC104_BuildSignal_Spon(1, 1, IEC104_INFOADDR_STATE_HXGF+0);//101 VV Block
	osMessagePut(EventsQueueHandle, 0x08020000, osWaitForever);
}

void VV_Unblock(void) {
	VV_Block_f = false;
	Save_State_f = true;
	LogWrite(0x08060000);
	IEC104_BuildSignal_Spon(1, 0, IEC104_INFOADDR_STATE_HXGF+0);//101 VV Unblock
	osMessagePut(EventsQueueHandle, 0x08020000, osWaitForever);
}

void Protection_Poll(void) {
	if(RMS_process) {
		unsigned long IrmsAtmp = 0, IrmsCtmp = 0, VrmsAtmp = 0;
		
		for(unsigned char cnt = 0; cnt < 19; cnt++)
		{
			VrmsAbuf[cnt] = VrmsAbuf[cnt + 1];
			VrmsAtmp += VrmsAbuf[cnt];
			IrmsAbuf[cnt] = IrmsAbuf[cnt + 1];
			IrmsAtmp += IrmsAbuf[cnt];
			IrmsCbuf[cnt] = IrmsCbuf[cnt + 1];
			IrmsCtmp += IrmsCbuf[cnt];
		}
		VrmsAbuf[19] = VrmsA_raw * VrmsA_raw;
		IrmsAbuf[19] = IrmsA_raw * IrmsA_raw;
		IrmsCbuf[19] = IrmsC_raw * IrmsC_raw;
		VrmsAtmp += VrmsAbuf[19];
		IrmsAtmp += IrmsAbuf[19];
		IrmsCtmp += IrmsCbuf[19];
		VrmsAfiltr += VrmsAtmp / 20;
		IrmsAfiltr += IrmsAtmp / 20;
		IrmsCfiltr += IrmsCtmp / 20;

		ADC_filtr_cnt++;
		if(ADC_filtr_cnt == 10)
		{
			ADC_filtr_cnt = 0;
			VrmsA_ist = VrmsA;
			if(AD7606_Start_timer == 0)
			{
				if(!Debug_mode)
				{
					IrmsA = round(((sqrt((float)(IrmsAfiltr / 10))) * _K_IA) * Ktt);
					IrmsC = round(((sqrt((float)(IrmsCfiltr / 10))) * _K_IC) * Ktt);
					VrmsA = round(((sqrt((float)(VrmsAfiltr / 10))) * _K_UA) * Ktn);
				}
				else
				{
					if(VV_On_State)
					{
						IrmsA = IrmsA_d;
						IrmsC = IrmsC_d;
					}
					else
					{
						IrmsA = 0;
						IrmsC = 0;
					}
					VrmsA = VrmsA_d;
				}
			}
			else
			{
				IrmsA = 0;
				IrmsC = 0;
				VrmsA = 0;
			}
			if(VrmsA < (unsigned short int)(20 * Ktn))
			{
				VrmsA = 0;
			}
			VrmsAfiltr = 0;
			IrmsAfiltr = 0;
			IrmsCfiltr = 0;

			switch(MSZ_state)
			{
				case 0:
				{
					if(IrmsA > Imsz2)
					{
						if(MSZ2A_timer == 0)
						{
							MSZ2A_timer = Tmsz2 * 100;
							PuskMSZ2();
						}
						if(MSZ_APV_cikl != 0)
						{
							MSZ_state = 1;
							MSZ_Worked_flags &= 255 - 1;
							MSZ_Worked_flags |= 2;
							Save_State_f = true;
						}
					}
					if(IrmsA < (Imsz2 * MSZ_gist))
					{
						if(MSZ2A_timer != 0)
						{
							MSZ2A_timer = 0;
							VozvratMSZ2();
						}
					}
					if(IrmsC > Imsz2)
					{
						if(MSZ2B_timer == 0)
						{
							MSZ2B_timer = Tmsz2 * 100;
							PuskMSZ2();
						}
						if(MSZ_APV_cikl != 0)
						{
							MSZ_state = 1;
							MSZ_Worked_flags &= 255 - 1;
							MSZ_Worked_flags |= 2;
							Save_State_f = true;
						}
					}
					if(IrmsC < (Imsz2 * MSZ_gist))
					{
						if(MSZ2B_timer != 0)
						{
							MSZ2B_timer = 0;
							VozvratMSZ2();
						}
					}

					if(SVvv)
					{
						if(IrmsA > Isv)
						{
							if(MSZsvA_timer == 0)
							{
								MSZsvA_timer = Tsv * 10;
								PuskSV();
							}
							if((MSZ_APV_cikl != 0) || (Tsv == 0))
							{
								MSZ_state = 1;
								MSZ_Worked_flags &= 255 - 3;
								MSZ_Worked_flags |= 4;
								Save_State_f = true;
							}
						}
						if(IrmsA < (Isv * MSZ_gist))
						{
							MSZsvA_timer = 0;
						}
						if(IrmsC > Isv)
						{
							if(MSZsvB_timer == 0)
							{
								MSZsvB_timer = Tsv * 10;
								PuskSV();
							}
							if((MSZ_APV_cikl != 0)  || (Tsv == 0))
							{
								MSZ_state = 1;
								MSZ_Worked_flags &= 255 - 3;
								MSZ_Worked_flags |= 4;
								Save_State_f = true;
							}
						}
						if(IrmsC < (Isv * MSZ_gist))
						{
							MSZsvB_timer = 0;
						}
					}
					if(MSZ1vv)
					{
						if(IrmsA > Imsz1)
						{
							if(Imsz1A == 0)
							{
								MSZ1A_timer = 0;
								PuskMSZ1();
							}
							if((IrmsA > Imsz1A) && (MSZ2A_timer == 0))
							{
								Imsz1A = IrmsA;
								Tmsz1A = (Tmsz1 * 1000) + (((double)(Tmsz2 * 100) - (double)(Tmsz1 * 1000)) / ((double)Imsz2 - (double)Imsz1)) * (IrmsA - Imsz1);
							}
							if(MSZ_APV_cikl != 0)
							{
								MSZ_state = 1;
								MSZ_Worked_flags |= 1;
								Save_State_f = true;
							}
						}
						if(IrmsA < (Imsz1 * MSZ_gist))
						{
							if(Imsz1A != 0)
							{
								Imsz1A = 0;
								VozvratMSZ1();
							}
						}
						if((Imsz1A != 0) && (MSZ1A_timer > Tmsz1A))
						{
							MSZ_state = 1;
							MSZ_Worked_flags |= 1;
							Save_State_f = true;
						}

						if(IrmsC > Imsz1)
						{
							if(Imsz1B == 0)
							{
								MSZ1B_timer = 0;
								PuskMSZ1();
							}
							if((IrmsC > Imsz1B) && (MSZ2B_timer == 0))
							{
								Imsz1B = IrmsC;
								Tmsz1B = (Tmsz1 * 1000) + (((double)(Tmsz2 * 100) - (double)(Tmsz1 * 1000)) / ((double)Imsz2 - (double)Imsz1)) * (IrmsC - Imsz1);
							}
							if(MSZ_APV_cikl != 0)
							{
								MSZ_state = 1;
								MSZ_Worked_flags |= 1;
								Save_State_f = true;
							}
						}
						if(IrmsC < (Imsz1 * MSZ_gist))
						{
							if(Imsz1B != 0)
							{
								Imsz1B = 0;
								VozvratMSZ1();
							}
						}
						if((Imsz1B != 0) && (MSZ1B_timer > Tmsz1B))
						{
							MSZ_state = 1;
							MSZ_Worked_flags |= 1;
							Save_State_f = true;
						}
					}
					break;
				}
				case 1:
				{
					if(MSZ_Worked_flags & 1)
					{
						WorkMSZ1();
					}
					if(MSZ_Worked_flags & 2)
					{
						WorkMSZ2();
					}
					if(MSZ_Worked_flags & 4)
					{
						WorkSV();
					}
					Imsz1A = 0;
					Imsz1B = 0;
					MSZ2A_timer = 0;
					MSZsvA_timer = 0;
					MSZ2B_timer = 0;
					MSZsvB_timer = 0;
					VV_OffCmd(3);
					IrmsA_poshk = IrmsA;
					IrmsC_poshk = IrmsC;
					Save_State_f = true;
					VV_Cmd_Timer = 1000;
					MSZ_state = 2;
					break;
				}
				case 2:
				{
					if((VV_Cmd_Timer == 0) || (VV_State == 0)) {
						if(((ChasGotovn_timer == 0) || (MSZ_APV_cikl == 1) || (MSZ_APV_cikl == 2)) && APVvv && (VV_Faults == 0) && (WorkAPV_ZMN_Timer == 0))
						{
							MSZ_APV_cikl++;
							if(MSZ_APV_cikl == 1)
							{
								MSZ_Worked_flags |= 64;
								Save_State_f = true;
								MSZ_APV_timer = APV_T1 * 100;
								MSZ_state = 3;
								PuskAPV1();
								break;
							}
							if((MSZ_APV_cikl == 2) && APV_T2vv)
							{
								MSZ_Worked_flags |= 128;
								Save_State_f = true;
								MSZ_APV_timer = APV_T2 * 1000;
								MSZ_state = 3;
								PuskAPV2();
								break;
							}
						}
						ChasGotovn_timer = 0;
						VV_Block();
						MSZ_state = 10;
					}
					break;
				}
				case 3:
				{
					if(MSZ_APV_timer == 0)
					{
						if(MB_PuskAPV1)
						{
							WorkAPV1();
						}
						if(MB_PuskAPV2)
						{
							WorkAPV2();
						}
						VV_OnCmd(3);
						MSZ_state = 4;
						VV_Cmd_Timer = 1000;
					}
					break;
				}
				case 4:
				{
					if((VV_Cmd_Timer == 0) || (VV_State == 1)) {
						MSZ_state = 0;
					}
					break;
				}
				case 10:
				{
					
					break;
				}
			}
			if(ZMNvv && !VV_Block_f)
			{
				switch(OZZ_state)
				{
					case 0:
					{
						if((VrmsA < (((unsigned short int)(Ubazovoe * Ktn) * ZMN_Lvl) / 100)) && (VV_On_State) && (!VV_Off_State))
						{
							if(Timer_OZZ == 0)
							{
								Timer_OZZ = ZMN_Toff * 1000;
								PuskZMN();
								ZMN_Toff_blnk = true;
							}
						}
						else
						{
							if(Timer_OZZ != 0)
							{
								Timer_OZZ = 0;
								VozvratZMN();
								ZMN_Toff_blnk = false;
							}
						}
						break;
					}
					case 1:
					{
						ZMN_Lvl_blnk = true;
						WorkZMN();
						ZMN_Toff_blnk = false;
						VrmsA_poshk = VrmsA;
						Save_State_f = true;
						VV_OffCmd(3);
						VV_Cmd_Timer = 1000;
						OZZ_state = 2;
						break;
					}
					case 2:
					{
						if((VV_Cmd_Timer == 0) || (VV_State == 0)) {
							if(!ZMN_Ton_vv)
							{
								OZZ_state = 0;
								VV_Block();
							}
							else
							{
								OZZ_state = 3;
								Save_State_f = true;
							}
						}
						break;
					}
					case 3:
					{
						ZMN_Lvl_blnk = true;
						if(VrmsA > (((unsigned short int)(Ubazovoe * Ktn) * ZMN_Lvl) / 100))
						{
							if(ZMN_Ton_vv)
							{
								if(Timer_OZZ == 0)
								{
									Timer_OZZ = ZMN_Ton * 1000;
									PuskAPV_ZMN();
									ZMN_Ton_blnk = true;
								}
							}
							else
							{
								OZZ_state = 4;
							}
						}
						else
						{
							if(Timer_OZZ != 0)
							{
								Timer_OZZ = 0;
								VozvratAPV_ZMN();
								ZMN_Ton_blnk = false;
							}
						}
						break;
					}
					case 4:
					{
						VV_OnCmd(4);
						WorkAPV_ZMN();
						WorkAPV_ZMN_timer = 1000;
						ZMN_Ton_blnk = false;
						OZZ_state = 5;
						MB_WorkZMN = false;
						Timer_OZZ = ZMN_Toff * 1000;
						break;
					}
					case 5:
					{
						if(VrmsA < (((unsigned short int)(Ubazovoe * Ktn) * ZMN_Lvl) / 100))
						{
							VV_OffCmd(3);
							WorkZMN();
							VrmsA_poshk = VrmsA;
							VV_Block();
						}
						break;
					}
					case 6:
					{
						OZZ_state = 0;
						Save_State_f = true;
						break;
					}
				}
			}
			else
			{
				Timer_OZZ = 0;
				if(OZZ_state != 0)
				{
					OZZ_state = 0;
					Save_State_f = true;
				}
			}
		}
		RMS_process = false;
	}
}
