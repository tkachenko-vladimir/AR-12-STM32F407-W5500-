#include "main.h"
#include "Settings.h"
#include "Iec104.h"
#include "cmsis_os.h"
#include "Events.h"
#include "VV.h"
#include "MAX7219.h"

uint8_t Inputs_State = 0;
bool IEC104_DoorOpen;

uint8_t Inputs_Read(void) {
	uint8_t Res = 0;
	
	if(!HAL_GPIO_ReadPin(INPUT_VV_ON_GPIO_Port, INPUT_VV_ON_Pin)) {
		Res |= 1;
	}
	if(!HAL_GPIO_ReadPin(INPUT_VV_OFF_GPIO_Port, INPUT_VV_OFF_Pin)) {
		Res |= 2;
	}
	if(!HAL_GPIO_ReadPin(INPUT_VV_READY_GPIO_Port, INPUT_VV_READY_Pin)) {
		Res |= 4;
	}
	if(!HAL_GPIO_ReadPin(INPUT_PWR_FLT_GPIO_Port, INPUT_PWR_FLT_Pin)) {
		Res |= 8;
	}
	if(HAL_GPIO_ReadPin(INPUT_RES_PWR_GPIO_Port, INPUT_RES_PWR_Pin)) {
		Res |= 16;
	}
	if(HAL_GPIO_ReadPin(INPUT_DOOR_CTRL_GPIO_Port, INPUT_DOOR_CTRL_Pin)) {
		Res |= 32;
	}
	if(HAL_GPIO_ReadPin(INPUT_VV_COIL_FLT_GPIO_Port, INPUT_VV_COIL_FLT_Pin)) {
		Res |= 64;
	}
	if(!HAL_GPIO_ReadPin(INPUT_DV8_GPIO_Port, INPUT_DV8_Pin)) {
		Res |= 128;
	}
	return Res;
}

void Inputs_Apply(uint8_t inp_state) {
	if(inp_state & 32) {
		INP_DoorOpen = false;
	} else {
		INP_DoorOpen = true;
	}

	if(!(inp_state & 1)) {
		VV_Off_State = false;
	} else {
		VV_Off_State = true;
	}
	if(!(inp_state & 2)) {
		VV_On_State = false;
	}	else {
		VV_On_State = true;
	}
	if(!(inp_state & 4)) {
		VV_Ready_State = false;
	} else {
		VV_Ready_State = true;
	}
	if(!(inp_state & 8)) {
		MAX7219_LED_ExtPow = false;
	} else {
		MAX7219_LED_ExtPow = true;
	}
	if(!(inp_state & 128)) {
		MAX7219_LED_Reserve = false;
	} else {
		MAX7219_LED_Reserve = true;
	}
}

void Inputs_Init(void) {
	Inputs_State = Inputs_Read();
	Inputs_Apply(Inputs_State);
}

void Inputs_Poll(void) {
	uint8_t Inputs_New = Inputs_Read();
	
	if(Inputs_New != Inputs_State) {
		Inputs_Apply(Inputs_New);

		if(((Inputs_New & 1) != (Inputs_New & 1)) || ((Inputs_New & 2) != (Inputs_New & 2))) {
			VV_ReadState_timer = 100;
		}
		if((Inputs_New & 32) != (Inputs_State & 32)) {
			if(!(Inputs_New & 32)) {
				MAX7219_Brightness_cur = 0;
			}
			if(Inputs_New & 32) {
				IEC104_BuildSignal_Spon(1, 0, IEC104_INFOADDR_STATE_HXGF+4);//105 VV Fault
			} else {
				IEC104_BuildSignal_Spon(1, 1, IEC104_INFOADDR_STATE_HXGF+4);//105 VV Fault
			}
		}
		uint32_t event_val = 0x02000000 + ((uint16_t)Inputs_State << 8) + Inputs_New;
		LogWrite(event_val);
		osMessagePut(EventsQueueHandle, event_val, osWaitForever);
		Inputs_State = Inputs_New;
	}
}
