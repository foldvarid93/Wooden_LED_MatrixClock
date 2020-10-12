#ifndef __ESP8266_H
#define __ESP8266_H
/***************************************************************************************
 * Includes
 **************************************************************************************/
#include "application.h"
#include "stdbool.h"
#include "typedef.h"
/***************************************************************************************
 * Defines
 **************************************************************************************/
#define SizeOf_HTML_Message		(2048u)
/**/
#define AT_OK					"OK"
#define AT_READY				"ready"
#define HTML_OK					"HTTP/1.1 200 OK"
#define MSG_ID01_START			"MsG_STRT-iD_01"
#define MSG_ID01_STOP			"MsG_STOP-iD_01"
#define MSG_ID02_START			"MsG_STRT-iD_02"
#define MSG_ID02_STOP			"MsG_STOP-iD_02"
#define MSG_ID03_START			"MsG_STRT-iD_03"
#define MSG_ID03_STOP			"MsG_STOP-iD_03"
#define MSG_ID04_START			"MsG_STRT-iD_04"
#define MSG_ID04_STOP			"MsG_STOP-iD_04"
#define MSG_ID05_START			"MsG_STRT-iD_05"
#define MSG_ID05_STOP			"MsG_STOP-iD_05"
#define MSG_ID06_START			"MsG_STRT-iD_06"
#define MSG_ID06_STOP			"MsG_STOP-iD_06"
#define MSG_ID07_START			"MsG_STRT-iD_07"
#define MSG_ID07_STOP			"MsG_STOP-iD_07"
#define MSG_ID08_START			"MsG_STRT-iD_08"
#define MSG_ID08_STOP			"MsG_STOP-iD_08"
/***************************************************************************************
 * Typedefs
 **************************************************************************************/

/***************************************************************************************
 * Function declarations
 **************************************************************************************/
/**/
void ESP8266_AT_Send(const char * command, ...);
/**/
HAL_StatusTypeDef ESP8266_AT_ReceiveWithTimeout(const char * Answer, uint16_t Timeout);
/**/
HAL_StatusTypeDef ESP8266_AT_SendAndReceiveWithTimeout(const char * Command, const char * Respond, uint32_t Timeout);
/**/
HAL_StatusTypeDef ESP8266_AccessPoint_InitAndRun(void);
/**/
HAL_StatusTypeDef HTML_GetMessage(uint8_t * Message);
/**/
HAL_StatusTypeDef HTML_Interpreter(uint8_t * Message);
/**/
/**************************************************************************************/
#endif
