/* RemoteXY.h 
   A RemoteXY Library - Remote device control
   version 2.4.4   
   ===========================================================
   For use RemoteXY library visit website http://remotexy.com
   This website will help you use the library for configuring 
   a remote control from a smartphone or tablet.
   
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
        
   Supported modes: 
   All boards:
    #define REMOTEXY_MODE__HARDSERIAL                 - direct data transfer via HARDSERIAL
    #define REMOTEXY_MODE__SOFTSERIAL                 - direct data transfer via SOFTSERIAL
    #define REMOTEXY_MODE__ETHERNET                   - data transfer using <ethernet.h> library and open server
    #define REMOTEXY_MODE__ETHERNET_CLOUD             - data transfer using <ethernet.h> library and cloud connection
    #define REMOTEXY_MODE__HARDSERIAL_ESP8266         - data transfer via HARDSERIAL using AT commands of ESP8266 and open server
    #define REMOTEXY_MODE__HARDSERIAL_ESP8266_POINT   - data transfer via HARDSERIAL using AT commands of ESP8266 and open access point with a server
    #define REMOTEXY_MODE__HARDSERIAL_ESP8266_CLOUD   - data transfer via HARDSERIAL using AT commands of ESP8266 and cloud connection
    #define REMOTEXY_MODE__SOFTSERIAL_ESP8266         - data transfer via SOFTSERIAL using AT commands of ESP8266 and open server
    #define REMOTEXY_MODE__SOFTSERIAL_ESP8266_POINT   - data transfer via SOFTSERIAL using AT commands of ESP8266 and open access point with a server
    #define REMOTEXY_MODE__SOFTSERIAL_ESP8266_CLOUD   - data transfer via SOFTSERIAL using AT commands of ESP8266 and cloud connection
    #define REMOTEXY_MODE__WIFI                       - data transfer using wifi.h library and open server
    
   Only ESP8266 boards:
    #define REMOTEXY_MODE__ESP8266CORE_ESP8266WIFI           - data transfer using <esp8266wifi.h> library and open server
    #define REMOTEXY_MODE__ESP8266CORE_ESP8266WIFI_POINT     - data transfer using <esp8266wifi.h> library and open access point with a server
    #define REMOTEXY_MODE__ESP8266CORE_ESP8266WIFI_CLOUD     - data transfer using <esp8266wifi.h> library and cloud connection

   Only ESP32 boards:
    #define REMOTEXY_MODE__ESP32CORE_WIFI                    - data transfer using <wifi.h> library and open server
    #define REMOTEXY_MODE__ESP32CORE_WIFI_POINT              - data transfer using <wifi.h> library and open access point with a server
    #define REMOTEXY_MODE__ESP32CORE_WIFI_CLOUD              - data transfer using <wifi.h> library and cloud connection
    #define REMOTEXY_MODE__ESP32CORE_BLE                     - data transfer using <BLEdevice.h> library
    #define REMOTEXY_MODE__ESP32CORE_BLUETOOTH               - data transfer using <BluetoothSerial.h> library of classic bluetooth

   Parameters depending on the selected mode (for example):
    #define REMOTEXY_SERIAL Serial  // for Hardware Serial
    #define REMOTEXY_SERIAL_SPEED 115200 
    #define REMOTEXY_SERIAL_RX 2   // for Software Serial
    #define REMOTEXY_SERIAL_TX 3   // for Software Serial
    #define REMOTEXY_WIFI_SSID "RemoteXY"  
    #define REMOTEXY_WIFI_PASSWORD "1234567890" 
    #define REMOTEXY_ETHERNET_MAC "DE:AD:BE:EF:EF:ED"  // for Ethernet modules
    #define REMOTEXY_SERVER_PORT 6377 
    #define REMOTEXY_CLOUD_TOKEN "xxxx" // for Cloud
    #define REMOTEXY_CLOUD_SERVER "cloud.remotexy.com" // for Cloud
    #define REMOTEXY_CLOUD_PORT 6376  // for Cloud
    #define REMOTEXY_ACCESS_PASSWORD "1" 

   Debug log info on 115200 (define before include this library):
    #define REMOTEXY__DEBUGLOGS Serial

   = Version history ========================================

   version 2.2.5   
     - support MPIDE;
   version 2.3.1    
     - Support the device access password;
     - Support the cloud server as beta test;
     - Fixed a bug where the length of variables more than 255;
     - Fixed a bug where ESP module reboot and device did not see it;
     - Fixed a bug where the connection was filed and the device 
         did not see it and reconnection is impossible 
   version 2.3.4  
     - Fixed a bug where the length of all input variables more than 256;
     - Fixed a bug where HAL_GetTick() overflow in 50 days;
     - Fixed some bugs;
   version 2.3.5  
     - Fixed some bugs;
   version 2.4.1
     - support ESP32 WiFi and BLE   
   version 2.4.2
     - Fixed some bugs;
   version 2.4.3
     - Fixed some bugs;
   version 2.4.4
     - Fixed ESP32 BLE bugs;
   version 2.4.5
     - support ESP32 classic Bluetooth   
     - Fixed some bugs;
   version 2.4.6
     - update input variables on a smartphone;
          
*/
#include "remotexy.h"
#include "UartRingbuffer_multi.h"

