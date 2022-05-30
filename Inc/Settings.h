#include "stm32f4xx_hal.h"
#include "stdbool.h"
#include "socket.h"

void Settings_Init(void);
void Settings_Poll(void);

void Set_Imsz1(uint16_t new_val);
void Set_Tmsz1(uint16_t new_val);
void Set_Imsz2(uint16_t new_val);
void Set_Tmsz2(uint16_t new_val);
void Set_Isv(uint16_t new_val);
void Set_Tsv(uint16_t new_val);
void Set_Tgot(uint16_t new_val);
void Set_APVt1(uint16_t new_val);
void Set_APVt2(uint16_t new_val);
void Set_ZMNlvl(uint16_t new_val);
void Set_ZMNoff(uint16_t new_val);
void Set_ZMNon(uint16_t new_val);
void Set_MSZ1vv(bool new_val);
void Set_SVvv(bool new_val);
void Set_APV_T2vv(bool new_val);
void Set_ZMN_Ton_vv(bool new_val);
void Set_APVvv(bool new_val);
void Set_ZMNvv(bool new_val);
void Set_DUvv(bool new_val);

extern const unsigned long MB_BR[5];

extern uint16_t _SERIAL_NBR;
extern float _K_IA, _K_IC, _K_UA;
extern int16_t _B_IA, _B_IC, _B_UA;
extern uint16_t Imsz1, Tmsz1, Imsz2, Tmsz2, Isv, Tsv;
extern uint8_t APV_Tgot, APV_T1, APV_T2, ZMN_Lvl, ZMN_Toff, ZMN_Ton;
extern bool MSZ1vv, SVvv, APV_T2vv, ZMN_Ton_vv;
extern bool APVvv, ZMNvv, DUvv;
extern uint8_t MAX7219_Brightness;
extern uint8_t MB1_adress, MB1_speed, MB2_adress, MB2_speed;
extern wiz_NetInfo net_info;
extern uint16_t IEC104_Port;
extern uint16_t IEC104_ASDU;
extern uint16_t Ubazovoe;
extern bool SW_auto;
extern int8_t TimeZone;
extern uint8_t Ktt;
extern float Ktn;
extern uint16_t VV_On_cnt, Reset_cnt;
extern uint32_t ModBus1_cnt, ModBus2_cnt, IEC104_cnt;
extern uint8_t LogMask;
