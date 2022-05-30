#include "main.h"
#include "eeprom24xx.h"
#include "nct75.h"
#include "Iec104.h"

const unsigned long MB_BR[5] = {9600, 19200, 38400, 57600, 115200};

uint16_t _SERIAL_NBR = 0;
float _K_IA = 0.1045, _K_IC = 0.1045, _K_UA = 0.239;
int16_t _B_IA = 0, _B_IC = 0, _B_UA = 0;
uint16_t Imsz1 = 5, Tmsz1 = 60, Imsz2 = 50, Tmsz2 = 10, Isv = 100, Tsv = 1;
uint8_t APV_Tgot = 10, APV_T1 = 20, APV_T2 = 10, ZMN_Lvl = 80, ZMN_Toff = 10, ZMN_Ton = 20;
bool MSZ1vv = true, SVvv = true, APV_T2vv = true, ZMN_Ton_vv = false;
bool APVvv = true, ZMNvv = false, DUvv = true;
uint8_t MAX7219_Brightness = 13;
uint8_t MB1_adress = 1, MB1_speed = 1, MB2_adress = 1, MB2_speed = 1;
wiz_NetInfo net_info = { .mac = {0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef},
                            .ip = {192, 168, 0, 10},
                            .sn = {255, 255, 255, 0},
                            .gw = {192, 168, 0, 1},
                            .dns = {8, 8, 8, 8},
                            .dhcp = NETINFO_STATIC };
uint16_t IEC104_Port = 2404;
uint16_t IEC104_ASDU = 0;
uint16_t Ubazovoe = 220;
bool SW_auto = false;
int8_t TimeZone = 2;
uint8_t Ktt = 10;
float Ktn = 45.46;
uint16_t VV_On_cnt = 0, Reset_cnt = 0;
uint32_t ModBus1_cnt = 0, ModBus2_cnt = 0, IEC104_cnt = 0;
uint8_t LogMask = 4;

uint8_t I2C_Send_Buf[128];

uint8_t CRC8(uint8_t * buf, uint8_t cnt) {
	uint8_t res = 0;
	
	for(uint8_t i = 0; i < cnt; i++) {
		res += *buf++;
	}
	return res;
}