#define REMOTEXY_WIFI__POINT
//#define REMOTEXY__DEBUGLOGS Serial
#define REMOTEXY__DEBUGLOGS_SPEED 115200
#define REMOTEXY_ESP8266_MAX_SEND_BYTES 2048
#define REMOTEXY_ESP8266_MODULETEST_TIMEOUT 30000
#define REMOTEXY_PACKAGE_START_BYTE 0x55
#define REMOTEXY_PASSWORD_LENGTH_MAX 26
#define REMOTEXY_TIMEOUT 5000
#define REMOTEXY_SERVER_TIMEOUT 7000

#define REMOTEXY_CLOUD_RETRY_TIMEOUT 500
#define REMOTEXY_CLOUD_CONNECT_TIMEOUT 10000
#define REMOTEXY_CLOUD_RECONNECT_TIMEOUT 30000
#define REMOTEXY_CLOUD_ECHO_TIMEOUT 60000
#define REMOTEXY_CLOUD_STATE_STOP 0
#define REMOTEXY_CLOUD_STATE_WAIT_RECONNECT 1
#define REMOTEXY_CLOUD_STATE_WAIT_NEXT_TRY 2
#define REMOTEXY_CLOUD_STATE_CONNECTION 3
#define REMOTEXY_CLOUD_STATE_REGISTRATION 6
#define REMOTEXY_CLOUD_STATE_WORKING 7

#define REMOTEXY_MODE__ESP8266_HARDSERIAL_POINT
#define REMOTEXY_SERIAL Serial
#define REMOTEXY_SERIAL_SPEED 115200
#define REMOTEXY_WIFI_SSID "RemoteXY"
#define REMOTEXY_WIFI_PASSWORD "12345678"
#define REMOTEXY_SERVER_PORT 6377
#define AT_BUFFER_STR_LENGTH 10

const char * AT_ANSWER_ERROR = "ERROR";
const char * AT_ANSWER_OK = "OK";
const char * AT_ANSWER_SEND_OK = "SEND OK";
const char * AT_MESSAGE_READY = "ready";
const char * AT_ANSWER_GO = ">";
const char * AT_MESSAGE_AT = "AT";
const char * AT_MESSAGE_CONNECT = "?,CONNECT";
const char * AT_MESSAGE_CLOSED = "?,CLOSED";
const char * AT_MESSAGE_CONNECT_FAIL = "?,CONNECT FAIL";
const char * AT_MESSAGE_IPD = "+IPD,?,*:";
//variables
CRemoteXY remotexy;

Serial_t serial={
		.read=NULL,
		.write=NULL,
		.available=NULL
};
extern UART_HandleTypeDef huart3;

extern ring_buffer *_rx_buffer1;



uint8_t RemoteXY_CONF[] =
  { 255,48,0,0,0,66,0,10,66,1,
  1,1,26,63,12,12,36,8,83,101,
  116,0,7,4,2,26,59,7,2,27,
  38,36,129,0,20,34,24,6,8,78,
  101,116,119,111,114,107,0,129,0,17,
  51,28,6,8,80,97,115,115,119,111,
  114,100,0,7,4,3,42,57,7,2,
  27,2,11
 };

/**********************************************************************************************************/
void RemoteXY_Init(void)
{
	CRemoteXY_Init(RemoteXY_CONF, &RemoteXY, REMOTEXY_ACCESS_PASSWORD, REMOTEXY_WIFI_SSID, REMOTEXY_WIFI_PASSWORD, REMOTEXY_SERVER_PORT);
}

