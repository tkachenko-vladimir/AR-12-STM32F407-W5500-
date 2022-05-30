#define _NCT75_I2C hi2c1
#define _NCT75_I2C_ADRESS 0x9E
#define _NCT75_FREERTOS_IS_ENABLE 1

#include "main.h"
#include "nct75.h"
#if (_NCT75_FREERTOS_IS_ENABLE==1)
#include "cmsis_os.h"
#endif

bool NCT75_IsConnected(void)
{
	if(HAL_I2C_IsDeviceReady(&_NCT75_I2C, _NCT75_I2C_ADRESS, 1, 100) == HAL_OK)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int8_t NCT75_Read(void)
{
	uint8_t data[2];

	HAL_I2C_Mem_Read(&_NCT75_I2C, _NCT75_I2C_ADRESS, 0, I2C_MEMADD_SIZE_8BIT, (uint8_t*)data, 2, 100);
	return (int8_t)data[0];
}
