#include "main.h"
#include "AD7606.h"
#include "Settings.h"

uint16_t MAX7219_blink_cnt = 0;
uint8_t MAX7219_IC1[8], MAX7219_IC2[8], MAX7219_IC3[8], MAX7219_IC4[8], MAX7219_IC5[8], MAX7219_IC6[8], MAX7219_IC7[8];
uint16_t MAX7219_Send_Buf[7];
uint8_t MAX7219_Digit_cnt = 0;
uint8_t MAX7219_Brightness_cur = 0;

void MAX7219_Init(void) {
	uint16_t Buf[7];

	Buf[0] = 0x09FF;
	Buf[1] = 0x09FF;
	Buf[2] = 0x09FF;
	Buf[3] = 0x09FF;
	Buf[4] = 0x09FF;
	Buf[5] = 0x09FF;
	Buf[6] = 0x090F;
	HAL_GPIO_WritePin(MAX7219_CS_GPIO_Port, MAX7219_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi2, (uint8_t*)Buf, 7, 1000);
	HAL_GPIO_WritePin(MAX7219_CS_GPIO_Port, MAX7219_CS_Pin, GPIO_PIN_SET);
	Buf[0] = 0x0B07;
	Buf[1] = 0x0B07;
	Buf[2] = 0x0B07;
	Buf[3] = 0x0B07;
	Buf[4] = 0x0B07;
	Buf[5] = 0x0B07;
	Buf[6] = 0x0B07;
	HAL_GPIO_WritePin(MAX7219_CS_GPIO_Port, MAX7219_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi2, (uint8_t*)Buf, 7, 1000);
	HAL_GPIO_WritePin(MAX7219_CS_GPIO_Port, MAX7219_CS_Pin, GPIO_PIN_SET);
}

