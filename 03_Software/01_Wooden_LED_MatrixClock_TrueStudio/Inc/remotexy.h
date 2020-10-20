#ifndef __REMOTEXY_H
#define __REMOTEXY_H
/***************************************************************************************
 * Includes
 **************************************************************************************/
#include "esp8266.h"
/***************************************************************************************
 * Defines
 **************************************************************************************/
#define REMOTEXY_ESP8266_MAX_SEND_BYTES 2048
#define REMOTEXY_ESP8266_MODULETEST_TIMEOUT 30000
#define REMOTEXY_PACKAGE_START_BYTE 0x55
#define REMOTEXY_PASSWORD_LENGTH_MAX 26
#define REMOTEXY_TIMEOUT 		5000
#define REMOTEXY_SERVER_TIMEOUT 7000

#define REMOTEXY_WIFI_SSID 		"RemoteXY"
#define REMOTEXY_WIFI_PASSWORD 	"12345678"
#define REMOTEXY_ACCESS_PASSWORD ""
#define REMOTEXY_SERVER_PORT 	6377
#define AT_BUFFER_STR_LENGTH 	10

#define AT_ANSWER_ERROR  		"ERROR"
#define AT_ANSWER_OK			"OK"
#define AT_ANSWER_SEND_OK		"SEND OK"
#define AT_MESSAGE_READY		"ready"
#define AT_ANSWER_GO			">"
#define AT_MESSAGE_AT			"AT"
#define AT_MESSAGE_CONNECT		"?,CONNECT"
#define AT_MESSAGE_CLOSED		"?,CLOSED"
#define AT_MESSAGE_CONNECT_FAIL	"?,CONNECT FAIL"
#define AT_MESSAGE_IPD			"+IPD,?,*:"

/***************************************************************************************
 * Typedefs
 **************************************************************************************/
/**/
//struct
//{
//		/*input variables*/
//		uint8_t button_1; // =1 if button pressed, else =0
//		char edit_1[36];  // string UTF8 end zero
//		char edit_2[11];  // string UTF8 end zero
//		/*other variable*/
//		uint8_t connect_flag;  // =1 if wire connected, else =0
//} RemoteXY;
struct {

    // input variables
  char TextBox_SSID[31];  // string UTF8 end zero
  char TextBox_PassWord[31];  // string UTF8 end zero
  uint8_t Btn_SSID_Send; // =1 if button pressed, else =0

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0

} RemoteXY;
/**/
typedef struct CRemoteXY_{
	  char * wifiSsid;
	  char * wifiPassword;
	  uint16_t port;
	  char connectCannel;
	  uint16_t connectAvailable;
	  uint16_t freeAvailable;
	  uint16_t ESP8266_RemoteXY_SendBytesAvailable;
	  uint16_t ESP8266_RemoteXY_SendBytesLater;
	  uint32_t moduleTestTimeout;
	  char bufferAT[AT_BUFFER_STR_LENGTH+1];
	  uint8_t bufferATPos;
	  char * params[3];
	  uint8_t paramsLength[3];
	  uint8_t haveEcho;
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
/**/
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
/***************************************************************************************
 * Variables
 **************************************************************************************/
/*globals*/
CRemoteXY remotexy;

Serial_t serial;
/***************************************************************************************
 * Function declarations
 **************************************************************************************/
void ESP8266_RemoteXY_InitAndStart(void);

void ESP8266_RemoteXY_Init(const void * _conf, void * _var,	const char * _accessPassword, const char * _wifiSsid,const char * _wifiPassword, uint16_t _port);

uint8_t ESP8266_RemoteXY_InitModule(void);

uint8_t ESP8266_RemoteXY_SetModule(void);

void ESP8266_RemoteXY_HandlerModule(void);

void ESP8266_RemoteXY_ReadyAT(void);

void ESP8266_RemoteXY_ConnectAT(void);

void ESP8266_RemoteXY_ClosedAT(void);

void ESP8266_RemoteXY_InputDataAT();

void ESP8266_RemoteXY_SendStart(uint16_t len);

void ESP8266_RemoteXY_SendByte(uint8_t b);

uint8_t ESP8266_RemoteXY_ReceiveByte(void);

uint8_t ESP8266_RemoteXY_AvailableByte(void);

void ESP8266_RemoteXY_InitAT(void);

void ESP8266_SendATCommand(const char * command, ...);

uint8_t ESP8266_WaitATAnswer(const char * answer, uint16_t delay);

// echo test, returns
//  0 - no answer
//  1 - no echo
//  2 - have echo

uint8_t testATecho(void);

void readATMessage();

const char * cmpBufferAT(void);

uint8_t strcmpAT(char * str, const char * temp);

uint16_t getATParamInt(uint8_t k);

void ESP8266_Serial_Init(void);

void UartWrite(uint8_t d);

uint8_t UartRead(void);

uint8_t UartAvailable(void);

uint8_t UartFind(char *str);

void UartPrintLn(char *str);

void UartPrint(char *str);

void UartPrintCharArray(char *str, uint8_t Length);

char* rxy_itos(uint16_t i, char* s);

uint8_t rxy_xctoi(char c);

void rxy_getMacAddr(char* s, uint8_t* m);

void ESP8266_RemoteXY_InitRemoteXY(const void * _conf, void * _var, const char * _accessPassword);

uint8_t ESP8266_RemoteXY_GetConfByte(uint8_t* p);

void ESP8266_RemoteXY_ResetWire(void);

void ESP8266_RemoteXY_Handler(void);

uint16_t ESP8266_RemoteXY_InitCRC(void);

void ESP8266_RemoteXY_UpdateCRC(uint16_t *crc, uint8_t b);

void ESP8266_RemoteXY_SendByteUpdateCRC(uint8_t b, uint16_t *crc);

void ESP8266_RemoteXY_SendPackage(uint8_t command, uint8_t *p, uint16_t length, uint8_t itConf);

void ESP8266_RemoteXY_SearchStartByte(uint16_t pos);

uint8_t ESP8266_RemoteXY_HandleReceivePackage(void);

uint8_t ESP8266_RemoteXY_IsConnected(void);

void ESP8266_RemoteXY_SendInputVariables(void);

uint8_t ESP8266_RemoteXY_DidSendInputVariables(void);

/**************************************************************************************/
#endif
