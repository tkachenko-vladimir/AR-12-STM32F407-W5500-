#include "main.h"

const uint16_t EPOCH_TIME_2000 = 10957; // Days from 01.01.1970 to 01.01.2000
const uint16_t EPOCH_TIME_2017 = 17167; // Days from 01.01.1970 to 01.01.2017
const uint8_t daysInMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
uint16_t mSec = 0;

bool isLeapYear(int16_t year) {
  return (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0));
}

uint32_t combineUnixTime(int8_t hour, int8_t minute, int8_t second, int8_t day, int8_t month, int16_t year) {
  uint16_t days = day - 1;
  int16_t y;

  if (year >= 2017) {
    days += EPOCH_TIME_2017;
    y = 2017;
  } else if (year >= 2000) {
    days += EPOCH_TIME_2000;
    y = 2000;
  } else
    y = 1970;
  for (; y < year; y++)
    days += 365 + isLeapYear(y);
  for (y = 1; y < month; y++)
    days += daysInMonth[y - 1];
  if ((month > 2) && isLeapYear(year))
    days++;

  return (((uint32_t)days * 24 + hour) * 60 + minute) * 60 + second;
}

void Time_Set(void) {
	
}

void Time_Get(void) {
	RTC_TimeTypeDef systemTime = {0};
	RTC_DateTypeDef systemDate = {0};

	HAL_RTC_GetTime(&hrtc, &systemTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &systemDate, RTC_FORMAT_BIN);
	Seconds = systemTime.Seconds;
	Minutes = systemTime.Minutes;
	Hours = systemTime.Hours;
	Date = systemDate.Date;
	Month = systemDate.Month;
	Year = systemDate.Year;
}

uint32_t TimeStamp_Get(void) {
	RTC_TimeTypeDef systemTime = {0};
	RTC_DateTypeDef systemDate = {0};

	HAL_RTC_GetTime(&hrtc, &systemTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &systemDate, RTC_FORMAT_BIN);
	
	mSec = 1000 - (systemTime.SubSeconds * 0.9775);
	return combineUnixTime(systemTime.Hours, systemTime.Minutes, systemTime.Seconds, systemDate.Date, systemDate.Month, systemDate.Year+2000);
}
