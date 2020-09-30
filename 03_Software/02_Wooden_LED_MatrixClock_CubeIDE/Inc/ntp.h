#ifndef __NTP_H
#define __NTP_H
/***************************************************************************************
 * Includes
 **************************************************************************************/
#include "application.h"
#include "stdbool.h"
#include "typedef.h"
/***************************************************************************************
 * Defines
 **************************************************************************************/
#define NUMBEROFSECONDS_YEAR		(31557600U)
#define NUMBEROFSECONDS_DAY 		(86400U)
#define NUMBEROFSECONDS_HOUR 		(3600U)
#define NUMBEROFSECONDS_MINUTE 		(60U)
#define NUMBEROFSECONDS_UTCOFFSET 	((NUMBEROFSECONDS_HOUR * UTC_TIMEZONE_HOURS) + (NUMBEROFSECONDS_MINUTE * UTC_TIMEZONE_MINUTES))
#define UTC_TIMEZONE_HOURS 			(2U)
#define UTC_TIMEZONE_MINUTES 		(0)
#define SHORT_PAUSE 				(1000U)
#define LONG_PAUSE  				(5000U)
#define SEVENTYYEARS  				(2208988800UL)
#define NUMBEROFSECONDSPERLEAPYEAR	(31622400U)
/*AT commands*/
#define OK_STR "OK"
/***************************************************************************************
 * Function declarations
 **************************************************************************************/

HAL_StatusTypeDef ESP8266_NTP_ATCommand(const char * Command, const char * Respond, uint32_t Timeout);

HAL_StatusTypeDef ESP8266_NTP_Init(void);

HAL_StatusTypeDef ESP8266_NTP_GetDateTime(RTC_DataType *DateTime);
/**************************************************************************************/
#endif
