#ifndef __NTP_H
#define __NTP_H
#include "application.h"
#include "stdbool.h"
#include "typedef.h"
// =====================================================================================
#define SECONDROUNDINGTHRESHOLD 	115
#define SEVENTYYEARS 				2208988800UL
#define NUMBEROFSECONDSPERDAY 		86400UL
#define NUMBEROFSECONDSPERHOUR 		3600UL
#define NUMBEROFSECONDSPERMINUTE 	60UL
#define UTC_DELTA_HOURS 			2
#define UTC_DELTA_MINUTES 			0
#define hardRestPIN 				22
#define SHORT_PAUSE 				1000UL
#define LONG_PAUSE  				5000UL
#define ESPSERIALBAUD 				(115200U) // Set to whatever is the default for your ESP.
#define seventyYears  				(2208988800UL)
#define LeapYearSeconds				(31622400U)
#define OrdinaryYearSeconds			(31536000U)
#define AvgYearSeconds				(31557600U)
#define DaySeconds					(86400U)
#define TimeZone 					(2U)
#define UTCOffset_Sec 				(3600U * TimeZone)
// =====================================================================================

void ESP8266_NTP_EmptyRX(unsigned long duration);

bool ESP8266_NTP_WaitForString(const char * endMarker, unsigned long duration);

bool ESP8266_NTP_ATCommand(const char * command, const char * endMarker,unsigned long duration);

unsigned long ESP8266_NTP_EpochUnixNTP(void);

HAL_StatusTypeDef ESP8266_NTP_Init(void);

HAL_StatusTypeDef ESP8266_NTP_GetDateTime(RTC_DataType *DateTime);
// =====================================================================================
#endif
