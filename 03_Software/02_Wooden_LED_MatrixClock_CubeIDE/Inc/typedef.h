#ifndef __TYPEDEF_H
#define __TYPEDEF_H
// =====================================================================================
#include "stdbool.h"
// =====================================================================================
typedef struct AppConfig_Type{
	/*defines*/
#define VirtAddr_SSID						(0x0)
#define SizeOf_SSID							(0x0100)
	/**/
#define VirtAddr_PassWord 					(VirtAddr_SSID + SizeOf_SSID)
#define SizeOf_PassWord						(0x0100)
	/**/
#define VirtAddr_ScrollText					(VirtAddr_PassWord + SizeOf_PassWord)
#define SizeOf_ScrollText					(0x0100)
	/**/
#define VirtAddr_ScrollTextIntervalInSec	(VirtAddr_ScrollText + SizeOf_ScrollText)
#define SizeOf_ScrollTextIntervalInSec		(0x0002)
	/**/
#define VirtAddr_ScrollDateIntervalInSec	(VirtAddr_ScrollTextIntervalInSec + SizeOf_ScrollTextIntervalInSec)
#define SizeOf_ScrollDateIntervalInSec		(0x0002)
	/**/
#define VirtAddr_TimeAnimation				(VirtAddr_ScrollDateIntervalInSec + SizeOf_ScrollDateIntervalInSec)
#define SizeOf_TimeAnimation				(0x0001)
	/**/
#define VirtAddr_Connected					(VirtAddr_TimeAnimation + SizeOf_TimeAnimation)
#define SizeOf_Connected					(0x0001)
	/**/
#define VirtAddr_FirstRun					(VirtAddr_Connected + SizeOf_Connected)
#define SizeOf_FirstRun						(0x0001)
	/**/
#define VirtAddr_UpdateTime					(VirtAddr_FirstRun + SizeOf_FirstRun)
#define SizeOf_UpdateTime					(0x0001)
	/**/
#define VirtAddr_FlipCounter				(VirtAddr_UpdateTime + SizeOf_UpdateTime)
#define SizeOf_FlipCounter					(0x0001)
	/**/
#define VirtAddr_Point						(VirtAddr_FlipCounter + SizeOf_FlipCounter)
#define SizeOf_Point						(0x0001)
	/**/
#define VirtAddr_ScrollDateSecCounter		(VirtAddr_Point + SizeOf_Point)
#define SizeOf_ScrollDateSecCounter			(0x0002)
	/**/
#define VirtAddr_DisplayMode				(VirtAddr_ScrollDateSecCounter + SizeOf_ScrollDateSecCounter)
#define SizeOf_DisplayMode					(0x0001)
	/*read out from eeprom elements*/
	uint8_t 	SSID[SizeOf_SSID];
	uint8_t		PassWord[SizeOf_PassWord];
	uint8_t		ScrollText[SizeOf_ScrollText];
	uint16_t	ScrollTextIntervalInSec;//0-65536 sec
	uint16_t	ScrollDateIntervalInSec;//0-65536 sec
	uint8_t 	TimeAnimation;//0 or 1
	/*normal variables*/
	uint8_t 	Connected;
	uint8_t		FirstRun;
	uint8_t 	UpdateTime;
	uint8_t		FlipCounter;
	uint8_t		Point;
	uint16_t	ScrollDateSecCounter;
	uint8_t 	DisplayMode;
	uint8_t 	FirstColumn;
}AppConfig_Type;
// =====================================================================================
typedef struct RTC_DataType{
	uint8_t		sec;//0-59
	uint8_t		min;//0-59
	uint8_t		hour;//0-24
	uint8_t		day;//1-7
	uint8_t		date;//1-31
	uint8_t		month;//1-12
	uint8_t		year;//0-100
}RTC_DataType;
// =====================================================================================
typedef struct TimeType{
	uint8_t		hour_tens;
	uint8_t		hour_singles;
	uint8_t		min_tens;
	uint8_t		min_singles;
	uint8_t		sec_tens;
	uint8_t		sec_singles;
}TimeType;
// =====================================================================================
typedef struct DateType{
	uint8_t		day;
	uint8_t		date_tens;
	uint8_t		date_singles;
	uint8_t		month_tens;
	uint8_t		month_singles;
	uint8_t 	year_thousands;
	uint8_t		year_hundreds;
	uint8_t		year_tens;
	uint8_t		year_singles;
}DateType;
// =====================================================================================
struct {
	unsigned char TIME_SET : 1;
	unsigned char DATE_TIME :1;
} MASK;
struct {
	unsigned char B0 :1;
	unsigned char B1 :1;
	unsigned char B2 :1;
	unsigned char B3 :1;
	unsigned char B4 :1;
	unsigned char B5 :1;
	unsigned char B6 :1;
	unsigned char B7 :1;
} MASKBYTE;
// =====================================================================================
#endif