void CRemoteXY_Init(const void * _conf, void * _var, const char * _accessPassword, const char * _wifiSsid, const char * _wifiPassword, uint16_t _port)
{
	initSerial();
    initAT();
    remotexy.wifiSsid = (char *) _wifiSsid;
    remotexy.wifiPassword = (char *) _wifiPassword;
    remotexy.port = _port;
    remotexy.connectCannel=0;
    remotexy.connectAvailable=0;
    remotexy.freeAvailable=0;
    remotexy.sendBytesAvailable=0;
    remotexy.sendBytesLater=0;
    init (_conf, _var, _accessPassword);
    remotexy.moduleTestTimeout = HAL_GetTick();
    //Uart_flush (&huart3);
  }



  uint8_t initModule () {
    
#if defined(REMOTEXY__DEBUGLOGS)
    DEBUGLOGS_write ("Find ESP module...");
#endif     

    uint8_t tryCount=20;
    while (--tryCount>0) {
      
      sendATCommand ("AT",0);
      if (waitATAnswer (AT_ANSWER_OK, 1000)) break;
    }
    if (tryCount==0) {
#if defined(REMOTEXY__DEBUGLOGS)
      DEBUGLOGS_write ("ESP module not found");
#endif     
      return 0;
    }
    sendATCommand ("AT+RST",0);
    if (!waitATAnswer (AT_ANSWER_OK, 1000)) return 0; 
    if (!waitATAnswer (AT_MESSAGE_READY, 5000)) return 0;
     
    return setModule (); 
  }
  
  uint8_t setModule () {    
    char sport[6];    
    rxy_itos (remotexy.port, sport);
    char stimeout[6];
    rxy_itos (REMOTEXY_SERVER_TIMEOUT/1000, stimeout); 
    
    remotexy.connectCannel=0;
    remotexy.connectAvailable=0;
    
    sendATCommand ("ATE0",0);
    if (!waitATAnswer (AT_ANSWER_OK, 1000)) return 0;   
#if defined(REMOTEXY_WIFI__POINT)
    sendATCommand ("AT+CWMODE=2",0);
    if (!waitATAnswer (AT_ANSWER_OK, 2000)) return 0;   
    sendATCommand ("AT+CWDHCP=0,1",0);
    if (!waitATAnswer (AT_ANSWER_OK, 2000)) return 0;    
    char crypt[2] = {*remotexy.wifiPassword?'4':'0',0};
    sendATCommand ("AT+CWSAP=" , "\"" , remotexy.wifiSsid, "\"," , "\"" , remotexy.wifiPassword , "\"," , "10,",crypt,0);
    if (!waitATAnswer (AT_ANSWER_OK, 5000)) return 0;  
#else
    sendATCommand ("AT+CWMODE=1",0);
    if (!waitATAnswer (AT_ANSWER_OK, 2000)) return 0;   
    sendATCommand ("AT+CWQAP",0);
    if (!waitATAnswer (AT_ANSWER_OK, 2000)) return 0;   
    sendATCommand ("AT+CWDHCP=1,1",0);
    if (!waitATAnswer (AT_ANSWER_OK, 2000)) return 0;    
    sendATCommand ("AT+CWJAP=",remotexy.wifiSsid,",",remotexy.wifiPassword,0);
    if (!waitATAnswer (AT_ANSWER_OK, 30000)) return 0;  
#if defined(REMOTEXY__DEBUGLOGS)
    sendATCommand ("AT+CIPSTA?",0);
    if (!waitATAnswer (AT_ANSWER_OK, 2000)) return 0;  
#endif     
#endif  
    sendATCommand ("AT+CIPMODE=0",0);
    if (!waitATAnswer (AT_ANSWER_OK, 2000)) return 0;   
    sendATCommand ("AT+CIPMUX=1",0);
    if (!waitATAnswer (AT_ANSWER_OK, 1000)) return 0;
    sendATCommand ("AT+CIPSERVER=1,",sport,0);
    if (!waitATAnswer (AT_ANSWER_OK, 1000)) return 0; 
    sendATCommand ("AT+CIPSTO=",stimeout,0);
    if (!waitATAnswer (AT_ANSWER_OK, 1000)) return 0;
    remotexy.moduleTestTimeout = HAL_GetTick();
    return 1;
  }

  
    
  void handlerModule(void)
  {
    while (serial.available()>0)
    {
      if (remotexy.connectAvailable) break;
      if (remotexy.freeAvailable) {
        serial.read();
        remotexy.freeAvailable--;
      }
      else {     
        readATMessage();
      }
      remotexy.moduleTestTimeout = HAL_GetTick();
    }
    if (HAL_GetTick() - remotexy.moduleTestTimeout > REMOTEXY_ESP8266_MODULETEST_TIMEOUT) {
      remotexy.moduleTestTimeout = HAL_GetTick();
      if (testATecho ()==2) setModule ();
    }  
  }
 
  void readyAT () {
    setModule ();
  }

  void connectAT () {
    if (remotexy.connectCannel==0) {
      remotexy.connectCannel=*(remotexy.params[0]);
      remotexy.connectAvailable=0;
    }
  };
 
  void closedAT () {
    if (remotexy.connectCannel==*(remotexy.params[0])) remotexy.connectCannel=0;
  }
  
  void inputDataAT () {
    uint16_t size;
    size=getATParamInt (1);
    if (remotexy.connectCannel==*(remotexy.params[0])) remotexy.connectAvailable=size;
    else remotexy.freeAvailable = size;
  }
  
  
  void sendStart (uint16_t len) {
    char s[8];
    if (remotexy.connectCannel) {
      remotexy.sendBytesLater=0;
      if (len>REMOTEXY_ESP8266_MAX_SEND_BYTES) {
        remotexy.sendBytesLater=len-REMOTEXY_ESP8266_MAX_SEND_BYTES;
        len=REMOTEXY_ESP8266_MAX_SEND_BYTES;
      }
      remotexy.sendBytesAvailable=len;
      rxy_itos (len, s+2);
      *s=remotexy.connectCannel;
      *(s+1)=',';      
      sendATCommand ("AT+CIPSEND=",s,0);
      if (!waitATAnswer (AT_ANSWER_GO, 1000)) remotexy.sendBytesAvailable=0;
    }
  }
  
  void sendByte (uint8_t b) {
    if (remotexy.sendBytesAvailable) {
      serial.write(b);
#if defined(REMOTEXY__DEBUGLOGS)
        DEBUGLOGS_writeOutputHex (b);
#endif
      remotexy.sendBytesAvailable--;
      if (!remotexy.sendBytesAvailable) {
        waitATAnswer (AT_ANSWER_SEND_OK, 1000);      
        if (remotexy.sendBytesLater) sendStart (remotexy.sendBytesLater);
      }
    }
  }
  
  
  uint8_t receiveByte(void) {
    uint8_t b;
    if (remotexy.connectAvailable) {
      if (serial.available()>0) {
        remotexy.connectAvailable--;
        b = serial.read();
#if defined(REMOTEXY__DEBUGLOGS)
        DEBUGLOGS_writeInputHex (b);
#endif
        return b;
      }
    }  
    return 0;
  }
  
  uint8_t availableByte(void) {
    if (remotexy.connectAvailable) {
      return serial.available()>0;
    }
    return 0;
  } 

