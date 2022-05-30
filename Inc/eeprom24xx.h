#include <stdbool.h>

bool EEPROM24XX_IsConnected(void);
bool EEPROM24XX_Save(unsigned short int Address,void *data,size_t size_of_data);	
bool EEPROM24XX_Load(unsigned short int Address,void *data,size_t size_of_data);
