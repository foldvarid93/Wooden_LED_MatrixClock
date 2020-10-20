#ifndef __NTP_H
#define __NTP_H
/***************************************************************************************
 * Includes
 **************************************************************************************/
#include "esp8266.h"
#include "stdbool.h"
#include "typedef.h"
#include "stdio.h"
/***************************************************************************************
 * Defines
 **************************************************************************************/
/*NTP time stamp is in the first 48 bytes of the message*/
#define NTP_PACKET_SIZE  			(48U)
/**/
#define NUMBEROFSECONDS_YEAR		(31557600U)
#define NUMBEROFSECONDS_DAY 		(86400U)
#define NUMBEROFSECONDS_HOUR 		(3600U)
#define NUMBEROFSECONDS_MINUTE 		(60U)
#define NUMBEROFSECONDS_UTCOFFSET 	((NUMBEROFSECONDS_HOUR * UTC_TIMEZONE_HOURS) + (NUMBEROFSECONDS_MINUTE * UTC_TIMEZONE_MINUTES))
#define UTC_TIMEZONE_HOURS 			(2U)
#define UTC_TIMEZONE_MINUTES 		(0U)
#define SHORT_PAUSE 				(1000U)
#define LONG_PAUSE  				(5000U)
#define SEVENTYYEARS  				(2208988800UL)
#define NUMBEROFSECONDSPERLEAPYEAR	(31622400U)
/***************************************************************************************
 * Function declarations
 **************************************************************************************/
/**/
HAL_StatusTypeDef ESP8266_NTP_Init(const uint8_t * NTP_SSID, const uint8_t * NTP_PassWord);
/**/
HAL_StatusTypeDef ESP8266_NTP_GetDateTime(RTC_DateTypeDef *Date, RTC_TimeTypeDef *Time);
/**/
HAL_StatusTypeDef RTC_NTPSync(const uint8_t * NTP_SSID, const uint8_t * NTP_PassWord);
/**/
HAL_StatusTypeDef Convert_UTCToDateTime(uint32_t UTCTime_Sec);
/**/
HAL_StatusTypeDef Convert_CharArrayToDateTime(const char* MSG);
/**************************************************************************************/
#endif