/**********************************************************************************************************/
  void initAT () {
    remotexy.bufferATPos=0;
  }

  void sendATCommand (const char * command, ...) 
  { 
   
    char *p = (char*) command;
    va_list argptr;
    while (serial.available() > 0)
    {
    	serial.read();
    }
    va_start (argptr, command);
    while (p) {
     uint8_t Len=strlen(p);
     for(uint8_t i=0; i<Len;i++){
    	 serial.write(*p);
    	 p++;
     }
#if defined(REMOTEXY__DEBUGLOGS)
      DEBUGLOGS_writeOutput (p);
#endif
      p=va_arg(argptr,char*);
    }
    va_end(argptr);     
    serial.write('\r');
    serial.write('\n');
  }    
  
  uint8_t waitATAnswer (const char * answer, uint16_t delay) 
  {
    uint8_t b;
    uint32_t timeOut = HAL_GetTick();
  
    uint8_t k=0;
    while (HAL_GetTick() - timeOut <= delay) {

      if (serial.available() > 0) {
        b=serial.read();
        if (b==10) continue;
#if defined(REMOTEXY__DEBUGLOGS)
        if (b==13) DEBUGLOGS_writeInputNewString ();
        else DEBUGLOGS_writeInputChar (b);
#endif
        if (b==13) {
          remotexy.bufferAT[k++]=0;
          remotexy.bufferATPos=0;
          k=0;
          if (strcmp (remotexy.bufferAT,answer)==0) return 1;
          if (strcmp (remotexy.bufferAT,AT_ANSWER_ERROR)==0) return 0;
          if (cmpBufferAT () ==  AT_MESSAGE_READY) return 0;       
          if (strcmp (remotexy.bufferAT,AT_MESSAGE_AT)==0) remotexy.haveEcho=1;
        }
        else {
          if (k<AT_BUFFER_STR_LENGTH) remotexy.bufferAT[k++]=b;
          if (b=='>') {
            if (answer==AT_ANSWER_GO) return 1;
          }
        }
      }
    } 
    return 0;  
  }

  // echo test, returns
  //  0 - no answer
  //  1 - no echo
  //  2 - have echo

  uint8_t testATecho (void)
  {
    remotexy.haveEcho = 0;
    sendATCommand ("AT",0);
    if (!waitATAnswer (AT_ANSWER_OK, 1000)) return 0;
    return (remotexy.haveEcho==0?1:2);
  }

  void readATMessage () 
  {
    uint8_t b;
    while (serial.available ()>0) {
      b=serial.read  ();
      if (b==10) continue;
#if defined(REMOTEXY__DEBUGLOGS)
      if (b==13) DEBUGLOGS_writeInputNewString ();
      else DEBUGLOGS_writeInputChar (b);
#endif
      if (b==13) {
        remotexy.bufferAT[remotexy.bufferATPos]=0;
        remotexy.bufferATPos=0;
         if (!cmpBufferAT ()) return;
      }
      else {
        if (remotexy.bufferATPos<AT_BUFFER_STR_LENGTH) remotexy.bufferAT[remotexy.bufferATPos++]=b;
        if (b==':') {
          remotexy.bufferAT[remotexy.bufferATPos]=0;
          if (strcmpAT (remotexy.bufferAT,AT_MESSAGE_IPD)==0) {
#if defined(REMOTEXY__DEBUGLOGS)
            DEBUGLOGS_writeInputNewString ();
#endif
            remotexy.bufferATPos=0;
            inputDataAT ();
            return;
          } 
        }
      }    
    }
  }
  
  const char * cmpBufferAT(void)
  {
    if (strcmpAT (remotexy.bufferAT,AT_MESSAGE_CONNECT)==0) {connectAT (); return AT_MESSAGE_CONNECT;}
    if (strcmpAT (remotexy.bufferAT,AT_MESSAGE_CLOSED)==0) {closedAT (); return AT_MESSAGE_CLOSED;}
    if (strcmpAT (remotexy.bufferAT,AT_MESSAGE_CONNECT_FAIL)==0) {closedAT (); return AT_MESSAGE_CONNECT_FAIL;}
    if (strcmpAT (remotexy.bufferAT,AT_MESSAGE_READY)==0) {readyAT (); return AT_MESSAGE_READY;}
    return 0;           
  }  
  
  uint8_t strcmpAT (char * str, const char * temp)
  {
    uint8_t k = 0;
    while (*temp) {
      if (!*str) return 1;
      switch (*temp) {
        case '?':
          remotexy.params[k]=str;
          remotexy.paramsLength[k]=1;
          temp++; 
          str++;   
          k++; 
          break;
        case '*':
          remotexy.params[k]=str;
          remotexy.paramsLength[k]=0;
          temp++;
          while (*str!=*temp) {
            if (!*str++) return 1; 
            remotexy.paramsLength[k]++;
          }
          k++;
          break;
        default: 
          if (*(str++)!=*(temp++)) return 1;
          break;
      }
    }
    if (*temp) return 1;
    return 0;
  }  
  
  uint16_t getATParamInt (uint8_t k)
  {
    uint16_t res = 0;
    char * p=remotexy.params[k];
    uint8_t i=remotexy.paramsLength[k];
    while (i--) res = res*10+(*p++)-'0';
    return res;
  }
  /**********************************************************************************************************/
  void initSerial (void)
  {
	  serial.write = &UartWrite;
	  serial.read = &UartRead;
	  serial.available = &UartAvailable;
  }
  void UartWrite(uint8_t d)
  {
	 Uart_write(d, &huart3);
  }

  uint8_t UartRead(void)
  {
	  return (uint8_t)Uart_read(&huart3);
  }
  uint8_t UartAvailable(void)
  {
	  return (uint8_t)IsDataAvailable(&huart3);
  }
