#include "stm32f4xx_hal.h"

struct LogMsg {
    uint32_t TimeStamp;
    uint16_t mSec;
    uint16_t Event_nbr;
    uint16_t Event_param;
};

/*
Event - 
0x00 - Events group:
00000001 - System
 0x00 - Event nbr
 0x00 - PowerOn

00000010 - Inputs, Outputs
 0x00 - Event nbr
 0x00 - inputs
	0x00 - old inputs
		0x00 - new inputs
 0x01 - outputs
	0x00 - old outputs
		0x00 - new outputs
		
		
		
00000100 - Protection
 0x00 - Event nbr
 0x00 - MB_PuskMSZ1
 0x01 - MB_VozvratMSZ1
 0x02 - MB_WorkMSZ1
 0x03 - MB_PuskMSZ2
 0x04 - MB_VozvratMSZ2
 0x05 - MB_WorkMSZ2
 0x06 - MB_PuskSV
 0x07 - MB_WorkSV
 0x08 - MB_GotovAPV
 0x09 - MB_PuskAPV1
 0x0A - MB_WorkAPV1
 0x0B - MB_PuskAPV2
 0x0C - MB_WorkAPV2
 0x0D - MB_PuskZMN
 0x0E - MB_VozvratZMN
 0x0F - MB_WorkZMN
 0x10 - MB_PuskAPV_ZMN
 0x11 - MB_VozvratAPV_ZMN
 0x12 - MB_WorkAPV_ZMN
	0x00 - reserve
		0x00 - reserve

00001000 - VV
 0x00 - Event nbr
 0x00 - VV On
   0x0000 - VV_On_Sourse
 0x01 - VV Off
   0x0000 - VV_Off_Sourse
 0x02 - VV Block
 0x03 - VV Error


*/

void Events_Poll(void);

extern struct LogMsg LogBuf[50];

extern uint8_t LogPos;
	
void LogWrite(uint32_t Event);
