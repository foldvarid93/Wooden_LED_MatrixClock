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
#define HTML_OK					"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n"
/*group 0*/
#define MSG_ID00_START			"MsG_STRT-iD_00"
#define MSG_ID00_STOP			"MsG_STOP-iD_00"
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
#define MSG_ID09_START			"MsG_STRT-iD_09"
#define MSG_ID09_STOP			"MsG_STOP-iD_09"
/*group 1*/
#define MSG_ID10_START			"MsG_STRT-iD_10"
#define MSG_ID10_STOP			"MsG_STOP-iD_10"
#define MSG_ID11_START			"MsG_STRT-iD_11"
#define MSG_ID11_STOP			"MsG_STOP-iD_11"
#define MSG_ID12_START			"MsG_STRT-iD_12"
#define MSG_ID12_STOP			"MsG_STOP-iD_12"
#define MSG_ID13_START			"MsG_STRT-iD_13"
#define MSG_ID13_STOP			"MsG_STOP-iD_13"
#define MSG_ID14_START			"MsG_STRT-iD_14"
#define MSG_ID14_STOP			"MsG_STOP-iD_14"
#define MSG_ID15_START			"MsG_STRT-iD_15"
#define MSG_ID15_STOP			"MsG_STOP-iD_15"
#define MSG_ID16_START			"MsG_STRT-iD_16"
#define MSG_ID16_STOP			"MsG_STOP-iD_16"
#define MSG_ID17_START			"MsG_STRT-iD_17"
#define MSG_ID17_STOP			"MsG_STOP-iD_17"
#define MSG_ID18_START			"MsG_STRT-iD_18"
#define MSG_ID18_STOP			"MsG_STOP-iD_18"
#define MSG_ID19_START			"MsG_STRT-iD_19"
#define MSG_ID19_STOP			"MsG_STOP-iD_19"
/*group 2*/
#define MSG_ID20_START			"MsG_STRT-iD_20"
#define MSG_ID20_STOP			"MsG_STOP-iD_20"
#define MSG_ID21_START			"MsG_STRT-iD_21"
#define MSG_ID21_STOP			"MsG_STOP-iD_21"
#define MSG_ID22_START			"MsG_STRT-iD_22"
#define MSG_ID22_STOP			"MsG_STOP-iD_22"
#define MSG_ID23_START			"MsG_STRT-iD_23"
#define MSG_ID23_STOP			"MsG_STOP-iD_23"
#define MSG_ID24_START			"MsG_STRT-iD_24"
#define MSG_ID24_STOP			"MsG_STOP-iD_24"
#define MSG_ID25_START			"MsG_STRT-iD_25"
#define MSG_ID25_STOP			"MsG_STOP-iD_25"
#define MSG_ID26_START			"MsG_STRT-iD_26"
#define MSG_ID26_STOP			"MsG_STOP-iD_26"
#define MSG_ID27_START			"MsG_STRT-iD_27"
#define MSG_ID27_STOP			"MsG_STOP-iD_27"
#define MSG_ID28_START			"MsG_STRT-iD_28"
#define MSG_ID28_STOP			"MsG_STOP-iD_28"
#define MSG_ID29_START			"MsG_STRT-iD_29"
#define MSG_ID29_STOP			"MsG_STOP-iD_29"
/*group 3*/
#define MSG_ID30_START			"MsG_STRT-iD_30"
#define MSG_ID30_STOP			"MsG_STOP-iD_30"
#define MSG_ID31_START			"MsG_STRT-iD_31"
#define MSG_ID31_STOP			"MsG_STOP-iD_31"
#define MSG_ID32_START			"MsG_STRT-iD_32"
#define MSG_ID32_STOP			"MsG_STOP-iD_32"
#define MSG_ID33_START			"MsG_STRT-iD_33"
#define MSG_ID33_STOP			"MsG_STOP-iD_33"
#define MSG_ID34_START			"MsG_STRT-iD_34"
#define MSG_ID34_STOP			"MsG_STOP-iD_34"
#define MSG_ID35_START			"MsG_STRT-iD_35"
#define MSG_ID35_STOP			"MsG_STOP-iD_35"
#define MSG_ID36_START			"MsG_STRT-iD_36"
#define MSG_ID36_STOP			"MsG_STOP-iD_36"
#define MSG_ID37_START			"MsG_STRT-iD_37"
#define MSG_ID37_STOP			"MsG_STOP-iD_37"
#define MSG_ID38_START			"MsG_STRT-iD_38"
#define MSG_ID38_STOP			"MsG_STOP-iD_38"
#define MSG_ID39_START			"MsG_STRT-iD_39"
#define MSG_ID39_STOP			"MsG_STOP-iD_39"
/*group 4*/
#define MSG_ID40_START			"MsG_STRT-iD_40"
#define MSG_ID40_STOP			"MsG_STOP-iD_40"
#define MSG_ID41_START			"MsG_STRT-iD_41"
#define MSG_ID41_STOP			"MsG_STOP-iD_41"
#define MSG_ID42_START			"MsG_STRT-iD_42"
#define MSG_ID42_STOP			"MsG_STOP-iD_42"
#define MSG_ID43_START			"MsG_STRT-iD_43"
#define MSG_ID43_STOP			"MsG_STOP-iD_43"
#define MSG_ID44_START			"MsG_STRT-iD_44"
#define MSG_ID44_STOP			"MsG_STOP-iD_44"
#define MSG_ID45_START			"MsG_STRT-iD_45"
#define MSG_ID45_STOP			"MsG_STOP-iD_45"
#define MSG_ID46_START			"MsG_STRT-iD_46"
#define MSG_ID46_STOP			"MsG_STOP-iD_46"
#define MSG_ID47_START			"MsG_STRT-iD_47"
#define MSG_ID47_STOP			"MsG_STOP-iD_47"
#define MSG_ID48_START			"MsG_STRT-iD_48"
#define MSG_ID48_STOP			"MsG_STOP-iD_48"
#define MSG_ID49_START			"MsG_STRT-iD_49"
#define MSG_ID49_STOP			"MsG_STOP-iD_49"
/**/
#define MSG_ID50_START			"MsG_STRT-iD_50"
#define MSG_ID50_STOP			"MsG_STOP-iD_50"
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
