#include "stm32f4xx_hal.h"

void Inputs_Init(void);
void Inputs_Poll(void);
void Inputs_Apply(uint8_t inp_state);

extern uint8_t Inputs_State;