void Settings_Init(void) {
	
	if(!NCT75_IsConnected()) {
		Internal_Faults |= 4;
	}

	if(!EEPROM24XX_Load(0, I2C_Send_Buf, 2)) {
		Internal_Faults |= 8;
		return;
	}
	
	EEPROM24XX_Load(0, I2C_Send_Buf, 16);
	if(I2C_Send_Buf[15] == CRC8(I2C_Send_Buf, 15)) {
		_SERIAL_NBR = ((uint16_t)I2C_Send_Buf[0] << 8) + I2C_Send_Buf[1];
		_K_IA = (float)(((uint16_t)I2C_Send_Buf[2] << 8) + I2C_Send_Buf[3]) / 100000;
		_K_IC = (float)(((uint16_t)I2C_Send_Buf[4] << 8) + I2C_Send_Buf[5]) / 100000;
		_K_UA = (float)(((uint16_t)I2C_Send_Buf[6] << 8) + I2C_Send_Buf[7]) / 100000;
		_B_IA = (int16_t)(((uint16_t)I2C_Send_Buf[8] << 8) + I2C_Send_Buf[9]) - 32768;
		_B_IC = (int16_t)(((uint16_t)I2C_Send_Buf[10] << 8) + I2C_Send_Buf[11]) - 32768;
		_B_UA = (int16_t)(((uint16_t)I2C_Send_Buf[12] << 8) + I2C_Send_Buf[13]) - 32768;
	} else {
		Internal_Faults |= 2;
		Save_Settings1_f = true;
	}

	EEPROM24XX_Load(16, I2C_Send_Buf, 64);
	if((I2C_Send_Buf[63] == CRC8(I2C_Send_Buf, 63)) && (I2C_Send_Buf[62] == _SETTINGS_VER)) {
		Imsz1 = ((uint16_t)I2C_Send_Buf[0] << 8) + I2C_Send_Buf[1];
		Tmsz1 = ((uint16_t)I2C_Send_Buf[2] << 8) + I2C_Send_Buf[3];
		Imsz2 = ((uint16_t)I2C_Send_Buf[4] << 8) + I2C_Send_Buf[5];
		Tmsz2 = ((uint16_t)I2C_Send_Buf[6] << 8) + I2C_Send_Buf[7];
		Isv = ((uint16_t)I2C_Send_Buf[8] << 8) + I2C_Send_Buf[9];
		Tsv = ((uint16_t)I2C_Send_Buf[10] << 8) + I2C_Send_Buf[11];
		APV_Tgot = I2C_Send_Buf[12];
		APV_T1 = I2C_Send_Buf[13];
		APV_T2 = I2C_Send_Buf[14];
		ZMN_Lvl = I2C_Send_Buf[15];
		ZMN_Toff = I2C_Send_Buf[16];
		ZMN_Ton = I2C_Send_Buf[17];
		MAX7219_Brightness = I2C_Send_Buf[18];
		Ktt = I2C_Send_Buf[19];
		uint16_t Ktn_tmp =((uint16_t)I2C_Send_Buf[20] << 8) + I2C_Send_Buf[21];
		Ktn = (float)Ktn_tmp / 100;
		TimeZone = (int8_t)I2C_Send_Buf[22] - 100;
		Ubazovoe = ((uint16_t)I2C_Send_Buf[23] << 8) + I2C_Send_Buf[24];
		if(I2C_Send_Buf[25] & 1) {
			SW_auto = true;
		} else {
			SW_auto = false;
		}
		APVvv = false;
		ZMNvv = false;
		DUvv = false;
		MSZ1vv = false;
		SVvv = false;
		APV_T2vv = false;
		ZMN_Ton_vv = false;
		if(I2C_Send_Buf[26] & 1) {
			APVvv = true;
		}
		if(I2C_Send_Buf[26] & 2) {
			ZMNvv = true;
		}
		if(I2C_Send_Buf[26] & 4) {
			DUvv = true;
		}
		if(I2C_Send_Buf[26] & 8) {
			MSZ1vv = true;
		}
		if(I2C_Send_Buf[26] & 16) {
			SVvv = true;
		}
		if(I2C_Send_Buf[26] & 32) {
			APV_T2vv = true;
		}
		if(I2C_Send_Buf[26] & 64) {
			ZMN_Ton_vv = true;
		}
		MB1_adress = (uint8_t)I2C_Send_Buf[27];
		MB1_speed = (uint8_t)I2C_Send_Buf[28];
		MB2_adress = (uint8_t)I2C_Send_Buf[29];
		MB2_speed = (uint8_t)I2C_Send_Buf[30];
		net_info.ip[0] = (uint8_t)I2C_Send_Buf[31];
		net_info.ip[1] = (uint8_t)I2C_Send_Buf[32];
		net_info.ip[2] = (uint8_t)I2C_Send_Buf[33];
		net_info.ip[3] = (uint8_t)I2C_Send_Buf[34];
		net_info.sn[0] = (uint8_t)I2C_Send_Buf[35];
		net_info.sn[1] = (uint8_t)I2C_Send_Buf[36];
		net_info.sn[2] = (uint8_t)I2C_Send_Buf[37];
		net_info.sn[3] = (uint8_t)I2C_Send_Buf[38];
		net_info.gw[0] = (uint8_t)I2C_Send_Buf[39];
		net_info.gw[1] = (uint8_t)I2C_Send_Buf[40];
		net_info.gw[2] = (uint8_t)I2C_Send_Buf[41];
		net_info.gw[3] = (uint8_t)I2C_Send_Buf[42];
		IEC104_Port = ((uint16_t)I2C_Send_Buf[43] << 8) + I2C_Send_Buf[44];
		IEC104_ASDU = ((uint16_t)I2C_Send_Buf[45] << 8) + I2C_Send_Buf[46];
		LogMask = (uint8_t)I2C_Send_Buf[47];
	} else {
		Internal_Faults |= 2;
		Save_Settings2_f = true;
	}

	EEPROM24XX_Load(96, I2C_Send_Buf, 32);
	if(I2C_Send_Buf[31] == CRC8(I2C_Send_Buf, 31)) {
		Reset_cnt = ((uint16_t)I2C_Send_Buf[0] << 8) + (uint16_t)I2C_Send_Buf[1];
		ModBus1_cnt = ((uint32_t)I2C_Send_Buf[2] << 24) + ((uint32_t)I2C_Send_Buf[3] << 16) + ((uint32_t)I2C_Send_Buf[4] << 8) + (uint32_t)I2C_Send_Buf[5];
		ModBus2_cnt = ((uint32_t)I2C_Send_Buf[6] << 24) + ((uint32_t)I2C_Send_Buf[7] << 16) + ((uint32_t)I2C_Send_Buf[8] << 8) + (uint32_t)I2C_Send_Buf[9];
		IEC104_cnt = ((uint32_t)I2C_Send_Buf[10] << 24) + ((uint32_t)I2C_Send_Buf[11] << 16) + ((uint32_t)I2C_Send_Buf[12] << 8) + (uint32_t)I2C_Send_Buf[13];
		Reset_cnt++;
		Save_Counts_f = true;
	} else {
		Internal_Faults |= 2;
		Save_Counts_f = true;
	}

	EEPROM24XX_Load(176, I2C_Send_Buf, 16);
	if(I2C_Send_Buf[15] == CRC8(I2C_Send_Buf, 15)) {
		OZZ_state = I2C_Send_Buf[0];
		VV_On_cnt = ((unsigned int)I2C_Send_Buf[1] << 8) + I2C_Send_Buf[2];
		if(I2C_Send_Buf[3] & 1) {
			VV_Block_f = true;
		} else {
			VV_Block_f = false;
		}
		if(I2C_Send_Buf[3] & 2) {
			MB_WorkZMN = true;
			ZMN_Lvl_blnk = true;
		} else {
			MB_WorkZMN = false;
		}
		VV_Off_State = I2C_Send_Buf[4];
		VV_On_State = I2C_Send_Buf[5];
		IrmsA_poshk = I2C_Send_Buf[6];
		IrmsC_poshk = I2C_Send_Buf[7];
		VrmsA_poshk = I2C_Send_Buf[8];
		MSZ_Worked_flags = I2C_Send_Buf[9];
	} else {
		Internal_Faults |= 2;
		Save_State_f = true;
	}
}

