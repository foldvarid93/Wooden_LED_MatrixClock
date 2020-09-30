#include "application.h"
// -------------------------------------------------------------
const uint8_t NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
const long UTC_DELTA = ((UTC_DELTA_HOURS * NUMBEROFSECONDSPERHOUR)
		+ (UTC_DELTA_MINUTES * NUMBEROFSECONDSPERMINUTE));

//const char * OK_STR = "OK\r\n";
#define OK_STR "OK"
extern Serial_t serial;
extern ring_buffer *_rx_buffer;
// =====================================================================================

// --------------------------------------
// ESP8266_NTP_ATCommand executes an AT commmand
// checks if endMarker string is received
// on the ESP Serial port for max duration ms
// returns a bool stating if the marker
// was found
// --------------------------------------

bool ESP8266_NTP_ATCommand(const char * command, const char * endMarker,unsigned long duration)
{
	sendATCommand(command, 0);
	//ESPSEPRIAL.println(command);
	return (waitATAnswer(endMarker, duration));
	//return ESP8266_NTP_WaitForString(endMarker, duration);
}
// =====================================================================================
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
	initSerial();
	/**/
	if(ESP8266_NTP_ATCommand("AT+RESTORE", "ready", LONG_PAUSE) == 0)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_NTP_ATCommand("AT", OK_STR, SHORT_PAUSE) == 0)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_NTP_ATCommand("ATE0", OK_STR, SHORT_PAUSE) == 0)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_NTP_ATCommand("AT+CWMODE=1", OK_STR, SHORT_PAUSE) == 0)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_NTP_ATCommand("AT+CWQAP", OK_STR, SHORT_PAUSE) == 0)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_NTP_ATCommand("AT+CWJAP=\"foldvarid93\",\"19701971\"", OK_STR,15000) == 0)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_NTP_ATCommand("AT+CIPMUX=0", OK_STR, SHORT_PAUSE) == 0)
	{
		  return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef ESP8266_NTP_GetDateTime(RTC_DataType *DateTime)
{
	/*Locals*/
	uint8_t NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
	uint8_t packetBuffer[ NTP_PACKET_SIZE];
	/**/
	Uart_flush();
	  if((ESP8266_NTP_ATCommand("AT+CIPSTART=\"UDP\",\"pool.ntp.org\",123", OK_STR, 1000)) == 0 ){//if connection failed -> error
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
	  if(ESP8266_NTP_ATCommand("AT+CIPSEND=48", OK_STR, SHORT_PAUSE) == 0){
		  return HAL_ERROR;
	  }

	  /**/
	  UartPrintCharArray((char*)packetBuffer,NTP_PACKET_SIZE);

	  memset(packetBuffer, 0, NTP_PACKET_SIZE);
	  int i = 0;
	  //HAL_Delay(1000);
	  /**/
	  if (Wait_forr("+IPD,48:",1000) == 1)
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
	  unsigned long UNIXTimeHungary_Sec = UTCTime_Sec - seventyYears + UTCOffset_Sec;
	  /*Year*/
	  uint32_t tmp = UTCTime_Sec + UTCOffset_Sec;//
	  //DateTime->year = 1900 + (tmp / AvgYearSeconds);
	  DateTime->year = (tmp / AvgYearSeconds) - 100;
	  /*Month*/
	  /*31, 28(29), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31*/
	  uint32_t tmp2 = tmp % AvgYearSeconds;
	  uint8_t index = 0;
	  const uint8_t MonthDays[12]={31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	  while(index <= 12)
	  {
		  if ( ((int32_t)(tmp2 - (MonthDays[index] * DaySeconds))) >= 0 ){
			  tmp2 = tmp2 - (MonthDays[index] * DaySeconds);
			  index++;
		  }
		  else{
			  DateTime->month = index + 1;// Because January is start from 1 not 0
			  break;
		  }
	  }
	  //TODO:
	  if((DateTime->year % 4 == 0))//if leap year
	  {

	  }
	  else//ordinary year
	  {

	  }
	  /*Date*/
	  tmp2 /= DaySeconds;
	  DateTime->date= tmp2 + 2; //Because +1 for indexing, and +1 for the current day
	  /*Weekday*/
	  tmp2 = tmp / DaySeconds;
	  tmp2 = tmp2 % 7;
	  DateTime->day = tmp2 + 1; // Because Monday is start from 1 not 0
	  /*Hour*/
	  DateTime->hour=((UNIXTimeHungary_Sec % 86400L) / 3600);
	  /*Minute*/
	  DateTime->min=((UNIXTimeHungary_Sec % 3600) / 60);
	  /*Second*/
	  DateTime->sec=(UNIXTimeHungary_Sec % 60);
	  /**/
	  if((ESP8266_NTP_ATCommand("AT+CIPCLOSE", OK_STR, SHORT_PAUSE)) == 0){
		  return HAL_ERROR;
	  }
	  /**/
	  return HAL_OK;
}
