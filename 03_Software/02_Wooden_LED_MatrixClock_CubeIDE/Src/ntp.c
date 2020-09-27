#include "ntp.h"
// -------------------------------------------------------------

const uint8_t NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
static const uint8_t NTP_Packet[48] = { 0xEC, 0x06, 0x00, 0xE3, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

const long UTC_DELTA = ((UTC_DELTA_HOURS * NUMBEROFSECONDSPERHOUR)
		+ (UTC_DELTA_MINUTES * NUMBEROFSECONDSPERMINUTE));

const char * OK_STR = "OK\r\n";
extern Serial_t serial;
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
	HAL_Delay(100);
	//
	Ringbuf_init();
	//
	initSerial();
	//
	ESP8266_NTP_ATCommand("AT+RESTORE", "ready", LONG_PAUSE); // reset
	ESP8266_NTP_EmptyRX(SHORT_PAUSE);
	ESP8266_NTP_ATCommand("AT", OK_STR, SHORT_PAUSE); //is all OK?
	ESP8266_NTP_ATCommand("AT+CWMODE=1", OK_STR, SHORT_PAUSE); //Set the wireless mode
	ESP8266_NTP_ATCommand("AT+CWQAP", OK_STR, SHORT_PAUSE); //disconnect  - it shouldn't be but just to make sure
	ESP8266_NTP_ATCommand("AT+CWJAP=\"foldvarid93\",\"19701971\"", "OK",LONG_PAUSE); // connect to wifi
	ESP8266_NTP_ATCommand("AT+CIPMUX=0", OK_STR, SHORT_PAUSE); //set the single connection mode
}

void ESP8266_NTP_GetPacket(void)
{
	 unsigned long epochUnix;
	 uint8_t  ntpHours, ntpMinutes, ntpSeconds;

	 //if (HAL_GetTick() - lastTime >= 5000UL) { // change 5000UL to adjust your RTC once a day use (NUMBEROFSECONDSPERDAY * 1000UL)
	 //now = rtc.now();
	 epochUnix = ESP8266_NTP_EpochUnixNTP();
	 ntpHours = (epochUnix  % NUMBEROFSECONDSPERDAY) / NUMBEROFSECONDSPERHOUR;
	 ntpMinutes = (epochUnix % NUMBEROFSECONDSPERHOUR) / NUMBEROFSECONDSPERMINUTE;
	 ntpSeconds = epochUnix % NUMBEROFSECONDSPERMINUTE;

	 // here you should actually check if now.hour() versus ntpHours are not way off. if so you might have been unlucky and caught midnight.
	 // I'm not doing it - left to the reader :-)
	 //rtc.adjust(DateTime(now.year(), now.month(), now.day(), ntpHours, ntpMinutes, ntpSeconds));
	 //now = rtc.now(); // get the adjusted time

	 // print the hour, minute and second:
	 //Serial.print("The local NTP time is ");
	 /*
	 Serial.print(ntpHours); // print the hour ()
	 Serial.print(':');
	 if ( ntpMinutes < 10 ) Serial.print('0');    // In the first 10 minutes of each hour, we'll want a leading '0'
	 Serial.print(ntpMinutes);
	 Serial.print(':');
	 if (ntpSeconds < 10) Serial.print('0');    // In the first 10 seconds of each minute, we'll want a leading '0'
	 Serial.println(ntpSeconds); // print the second

	 // print the hour, minute and second:
	 Serial.print("The RTC time is ");
	 Serial.print(now.year(), DEC);
	 Serial.print('/');
	 Serial.print(now.month(), DEC);
	 Serial.print('/');
	 Serial.print(now.day(), DEC);
	 Serial.print(" - ");
	 Serial.print(now.hour(), DEC);
	 Serial.print(':');
	 Serial.print(now.minute(), DEC);
	 Serial.print(':');
	 Serial.print(now.second(), DEC);
	 Serial.println();

	 lastTime = HAL_GetTick();
	 */
	// }
}