void Settings_Poll() {
	if(!(Internal_Faults & 4)) {
		if(Read_Temp_f) {
			Read_Temp_f = false;
			Temp = NCT75_Read();
		}
	}
	if(!(Internal_Faults & 8)) {
		if(Save_State_f) {
			Save_State_f = false;
			I2C_Send_Buf[0] = OZZ_state;
			I2C_Send_Buf[1] = (uint8_t)(VV_On_cnt >> 8);
			I2C_Send_Buf[2] = (uint8_t)(VV_On_cnt);
			I2C_Send_Buf[3] = 0;
			if(VV_Block_f) {
				I2C_Send_Buf[3] |= 1;
			}
			if(MB_WorkZMN) {
				I2C_Send_Buf[3] |= 2;
			}
			I2C_Send_Buf[4] = VV_Off_State;
			I2C_Send_Buf[5] = VV_On_State;
			I2C_Send_Buf[6] = IrmsA_poshk;
			I2C_Send_Buf[7] = IrmsC_poshk;
			I2C_Send_Buf[8] = VrmsA_poshk;
			I2C_Send_Buf[9] = MSZ_Worked_flags;
			I2C_Send_Buf[10] = 0;
			I2C_Send_Buf[11] = 0;
			I2C_Send_Buf[12] = 0;
			I2C_Send_Buf[13] = 0;
			I2C_Send_Buf[14] = 0;
			I2C_Send_Buf[15] = CRC8(I2C_Send_Buf, 15);
			EEPROM24XX_Save(176, I2C_Send_Buf, 16);
		}
		
		if(Save_Settings1_f) {
			Save_Settings1_f = false;
			I2C_Send_Buf[0] = (uint8_t)((_SERIAL_NBR) >> 8);
			I2C_Send_Buf[1] = (uint8_t)_SERIAL_NBR;
			I2C_Send_Buf[2] = (uint8_t)((short int)(_K_IA * 100000) >> 8);
			I2C_Send_Buf[3] = (uint8_t)((short int)(_K_IA * 100000));
			I2C_Send_Buf[4] = (uint8_t)((short int)(_K_IC * 100000) >> 8);
			I2C_Send_Buf[5] = (uint8_t)((short int)(_K_IC * 100000));
			I2C_Send_Buf[6] = (uint8_t)((short int)(_K_UA * 100000) >> 8);
			I2C_Send_Buf[7] = (uint8_t)((short int)(_K_UA * 100000));
			I2C_Send_Buf[8] = (uint8_t)((_B_IA + 32768) >> 8);
			I2C_Send_Buf[9] = (uint8_t)(_B_IA + 32768);
			I2C_Send_Buf[10] = (uint8_t)((_B_IC + 32768) >> 8);
			I2C_Send_Buf[11] = (uint8_t)(_B_IC + 32768);
			I2C_Send_Buf[12] = (uint8_t)((_B_UA + 32768) >> 8);
			I2C_Send_Buf[13] = (uint8_t)(_B_UA + 32768);
			I2C_Send_Buf[14] = 0;
			I2C_Send_Buf[15] = CRC8(I2C_Send_Buf, 15);
			EEPROM24XX_Save(0, I2C_Send_Buf, 16);
		}

		if(Save_Settings2_f) {
			Save_Settings2_f = false;
			I2C_Send_Buf[0] = (uint8_t)(Imsz1 >> 8);
			I2C_Send_Buf[1] = (uint8_t)(Imsz1);
			I2C_Send_Buf[2] = (uint8_t)(Tmsz1 >> 8);
			I2C_Send_Buf[3] = (uint8_t)(Tmsz1);
			I2C_Send_Buf[4] = (uint8_t)(Imsz2 >> 8);
			I2C_Send_Buf[5] = (uint8_t)(Imsz2);
			I2C_Send_Buf[6] = (uint8_t)(Tmsz2 >> 8);
			I2C_Send_Buf[7] = (uint8_t)(Tmsz2);
			I2C_Send_Buf[8] = (uint8_t)(Isv >> 8);
			I2C_Send_Buf[9] = (uint8_t)(Isv);
			I2C_Send_Buf[10] = (uint8_t)(Tsv >> 8);
			I2C_Send_Buf[11] = (uint8_t)(Tsv);
			I2C_Send_Buf[12] = APV_Tgot;
			I2C_Send_Buf[13] = APV_T1;
			I2C_Send_Buf[14] = APV_T2;
			I2C_Send_Buf[15] = ZMN_Lvl;
			I2C_Send_Buf[16] = ZMN_Toff;
			I2C_Send_Buf[17] = ZMN_Ton;
			I2C_Send_Buf[18] = MAX7219_Brightness;
			I2C_Send_Buf[19] = Ktt;
			uint16_t Ktn_tmp = (uint16_t)(Ktn * 100);
			I2C_Send_Buf[20] = (uint8_t)(Ktn_tmp >> 8);
			I2C_Send_Buf[21] = (uint8_t)(Ktn_tmp);
			I2C_Send_Buf[22] = (uint8_t)(TimeZone + 100);
			I2C_Send_Buf[23] = (uint8_t)(Ubazovoe >> 8);
			I2C_Send_Buf[24] = (uint8_t)(Ubazovoe);
			I2C_Send_Buf[25] = 0;
			if(SW_auto) {
				I2C_Send_Buf[25] = I2C_Send_Buf[2] | 1;
			}
			I2C_Send_Buf[26] = 0;
			if(APVvv) {
				I2C_Send_Buf[26] |= 1;
			}
			if(ZMNvv) {
				I2C_Send_Buf[26] |= 2;
			}
			if(DUvv) {
				I2C_Send_Buf[26] |= 4;
			}
			if(MSZ1vv) {
				I2C_Send_Buf[26] |= 8;
			}
			if(SVvv) {
				I2C_Send_Buf[26] |= 16;
			}
			if(APV_T2vv) {
				I2C_Send_Buf[26] |= 32;
			}
			if(ZMN_Ton_vv) {
				I2C_Send_Buf[26] |= 64;
			}
			I2C_Send_Buf[27] = MB1_adress;
			I2C_Send_Buf[28] = MB1_speed;
			I2C_Send_Buf[29] = MB2_adress;
			I2C_Send_Buf[30] = MB2_speed;
			I2C_Send_Buf[31] = net_info.ip[0];
			I2C_Send_Buf[32] = net_info.ip[1];
			I2C_Send_Buf[33] = net_info.ip[2];
			I2C_Send_Buf[34] = net_info.ip[3];
			I2C_Send_Buf[35] = net_info.sn[0];
			I2C_Send_Buf[36] = net_info.sn[1];
			I2C_Send_Buf[37] = net_info.sn[2];
			I2C_Send_Buf[38] = net_info.sn[3];
			I2C_Send_Buf[39] = net_info.gw[0];
			I2C_Send_Buf[40] = net_info.gw[1];
			I2C_Send_Buf[41] = net_info.gw[2];
			I2C_Send_Buf[42] = net_info.gw[3];
			I2C_Send_Buf[43] = (uint8_t)(IEC104_Port >> 8);
			I2C_Send_Buf[44] = (uint8_t)(IEC104_Port);
			I2C_Send_Buf[45] = (uint8_t)(IEC104_ASDU >> 8);
			I2C_Send_Buf[46] = (uint8_t)(IEC104_ASDU);
			I2C_Send_Buf[47] = LogMask;
			I2C_Send_Buf[48] = 0;
			I2C_Send_Buf[49] = 0;
			I2C_Send_Buf[50] = 0;
			I2C_Send_Buf[51] = 0;
			I2C_Send_Buf[52] = 0;
			I2C_Send_Buf[53] = 0;
			I2C_Send_Buf[54] = 0;
			I2C_Send_Buf[55] = 0;
			I2C_Send_Buf[56] = 0;
			I2C_Send_Buf[57] = 0;
			I2C_Send_Buf[58] = 0;
			I2C_Send_Buf[59] = 0;
			I2C_Send_Buf[60] = 0;
			I2C_Send_Buf[61] = 0;
			I2C_Send_Buf[62] = _SETTINGS_VER;
			I2C_Send_Buf[63] = CRC8(I2C_Send_Buf, 63);
			EEPROM24XX_Save(16, &I2C_Send_Buf[0], 32);
			EEPROM24XX_Save(48, &I2C_Send_Buf[32], 32);
		}
		if(Save_Counts_f) {
			Save_Counts_f = false;
			I2C_Send_Buf[0] = (uint8_t)(Reset_cnt >> 8);
			I2C_Send_Buf[1] = (uint8_t)Reset_cnt;
			I2C_Send_Buf[2] = (uint8_t)(ModBus1_cnt >> 24);
			I2C_Send_Buf[3] = (uint8_t)(ModBus1_cnt >> 16);
			I2C_Send_Buf[4] = (uint8_t)(ModBus1_cnt >> 8);
			I2C_Send_Buf[5] = (uint8_t)ModBus1_cnt;
			I2C_Send_Buf[6] = (uint8_t)(ModBus2_cnt >> 24);
			I2C_Send_Buf[7] = (uint8_t)(ModBus2_cnt >> 16);
			I2C_Send_Buf[8] = (uint8_t)(ModBus2_cnt >> 8);
			I2C_Send_Buf[9] = (uint8_t)ModBus2_cnt;
			I2C_Send_Buf[10] = (uint8_t)(IEC104_cnt >> 24);
			I2C_Send_Buf[11] = (uint8_t)(IEC104_cnt >> 16);
			I2C_Send_Buf[12] = (uint8_t)(IEC104_cnt >> 8);
			I2C_Send_Buf[13] = (uint8_t)IEC104_cnt;
			I2C_Send_Buf[14] = 0;
			I2C_Send_Buf[15] = 0;
			I2C_Send_Buf[16] = 0;
			I2C_Send_Buf[17] = 0;
			I2C_Send_Buf[18] = 0;
			I2C_Send_Buf[19] = 0;
			I2C_Send_Buf[20] = 0;
			I2C_Send_Buf[21] = 0;
			I2C_Send_Buf[22] = 0;
			I2C_Send_Buf[23] = 0;
			I2C_Send_Buf[24] = 0;
			I2C_Send_Buf[25] = 0;
			I2C_Send_Buf[26] = 0;
			I2C_Send_Buf[27] = 0;
			I2C_Send_Buf[28] = 0;
			I2C_Send_Buf[29] = 0;
			I2C_Send_Buf[30] = 0;
			I2C_Send_Buf[31] = CRC8(I2C_Send_Buf, 31);
			EEPROM24XX_Save(96, &I2C_Send_Buf[0], 32);
		}
	}
}

