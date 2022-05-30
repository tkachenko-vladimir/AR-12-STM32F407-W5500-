#include "main.h"
#include "Settings.h"
#include "Events.h"
#include "Protection.h"

const unsigned short int cb_rep_spd_tab[4] = {15, 7, 4, 1};
unsigned char kbrd_state = 0, kbrd_code = 0, cb_rep_spd = 0;
unsigned short int cb_rep = 0, cb_rep_cnt = 0;
unsigned short int cb32 = 0, cb34 = 0;
unsigned short int cb[49] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

void btn_process(unsigned char* ret_value, unsigned char btn_nbr, bool btn_state, unsigned char mode)
{
	if(!VV_Block_f || (btn_nbr == 30) || (btn_nbr == 35) || (btn_nbr > 40))
	{
		switch(mode)
		{
			case 0:
			{
				if(btn_state)
				{
					if(cb[btn_nbr] != 3)
					{
						cb[btn_nbr]++;
						if(cb[btn_nbr] == 3)
						{
							*ret_value = btn_nbr;
						}
					}
				}
				else
				{
					if(cb[btn_nbr] != 0)
					{
						cb[btn_nbr]--;
						if(cb[btn_nbr] == 0)
						{
							*ret_value = btn_nbr + 0x80;
						}
					}
				}
				break;
			}
			case 1:
			{
				if(btn_state)
				{
					cb[btn_nbr]++;
					if(cb[btn_nbr] == 3)
					{
						cb_rep = 0;
						cb_rep_spd = 0;
						cb_rep_cnt = 0;
						*ret_value = btn_nbr;
					}
					else
					{
						cb_rep++;
						if(cb_rep >= cb_rep_spd_tab[cb_rep_spd])
						{
							cb_rep = 0;
							*ret_value = btn_nbr;
						}
					}
				}
				else
				{
					cb[btn_nbr] = 0;
				}
				break;
			}
		}
	}
}

