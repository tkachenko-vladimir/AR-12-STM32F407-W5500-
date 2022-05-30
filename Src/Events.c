#include "cmsis_os.h"
#include "main.h"
#include "Settings.h"
#include "VV.h"
#include "Inputs.h"
#include "Iec104.h"
#include "time.h"

struct LogMsg {
    uint32_t TimeStamp;
    uint16_t mSec;
    uint16_t Event_nbr;
    uint16_t Event_param;
};

struct LogMsg LogBuf[50];

uint8_t LogPos = 0;

osEvent event;

void LogWrite(uint32_t Event) {
	if((Event >> 24) & LogMask) {
		LogBuf[LogPos].TimeStamp = TimeStamp_Get();
		LogBuf[LogPos].mSec = mSec;
		LogBuf[LogPos].Event_nbr = Event >> 16;
		LogBuf[LogPos].Event_param = (uint16_t)(Event & 0xFFFF);
		LogPos++;
		if(LogPos == 50) {
			LogPos = 0;
		}
	}
}

void Events_Poll(void) {
	event = osMessageGet(EventsQueueHandle, osWaitForever);
  if(event.status == osEventMessage) {
		uint16_t events_inf = event.value.v & 0xFFFF;

		if((event.value.v >> 24) & 1) {
			
		}
		if((event.value.v >> 24) & 2) {
			uint8_t inp_old = (uint8_t)(events_inf >> 8);
			uint8_t inp_now = (uint8_t)events_inf;
			Inputs_Apply(inp_now);

			if(((inp_old & 1) != (inp_now & 1)) || ((inp_old & 2) != (inp_now & 2))) {
				VV_ReadState_timer = 100;
			}
/*			if((inp_old & 32) != (inp_now & 32)) {
				if(inp_now & 32)
					IEC104_BuildSignal_Spon(1, 0, IEC104_INFOADDR_STATE_HXGF+4);
				else
					IEC104_BuildSignal_Spon(1, 1, IEC104_INFOADDR_STATE_HXGF+4);
			}
			if((inp_old & 8) != (inp_now & 8)) {
				if(inp_now & 8)
					IEC104_BuildSignal_Spon(1, 0, IEC104_INFOADDR_STATE_HXGF+5);
				else
					IEC104_BuildSignal_Spon(1, 1, IEC104_INFOADDR_STATE_HXGF+5);
			}
			if((inp_old & 16) != (inp_now & 16)) {
				if(inp_now & 16)
					IEC104_BuildSignal_Spon(1, 0, IEC104_INFOADDR_STATE_HXGF+6);
				else
					IEC104_BuildSignal_Spon(1, 1, IEC104_INFOADDR_STATE_HXGF+6);
			}
			if((inp_old & 128) != (inp_now & 128)) {
				if(inp_now & 128)
					IEC104_BuildSignal_Spon(1, 0, IEC104_INFOADDR_STATE_HXGF+7);
				else
					IEC104_BuildSignal_Spon(1, 1, IEC104_INFOADDR_STATE_HXGF+7);
			}
*/
		}
		if((event.value.v >> 24) & 4) {
			
		}
  }
}