void Set_Imsz1(uint16_t new_val) {
	if((new_val >= Imsz1_MIN) && (new_val <= Imsz1_MAX))
	{
		Imsz1 = new_val;
		IEC104_BuildDetectF_Spont(1, (float)Imsz1, IEC104_INFOADDR_VALUE_HXGF+8);
		Save_Settings_Del_f = true;
	}
}

void Set_Tmsz1(uint16_t new_val) {
	if(((new_val > Tmsz1_MIN) || (new_val == Tmsz1_MIN)) && (new_val <= Tmsz1_MAX))
	{
		Tmsz1 = new_val;
		IEC104_BuildDetectF_Spont(1, (float)Tmsz1, IEC104_INFOADDR_VALUE_HXGF+9);
		Save_Settings_Del_f = true;
	}
}

void Set_Imsz2(uint16_t new_val) {
	if((new_val >= Imsz2_MIN) && (new_val <= Imsz2_MAX))
	{
		Imsz2 = new_val;
		IEC104_BuildDetectF_Spont(1, (float)Imsz2, IEC104_INFOADDR_VALUE_HXGF+10);
		Save_Settings_Del_f = true;
	}
}

void Set_Tmsz2(uint16_t new_val) {
	if(((new_val > Tmsz2_MIN) || (new_val == Tmsz2_MIN)) && (new_val <= Tmsz2_MAX))
	{
		Tmsz2 = new_val;
		IEC104_BuildDetectF_Spont(1, (float)Tmsz2/10, IEC104_INFOADDR_VALUE_HXGF+11);
		Save_Settings_Del_f = true;
	}
}