void Buttons_Read(void) {
	for(kbrd_state = 0; kbrd_state < 5; kbrd_state++) {
		switch(kbrd_state)
		{
			case 0:
			{
				HAL_GPIO_WritePin(KBRD_O1_GPIO_Port, KBRD_O1_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(KBRD_O2_GPIO_Port, KBRD_O2_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(KBRD_O3_GPIO_Port, KBRD_O3_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(KBRD_O4_GPIO_Port, KBRD_O4_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(KBRD_O5_GPIO_Port, KBRD_O5_Pin, GPIO_PIN_SET);

				btn_process(&kbrd_code, 1, !HAL_GPIO_ReadPin(KBRD_I1_GPIO_Port, KBRD_I1_Pin), 1);
				btn_process(&kbrd_code, 6, !HAL_GPIO_ReadPin(KBRD_I2_GPIO_Port, KBRD_I2_Pin), 1);
				btn_process(&kbrd_code, 11, !HAL_GPIO_ReadPin(KBRD_I3_GPIO_Port, KBRD_I3_Pin), 1);
				btn_process(&kbrd_code, 16, !HAL_GPIO_ReadPin(KBRD_I4_GPIO_Port, KBRD_I4_Pin), 1);
				btn_process(&kbrd_code, 21, !HAL_GPIO_ReadPin(KBRD_I5_GPIO_Port, KBRD_I5_Pin), 1);
				btn_process(&kbrd_code, 26, !HAL_GPIO_ReadPin(KBRD_I6_GPIO_Port, KBRD_I6_Pin), 1);
				btn_process(&kbrd_code, 31, !HAL_GPIO_ReadPin(KBRD_I7_GPIO_Port, KBRD_I7_Pin), 0);
				break;
			}
			case 1:
			{
				HAL_GPIO_WritePin(KBRD_O1_GPIO_Port, KBRD_O1_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(KBRD_O3_GPIO_Port, KBRD_O2_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(KBRD_O3_GPIO_Port, KBRD_O3_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(KBRD_O4_GPIO_Port, KBRD_O4_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(KBRD_O5_GPIO_Port, KBRD_O5_Pin, GPIO_PIN_SET);

				btn_process(&kbrd_code, 2, !HAL_GPIO_ReadPin(KBRD_I1_GPIO_Port, KBRD_I1_Pin), 1);
				btn_process(&kbrd_code, 7, !HAL_GPIO_ReadPin(KBRD_I2_GPIO_Port, KBRD_I2_Pin), 1);
				btn_process(&kbrd_code, 12, !HAL_GPIO_ReadPin(KBRD_I3_GPIO_Port, KBRD_I3_Pin), 1);
				btn_process(&kbrd_code, 17, !HAL_GPIO_ReadPin(KBRD_I4_GPIO_Port, KBRD_I4_Pin), 1);
				btn_process(&kbrd_code, 22, !HAL_GPIO_ReadPin(KBRD_I5_GPIO_Port, KBRD_I5_Pin), 1);
				if(!VV_Block_f)
				{
					if(!HAL_GPIO_ReadPin(KBRD_I7_GPIO_Port, KBRD_I7_Pin))
					{
						if(cb32 != 400)
						{
							cb32++;
							if(cb32 == 400)
							{
								if((Setup2_View != 0) && (Setup2_View != 1))
								{
									Setup2_En = true;
									Setup2_View_cnt = 0;
									Setup2_En_cnt = 15000;
								}
							}
						}
					}
					else
					{
						if(cb32 != 400)
						{
							if(cb32 > 3)
							{
								if(Setup2_En)
								{
									Setup2End();
									Setup2_En_cnt = 0;
								}
								else
								{
									if(Setup_mode)
									{
										Setup1End();
										Setup_En_cnt = 0;
									}
									else
									{
										Setup_mode = true;
										Setup_En_cnt = 120000;
										MSZ1vv_setup = MSZ1vv;
										SVvv_setup = SVvv;
										APV_T2vv_setup = APV_T2vv;
										ZMN_Ton_vv_setup = ZMN_Ton_vv;
										Imsz1_setup = Imsz1; Tmsz1_setup = Tmsz1; Imsz2_setup = Imsz2; Tmsz2_setup = Tmsz2; Isv_setup = Isv; Tsv_setup = Tsv;
										APV_Tgot_setup = APV_Tgot; APV_T1_setup = APV_T1; APV_T2_setup = APV_T2; ZMN_Lvl_setup = ZMN_Lvl; ZMN_Toff_setup = ZMN_Toff; ZMN_Ton_setup = ZMN_Ton;
									}
								}
							}
						}
						cb32 = 0;
					}
				}
				if(!VV_Block_f)
				{
					if(!HAL_GPIO_ReadPin(KEY_VV_ON_GPIO_Port, KEY_VV_ON_Pin))
					{
						if(cb34 != 100)
						{
							cb34++;
							if(cb34 == 100)
							{
								kbrd_code = 34;
							}
						}
					}
					else
					{
						cb34 = 0;
					}
				}
				break;
			}
			case 2:
			{
				HAL_GPIO_WritePin(KBRD_O1_GPIO_Port, KBRD_O1_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(KBRD_O3_GPIO_Port, KBRD_O2_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(KBRD_O3_GPIO_Port, KBRD_O3_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(KBRD_O4_GPIO_Port, KBRD_O4_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(KBRD_O5_GPIO_Port, KBRD_O5_Pin, GPIO_PIN_SET);

				btn_process(&kbrd_code, 3, !HAL_GPIO_ReadPin(KBRD_I1_GPIO_Port, KBRD_I1_Pin), 1);
				btn_process(&kbrd_code, 8, !HAL_GPIO_ReadPin(KBRD_I2_GPIO_Port, KBRD_I2_Pin), 1);
				btn_process(&kbrd_code, 13, !HAL_GPIO_ReadPin(KBRD_I3_GPIO_Port, KBRD_I3_Pin), 1);
				btn_process(&kbrd_code, 18, !HAL_GPIO_ReadPin(KBRD_I4_GPIO_Port, KBRD_I4_Pin), 1);
				btn_process(&kbrd_code, 23, !HAL_GPIO_ReadPin(KBRD_I5_GPIO_Port, KBRD_I5_Pin), 1);
				btn_process(&kbrd_code, 28, !HAL_GPIO_ReadPin(KBRD_I6_GPIO_Port, KBRD_I6_Pin), 0);
				btn_process(&kbrd_code, 33, !HAL_GPIO_ReadPin(KBRD_I7_GPIO_Port, KBRD_I7_Pin), 0);
				break;
			}
			case 3:
			{
				HAL_GPIO_WritePin(KBRD_O1_GPIO_Port, KBRD_O1_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(KBRD_O3_GPIO_Port, KBRD_O2_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(KBRD_O3_GPIO_Port, KBRD_O3_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(KBRD_O4_GPIO_Port, KBRD_O4_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(KBRD_O5_GPIO_Port, KBRD_O5_Pin, GPIO_PIN_SET);

				btn_process(&kbrd_code, 4, !HAL_GPIO_ReadPin(KBRD_I1_GPIO_Port, KBRD_I1_Pin), 1);
				btn_process(&kbrd_code, 9, !HAL_GPIO_ReadPin(KBRD_I2_GPIO_Port, KBRD_I2_Pin), 1);
				btn_process(&kbrd_code, 14, !HAL_GPIO_ReadPin(KBRD_I3_GPIO_Port, KBRD_I3_Pin), 1);
				btn_process(&kbrd_code, 19, !HAL_GPIO_ReadPin(KBRD_I4_GPIO_Port, KBRD_I4_Pin), 1);
				btn_process(&kbrd_code, 24, !HAL_GPIO_ReadPin(KBRD_I5_GPIO_Port, KBRD_I5_Pin), 1);
				btn_process(&kbrd_code, 29, !HAL_GPIO_ReadPin(KBRD_I6_GPIO_Port, KBRD_I6_Pin), 1);
				break;
			}
			case 4:
			{
				HAL_GPIO_WritePin(KBRD_O1_GPIO_Port, KBRD_O1_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(KBRD_O3_GPIO_Port, KBRD_O2_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(KBRD_O3_GPIO_Port, KBRD_O3_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(KBRD_O4_GPIO_Port, KBRD_O4_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(KBRD_O5_GPIO_Port, KBRD_O5_Pin, GPIO_PIN_RESET);

				btn_process(&kbrd_code, 5, !HAL_GPIO_ReadPin(KBRD_I1_GPIO_Port, KBRD_I1_Pin), 1);
				btn_process(&kbrd_code, 10, !HAL_GPIO_ReadPin(KBRD_I2_GPIO_Port, KBRD_I2_Pin), 1);
				btn_process(&kbrd_code, 15, !HAL_GPIO_ReadPin(KBRD_I3_GPIO_Port, KBRD_I3_Pin), 1);
				btn_process(&kbrd_code, 20, !HAL_GPIO_ReadPin(KBRD_I4_GPIO_Port, KBRD_I4_Pin), 1);
				btn_process(&kbrd_code, 25, !HAL_GPIO_ReadPin(KBRD_I5_GPIO_Port, KBRD_I5_Pin), 1);
				btn_process(&kbrd_code, 30, !HAL_GPIO_ReadPin(KBRD_I6_GPIO_Port, KBRD_I6_Pin), 1);

				btn_process(&kbrd_code, 35, !HAL_GPIO_ReadPin(KEY_VV_OFF_GPIO_Port, KEY_VV_OFF_Pin), 0);
				break;
			}
		}
		if(kbrd_code != 0)
		{
			if((!DUvv) && (kbrd_code != 33) && (kbrd_code != 35))
			{
				DUvv_cnt = 43200;
			}

			if(Setup_mode)
			{
				switch(kbrd_code)
				{
					case 1:
					{
						if(Imsz1_setup != Imsz1_MIN)
						{
							Imsz1_setup--;
						}
						else
						{
							MSZ1vv_setup = false;
						}
						break;
					}
					case 2:
					{
						if(Imsz1_setup != Imsz1_MAX)
						{
							Imsz1_setup++;
							MSZ1vv_setup = true;
						}
						break;
					}
					case 3:
					{
						if((Tmsz1_setup != Tmsz1_MIN) && (MSZ1vv_setup))
						{
							Tmsz1_setup--;
						}
						break;
					}
					case 4:
					{
						if((Tmsz1_setup != Tmsz1_MAX) && (MSZ1vv_setup))
						{
							Tmsz1_setup++;
						}
						break;
					}
					case 5:
					{
						if(Imsz2_setup != Imsz2_MIN)
						{
							Imsz2_setup--;
						}
						break;
					}
					case 6:
					{
						if(Imsz2_setup != Imsz2_MAX)
						{
							Imsz2_setup++;
						}
						break;
					}
					case 7:
					{
						if(Tmsz2_setup != Tmsz2_MIN)
						{
							Tmsz2_setup--;
						}
						break;
					}
					case 8:
					{
						if(Tmsz2_setup != Tmsz2_MAX)
						{
							Tmsz2_setup++;
						}
						break;
					}
					case 9:
					{
						if(Isv_setup != Isv_MIN)
						{
							Isv_setup--;
						}
						else
						{
							SVvv_setup = false;
						}
						break;
					}
					case 10:
					{
						if(Isv_setup != Isv_MAX)
						{
							Isv_setup++;
							SVvv_setup = true;
						}
						break;
					}
					case 11:
					{
						if((Tsv_setup != Tsv_MIN) && (SVvv_setup))
						{
							Tsv_setup--;
						}
						break;
					}
					case 12:
					{
						if((Tsv_setup != Tsv_MAX) && (SVvv_setup))
						{
							Tsv_setup++;
						}
						break;
					}
					case 15:
					{
						if(APV_Tgot_setup != APV_Tgot_MIN)
						{
							APV_Tgot_setup--;
						}
						break;
					}
					case 16:
					{
						if(APV_Tgot_setup != APV_Tgot_MAX)
						{
							APV_Tgot_setup++;
						}
						break;
					}
					case 17:
					{
						if(APV_T1_setup != APV1_T_MIN)
						{
							APV_T1_setup--;
						}
						break;
					}
					case 18:
					{
						if(APV_T1_setup != APV1_T_MAX)
						{
							APV_T1_setup++;
						}
						break;
					}
					case 19:
					{
						if(APV_T2_setup != APV2_T_MIN)
						{
							APV_T2_setup--;
						}
						else
						{
							APV_T2vv_setup = false;
						}
						break;
					}
					case 20:
					{
						if(APV_T2_setup != APV2_T_MAX)
						{
							APV_T2_setup++;
							APV_T2vv_setup = true;
						}
						break;
					}
					case 21:
					{
						if(ZMN_Lvl_setup != ZMN_level_MIN)
						{
							ZMN_Lvl_setup -= 10;
						}
						break;
					}
					case 22:
					{
						if(ZMN_Lvl_setup != ZMN_level_MAX)
						{
							ZMN_Lvl_setup += 10;
						}
						break;
					}
					case 23:
					{
						if(ZMN_Toff_setup != ZMN_Tvidkl_MIN)
						{
							ZMN_Toff_setup--;
						}
						break;
					}
					case 24:
					{
						if(ZMN_Toff_setup != ZMN_Tvidkl_MAX)
						{
							ZMN_Toff_setup++;
						}
						break;
					}
					case 25:
					{
						if(ZMN_Ton_setup != ZMN_Tvkl_MIN)
						{
							ZMN_Ton_setup--;
						}
						else
						{
							ZMN_Ton_vv_setup = false;
						}
						break;
					}
					case 26:
					{
						if(ZMN_Ton_setup != ZMN_Tvkl_MAX)
						{
							ZMN_Ton_setup++;
							ZMN_Ton_vv_setup = true;
						}
						break;
					}
				}
				Setup_En_cnt = 120000;
			}
			if(Setup2_En)
			{
				switch(kbrd_code)
				{
					case 13:
					{
						Setup2_En_cnt = 15000;
						if(Setup2_View == 2)
						{
							switch(Ktt)
							{
								case 10:
									Ktt = 120;
									break;
								case 20:
									Ktt = 10;
									break;
								case 30:
									Ktt = 20;
									break;
								case 40:
									Ktt = 30;
									break;
								case 80:
									Ktt = 40;
									break;
								case 120:
									Ktt = 80;
									break;
							}
						}
						if(Setup2_View == 3)
						{
							MB1_adress--;
							if(MB1_adress == 0)
							{
								MB1_adress = 255;
							}
						}
						if(Setup2_View == 4)
						{
							MB1_speed--;
							if(MB1_speed == 255)
							{
								MB1_speed = 4;
							}
						}
						break;
					}
					case 14:
					{
						Setup2_En_cnt = 15000;
						if(Setup2_View == 2)
						{
							switch(Ktt)
							{
								case 10:
									Ktt = 20;
									break;
								case 20:
									Ktt = 30;
									break;
								case 30:
									Ktt = 40;
									break;
								case 40:
									Ktt = 80;
									break;
								case 80:
									Ktt = 120;
									break;
								case 120:
									Ktt = 10;
									break;
							}
						}
						if(Setup2_View == 3)
						{
							MB1_adress++;
							if(MB1_adress == 0)
							{
								MB1_adress = 1;
							}
						}
						if(Setup2_View == 4)
						{
							MB1_speed++;
							if(MB1_speed == 5)
							{
								MB1_speed = 0;
							}
						}
						break;
					}
				}
			}
			else
			{
				switch(kbrd_code)
				{
					case 13:
					{
						Setup2_View_cnt = 30000;
						Setup2_View--;
						if(Setup2_View == 255)
						{
							Setup2_View = 4;
						}
						break;
					}
					case 14:
					{
						Setup2_View_cnt = 30000;
						Setup2_View++;
						if(Setup2_View == 5)
						{
							Setup2_View = 0;
						}
						break;
					}
				}
			}
			switch(kbrd_code)
			{
				case 28:
				{
					if(APVvv)
					{
						Set_APVvv(false);
					}
					else
					{
						Set_APVvv(true);
					}
					Save_Settings2_f = true;
					break;
				}
				case 29:
				{
					if(ZMNvv)
					{
						Set_ZMNvv(false);
					}
					else
					{
						Set_ZMNvv(true);
					}
					Save_Settings2_f = true;
					break;
				}
				case 30:
				{
					VV_Unblock_f = true;
					break;
				}
				case 31:
				{
					MAX7219_ChkMode = true;
					break;
				}
				case (31 + 0x80):
				{
					MAX7219_ChkMode = false;
					break;
				}
				case 33:
				{
					if(DUvv)
					{
						Set_DUvv(false);
					}
					else
					{
						Set_DUvv(true);
					}
					Save_Settings2_f = true;
					break;
				}
				case 34:
				{
					if(HAL_GPIO_ReadPin(KEY_VV_OFF_GPIO_Port, KEY_VV_OFF_Pin))
					{
						VV_OnCmd(1);
					}
					break;
				}
				case 35:
				{
					VV_OffCmd(1);
					break;
				}
			}
			kbrd_code = 0;
		}
	}
	if(cb_rep != 0)
	{
		if(cb_rep_cnt != 200)
		{
			cb_rep_cnt++;
			if(cb_rep_cnt == 50)
			{
				cb_rep_spd = 1;
			}
			if(cb_rep_cnt == 100)
			{
				cb_rep_spd = 2;
			}
			if(cb_rep_cnt == 200)
			{
				cb_rep_spd = 3;
			}
		}
	}
}