/**********************************************************************************************************/

char* rxy_itos(uint16_t i, char* s)
{
  uint8_t len=0;
  char *p=s+5;
  while (i) {
    *p--=i%10+'0';
    i/=10;
    len++;
  }
  for (i=0; i<len; i++) *s++=*(++p);
  *s=0;
  return s;  
}

uint8_t rxy_xctoi (char c)
{
  uint8_t b;
  if (c<='9') b=c-'0';
  else if (c<='f') b=c-'A'+10; 
  else b=c-'a'+10;
  return b&0x0f;
}

void rxy_getMacAddr (char* s, uint8_t* m)
{
  uint8_t i, b;
  for (i=6; i>0; i--) {
    b=rxy_xctoi (*s++)<<4;
    b|=rxy_xctoi (*s++);
    *m++=b;
    s++;
  }    
}

/**********************************************************************************************************/
  void init (const void * _conf, void * _var, const char * _accessPassword)
  {
    uint8_t i;
    uint8_t* p = (uint8_t*)_conf;
    uint8_t b = getConfByte(p++);
    if (b==0xff) {
      remotexy.inputLength = getConfByte (p++);
      remotexy.inputLength |= getConfByte (p++)<<8;
      remotexy.outputLength = getConfByte (p++);
      remotexy.outputLength |= getConfByte (p++)<<8;
    }
    else {
      remotexy.inputLength = b;
      remotexy.outputLength = getConfByte (p++);
    }
    remotexy.confLength = getConfByte (p++);
    remotexy.confLength |= getConfByte (p++)<<8;
    remotexy.conf = p;
    remotexy.confVersion = getConfByte (p);
    remotexy.var = (uint8_t*)_var;
    uint16_t varLength = remotexy.outputLength+remotexy.inputLength;
    remotexy.connect_flag = remotexy.var+varLength;
    *remotexy.connect_flag = 0;
        
    remotexy.accessPassword = (uint8_t*)_accessPassword;

    remotexy.receiveBufferLength=remotexy.inputLength;
    if ((*remotexy.accessPassword!=0)&&(remotexy.receiveBufferLength<REMOTEXY_PASSWORD_LENGTH_MAX))
      remotexy.receiveBufferLength = REMOTEXY_PASSWORD_LENGTH_MAX;
    remotexy.receiveBufferLength +=6;
    
    remotexy.receiveBuffer = (uint8_t*)malloc (remotexy.receiveBufferLength);
    
    p = remotexy.var;
    i = varLength;
    while (i--) *p++=0;
    
    resetWire();
 
#if defined(REMOTEXY__DEBUGLOGS)
    DEBUGLOGS_init ();
    DEBUGLOGS_write("RemoteXY started");
#endif
  
    remotexy.moduleRunning = initModule ();
#if defined(REMOTEXY__DEBUGLOGS)
    if (!remotexy.moduleRunning) {
      DEBUGLOGS_write ("Wire module not available, RemoteXY stoped");
    }    
#endif     
    
  }

  uint8_t getConfByte (uint8_t* p)
  {
    return  *(p);
  }
  

  void resetWire(void)
  {
    remotexy.receiveIndex=0;
    remotexy.receiveCRC=initCRC ();
    *remotexy.connect_flag = 0;
    remotexy.inputVarNeedSend = 1;
    remotexy.wireTimeOut= HAL_GetTick();
  }
  
  void handler ()
  {
    uint8_t *p;
    uint16_t i;  
    uint8_t b;
    uint16_t packageLength;
    
    if (!remotexy.moduleRunning) return;
    
    handlerModule();
    
#if defined(REMOTEXY_CLOUD)  
    handlerCloud ();
#endif
    
    while (availableByte() > 0) {
      b = receiveByte();
      
      if ((remotexy.receiveIndex==0) && (b!=REMOTEXY_PACKAGE_START_BYTE)) continue;
      remotexy.receiveBuffer[remotexy.receiveIndex++]=b;
      updateCRC (&remotexy.receiveCRC, b);
      if (remotexy.receiveIndex>remotexy.receiveBufferLength) {
        searchStartByte (1); //remotexy.receiveBuffer overflow
      }
      while (true) {
        if (remotexy.receiveIndex<6) break;
        packageLength = remotexy.receiveBuffer[1]|(remotexy.receiveBuffer[2]<<8);
        if (packageLength>remotexy.receiveBufferLength) searchStartByte(1); // error
        else if (packageLength<6) searchStartByte(1); // error
        else if (packageLength==remotexy.receiveIndex) {
          if (remotexy.receiveCRC==0) {
            if (handleReceivePackage ()) {
              remotexy.receiveIndex=0;
              remotexy.receiveCRC=initCRC ();
              break;
            }
          }
          searchStartByte(1); // error
        }
        else if (packageLength<remotexy.receiveIndex) {
          uint16_t crc = initCRC ();
          p = remotexy.receiveBuffer;
          i=packageLength;
          while (i--) updateCRC (&crc, *(p++)); 
          if (crc==0) {
            if (handleReceivePackage ()) {
              searchStartByte (packageLength);
              continue;
            }
          }
          searchStartByte (1);        
        }
        else break;
      }
    }  
    
    if (HAL_GetTick() - remotexy.wireTimeOut > REMOTEXY_TIMEOUT) {
      resetWire ();
    }      
  }
  

  uint16_t initCRC (void)
  {
    return 0xffff;
  }          
    

  void updateCRC (uint16_t *crc, uint8_t b)
  {
    *crc ^= b;
    for (uint8_t i=0; i<8; ++i) {
      if ((*crc) & 1) *crc = ((*crc) >> 1) ^ 0xA001;
      else *crc >>= 1;
    }
  } 
  

  void sendByteUpdateCRC (uint8_t b, uint16_t *crc) {
    sendByte (b); 
    updateCRC (crc, b);
  }  


  void sendPackage (uint8_t command, uint8_t *p, uint16_t length, uint8_t itConf) {
    uint16_t crc = initCRC ();
    uint16_t packageLength = length+6;
    sendStart (packageLength);
    sendByteUpdateCRC (REMOTEXY_PACKAGE_START_BYTE, &crc);
    sendByteUpdateCRC (packageLength, &crc);
    sendByteUpdateCRC (packageLength>>8, &crc);
    sendByteUpdateCRC (command, &crc);  
    uint8_t b;
    while (length--) {
      if (itConf) b=getConfByte (p++);
      else b=*p++;
      sendByteUpdateCRC (b, &crc);
    }
    sendByte (crc);  
    sendByte (crc>>8);
  }
  

  void searchStartByte (uint16_t pos) {
    uint8_t *p, *kp;
    uint16_t i;
    uint16_t ri = remotexy.receiveIndex;
    p=remotexy.receiveBuffer+pos;
    remotexy.receiveCRC=initCRC ();
    for (i=pos; i<ri; i++) {
      if (*p==REMOTEXY_PACKAGE_START_BYTE) {      
        kp=remotexy.receiveBuffer;
        remotexy.receiveIndex=remotexy.receiveIndex-i;
        while (i++<ri) {
          updateCRC (&remotexy.receiveCRC, *p);
          *(kp++)=*(p++);
        }
        return;
      }
      p++;
    }        
    remotexy.receiveIndex=0;
  }  
  
  uint8_t handleReceivePackage () {
    uint8_t command;
    uint16_t i;
    uint16_t length;
    uint8_t *p, *kp;
    uint8_t available;

    length = remotexy.receiveBuffer[1]|(remotexy.receiveBuffer[2]>>8);
    length-=6;
    command = remotexy.receiveBuffer[3];
    switch (command) {  
      case 0x00:  

        if (length==0) {
          if (*remotexy.accessPassword==0) available=1;
          else available=0;
        }
        else {
          uint8_t ch;
          available = 1;
          p = remotexy.receiveBuffer+4;
          kp = remotexy.accessPassword;
          while (true) {
            ch=*kp++;
            if (ch!=*p++) available=0;
            if (!ch) break;
          }                               
        } 
        if (available!=0) {
          sendPackage (command, remotexy.conf, remotexy.confLength,  1);
          *remotexy.connect_flag = 1;
        }
        else {
          uint8_t buf[4];
          p = buf;
          kp = remotexy.conf;
          i=remotexy.confVersion>=5?3:2;
          length = i+1;
          while (i--) *p++ = getConfByte(kp++);
          *p++ = 0xf0;
          sendPackage (command, buf, length,  0);
        }          
        break;   
      case 0x40:  
        remotexy.inputVarNeedSend = 0;
        sendPackage (command, remotexy.var, remotexy.inputLength+remotexy.outputLength, 0);
        break;   
      case 0x80:  
        if ((length==remotexy.inputLength) && (remotexy.inputVarNeedSend==0)) {
          p=remotexy.receiveBuffer+4;
          kp=remotexy.var;
          i= remotexy.inputLength;
          while (i--) *kp++=*p++;
        }
        sendPackage (command, 0, 0, 0);
        break;   
      case 0xC0:  
        if (remotexy.inputVarNeedSend!=0) command |= 0x01;
        sendPackage (command, remotexy.var+remotexy.inputLength, remotexy.outputLength, 0);
        break;   
#if defined(REMOTEXY_CLOUD)  
      case 0x10: // echo
        sendPackage (command, 0, 0, 0);
        break;   
      case 0x11:
        if (cloudState==REMOTEXY_CLOUD_STATE_REGISTRATION) {
          setCloudState (REMOTEXY_CLOUD_STATE_WORKING);
        }
        break;   
#endif //REMOTEXY_CLOUD       
      default:
        return 0; 
    }  
    
    remotexy.wireTimeOut=HAL_GetTick();
#if defined(REMOTEXY_CLOUD)  
    if (cloudState==REMOTEXY_CLOUD_STATE_WORKING) {
      cloudTimeOut=HAL_GetTick();
    }
#endif //REMOTEXY_CLOUD       
    return 1;
  }
  
  