void Set_Isv(uint16_t new_val) {
	if((new_val >= Isv_MIN) && (new_val <= Isv_MAX))
	{
		Isv = new_val;
		IEC104_BuildDetectF_Spont(1, (float)Isv, IEC104_INFOADDR_VALUE_HXGF+12);
		Save_Settings_Del_f = true;
	}
}

void Set_Tsv(uint16_t new_val) {
	if(((new_val > Tsv_MIN) || (new_val == Tsv_MIN)) && (new_val <= Tsv_MAX))
	{
		Tsv = new_val;
		IEC104_BuildDetectF_Spont(1, (float)Tsv/100, IEC104_INFOADDR_VALUE_HXGF+13);
		Save_Settings_Del_f = true;
	}
}

void Set_Tgot(uint16_t new_val) {
	if((new_val >= APV_Tgot_MIN) && (new_val <= APV_Tgot_MAX))
	{
		APV_Tgot = new_val;
		IEC104_BuildDetectF_Spont(1, (float)APV_Tgot, IEC104_INFOADDR_VALUE_HXGF+14);
		Save_Settings_Del_f = true;
	}
}

void Set_APVt1(uint16_t new_val) {
	if((new_val >= APV1_T_MIN) && (new_val <= APV1_T_MAX))
	{
		APV_T1 = new_val;
		IEC104_BuildDetectF_Spont(1, (float)APV_T1/10, IEC104_INFOADDR_VALUE_HXGF+15);
		Save_Settings_Del_f = true;
	}
}

