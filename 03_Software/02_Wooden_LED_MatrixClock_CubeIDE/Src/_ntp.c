#include "ntp.h"
extern Serial_t serial;
void ntpupdate()
{
  const uint8_t NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
  uint8_t packetBuffer[ NTP_PACKET_SIZE];
 
  serial.println("AT+CIPSTART=\"UDP\",\"pool.ntp.org\",123");
  HAL_Delay(500);
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
 
 // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
 
  packetBuffer[0] = 0b11100011; // LI, Version, Mode
  packetBuffer[1] = 1; // Stratum, or type of clock
  packetBuffer[2] = 6; // Polling Interval
  packetBuffer[3] = 0xEC; // Peer Clock Precision
 
 // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  serial.println("AT+CIPSEND=48");
  //serial.println(NTP_PACKET_SIZE);
  HAL_Delay(500);
  for (uint8_t i = 0; i < NTP_PACKET_SIZE; i++) {
    serial.write(packetBuffer[i]);
  }

  int counta = 0;
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  if (serial.find("+IPD,48:"))
  {
    //dbgSerial.println("Server answer : ");

    int i = 0;
    while (serial.available() > 0) {
      uint8_t ch = serial.read();
      if (i < NTP_PACKET_SIZE)
      {
        packetBuffer[i] = ch;
      }
      i++;
      if ( ( i < NTP_PACKET_SIZE ) && ( serial.available() == 0 ) )
      {
        while (serial.available() == 0) // you may have to wait for some bytes
        {
          counta += 1;
          //dbgSerial.print("!");
          HAL_Delay(100);
          if (counta == 15) {
            exit;
          }
        }
      }
    }
  }
   //the timestamp starts at byte 40 of the received packet and is four bytes,
  // or two words, long. First, esxtract the two words:

  //unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
  //unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
  // combine the four bytes (two words) into a long integer
  // this is NTP time (seconds since Jan 1 1900):
  //unsigned long secsSince1900 = highWord << 16 | lowWord;
  //dbgSerial.print("Seconds since Jan 1 1900 = " );
  //dbgSerial.println(secsSince1900);

  // now convert NTP time into everyday time:
  //dbgSerial.print("Unix time = ");
  // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
  //const unsigned long seventyYears = 2208988800UL;
  // subtract seventy years:
  //unsigned long epoch = secsSince1900 - seventyYears;
  // print Unix time:
  //dbgSerial.println(epoch);

  // print the hour, minute and second:
  //dbgSerial.print("The UTC time is "); // UTC is the time at Greenwich Meridian (GMT)
  //dbgSerial.print((epoch % 86400L) / 3600); // print the hour (86400 equals secs per day)
  //dbgSerial.print(':');
  //if ( ((epoch % 3600) / 60) < 10 ) {
    // In the first 10 minutes of each hour, we'll want a leading '0'
    //dbgSerial.print('0');
  //}
  //dbgSerial.print((epoch % 3600) / 60); // print the minute (3600 equals secs per minute)
  //dbgSerial.print(':');
  //if ( (epoch % 60) < 10 ) {
    // In the first 10 seconds of each minute, we'll want a leading '0'
    //dbgSerial.print('0');
 // }
  //dbgSerial.println(epoch % 60); // print the second
  //dbgSerial.println(" ");
  serial.println("AT+CIPCLOSE");
}