///////////////////////////////////////////////////////////////////////////////
// PUBLIC DOCUMENTED FUNCTIONS
  

  uint8_t isConnected () {
    return *remotexy.connect_flag;
  }
  
  // transmit the input vars to smartphone, while input variables are transmitted the output variables are not sent

  void sendInputVariables () {
    remotexy.inputVarNeedSend = 1;
  }

  //check if input variables were sent, return 1 if sent

  uint8_t didSendInputVariables () {
    if (remotexy.inputVarNeedSend==0) return 1;
    else return 0;
  }

///////////////////////////////////////////////////////////////////////////////
// CLOUD SUPPORT 


#if defined(REMOTEXY_CLOUD)  

  char *cloudServer;
  uint16_t cloudPort;
  uint8_t cloudRegistPackage[38];
  uint8_t cloudState;   
  uint32_t cloudTimeOut;


  virtual int8_t connectServerCloud (char * _cloudServer, uint16_t _cloudPort) {return 0;};


  void initCloud (const char * _cloudServer, uint16_t _cloudPort, const char * _cloudToken) {
    cloudServer = (char *) _cloudServer;
    cloudPort = _cloudPort;
    
    uint8_t i;
    uint8_t *p = cloudRegistPackage;
    *p++ = getConfByte(remotexy.conf+0);
    *p++ = 0;    
    for (i=0; i<32; i++) {
      if (*_cloudToken==0) *(p++)=0;
      else *(p++)=*(_cloudToken++);
    }
    uint16_t *len = (uint16_t*)p;
    *len = remotexy.outputLength + remotexy.inputLength;
    if (remotexy.confLength>*len) *len = remotexy.confLength;
    *len += 6+1;    
    len = (uint16_t*)(p+2);     
    *len = remotexy.receiveBufferLength;
    
    cloudState = REMOTEXY_CLOUD_STATE_STOP;
  }
  

  void startCloudConnection () {
    if (cloudState<REMOTEXY_CLOUD_STATE_CONNECTION) {
      setCloudState (REMOTEXY_CLOUD_STATE_CONNECTION);
    }
  }  
  

  void stopCloud () {
    closeConnection ();
    resetWire ();
    if (cloudState<REMOTEXY_CLOUD_STATE_CONNECTION) return;
#if defined(REMOTEXY__DEBUGLOGS)
    DEBUGLOGS_write("Cloud server disconnected");
#endif
    setCloudState (REMOTEXY_CLOUD_STATE_WAIT_RECONNECT);
  }
  

  void setCloudState (uint8_t state) {
    cloudState = state;
    cloudTimeOut = HAL_GetTick();
#if defined(REMOTEXY__DEBUGLOGS)
    switch (state) {
      case REMOTEXY_CLOUD_STATE_WAIT_RECONNECT: 
        DEBUGLOGS_write("Waiting to reconnect to the cloud server");
        break;
      case REMOTEXY_CLOUD_STATE_WAIT_NEXT_TRY: 
        DEBUGLOGS_write("Waiting to next try to connect to the cloud server");
        break;
      case REMOTEXY_CLOUD_STATE_CONNECTION: 
        DEBUGLOGS_write("Started connecting to cloud server");
        break;
      case REMOTEXY_CLOUD_STATE_REGISTRATION: 
        DEBUGLOGS_write("Waiting for registration on cloud server");
        break;
      case REMOTEXY_CLOUD_STATE_WORKING: 
        DEBUGLOGS_write("Connect to the cloud server successfully");
        break;
      default:
        DEBUGLOGS_write("Unknown cloud state ");
        REMOTEXY__DEBUGLOGS.print(cloudState);
        break;
    }
#endif
  }
  

  void handlerCloud () {
    int8_t res;
    if (!remotexy.moduleRunning) return;
    switch (cloudState) {
      
      case REMOTEXY_CLOUD_STATE_WAIT_RECONNECT:  
        if (HAL_GetTick() - cloudTimeOut > REMOTEXY_CLOUD_RECONNECT_TIMEOUT)
          setCloudState (REMOTEXY_CLOUD_STATE_CONNECTION);          
        break;
        
      case REMOTEXY_CLOUD_STATE_WAIT_NEXT_TRY:  
        if (HAL_GetTick() - cloudTimeOut > REMOTEXY_CLOUD_RETRY_TIMEOUT)
          setCloudState (REMOTEXY_CLOUD_STATE_CONNECTION);            
        break;
        
      case REMOTEXY_CLOUD_STATE_CONNECTION:  
        res = connectServerCloud (cloudServer, cloudPort);
        if (res == 1) {          
          setCloudState (REMOTEXY_CLOUD_STATE_REGISTRATION);   
          sendPackage (0x11, cloudRegistPackage, 38, 0);
        }
        else if (res == 0) {
#if defined(REMOTEXY__DEBUGLOGS)
          DEBUGLOGS_write("Cloud server connection error");          
#endif         
          setCloudState (REMOTEXY_CLOUD_STATE_WAIT_RECONNECT); 
        }
        else {
          setCloudState (REMOTEXY_CLOUD_STATE_WAIT_NEXT_TRY); 
        }
        break;
        
      case REMOTEXY_CLOUD_STATE_REGISTRATION:  
        if (HAL_GetTick() - cloudTimeOut > REMOTEXY_CLOUD_CONNECT_TIMEOUT)
          stopCloud ();
        break;
        
      case REMOTEXY_CLOUD_STATE_WORKING:  
        if (HAL_GetTick() - cloudTimeOut > REMOTEXY_CLOUD_ECHO_TIMEOUT)
          stopCloud ();
        break;
    }
  } 
  
