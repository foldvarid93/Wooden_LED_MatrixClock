#include "application.h"
// -------------------------------------------------------------

const uint8_t NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
static const uint8_t NTP_Packet[48] = { 0xEC, 0x06, 0x00, 0xE3, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

const long UTC_DELTA = ((UTC_DELTA_HOURS * NUMBEROFSECONDSPERHOUR)
		+ (UTC_DELTA_MINUTES * NUMBEROFSECONDSPERMINUTE));

//const char * OK_STR = "OK\r\n";
#define OK_STR "OK"
extern Serial_t serial;
extern ring_buffer *_rx_buffer;
// =====================================================================================

// --------------------------------------
// ESP8266_NTP_EmptyRX waits for duration ms
// and get rid of anything arriving
// on the ESP Serial port during that delay
// --------------------------------------

void ESP8266_NTP_EmptyRX(unsigned long duration)
{
	unsigned long currentTime;
	currentTime = HAL_GetTick();
	while (HAL_GetTick() - currentTime <= duration) {
		if (serial.available() > 0)
			serial.read();
	}
}

// --------------------------------------
// ESP8266_NTP_WaitForString wait max for duration ms
// while checking if endMarker string is received
// on the ESP Serial port
// returns a bool stating if the marker
// was found
// --------------------------------------

bool ESP8266_NTP_WaitForString(const char * endMarker, unsigned long duration)
{
	int localBufferSize = strlen(endMarker); // we won't need an \0 at the end
	char localBuffer[localBufferSize];
	int index = 0;
	bool endMarkerFound = false;
	unsigned long currentTime;

	memset(localBuffer, '\0', localBufferSize); // clear buffer

	currentTime = HAL_GetTick();
	while (HAL_GetTick() - currentTime <= duration) {
		if (serial.available() > 0) {
			if (index == localBufferSize)
				index = 0;
			localBuffer[index] = (uint8_t) serial.read();
			endMarkerFound = true;
			for (int i = 0; i < localBufferSize; i++) {
				if (localBuffer[(index + 1 + i) % localBufferSize]
						!= endMarker[i]) {
					endMarkerFound = false;
					break;
				}
			}
			index++;
		}
		if (endMarkerFound)
			break;
	}
	return endMarkerFound;
}

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

// --------------------------------------
// ESP8266_NTP_EpochUnixNTP returns the UNIX time
// number of seconds sice Jan 1 1970
// adjusted for timezoneDeltaFromUTC
// --------------------------------------
void sendNTPPacket(const char * Packet, uint8_t Length)
{
	for (uint8_t i = 0; i < Length; i++) {
		serial.write(*(Packet+i));
	}
}

unsigned long ESP8266_NTP_EpochUnixNTP(void)
{
	unsigned long secsSince1900 = 0UL;
	unsigned long epochUnix;

	// AT+CIPSTART="UDP","fr.pool.ntp.org",123
	ESP8266_NTP_ATCommand("AT+CIPSTART=\"UDP\",\"pool.ntp.org\",123", OK_STR,LONG_PAUSE);
	ESP8266_NTP_ATCommand("AT+CIPSEND=48", OK_STR, SHORT_PAUSE); //send 48 (NTP_PACKET_SIZE) bytes
	ESP8266_NTP_EmptyRX(1000UL); // empty the buffer (we get a > character)
	sendNTPPacket((char*)NTP_Packet, NTP_PACKET_SIZE);
	//sendATCommand((char *)NTP_Packet);
	//serial.write((const uint8_t) *NTP_Packet); // the first 4 bytes matters, then we don't care, whatever is in the memory will do

	// skip  AT command answer ("Recv 48 bytes\r\n\r\nSEND OK\r\n\r\n+IPD,48:")
	ESP8266_NTP_WaitForString(":", SHORT_PAUSE);

	// read the NTP packet, extract the TRANSMIT TIMESTAMP in Seconds from bytes 40,41,42,43
	for (int i = 0; i < NTP_PACKET_SIZE; i++) {
		while (!serial.available())
			;
		int c = serial.read();
		if ((i >= 40) && (i < 44))
			secsSince1900 = (secsSince1900 << 8)
					+ (unsigned long) ((uint8_t) (c & (int) 0x00FF)); // Read the integer part of sending time
		else if (i == 44)
			secsSince1900 += (((uint8_t) c) > SECONDROUNDINGTHRESHOLD ? 1 : 0);
	}

	// subtract seventy years:
	epochUnix = secsSince1900 - SEVENTYYEARS;

	ESP8266_NTP_ATCommand("AT+CIPCLOSE", OK_STR, SHORT_PAUSE); // close connection
	return epochUnix + UTC_DELTA;
}

// =====================================================================================
void ESP8266_NTP_Init(void)
{
	//ESP8266 HW reset and enabling
	HAL_GPIO_WritePin(ESP8266_RST_GPIO_Port, ESP8266_RST_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ESP8266_EN_GPIO_Port, ESP8266_EN_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ESP8266_EN_GPIO_Port, ESP8266_EN_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(ESP8266_RST_GPIO_Port, ESP8266_RST_Pin, GPIO_PIN_SET);
	HAL_Delay(1000);
	//
	Ringbuf_init();
	//
	initSerial();

	//
	/*
	sendATCommand("AT+RESTORE", 0);
	HAL_Delay(1500);
	Uart_flush();
	sendATCommand("AT", 0);
	if(Wait_forr((char*)"\r\nOK\r\n",1000) == 0 ){
		Error_Handler();
	}
	sendATCommand("ATE0", 0);
	if(Wait_forr((char*)"\r\nOKI\r\n",1000) == 0 ){
		Error_Handler();
	}
	 */

	/**/
	if(ESP8266_NTP_ATCommand("AT+RESTORE", "ready", LONG_PAUSE) == 0){
		  while(1){
			  asm("nop");//debugnop
		  }
	}
	if(ESP8266_NTP_ATCommand("AT", OK_STR, SHORT_PAUSE) == 0){
		Error_Handler();		  while(1){
			  asm("nop");//debugnop
		  }	}
	if(ESP8266_NTP_ATCommand("ATE0", OK_STR, SHORT_PAUSE) == 0){
		  while(1){
			  asm("nop");//debugnop
		  }
	}
	if(ESP8266_NTP_ATCommand("AT+CWMODE=1", OK_STR, SHORT_PAUSE) == 0){
		  while(1){
			  asm("nop");//debugnop
		  }
	}
	if(ESP8266_NTP_ATCommand("AT+CWQAP", OK_STR, SHORT_PAUSE) == 0){
		  while(1){
			  asm("nop");//debugnop
		  }
	}
	if(ESP8266_NTP_ATCommand("AT+CWJAP=\"foldvarid93\",\"19701971\"", OK_STR,15000) == 0){
		  while(1){
			  asm("nop");//debugnop
		  }
	}
	if(ESP8266_NTP_ATCommand("AT+CIPMUX=0", OK_STR, SHORT_PAUSE) == 0){
		  while(1){
			  asm("nop");//debugnop
		  }
	}
	/* */
	/*
	ESP8266_NTP_ATCommand("AT+RESTORE", "ready", LONG_PAUSE); // reset
	ESP8266_NTP_ATCommand("AT", OK_STR, SHORT_PAUSE); // reset
	ESP8266_NTP_ATCommand("ATE0", OK_STR, SHORT_PAUSE); // reset
	ESP8266_NTP_ATCommand("AT+CWMODE=1", OK_STR, SHORT_PAUSE); //Set the wireless mode
	ESP8266_NTP_ATCommand("AT+CWQAP", OK_STR, SHORT_PAUSE); //disconnect  - it shouldn't be but just to make sure

	//ESP8266_NTP_ATCommand("AT+CWJAP=\"Foldvari-Net\",\"19701971\"", "OK",LONG_PAUSE); // connect to wifi
	ESP8266_NTP_ATCommand("AT+CWJAP=\"foldvarid93\",\"19701971\"", OK_STR,15000); // connect to wifi

	ESP8266_NTP_ATCommand("AT+CIPMUX=0", OK_STR, SHORT_PAUSE); //set the single connection mode
	*/
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
	  /**/
	  uint32_t secsSince1900 = packetBuffer[40] << (24) | packetBuffer[41]<<16 | packetBuffer[42]<<8 | packetBuffer[43];

	  /*subtract seventy years*/
	  unsigned long epoch = secsSince1900 - seventyYears + UTCOffset;
	  /**/
	  DateTime->hour=((epoch % 86400L) / 3600);
	  DateTime->min=((epoch % 3600) / 60);
	  DateTime->sec=(epoch % 60);
	  /**/
	  if((ESP8266_NTP_ATCommand("AT+CIPCLOSE", OK_STR, SHORT_PAUSE)) == 0){
		  return HAL_ERROR;
	  }
	  /**/
	  return HAL_OK;
}