void Set_APVt2(uint16_t new_val) {
	if((new_val >= APV2_T_MIN) && (new_val <= APV2_T_MAX))
	{
		APV_T2 = new_val;
		IEC104_BuildDetectF_Spont(1, (float)APV_T2, IEC104_INFOADDR_VALUE_HXGF+16);
		Save_Settings_Del_f = true;
	}
}

void Set_ZMNlvl(uint16_t new_val) {
	if((new_val >= ZMN_level_MIN) && (new_val <= ZMN_level_MAX))
	{
		ZMN_Lvl = new_val;
		IEC104_BuildDetectF_Spont(1, (float)ZMN_Lvl, IEC104_INFOADDR_VALUE_HXGF+17);
		Save_Settings_Del_f = true;
	}
}

void Set_ZMNoff(uint16_t new_val) {
	if((new_val >= ZMN_Tvidkl_MIN) && (new_val <= ZMN_Tvidkl_MAX))
	{
		ZMN_Toff = new_val;
		IEC104_BuildDetectF_Spont(1, (float)ZMN_Toff, IEC104_INFOADDR_VALUE_HXGF+18);
		Save_Settings_Del_f = true;
	}
}

void Set_ZMNon(uint16_t new_val) {
	if((new_val >= ZMN_Tvkl_MIN) && (new_val <= ZMN_Tvkl_MAX))
	{
		ZMN_Ton = new_val;
		IEC104_BuildDetectF_Spont(1, (float)ZMN_Ton, IEC104_INFOADDR_VALUE_HXGF+19);
		Save_Settings_Del_f = true;
	}
}

