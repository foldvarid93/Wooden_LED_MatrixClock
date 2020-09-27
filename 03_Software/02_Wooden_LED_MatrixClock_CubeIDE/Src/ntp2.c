#include "ntp.h"
#include "remotexy.h"
#define NTP_PACKET_SIZE  48 // NTP time stamp is in the first 48 uint8_ts of the message
uint8_t packetBuffer [NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
extern Serial_t serial;
unsigned long GetTime(void)
{
  char cmd[] = "AT+CIPSTART=\"UDP\",\"192.168.0.28\",123"; // my own NTP server, synced by GPS
  sendATCommand(cmd); //serial.println(cmd);
  HAL_Delay(2000);
	if (!waitATAnswer(AT_ANSWER_OK, 1000))
		return 0;
  if(1
		  //serial.find("Error")
		  ){
    //Serial.print("RECEIVED: Error");
    return 0;
  }
  
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  
  //serial.print("AT+CIPSEND=");
  serial.println(NTP_PACKET_SIZE);
  if(serial.find(">"))
  {
    for (uint8_t i = 0; i < NTP_PACKET_SIZE; i++)
    {
      serial.write(packetBuffer[i]);
      HAL_Delay(5);
    }
  }else{
    serial.println("AT+CIPCLOSE");
    return 0;
  }
  
  //serial.find("+IPD,48:");
  
  int acksize = NTP_PACKET_SIZE + 1 + 2 + 8; // ESP8266 adds a space, a CRLF and starts with "+IPD,48:"
  
  //Serial.println("ESP2866 ACK : ");
  for (uint8_t i = 0; i < acksize; i++)
  {
    while (serial.available() == 0) // you may have to wait for some uint8_ts
    {
      //Serial.print(".");
    }
    uint8_t ch = serial.read();
    if (ch < 0x10) //Serial.print('0');
    //Serial.print(ch,HEX);
    //Serial.print(' ');
    if ( (((i+1) % 15) == 0) ) { //Serial.println(); }
  }
  //Serial.println();
  //Serial.println();

  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  
  //Serial.println("Server answer : ");

  int i = 0;
  while (serial.available() > 0) {
    uint8_t ch = serial.read();
    if (i <= NTP_PACKET_SIZE)
    {
      packetBuffer[i] = ch;
    }
    if (ch < 0x10) //Serial.print('0');
    //Serial.print(ch,HEX);
    //Serial.print(' ');
    if ( (((i+1) % 15) == 0) ) { //Serial.println(); }
    HAL_Delay(5);
    i++;
    if ( ( i < NTP_PACKET_SIZE ) && ( serial.available() == 0 ) )
    {
      while (serial.available() == 0)  // you may have to wait for some uint8_ts
      {
        //Serial.print("!");
      }
    }
  }
  
  //Serial.println();
  //Serial.println();
  //Serial.print(i+1);
  //Serial.println(" uint8_ts received"); // will be more than 48
  
  //Serial.print(packetBuffer[40],HEX);
  //Serial.print(" ");
  //Serial.print(packetBuffer[41],HEX);
  //Serial.print(" ");
  //Serial.print(packetBuffer[42],HEX);
  //Serial.print(" ");
  //Serial.print(packetBuffer[43],HEX);
  //Serial.print(" = ");

  unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
  unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);  
    // combine the four uint8_ts (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
  unsigned long secsSince1900 = highWord << 16 | lowWord;
  
  //Serial.print(secsSince1900,DEC);
  
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
  const unsigned long seventyYears  = 2208988800UL;
    // subtract seventy years:
  unsigned long epoch = secsSince1900 - seventyYears;
  
  unsigned long DST = 60*60*2; // adjust to your GMT+DST
  
  unsigned long timestamp = epoch + DST;

  //Serial.println();
  //Serial.print("Epoch : ");
  //Serial.println(epoch,DEC);

  return timestamp;
  }
}
}