///////////////////////////////////////////////////////////////////////////////
// PUBLIC DOCUMENTED FUNCTIONS OF CLOUD
  

  uint8_t isCloudConnected () {
    return (cloudState==REMOTEXY_CLOUD_STATE_WORKING ? 1:0);
  }
  
  
#endif //REMOTEXY_CLOUD

///////////////////////////////////////////////////////////////////////////////
// DEBUG FUNCTIONS

#if defined(REMOTEXY__DEBUGLOGS)
  uint8_t debug_flags;
  uint8_t debug_hexcounter;
  

  void DEBUGLOGS_init () {
    debug_flags=0;
    REMOTEXY__DEBUGLOGS.begin (REMOTEXY__DEBUGLOGS_SPEED);
    REMOTEXY__DEBUGLOGS.println();
  }


  void DEBUGLOGS_writeTime () {
    uint32_t d = HAL_GetTick();
    char s[15];
    sprintf (s, "[%5ld.%03d] ",(long)(d/1000), (int)(d%1000));     
    REMOTEXY__DEBUGLOGS.println ();    
    REMOTEXY__DEBUGLOGS.print (s);
  }
  

  void DEBUGLOGS_write (const char *s) {
    debug_flags = 0;
    DEBUGLOGS_writeTime (); 
    REMOTEXY__DEBUGLOGS.print(s);
  }
  

  void DEBUGLOGS_writeInput (char *s) {
    if ((debug_flags & 0x01)==0) {
      DEBUGLOGS_writeTime ();
      REMOTEXY__DEBUGLOGS.print("<- ");
    }
    debug_flags = 0x01;   
    REMOTEXY__DEBUGLOGS.print(s);
  }


  void DEBUGLOGS_writeOutput (char *s) {
    if ((debug_flags & 0x02)==0) {
      DEBUGLOGS_writeTime ();
      REMOTEXY__DEBUGLOGS.print("-> ");
    }
    debug_flags = 0x02;   
    REMOTEXY__DEBUGLOGS.print(s);
  }


  void DEBUGLOGS_writeInputHex (uint8_t b) {
    if ((debug_flags & 0x01)==0) {
      DEBUGLOGS_writeTime ();
      REMOTEXY__DEBUGLOGS.print("<-");
      debug_hexcounter=0;
    }
    debug_flags = 0x01;   
    DEBUGLOGS_writeHex (b);
  }


  void DEBUGLOGS_writeOutputHex (uint8_t b) {
    if ((debug_flags & 0x02)==0) {
      DEBUGLOGS_writeTime ();
      REMOTEXY__DEBUGLOGS.print("->");
      debug_hexcounter=0;
    }
    debug_flags = 0x02;
    DEBUGLOGS_writeHex (b);
  }
  

  void DEBUGLOGS_writeInputChar (char s) {
    if ((debug_flags & 0x01)==0) {
      DEBUGLOGS_writeTime ();
      REMOTEXY__DEBUGLOGS.print("<- ");
    }
    debug_flags = 0x01;   
    REMOTEXY__DEBUGLOGS.print(s);
  }


  void DEBUGLOGS_writeInputNewString () {
    debug_flags = 0;
  }
  

  void DEBUGLOGS_writeHex (uint8_t b) {
    debug_hexcounter++;
    if (debug_hexcounter>16) {
      REMOTEXY__DEBUGLOGS.println();
      REMOTEXY__DEBUGLOGS.print("              ");
      debug_hexcounter=1;
    }
    REMOTEXY__DEBUGLOGS.print(' ');
    REMOTEXY__DEBUGLOGS.print(b>>4, HEX); 
    REMOTEXY__DEBUGLOGS.print(b&0x0f, HEX); 
    
    
    
  }  
  
#endif //REMOTEXY__DEBUGLOGS
