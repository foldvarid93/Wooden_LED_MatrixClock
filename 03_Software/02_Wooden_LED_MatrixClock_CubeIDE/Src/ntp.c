/***************************************************************************************
 * Includes
 **************************************************************************************/
//#include "application.h"
#include "ntp.h"
/***************************************************************************************
 * Variables
 **************************************************************************************/
/*globals*/

/*externs*/
extern Serial_t serial;
/***************************************************************************************
 * Function definitions
 **************************************************************************************/
/*Init ESP8266 for NTP communication*/
HAL_StatusTypeDef ESP8266_NTP_Init(const uint8_t * NTP_SSID, const uint8_t * NTP_PassWord)
{
	char TmpBuff[100];
	sprintf(TmpBuff,"AT+CWJAP=\"%s\",\"%s\"",NTP_SSID,NTP_PassWord);
	/*ESP8266 HW reset and enabling*/
	HAL_GPIO_WritePin(ESP8266_RST_GPIO_Port, ESP8266_RST_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ESP8266_EN_GPIO_Port, ESP8266_EN_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ESP8266_EN_GPIO_Port, ESP8266_EN_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(ESP8266_RST_GPIO_Port, ESP8266_RST_Pin, GPIO_PIN_SET);
	HAL_Delay(1000);
	/**/
	Ringbuf_init();
	/**/
	ESP8266_Serial_Init();
	/**/
	if(ESP8266_AT_SendAndReceiveWithTimeout("AT+RESTORE", AT_READY, LONG_PAUSE) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_AT_SendAndReceiveWithTimeout("AT", AT_OK, SHORT_PAUSE) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_AT_SendAndReceiveWithTimeout("ATE0", AT_OK, SHORT_PAUSE) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_AT_SendAndReceiveWithTimeout("AT+CWMODE=1", AT_OK, SHORT_PAUSE) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_AT_SendAndReceiveWithTimeout("AT+CWQAP", AT_OK, SHORT_PAUSE) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_AT_SendAndReceiveWithTimeout((char*)TmpBuff, AT_OK,10000) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_AT_SendAndReceiveWithTimeout("AT+CIPMUX=0", AT_OK, SHORT_PAUSE) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	return HAL_OK;
}
/**/
HAL_StatusTypeDef ESP8266_NTP_GetDateTime(RTC_DateTypeDef *Date, RTC_TimeTypeDef *Time)
{
	/*Locals*/
	uint8_t packetBuffer[ NTP_PACKET_SIZE];
	/**/
	Uart_flush();
	if((ESP8266_AT_SendAndReceiveWithTimeout("AT+CIPSTART=\"UDP\",\"pool.ntp.org\",123", AT_OK, 1000)) != HAL_OK )//if connection failed -> error
	{
	  return HAL_ERROR;
	}
	/**/
	memset(packetBuffer, 0, NTP_PACKET_SIZE);

	/* Initialize values needed to form NTP request*/
	packetBuffer[0] = 0b11100011; // LI, Version, Mode
	packetBuffer[1] = 1; // Stratum, or type of clock
	packetBuffer[2] = 6; // Polling Interval
	packetBuffer[3] = 0xEC; // Peer Clock Precision
	// 8 bytes of zero for Root Delay & Root Dispersion
	packetBuffer[12] = 49;
	packetBuffer[13] = 0x4E;
	packetBuffer[14] = 49;
	packetBuffer[15] = 52;

	/**/
	if(ESP8266_AT_SendAndReceiveWithTimeout("AT+CIPSEND=48", AT_OK, SHORT_PAUSE) != HAL_OK)
	{
	  return HAL_ERROR;
	}
	/**/
	UartPrintCharArray((char*)packetBuffer,NTP_PACKET_SIZE);

	memset(packetBuffer, 0, NTP_PACKET_SIZE);
	int i = 0;
	/**/
	if (Wait_for_timeout("+IPD,48:",1000) == 1)
	{
	  while (1)
	  {
		  if(serial.available()>=48)
		  {
			 break;
		  }
	  }
	  while (serial.available() > 0)
	  {
		  uint8_t ch = serial.read();
		  if (i < NTP_PACKET_SIZE)
		  {
			  packetBuffer[i] = ch;
		  }
		  i++;
		  if ( ( i < NTP_PACKET_SIZE ) && ( serial.available() == 0 ) )
		  {
			  return HAL_ERROR;
		  }
	  }
	}
	else
	{
	  return HAL_ERROR;
	}
	/**/
	if((ESP8266_AT_SendAndReceiveWithTimeout("AT+CIPCLOSE", AT_OK, SHORT_PAUSE)) != HAL_OK)
	{
		return HAL_ERROR;
	}
	/*UTC time: 1900 jan 1, monday, 0:00:00*/
	uint32_t UTCTime_Sec = packetBuffer[40] << (24) | packetBuffer[41]<<16 | packetBuffer[42]<<8 | packetBuffer[43];
	/**/
	if (Convert_UTCToDateTime( UTCTime_Sec) == HAL_ERROR)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}
/**/
HAL_StatusTypeDef RTC_NTPSync(const uint8_t * NTP_SSID, const uint8_t * NTP_PassWord)
{
	RTC_TimeTypeDef HAL_Time={0,0,0,0,0,0,RTC_DAYLIGHTSAVING_NONE,RTC_STOREOPERATION_RESET};
	RTC_DateTypeDef HAL_Date={0,0,0,0};
	uint8_t Attempt=0;
#define NumberOfAttepmts 	3
	/*Try to connect AP*/
	while(1)
	{
		Attempt++;
		if(Attempt < NumberOfAttepmts)
		{
			if(ESP8266_NTP_Init(NTP_SSID, NTP_PassWord) == HAL_OK)
			{
				AppCfg.NTP_Connected = true;
				break;
			}
		}
		else
		{
			return HAL_ERROR;
		}
	}
	/*Try to get NTP packet*/
	Attempt=0;
	while(1)
	{
		Attempt++;
		if(Attempt < NumberOfAttepmts)
		{
			if(ESP8266_NTP_GetDateTime(&HAL_Date, &HAL_Time) == HAL_OK)
			{

			}
		}
		else
		{
			return HAL_ERROR;
		}
	}
}
/**/
HAL_StatusTypeDef Convert_UTCToDateTime(uint32_t UTCTime_Sec)
{
	RTC_DateTypeDef Date;
	RTC_TimeTypeDef Time;
	uint32_t UNIXTimeHungary_Sec = UTCTime_Sec - SEVENTYYEARS + NUMBEROFSECONDS_UTCOFFSET;
	/*Year*/
	uint32_t tmp = UTCTime_Sec + NUMBEROFSECONDS_UTCOFFSET;
	Date.Year = (tmp / NUMBEROFSECONDS_YEAR) - 100;

	/*Month*/
	/*31, 28(29), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31*/
	const uint8_t MonthDays[12]={31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	uint32_t tmp2 = tmp % NUMBEROFSECONDS_YEAR;
	uint32_t tmp3 = (tmp2 /NUMBEROFSECONDS_DAY) + 1;//add 1 because current day
	uint8_t index = 0;
	uint8_t LeapYearIndex = 0;//no leap year
	if((Date.Year % 4) == 0)
	{
	  LeapYearIndex = 1;
	}
	Date.Month = 1;//start at januar=1

	while(index < 12)
	{
	  if(index == 1)//if subtract februar
	  {
		  if( ((int32_t)(tmp3-(MonthDays[index]+LeapYearIndex))) >= 0 )
		  {
			  tmp3 = tmp3 - (MonthDays[index]+LeapYearIndex);
			  Date.Month++;
			  index++;
		  }
	  }
	  else
	  {
		  if( ((int32_t)(tmp3 - (MonthDays[index]))) >= 0 )
		  {
			  tmp3 = tmp3 - MonthDays[index];
			  Date.Month++;
			  index++;
		  }
		  else {
			  break;
		  }
	  }

	}
	/*Date*/
	Date.Date = 1 + tmp3;
	/*Weekday*/
	tmp2 = tmp / NUMBEROFSECONDS_DAY;
	tmp2 = tmp2 % 7;
	Date.WeekDay = tmp2 + 1; // Because Monday is start from 1 not 0

	/*Hour*/
	Time.Hours = ((UNIXTimeHungary_Sec % NUMBEROFSECONDS_DAY) / NUMBEROFSECONDS_HOUR);
	/*Minute*/
	Time.Minutes = ((UNIXTimeHungary_Sec % NUMBEROFSECONDS_HOUR) / NUMBEROFSECONDS_MINUTE);
	/*Second*/
	Time.Seconds = (UNIXTimeHungary_Sec % NUMBEROFSECONDS_MINUTE);
	/**/
	Time.StoreOperation = 0;
	/**/
	Time.SecondFraction = 0;

	/*IRQ Disable*/
	HAL_NVIC_DisableIRQ(RTC_Alarm_IRQn);
	/*save date and time*/
	if(HAL_RTC_SetTime(&hrtc, &Time,RTC_FORMAT_BIN) == HAL_ERROR)
	{
		return HAL_ERROR;
	}
	else
	{
		if(HAL_RTC_SetDate(&hrtc,&Date,RTC_FORMAT_BIN) == HAL_ERROR)
		{
			return HAL_ERROR;
		}
	}
	/*IRQ Enable*/
	HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
	return HAL_OK;
}
/**/
HAL_StatusTypeDef Convert_CharArrayToDateTime(const char* MSG)
{
	RTC_DateTypeDef Date;
	RTC_TimeTypeDef Time;
	uint8_t i=0;
	char TmpBuf[20];
	/*year*/
	for(uint8_t j=0 ; MSG[i] != '.' ; i++ , j++)
	{
		TmpBuf[j] = MSG[i];
		TmpBuf[j+1] = '\0';
	}
	i++;
	Date.Year =  (atoi((const char*)TmpBuf)) - 2000;
	/*month*/
	for(uint8_t j=0 ; MSG[i] != '.' ; i++ , j++)
	{
		TmpBuf[j] = MSG[i];
		TmpBuf[j+1] = '\0';
	}
	i++;
	Date.Month =  atoi((const char*)TmpBuf);
	/*date*/
	for(uint8_t j=0 ; MSG[i] != '.' ; i++ , j++)
	{
		TmpBuf[j] = MSG[i];
		TmpBuf[j+1] = '\0';
	}
	i++;
	Date.Date =  atoi((const char*)TmpBuf);
	/*Weekday*/
	const uint8_t MonthDays[12]={31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	float Tmp = (Date.Year + 100) * 365.25;

	for(uint8_t i = 0; i < Date.Month - 1; i++)
	{
		Tmp += MonthDays[i];
	}
	Tmp += Date.Date;
	Date.WeekDay = ((uint32_t)Tmp % 7);
	/*hour*/
	for(uint8_t j=0 ; MSG[i] != '.' ; i++ , j++)
	{
		TmpBuf[j] = MSG[i];
		TmpBuf[j+1] = '\0';
	}
	i++;
	Time.Hours = atoi((const char*)TmpBuf);
	/*Minute*/
	for(uint8_t j=0 ; MSG[i] != '\0' ; i++ , j++)
	{
		TmpBuf[j] = MSG[i];
		TmpBuf[j+1] = '\0';
	}
	Time.Minutes =  atoi((const char*)TmpBuf);
	/**/
	Time.Seconds = 0;
	/**/
	Time.StoreOperation = 0;
	/**/
	Time.SecondFraction = 0;

	/*IRQ Disable*/
	HAL_NVIC_DisableIRQ(RTC_Alarm_IRQn);
	/*save date and time*/
	if(HAL_RTC_SetTime(&hrtc, &Time,RTC_FORMAT_BIN) == HAL_ERROR)
	{
		return HAL_ERROR;
	}
	else
	{
		if(HAL_RTC_SetDate(&hrtc,&Date,RTC_FORMAT_BIN) == HAL_ERROR)
		{
			return HAL_ERROR;
		}
	}
	/*IRQ Enable*/
	HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);

	return HAL_OK;
}
