#include "stm32f4xx_hal.h"

void Protection_Poll(void);
void VV_Unblock(void);
void VV_OnCmd(uint8_t source);
void VV_OffCmd(uint8_t source);
void GotovAPV(void);

extern uint16_t VV_Cmd_Timer, WorkAPV_ZMN_Timer;
