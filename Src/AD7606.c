#include "main.h"
#include "Settings.h"

uint16_t AD7606_Read_Buf[6];
uint8_t AD7606_BUSY_timer = 10, AD7606_Start_timer = 200;
uint16_t VrmsA_raw = 0, IrmsA_raw = 0, IrmsC_raw = 0;
uint32_t AD7606_Fault_cnt = 0;

void AD7606_Init(void) {
	HAL_GPIO_WritePin(AD7606_RESET_GPIO_Port, AD7606_RESET_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(AD7606_RESET_GPIO_Port, AD7606_RESET_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
}

void HAL_GPIO_EXTI_Callback(unsigned short int GPIO_Pin) {
  if(GPIO_Pin == AD7606_BUSY_Pin) {
		HAL_GPIO_WritePin(AD7606_CONV_GPIO_Port, AD7606_CONV_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(AD7606_CS_GPIO_Port, AD7606_CS_Pin, GPIO_PIN_RESET);
		HAL_SPI_Receive_DMA(&hspi1, (uint8_t*)AD7606_Read_Buf, 6);
  } 
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {
	HAL_GPIO_WritePin(AD7606_CS_GPIO_Port, AD7606_CS_Pin, GPIO_PIN_SET);
	AD7606_BUSY_timer = 250;
	Internal_Faults &= 0xFF - 1;
	if((AD7606_Read_Buf[0] & 0x8000) != 0) {
		IrmsA_raw = ((0x8000 - (AD7606_Read_Buf[0] & 0x7FFF)) >> 4) + _B_IA;
	} else {
		IrmsA_raw = (short int)(AD7606_Read_Buf[0] >> 4) + _B_IA;
	}
	if((AD7606_Read_Buf[1] & 0x8000) != 0) {
		IrmsC_raw = ((0x8000 - (AD7606_Read_Buf[1] & 0x7FFF)) >> 4) + _B_IC;
	} else {
		IrmsC_raw = (short int)(AD7606_Read_Buf[1] >> 4) + _B_IC;
	}
	if((AD7606_Read_Buf[5] & 0x8000) != 0) {
		VrmsA_raw = ((0x8000 - (AD7606_Read_Buf[5] & 0x7FFF)) >> 4) + _B_UA;
	} else {
		VrmsA_raw = (short int)(AD7606_Read_Buf[5] >> 4) + _B_UA;
	}
	if(RMS_process) {
		AD7606_Fault_cnt++;
	} else {
		RMS_process = true;
	}
}

void AD7606_Timers(void) {
	if(AD7606_BUSY_timer != 0) {
		AD7606_BUSY_timer--;
		if(AD7606_BUSY_timer == 0) {
			Internal_Faults |= 1;
			ADC_RST_f = true;
		}
	}
	if(AD7606_Start_timer != 0) {
		AD7606_Start_timer--;
	}
}
