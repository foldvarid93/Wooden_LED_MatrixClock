#ifndef __TYPEDEF_H
#define __TYPEDEF_H
// =====================================================================================
#include "stdbool.h"
// =====================================================================================
typedef struct AppConfig_Type{
	/*defines*/
#define VA_NTP_SSID							(0x0)
#define SO_NTP_SSID							(50u)
	/**/
#define VA_NTP_PassWord 					(VA_NTP_SSID + SO_NTP_SSID)
#define SO_NTP_PassWord						(50u)
	/**/
#define VA_NTP_SyncEnabled 					(VA_NTP_PassWord + SO_NTP_PassWord)
#define SO_NTP_SyncEnabled					(2u)
	/**/
#define VA_NTP_SyncInterval 				(VA_NTP_SyncEnabled + SO_NTP_SyncEnabled)
#define SO_NTP_SyncInterval					(2u)
	/**/
#define VA_Text_Message						(VA_NTP_SyncInterval + SO_NTP_SyncInterval)
#define SO_Text_Message						(256u)
	/**/
#define VA_Text_Enabled						(VA_Text_Message + SO_Text_Message)
#define SO_Text_Enabled						(2u)
	/**/
#define VA_Text_ScrollingMode				(VA_Text_Enabled + SO_Text_Enabled)
#define SO_Text_ScrollingMode				(2u)
	/**/
#define VA_Text_ScrollIntervalInSec			(VA_Text_ScrollingMode + SO_Text_ScrollingMode)
#define SO_Text_ScrollIntervalInSec			(2u)
	/**/
#define VA_Date_Enabled						(VA_Text_ScrollIntervalInSec + SO_Text_ScrollIntervalInSec)
#define SO_Date_Enabled						(2u)
	/**/
#define VA_Date_ScrollingMode				(VA_Date_Enabled + SO_Date_Enabled)
#define SO_Date_ScrollingMode				(2u)
	/**/
#define VA_Date_ScrollIntervalInSec			(VA_Date_ScrollingMode + SO_Date_ScrollingMode)
#define SO_Date_ScrollIntervalInSec			(2u)
	/**/
#define VA_TimeAnimation					(VA_Date_ScrollIntervalInSec + SO_Date_ScrollIntervalInSec)
#define SO_TimeAnimation					(2u)
	/**/
#define VA_DisplayBrightnessMode			(VA_TimeAnimation + SO_TimeAnimation)
#define SO_DisplayBrightnessMode			(2u)
	/**/
#define VA_DisplayBrightness				(VA_DisplayBrightnessMode + SO_DisplayBrightnessMode)
#define SO_DisplayBrightness				(2u)
	/**/
#define SizeOf_CharacterOnDisplay			(0x0006)
#define SizeOf_DisplayTextColumnArray		((SizeOf_CharacterOnDisplay * SO_Text_Message)+(2*NumberOf_DisplayColumn))
#define SizeOf_WhiteSpaces					(NumberOf_DisplayColumn/SizeOf_CharacterOnDisplay)
#define NumberOf_ColumnOfOneDisplay			(8)
#define NumberOf_Display					(12)
#define NumberOf_DisplayColumn				(NumberOf_Display * NumberOf_ColumnOfOneDisplay)
	/*NTP*/
	uint8_t 	NTP_SSID[SO_NTP_SSID];
	uint8_t		NTP_PassWord[SO_NTP_PassWord];
	uint16_t 	NTP_SyncEnabled;
	uint16_t	NTP_SyncInterval;
	/*Text*/
	uint8_t		Text_Message[SO_Text_Message];
	uint16_t 	Text_Enabled;//0 or 1
	uint16_t	Text_ScrollingMode;//0 or 1
	uint16_t	Text_ScrollIntervalInSec;//0-65536 sec
	/*Date*/
	uint16_t	Date_Enabled;//0 or 1
	uint16_t	Date_ScrollingMode;//0 or 1
	uint16_t	Date_ScrollIntervalInSec;//0-65536 sec
	/*Other*/
	uint16_t 	TimeAnimation;//0 or 1
	uint16_t 	DisplayBrightnessMode;//0 or 1
	uint16_t 	DisplayBrightness;//1,3,5,7.. 31
	/*normal variables*/
	uint8_t		DisplayTextArray[SO_Text_Message];
	uint8_t 	DisplayTextColumnArray[SizeOf_DisplayTextColumnArray];
	bool	 	NTP_Connected;
	uint32_t 	NTP_SyncTimerCounter;
	uint8_t 	SM_WiFiStatus;
	uint8_t 	SM_AppStatus;
	uint8_t 	SM_NextState;
	bool		FirstRun;
	uint8_t 	UpdateTime;
	uint8_t		FlipCounter;
	uint8_t		Point;
	uint32_t 	ScrollSecCounter;
	uint32_t 	RTCIntSecCounter;
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
	uint32_t 	Date_LastTimeStamp;
	uint32_t 	Text_LastTimeStamp;
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
