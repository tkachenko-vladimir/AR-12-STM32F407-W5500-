#include "stdbool.h"

void ModBus_Init(void);
void ModBus_Poll(void);
bool WriteHoldingReg(unsigned short int RegAdress, unsigned short int RegData);
uint16_t ReadHoldingReg(uint16_t RegAdress);
