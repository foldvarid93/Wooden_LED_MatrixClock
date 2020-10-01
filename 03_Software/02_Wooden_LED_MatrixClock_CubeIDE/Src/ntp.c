/***************************************************************************************
 * Includes
 **************************************************************************************/
//#include "application.h"
#include "ntp.h"
/***************************************************************************************
 * Variables
 **************************************************************************************/
/*globals*/
const uint8_t NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

/*externs*/
extern Serial_t serial;

extern ring_buffer *_rx_buffer;
/***************************************************************************************
 * Function definitions
 **************************************************************************************/
/*Sending AT command and wait */
HAL_StatusTypeDef ESP8266_NTP_ATCommand(const char * Command, const char * Respond, uint32_t Timeout)
{
	ESP8266_SendATCommand(Command, 0);

	if((ESP8266_WaitATAnswer(Respond, Timeout)) == 1){
		return HAL_OK;
	}
	return HAL_ERROR;
}

/*Init ESP8266 for NTP communication*/
HAL_StatusTypeDef ESP8266_NTP_Init(void)
{
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
	if(ESP8266_NTP_ATCommand("AT+RESTORE", "ready", LONG_PAUSE) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_NTP_ATCommand("AT", OK_STR, SHORT_PAUSE) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_NTP_ATCommand("ATE0", OK_STR, SHORT_PAUSE) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_NTP_ATCommand("AT+CWMODE=1", OK_STR, SHORT_PAUSE) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_NTP_ATCommand("AT+CWQAP", OK_STR, SHORT_PAUSE) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_NTP_ATCommand("AT+CWJAP=\"foldvarid93\",\"19701971\"", OK_STR,15000) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_NTP_ATCommand("AT+CIPMUX=0", OK_STR, SHORT_PAUSE) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	return HAL_OK;
}
/**/
HAL_StatusTypeDef ESP8266_NTP_GetDateTime(RTC_DataType *DateTime)
{
	/*Locals*/
	uint8_t NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
	uint8_t packetBuffer[ NTP_PACKET_SIZE];
	/**/
	Uart_flush();
	  if((ESP8266_NTP_ATCommand("AT+CIPSTART=\"UDP\",\"pool.ntp.org\",123", OK_STR, 1000)) != HAL_OK ){//if connection failed -> error
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
	  if(ESP8266_NTP_ATCommand("AT+CIPSEND=48", OK_STR, SHORT_PAUSE) != HAL_OK){
		  return HAL_ERROR;
	  }
	  /**/
	  UartPrintCharArray((char*)packetBuffer,NTP_PACKET_SIZE);

	  memset(packetBuffer, 0, NTP_PACKET_SIZE);
	  int i = 0;
	  //HAL_Delay(1000);
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
		  while (serial.available() > 0) {
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
	  else{
		  return HAL_ERROR;
	  }
	  /*UTC time: 1900 jan 1, monday, 0:00:00*/
	  uint32_t UTCTime_Sec = packetBuffer[40] << (24) | packetBuffer[41]<<16 | packetBuffer[42]<<8 | packetBuffer[43];
	  /*subtract seventy years*/
	  uint32_t UNIXTimeHungary_Sec = UTCTime_Sec - SEVENTYYEARS + NUMBEROFSECONDS_UTCOFFSET;
	  /*Year*/
	  uint32_t tmp = UTCTime_Sec + NUMBEROFSECONDS_UTCOFFSET;
	  DateTime->year = (tmp / NUMBEROFSECONDS_YEAR) - 100;

	   /*Month*/
	  /*31, 28(29), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31*/
	  const uint8_t MonthDays[12]={31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	  uint32_t tmp2 = tmp % NUMBEROFSECONDS_YEAR;
	  uint32_t tmp3 = (tmp2 /NUMBEROFSECONDS_DAY) + 1;//add 1 because current day
	  uint8_t index = 0;
	  DateTime->month = 1;//start at januar=1

	  while(index<=12)
	  {
		  if( ((int32_t)(tmp3-(MonthDays[index]))) >=0 )
		  {
			  tmp3 = tmp3 - MonthDays[index];
			  DateTime->month++;
			  index++;
		  }
		  else {
			  break;
		  }
	  }
	  //TODO:Leap year handling
	  if((DateTime->year % 4 == 0))//if leap year
	  {

	  }
	  else//ordinary year
	  {

	  }
	  /*Date*/
	  DateTime->date=1 + tmp3;
	  /*Weekday*/
	  tmp2 = tmp / NUMBEROFSECONDS_DAY;
	  tmp2 = tmp2 % 7;
	  DateTime->day = tmp2 + 1; // Because Monday is start from 1 not 0

	  /*Hour*/
	  DateTime->hour=((UNIXTimeHungary_Sec % NUMBEROFSECONDS_DAY) / NUMBEROFSECONDS_HOUR);
	  /*Minute*/
	  DateTime->min=((UNIXTimeHungary_Sec % NUMBEROFSECONDS_HOUR) / NUMBEROFSECONDS_MINUTE);
	  /*Second*/
	  DateTime->sec=(UNIXTimeHungary_Sec % NUMBEROFSECONDS_MINUTE);
	  /**/
	  if((ESP8266_NTP_ATCommand("AT+CIPCLOSE", OK_STR, SHORT_PAUSE)) != HAL_OK){
		  return HAL_ERROR;
	  }
	  /**/
	  return HAL_OK;
}
/**/