void MAX7219_Refresh(void) {
	uint16_t tmp;
	if(MAX7219_ChkMode) {
		MAX7219_IC7[0] = 0x88;
		MAX7219_IC7[1] = 0x88;
		MAX7219_IC7[2] = 0x88;
		MAX7219_IC7[3] = 0x88;
		MAX7219_IC6[0] = 0x88;
		MAX7219_IC6[1] = 0x88;
		MAX7219_IC6[2] = 0x88;
		MAX7219_IC6[3] = 0x88;
		MAX7219_IC6[4] = 0x88;
		MAX7219_IC6[5] = 0x88;
		MAX7219_IC6[6] = 0x88;
		MAX7219_IC6[7] = 0x88;
		MAX7219_IC5[0] = 0x88;
		MAX7219_IC5[1] = 0x88;
		MAX7219_IC5[2] = 0x88;
		MAX7219_IC5[3] = 0x88;
		MAX7219_IC5[4] = 0x88;
		MAX7219_IC5[5] = 0x88;
		MAX7219_IC5[6] = 0x88;
		MAX7219_IC5[7] = 0x88;
		MAX7219_IC4[0] = 0x88;
		MAX7219_IC4[1] = 0x88;
		MAX7219_IC4[2] = 0x88;
		MAX7219_IC4[3] = 0x88;
		MAX7219_IC4[4] = 0x88;
		MAX7219_IC4[5] = 0x88;
		MAX7219_IC4[6] = 0x88;
		MAX7219_IC4[7] = 0x88;
		MAX7219_IC3[0] = 0x88;
		MAX7219_IC3[1] = 0x88;
		MAX7219_IC3[2] = 0x88;
		MAX7219_IC3[3] = 0x88;
		MAX7219_IC3[4] = 0x88;
		MAX7219_IC3[5] = 0x88;
		MAX7219_IC3[6] = 0x88;
		MAX7219_IC3[7] = 0x88;
		MAX7219_IC2[0] = 0x88;
		MAX7219_IC2[1] = 0x88;
		MAX7219_IC2[2] = 0x88;
		MAX7219_IC2[3] = 0x88;
		MAX7219_IC2[4] = 0x88;
		MAX7219_IC2[5] = 0x88;
		MAX7219_IC2[6] = 0x88;
		MAX7219_IC2[7] = 0x88;
		MAX7219_IC1[0] = 0x88;
		MAX7219_IC1[1] = 0x88;
		MAX7219_IC1[2] = 0x88;
		MAX7219_IC1[3] = 0x88;
		MAX7219_IC1[4] = 0x88;
		MAX7219_IC1[5] = 0x88;
		MAX7219_IC1[6] = 0x88;
		MAX7219_IC1[7] = 0x88;
		MAX7219_IC7[4] = 1 + 2 + 4 + 8 + 16 + 128;
		MAX7219_IC7[5] = 4 + 8 + 16 + 32 + 64;
		MAX7219_IC7[6] = 0;
		MAX7219_IC7[7] = 0;
	} else {
		if((MAX7219_blink_cnt < 200) && ((Setup_mode && (Setup_error_flg == 0)) || (Setup_error_flg == 1) || (Setup_error_flg & 1)  || (Imsz1A != 0) || (Imsz1B != 0) || (MSZ_Worked_flags & 1) || Imsz1_blnk)) {
			MAX7219_IC7[0] = 0x0F;
			MAX7219_IC7[1] = 0x0F;
			MAX7219_IC7[2] = 0x0F;
			MAX7219_IC7[3] = 0x0F;
		} else {
			if(Setup_mode) {
				if(MSZ1vv_setup) {
					tmp = Imsz1_setup;
					MAX7219_IC7[0] = tmp / 1000;
					tmp = tmp % 1000;
					MAX7219_IC7[1] = tmp / 100;
					tmp = tmp % 100;
					MAX7219_IC7[2] = tmp / 10;
					MAX7219_IC7[3] = tmp % 10;
				} else {
					MAX7219_IC7[0] = 0x0A;
					MAX7219_IC7[1] = 0x0A;
					MAX7219_IC7[2] = 0x0A;
					MAX7219_IC7[3] = 0x0A;
				}
			} else {
				if(MSZ1vv) {
					tmp = Imsz1;
					MAX7219_IC7[0] = tmp / 1000;
					tmp = tmp % 1000;
					MAX7219_IC7[1] = tmp / 100;
					tmp = tmp % 100;
					MAX7219_IC7[2] = tmp / 10;
					MAX7219_IC7[3] = tmp % 10;
				} else {
					MAX7219_IC7[0] = 0x0A;
					MAX7219_IC7[1] = 0x0A;
					MAX7219_IC7[2] = 0x0A;
					MAX7219_IC7[3] = 0x0A;
				}
			}
		}
		if((MAX7219_blink_cnt < 200) && ((Setup_mode && (Setup_error_flg == 0)) || (Setup_error_flg & 4) || Tmsz1_blnk)) {
			MAX7219_IC6[0] = 0x0F;
			MAX7219_IC6[1] = 0x0F;
			MAX7219_IC6[2] = 0x0F;
			MAX7219_IC6[3] = 0x0F;
		} else {
			if(Setup_mode) {
				if(MSZ1vv_setup) {
					tmp = Tmsz1_setup;
					MAX7219_IC6[0] = tmp / 1000;
					tmp = tmp % 1000;
					MAX7219_IC6[1] = tmp / 100;
					tmp = tmp % 100;
					MAX7219_IC6[2] = tmp / 10;
					MAX7219_IC6[3] = tmp % 10;
				} else {
					MAX7219_IC6[0] = 0x0A;
					MAX7219_IC6[1] = 0x0A;
					MAX7219_IC6[2] = 0x0A;
					MAX7219_IC6[3] = 0x0A;
				}
			} else {
				if(MSZ1vv) {
					tmp = Tmsz1;
					MAX7219_IC6[0] = tmp / 1000;
					tmp = tmp % 1000;
					MAX7219_IC6[1] = tmp / 100;
					tmp = tmp % 100;
					MAX7219_IC6[2] = tmp / 10;
					MAX7219_IC6[3] = tmp % 10;
				} else {
					MAX7219_IC6[0] = 0x0A;
					MAX7219_IC6[1] = 0x0A;
					MAX7219_IC6[2] = 0x0A;
					MAX7219_IC6[3] = 0x0A;
				}
			}
		}
		if((MAX7219_blink_cnt < 200) && ((Setup_mode && (Setup_error_flg == 0)) || (Setup_error_flg & 3) || (MSZ2A_timer != 0) || (MSZ2B_timer != 0) || (MSZ_Worked_flags & 2) || Imsz2_blnk)) {
			MAX7219_IC6[4] = 0x0F;
			MAX7219_IC6[5] = 0x0F;
			MAX7219_IC6[6] = 0x0F;
			MAX7219_IC6[7] = 0x0F;
		} else {
			if(Setup_mode) {
				tmp = Imsz2_setup;
			} else {
				tmp = Imsz2;
			}
			MAX7219_IC6[4] = tmp / 1000;
			tmp = tmp % 1000;
			MAX7219_IC6[5] = tmp / 100;
			tmp = tmp % 100;
			MAX7219_IC6[6] = tmp / 10;
			MAX7219_IC6[7] = tmp % 10;
		}
		if((MAX7219_blink_cnt < 200) && ((Setup_mode && (Setup_error_flg == 0)) || (Setup_error_flg & 12) || Tmsz2_blnk)) {
			MAX7219_IC5[4] = 0x0F;
			MAX7219_IC5[5] = 0x0F;
			MAX7219_IC5[6] = 0x0F;
			MAX7219_IC5[7] = 0x0F;
		} else {
			if(Setup_mode) {
				tmp = Tmsz2_setup;
			} else {
				tmp = Tmsz2;
			}
			MAX7219_IC5[4] = tmp / 1000;
			tmp = tmp % 1000;
			MAX7219_IC5[5] = tmp / 100;
			tmp = tmp % 100;
			MAX7219_IC5[6] = (tmp / 10) + 0x80;
			MAX7219_IC5[7] = tmp % 10;
		}
		if((MAX7219_blink_cnt < 200) && ((Setup_mode && (Setup_error_flg == 0)) || (Setup_error_flg & 2)|| (MSZsvA_timer != 0) || (MSZsvB_timer != 0) || (MSZ_Worked_flags & 4) || Isv_blnk)) {
			MAX7219_IC5[0] = 0x0F;
			MAX7219_IC5[1] = 0x0F;
			MAX7219_IC5[2] = 0x0F;
			MAX7219_IC5[3] = 0x0F;
		} else {
			if(Setup_mode) {
				if(SVvv_setup) {
					tmp = Isv_setup;
					MAX7219_IC5[0] = tmp / 1000;
					tmp = tmp % 1000;
					MAX7219_IC5[1] = tmp / 100;
					tmp = tmp % 100;
					MAX7219_IC5[2] = tmp / 10;
					MAX7219_IC5[3] = tmp % 10;
				} else {
					MAX7219_IC5[0] = 0x0A;
					MAX7219_IC5[1] = 0x0A;
					MAX7219_IC5[2] = 0x0A;
					MAX7219_IC5[3] = 0x0A;
				}
			} else {
				if(SVvv) {
					tmp = Isv;
					MAX7219_IC5[0] = tmp / 1000;
					tmp = tmp % 1000;
					MAX7219_IC5[1] = tmp / 100;
					tmp = tmp % 100;
					MAX7219_IC5[2] = tmp / 10;
					MAX7219_IC5[3] = tmp % 10;
				} else {
					MAX7219_IC5[0] = 0x0A;
					MAX7219_IC5[1] = 0x0A;
					MAX7219_IC5[2] = 0x0A;
					MAX7219_IC5[3] = 0x0A;
				}
			}
		}
		if((MAX7219_blink_cnt < 200) && ((Setup_mode && (Setup_error_flg == 0)) || (Setup_error_flg & 8) || Tsv_blnk)) {
			MAX7219_IC4[4] = 0x0F;
			MAX7219_IC4[5] = 0x0F;
			MAX7219_IC4[6] = 0x0F;
			MAX7219_IC4[7] = 0x0F;
		} else {
			if(Setup_mode) {
				if(SVvv_setup) {
					tmp = Tsv_setup;
					MAX7219_IC4[4] = tmp / 1000;
					tmp = tmp % 1000;
					MAX7219_IC4[5] = (tmp / 100) + 0x80;
					tmp = tmp % 100;
					MAX7219_IC4[6] = tmp / 10;
					MAX7219_IC4[7] = tmp % 10;
				} else {
					MAX7219_IC4[4] = 0x0A;
					MAX7219_IC4[5] = 0x0A;
					MAX7219_IC4[6] = 0x0A;
					MAX7219_IC4[7] = 0x0A;
				}
			} else {
				if(SVvv) {
					tmp = Tsv;
					MAX7219_IC4[4] = tmp / 1000;
					tmp = tmp % 1000;
					MAX7219_IC4[5] = (tmp / 100) + 0x80;
					tmp = tmp % 100;
					MAX7219_IC4[6] = tmp / 10;
					MAX7219_IC4[7] = tmp % 10;
				} else {
					MAX7219_IC4[4] = 0x0A;
					MAX7219_IC4[5] = 0x0A;
					MAX7219_IC4[6] = 0x0A;
					MAX7219_IC4[7] = 0x0A;
				}
			}
		}
		if((MAX7219_blink_cnt < 200) && (Setup2_En || Time_blnk))
		{
			MAX7219_IC4[0] = 0x0F;
			MAX7219_IC4[1] = 0x0F;
			MAX7219_IC4[2] = 0x0F;
			MAX7219_IC4[3] = 0x0F;
		}
		else
		{
			switch(Setup2_View)
			{
				case 0:
				{
					MAX7219_IC4[0] = Hours / 10;
					MAX7219_IC4[1] = Hours % 10;
					if(MAX7219_blink_cnt > 200)
					{
						MAX7219_IC4[1] += 0x80;
					}
					MAX7219_IC4[2] = Minutes / 10;
					MAX7219_IC4[3] = Minutes % 10;
					break;
				}
				case 1:
				{
					tmp = (unsigned short int)(VrmsA / 1000);
					MAX7219_IC4[0] = tmp / 1000;
					tmp = tmp % 1000;
					MAX7219_IC4[1] = tmp / 100;
					tmp = tmp % 100;
					MAX7219_IC4[2] = tmp / 10;
					MAX7219_IC4[3] = tmp % 10;
					break;
				}
				case 2:
				{
					if(Ktt == 10)
					{
						tmp = 505;
					}
					if(Ktt == 20)
					{
						tmp = 1005;
					}
					if(Ktt == 30)
					{
						tmp = 1505;
					}
					if(Ktt == 40)
					{
						tmp = 2005;
					}
					if(Ktt == 80)
					{
						tmp = 4005;
					}
					if(Ktt == 120)
					{
						tmp = 6005;
					}
					MAX7219_IC4[0] = tmp / 1000;
					tmp = tmp % 1000;
					MAX7219_IC4[1] = tmp / 100;
					tmp = tmp % 100;
					MAX7219_IC4[2] = (tmp / 10) + 0x80;
					MAX7219_IC4[3] = tmp % 10;
					break;
				}
				case 3:
				{
					tmp = MB1_adress;
					MAX7219_IC4[0] = tmp / 1000;
					tmp = tmp % 1000;
					MAX7219_IC4[1] = tmp / 100;
					tmp = tmp % 100;
					MAX7219_IC4[2] = tmp / 10;
					MAX7219_IC4[3] = tmp % 10;
					break;
				}
				case 4:
				{
					tmp = (unsigned int)(MB_BR[MB1_speed] / 100);
					MAX7219_IC4[0] = tmp / 1000;
					tmp = tmp % 1000;
					MAX7219_IC4[1] = tmp / 100;
					tmp = tmp % 100;
					MAX7219_IC4[2] = tmp / 10;
					MAX7219_IC4[3] = tmp % 10;
					break;
				}
			}
		}
		if((MAX7219_blink_cnt < 200) && ((Setup_mode && (Setup_error_flg == 0)) || (ChasGotovn_timer != 0) || APV_Tgot_blnk)) {
			MAX7219_IC3[0] = 0x0F;
			MAX7219_IC3[1] = 0x0F;
		} else {
			if(Setup_mode) {
				tmp = APV_Tgot_setup;
				MAX7219_IC3[0] = tmp / 10;
				MAX7219_IC3[1] = tmp % 10;
			} else {
				tmp = APV_Tgot;
				MAX7219_IC3[0] = tmp / 10;
				MAX7219_IC3[1] = tmp % 10;
			}
		}
		if((MAX7219_blink_cnt < 200) && ((Setup_mode && (Setup_error_flg == 0)) || (MSZ_Worked_flags & 64) || APV_T1_blnk)) {
			MAX7219_IC3[2] = 0x0F;
			MAX7219_IC3[3] = 0x0F;
		} else {
			if(Setup_mode) {
				tmp = APV_T1_setup;
				MAX7219_IC3[2] = (tmp / 10) + 0x80;
				MAX7219_IC3[3] = tmp % 10;
			} else {
				tmp = APV_T1;
				MAX7219_IC3[2] = (tmp / 10) + 0x80;
				MAX7219_IC3[3] = tmp % 10;
			}
		}
		if((MAX7219_blink_cnt < 200) && ((Setup_mode && (Setup_error_flg == 0)) || (MSZ_Worked_flags & 128) || APV_T2_blnk)) {
			MAX7219_IC3[4] = 0x0F;
			MAX7219_IC3[5] = 0x0F;
		} else {
			if(Setup_mode) {
				if(APV_T2vv_setup) {
					tmp = APV_T2_setup;
					MAX7219_IC3[4] = tmp / 10;
					MAX7219_IC3[5] = tmp % 10;
				} else {
					MAX7219_IC3[4] = 0x0A;
					MAX7219_IC3[5] = 0x0A;
				}
			} else {
				if(APV_T2vv) {
					tmp = APV_T2;
					MAX7219_IC3[4] = tmp / 10;
					MAX7219_IC3[5] = tmp % 10;
				} else {
					MAX7219_IC3[4] = 0x0A;
					MAX7219_IC3[5] = 0x0A;
				}
			}
		}
		MAX7219_IC3[6] = 0x0F;
		MAX7219_IC3[7] = 0x0F;

		if((MAX7219_blink_cnt < 200) && ((Setup_mode && (Setup_error_flg == 0)) || ZMN_Lvl_blnk)) {
			MAX7219_IC2[4] = 0x0F;
			MAX7219_IC2[5] = 0x0F;
		} else {
			if(Setup_mode) {
				tmp = ZMN_Lvl_setup;
				MAX7219_IC2[4] = tmp / 10;
				MAX7219_IC2[5] = tmp % 10;
			} else {
				tmp = ZMN_Lvl;
				MAX7219_IC2[4] = tmp / 10;
				MAX7219_IC2[5] = tmp % 10;
			}
		}
		if((MAX7219_blink_cnt < 200) && ((Setup_mode && (Setup_error_flg == 0)) || ZMN_Toff_blnk)) {
				MAX7219_IC2[2] = 0x0F;
				MAX7219_IC2[3] = 0x0F;
		} else {
			if(Setup_mode) {
				tmp = ZMN_Toff_setup;
				MAX7219_IC2[2] = tmp / 10;
				MAX7219_IC2[3] = tmp % 10;
			} else {
				tmp = ZMN_Toff;
				MAX7219_IC2[2] = tmp / 10;
				MAX7219_IC2[3] = tmp % 10;
			}
		}
		if((MAX7219_blink_cnt < 200) && ((Setup_mode && (Setup_error_flg == 0)) || ZMN_Ton_blnk)) {
			MAX7219_IC2[0] = 0x0F;
			MAX7219_IC2[1] = 0x0F;
		} else {
			if(Setup_mode) {
				if(ZMN_Ton_vv_setup) {
					tmp = ZMN_Ton_setup;
					MAX7219_IC2[0] = tmp / 10;
					MAX7219_IC2[1] = tmp % 10;
				} else {
					MAX7219_IC2[0] = 0x0A;
					MAX7219_IC2[1] = 0x0A;
				}
			} else {
				if(ZMN_Ton_vv) {
					tmp = ZMN_Ton;
					MAX7219_IC2[0] = tmp / 10;
					MAX7219_IC2[1] = tmp % 10;
				} else {
					MAX7219_IC2[0] = 0x0A;
					MAX7219_IC2[1] = 0x0A;
				}
			}
		}
		MAX7219_IC2[6] = 0x0F;
		MAX7219_IC2[7] = 0x0F;

		tmp = IrmsA;
		MAX7219_IC1[0] = tmp / 1000;
		tmp = tmp % 1000;
		MAX7219_IC1[1] = tmp / 100;
		tmp = tmp % 100;
		MAX7219_IC1[2] = tmp / 10;
		MAX7219_IC1[3] = tmp % 10;
		tmp = IrmsC;
		MAX7219_IC1[4] = tmp / 1000;
		tmp = tmp % 1000;
		MAX7219_IC1[5] = tmp / 100;
		tmp = tmp % 100;
		MAX7219_IC1[6] = tmp / 10;
		MAX7219_IC1[7] = tmp % 10;

		MAX7219_IC7[4] = 0;
		MAX7219_IC7[5] = 0;
		MAX7219_IC7[6] = 0;
		MAX7219_IC7[7] = 0;

		if(APVvv) {
			MAX7219_IC7[4] += 16;
		} else {
			MAX7219_IC7[4] += 8;
		}
		if(ZMNvv) {
			MAX7219_IC7[4] += 4;
		} else {
			MAX7219_IC7[4] += 2;
		}
		if(DUvv) {
			MAX7219_IC7[5] += 8;
		} else {
			MAX7219_IC7[5] += 4;
		}
		if(MAX7219_LED_Block) {
			MAX7219_IC7[4] += 1;
		}
		if(MAX7219_LED_Nespr) {
			MAX7219_IC7[4] += 128;
		}
		if(MAX7219_LED_ErrZv) {
			MAX7219_IC7[5] += 64;
		}
		if(MAX7219_LED_Reserve) {
			MAX7219_IC7[5] += 32;
		}
		if(MAX7219_LED_ExtPow) {
			MAX7219_IC7[5] += 16;
		}
	}

	MAX7219_Send_Buf[0] = 0x0100 + MAX7219_IC1[0];
	MAX7219_Send_Buf[1] = 0x0100 + MAX7219_IC2[0];
	MAX7219_Send_Buf[2] = 0x0100 + MAX7219_IC3[0];
	MAX7219_Send_Buf[3] = 0x0100 + MAX7219_IC4[0];
	MAX7219_Send_Buf[4] = 0x0100 + MAX7219_IC5[0];
	MAX7219_Send_Buf[5] = 0x0100 + MAX7219_IC6[0];
	MAX7219_Send_Buf[6] = 0x0100 + MAX7219_IC7[0];
	MAX7219_Digit_cnt = 0;
	HAL_GPIO_WritePin(MAX7219_CS_GPIO_Port, MAX7219_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit_IT(&hspi2, (uint8_t*)MAX7219_Send_Buf, 7);
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
  HAL_GPIO_WritePin(MAX7219_CS_GPIO_Port, MAX7219_CS_Pin, GPIO_PIN_SET);
	MAX7219_Digit_cnt++;
	if(MAX7219_Digit_cnt < 8) {
		MAX7219_Send_Buf[0] = ((MAX7219_Digit_cnt + 1) << 8) + MAX7219_IC1[MAX7219_Digit_cnt];
		MAX7219_Send_Buf[1] = ((MAX7219_Digit_cnt + 1) << 8) + MAX7219_IC2[MAX7219_Digit_cnt];
		MAX7219_Send_Buf[2] = ((MAX7219_Digit_cnt + 1) << 8) + MAX7219_IC3[MAX7219_Digit_cnt];
		MAX7219_Send_Buf[3] = ((MAX7219_Digit_cnt + 1) << 8) + MAX7219_IC4[MAX7219_Digit_cnt];
		MAX7219_Send_Buf[4] = ((MAX7219_Digit_cnt + 1) << 8) + MAX7219_IC5[MAX7219_Digit_cnt];
		MAX7219_Send_Buf[5] = ((MAX7219_Digit_cnt + 1) << 8) + MAX7219_IC6[MAX7219_Digit_cnt];
		MAX7219_Send_Buf[6] = ((MAX7219_Digit_cnt + 1) << 8) + MAX7219_IC7[MAX7219_Digit_cnt];
	} else {
		switch(MAX7219_Digit_cnt) {
			case 8:
				for(unsigned char i = 0; i < 7; i++) {
					if(INP_DoorOpen) {
						MAX7219_Send_Buf[i] = 0x0C01;
					} else {
						MAX7219_Send_Buf[i] = 0x0C00;
					}
				}
				break;
			case 9:
				for(unsigned char i = 0; i < 7; i++) {
					if(MAX7219_ChkMode && (INP_DoorOpen)) {
						MAX7219_Send_Buf[i] = 0x0F01;
					} else {
						MAX7219_Send_Buf[i] = 0x0F00;
					}
				}
				break;
			case 10:
				for(unsigned char i = 0; i < 7; i++) {
					if(MAX7219_ChkMode) {
						MAX7219_Send_Buf[i] = 0x0A06;
					} else {
						MAX7219_Send_Buf[i] = 0x0A00 + MAX7219_Brightness_cur;
					}
				}
				break;
			case 11:
				MAX7219_Send_Buf[0] = 0x09FF;
				MAX7219_Send_Buf[1] = 0x09FF;
				MAX7219_Send_Buf[2] = 0x09FF;
				MAX7219_Send_Buf[3] = 0x09FF;
				MAX7219_Send_Buf[4] = 0x09FF;
				MAX7219_Send_Buf[5] = 0x09FF;
				MAX7219_Send_Buf[6] = 0x090F;
				break;
			case 12:
				for(unsigned char i = 0; i < 7; i++) {
					MAX7219_Send_Buf[i] = 0x0B07;
				}
				break;
		}
	}
	if(MAX7219_Digit_cnt < 13) {
		HAL_GPIO_WritePin(MAX7219_CS_GPIO_Port, MAX7219_CS_Pin, GPIO_PIN_RESET);
		HAL_SPI_Transmit_IT(&hspi2, (uint8_t*)MAX7219_Send_Buf, 7);
	}
}

void MAX7219_Timers(void) {
	MAX7219_blink_cnt++;
	if(MAX7219_blink_cnt > 999) {
		MAX7219_blink_cnt = 0;
	}
}
