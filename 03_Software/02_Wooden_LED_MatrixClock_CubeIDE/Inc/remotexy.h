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
     - Fixed a bug where millis() overflow in 50 days;
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
#include <inttypes.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"


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
//
struct {

    // input variables
  uint8_t button_1; // =1 if button pressed, else =0
  char edit_1[36];  // string UTF8 end zero
  char edit_2[11];  // string UTF8 end zero

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0

} RemoteXY;
//
typedef struct CRemoteXY_{
	  char * wifiSsid;
	  char * wifiPassword;
	  uint16_t port;
	  char connectCannel;
	  uint16_t connectAvailable;
	  uint16_t freeAvailable;

	  uint16_t sendBytesAvailable;
	  uint16_t sendBytesLater;

	  uint32_t moduleTestTimeout;
	  //
	  char bufferAT[AT_BUFFER_STR_LENGTH+1];
	  uint8_t bufferATPos;
	  char * params[3];
	  uint8_t paramsLength[3];
	  uint8_t haveEcho;
	  //
	  uint8_t confVersion;
	  uint8_t *conf;
	  uint8_t *var;
	  uint8_t *accessPassword;
	  uint16_t outputLength;
	  uint16_t inputLength;
	  uint16_t confLength;
	  uint8_t *connect_flag;
	  uint8_t inputVarNeedSend;

	  uint8_t *receiveBuffer;
	  uint16_t receiveBufferLength;
	  uint16_t receiveIndex;
	  uint16_t receiveCRC;


	  uint32_t wireTimeOut;

	  uint8_t moduleRunning;
} CRemoteXY;


struct Serial_t;
typedef struct Serial_t Serial_t;

typedef uint8_t (*ReadFunc)(void);
typedef void (*WriteFunc)(uint8_t d);
typedef uint8_t (*AvailableFunc)(void);
typedef uint8_t (*FindFunc)(char *str);
typedef void (*PrintLnFunc)(char *str);
typedef void (*PrintFunc)(char *str);
struct Serial_t
{
	ReadFunc read;
	WriteFunc write;
	AvailableFunc available;
	FindFunc find;
	PrintLnFunc println;
	PrintFunc print;
};
//variables

//
#ifndef REMOTEXY_ACCESS_PASSWORD 
#define REMOTEXY_ACCESS_PASSWORD ""
#endif 

//#define RemoteXY_Handler() handler ()
#define RemoteXY_isConnected() isConnected ()
#define RemoteXY_sendInputVariables() sendInputVariables ()
#define RemoteXY_didSendInputVariables() didSendInputVariables ()
/**********************************************************************************************************/
	void RemoteXY_Init(void);

	void CRemoteXY_Init(const void * _conf, void * _var, const char * _accessPassword, const char * _wifiSsid, const char * _wifiPassword, uint16_t _port);

	uint8_t initModule(void);

	uint8_t setModule(void);

	void handlerModule(void);

	void readyAT(void);

	void connectAT(void);
 
	void closedAT(void);
  
	void inputDataAT(void);
  
	void sendStart(uint16_t len);
  
	void sendByte(uint8_t b);
  
	uint8_t receiveByte();
  
	uint8_t availableByte();

	void initAT(void);

	void sendATCommand(const char * command, ...);
  
	uint8_t waitATAnswer(const char * answer, uint16_t delay);

	uint8_t testATecho (void);

	void readATMessage(void);
  
	const char * cmpBufferAT(void);
  
	uint8_t strcmpAT(char * str, const char * temp);
  
	uint16_t getATParamInt(uint8_t k);

	void initSerial(void);

	void UartWrite(uint8_t);

	uint8_t UartRead(void);

	uint8_t UartAvailable(void);

	uint8_t UartFind(char *str);

	void UartPrintCharArray(char *str, uint8_t Length);

	void UartPrintLn(char *str);

	void UartPrint(char *str);

	char* rxy_itos(uint16_t i, char* s);

	uint8_t rxy_xctoi(char c);

	void rxy_getMacAddr(char* s, uint8_t* m);

	void init(const void * _conf, void * _var, const char * _accessPassword);

	uint8_t getConfByte (uint8_t* p);
  
	void resetWire(void);
  
	void RemoteXY_Handler(void);

	void handler(void);

	uint16_t initCRC(void);
    
	void updateCRC(uint16_t *crc, uint8_t b);
  
	void sendByteUpdateCRC(uint8_t b, uint16_t *crc);

	void sendPackage(uint8_t command, uint8_t *p, uint16_t length, uint8_t itConf);
  
	void searchStartByte (uint16_t pos);
  
	uint8_t handleReceivePackage(void);
  
	uint8_t isConnected(void);

	void sendInputVariables(void);

	uint8_t didSendInputVariables(void);

	void initCloud(const char * _cloudServer, uint16_t _cloudPort, const char * _cloudToken);
  
	void startCloudConnection(void);

	void stopCloud(void);

	void setCloudState(uint8_t state);

	void handlerCloud(void);
  
	void DEBUGLOGS_init(void);

	void DEBUGLOGS_writeTime(void);
  
	void DEBUGLOGS_write(const char *s);
  
	void DEBUGLOGS_writeInput(char *s);

	void DEBUGLOGS_writeOutput(char *s);

	void DEBUGLOGS_writeInputHex(uint8_t b);

	void DEBUGLOGS_writeOutputHex(uint8_t b);
  
	void DEBUGLOGS_writeInputChar(char s);

	void DEBUGLOGS_writeInputNewString(void);

	void DEBUGLOGS_writeHex(uint8_t b);
