#include "stm32f4xx_hal.h"

uint32_t combineUnixTime(uint8_t hour, uint8_t minute, uint8_t second, uint8_t day, uint8_t month, uint16_t year);
void Time_Set(void);
void Time_Get(void);
uint32_t TimeStamp_Get(void);

extern uint16_t mSec;
