#ifndef __TYPEDEF_H
#define __TYPEDEF_H
// =====================================================================================
#include "stdbool.h"
// =====================================================================================
typedef struct AppConfig_Type{
	/*defines*/
#define VirtAddr_SSID						(0x0)
#define SizeOf_SSID							(50u)
	/**/
#define VirtAddr_PassWord 					(VirtAddr_SSID + SizeOf_SSID)
#define SizeOf_PassWord						(50u)
	/**/
#define VirtAddr_ScrollText					(VirtAddr_PassWord + SizeOf_PassWord)
#define SizeOf_ScrollText					(256u)
	/**/
#define VirtAddr_ScrollTextIntervalInSec	(VirtAddr_ScrollText + SizeOf_ScrollText)
#define SizeOf_ScrollTextIntervalInSec		(2u)
	/**/
#define VirtAddr_ScrollDateIntervalInSec	(VirtAddr_ScrollTextIntervalInSec + SizeOf_ScrollTextIntervalInSec)
#define SizeOf_ScrollDateIntervalInSec		(2u)
	/**/
#define VirtAddr_TimeAnimation				(VirtAddr_ScrollDateIntervalInSec + SizeOf_ScrollDateIntervalInSec)
#define SizeOf_TimeAnimation				(2u)
	/**/
#define VirtAddr_TextScrollingMode			(VirtAddr_TimeAnimation + SizeOf_TimeAnimation)
#define SizeOf_TextScrollingMode			(2u)
	/**/
#define VirtAddr_DateScrollingMode			(VirtAddr_TextScrollingMode + SizeOf_TextScrollingMode)
#define SizeOf_DateScrollingMode			(2u)
	/**/
#define SizeOf_CharacterOnDisplay			(0x0006)
#define SizeOf_DisplayTextColumnArray		((SizeOf_CharacterOnDisplay * SizeOf_ScrollText)+(2*NumberOf_DisplayColumn))
#define SizeOf_WhiteSpaces					(NumberOf_DisplayColumn/SizeOf_CharacterOnDisplay)
#define NumberOf_ColumnOfOneDisplay			(8)
#define NumberOf_Display					(12)
#define NumberOf_DisplayColumn				(NumberOf_Display * NumberOf_ColumnOfOneDisplay)

	/*read out from eeprom elements*/
	uint8_t 	NTP_SSID[SizeOf_SSID];
	uint8_t		NTP_PassWord[SizeOf_PassWord];
	uint16_t 	NTP_SyncEnabled;
	uint16_t	NTP_SyncInterval;
	/*Text*/
	uint8_t		Text_Message[SizeOf_ScrollText];
	uint16_t 	Text_Enabled;
	uint16_t	Text_ScrollingMode;//0 or 1
	uint16_t	Text_ScrollIntervalInSec;//0-65536 sec
	/*Date*/
	uint16_t	Date_Enabled;
	uint16_t	Date_ScrollingMode;//0 or 1
	uint16_t	Date_ScrollIntervalInSec;//0-65536 sec
	/*Other*/
	uint16_t 	TimeAnimation;//0 or 1
	uint16_t 	DisplayBrightnessMode;//0 or 1
	uint16_t 	DisplayBrightness;//1,3,5,7.. 31
	/*normal variables*/
	uint8_t		DisplayTextArray[SizeOf_ScrollText];
	uint8_t 	DisplayTextColumnArray[SizeOf_DisplayTextColumnArray];
	uint8_t 	Connected;
	bool		FirstRun;
	uint8_t 	UpdateTime;
	uint8_t		FlipCounter;
	uint8_t		Point;
	uint16_t 	ScrollSecCounter;
	uint16_t	LastScrolled;
	uint8_t 	DisplayMode;
	uint16_t 	FirstColumn;
	uint16_t	LastColumn;
	bool 		TextScrolling;
	uint8_t 	TextLength;
	uint8_t		ScrollingMode;
	uint8_t		NewTimeDataArray[36];
	bool 		TimeDiffIndicator[6];
	uint8_t		DisplayData[NumberOf_DisplayColumn];
	uint32_t	TimeStamp;
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
