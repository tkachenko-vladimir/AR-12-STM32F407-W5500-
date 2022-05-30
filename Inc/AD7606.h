#include "stm32f4xx_hal.h"

void AD7606_Init(void);
void AD7606_Timers(void);

extern uint16_t VrmsA_raw, IrmsA_raw, IrmsC_raw;
extern uint8_t AD7606_Start_timer;
extern uint32_t AD7606_Fault_cnt;