void Set_MSZ1vv(bool new_val) {
	MSZ1vv = new_val;
	IEC104_BuildSignal_Spon(1, MSZ1vv, IEC104_INFOADDR_STATE_HXGF+32);//133 
	Save_Settings_Del_f = true;
}

void Set_SVvv(bool new_val) {
	SVvv = new_val;
	IEC104_BuildSignal_Spon(1, SVvv, IEC104_INFOADDR_STATE_HXGF+33);//134 
	Save_Settings_Del_f = true;
}

void Set_APV_T2vv(bool new_val) {
	APV_T2vv = new_val;
	IEC104_BuildSignal_Spon(1, APV_T2vv, IEC104_INFOADDR_STATE_HXGF+35);//136 
	Save_Settings_Del_f = true;
}

void Set_ZMN_Ton_vv(bool new_val) {
	ZMN_Ton_vv = new_val;
	IEC104_BuildSignal_Spon(1, ZMN_Ton_vv, IEC104_INFOADDR_STATE_HXGF+37);//138 
	Save_Settings_Del_f = true;
}

void Set_ZMNvv(bool new_val) {
	ZMNvv = new_val;
	IEC104_BuildSignal_Spon(1, ZMNvv, IEC104_INFOADDR_STATE_HXGF+36);//137 
	Save_Settings_Del_f = true;
	if(!ZMNvv) {
		MB_PuskZMN = false;
		ZMN_Lvl_blnk = false;
		ZMN_Toff_blnk = false;
		ZMN_Ton_blnk = false;
		Timer_OZZ = 0;
		OZZ_state = 0;
		MB_WorkZMN = false;
		MB_VozvratAPV_ZMN = false;
		MB_PuskAPV_ZMN = false;
		MB_VozvratAPV_ZMN = false;
		MB_WorkAPV_ZMN = false;
		Save_State_f = true;
	}
}

void Set_APVvv(bool new_val) {
	APVvv = new_val;
	IEC104_BuildSignal_Spon(1, APVvv, IEC104_INFOADDR_STATE_HXGF+34);//135 
	Save_Settings_Del_f = true;
}

void Set_DUvv(bool new_val) {
	DUvv = new_val;
	IEC104_BuildSignal_Spon(1, DUvv, IEC104_INFOADDR_STATE_HXGF+38);//139 
	if(!DUvv) {
		Set_APVvv(false);
		Set_ZMNvv(false);
		DUvv_cnt = 43200;
	}	else {
		DUvv_cnt = 0;
	}
	Save_Settings_Del_f = true;
}
