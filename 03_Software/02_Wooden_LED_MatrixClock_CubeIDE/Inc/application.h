#ifndef _APPLICATION_H_
#define	_APPLICATION_H_
/***********************************************///includes begin
/*stm32 libraries*/
#include "stm32f4xx_hal.h"
#include "adc.h"
#include "i2c.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
/*user libraries*/
#include "characters.h"
#include "remotexy.h"
#include "UartRingbuffer.h"
#include "eeprom.h"
#include "ntp.h"
#include "typedef.h"
#include "tmp100.h"
/*std libraries*/
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
/**/
/***********************************************///includes end
#define StartIdx 			28
#define HourTensStartIdx 	StartIdx
#define HourSinglesStartIdx StartIdx+6
#define HourMinDoubleDot 	HourSinglesStartIdx+6
#define MinTensStartIdx 	HourMinDoubleDot+2
#define MinSinglesStartIdx 	MinTensStartIdx+6
#define MinSecDoubleDot 	MinSinglesStartIdx+6
#define SecTensStartIdx 	MinSecDoubleDot+2
#define SecSinglesStartIdx 	SecTensStartIdx+6
/***********************************************///MAX7219 define constants begin
#define ESP_RESET_PIN		ESP8266_RST_Pin
#define ESP_RESET_PORT		ESP8266_RST_GPIO_Port
#define MAX7219_CS_PIN		SPI2_CS_Pin			//Chip select pin
#define MAX7219_CS_PORT		SPI2_CS_GPIO_Port
//MAX7219 register definitions
#define REG_NO_OP			0x00
#define REG_DIG0			0x01
#define REG_DIG1			0x02
#define REG_DIG2			0x03
#define REG_DIG3			0x04
#define REG_DIG4			0x05
#define REG_DIG5			0x06
#define REG_DIG6			0x07
#define REG_DIG7			0x08
#define REG_DECODE			0x09
#define REG_INTENSITY		0x0A
#define REG_SCANLIMIT		0x0B
#define REG_SHTDWN			0x0C
#define REG_DISPTEST		0x0D
//MAX7219 Shutdown commands
#define SHUTDOWN_MODE		0x00
#define NORMAL_MODE			0x01
//MAX7219 decode mode commands
#define NO_DECODE			0x00
#define DECODE0				0x01
#define DECODE3_0			0x0F
#define DECODE7_0			0xFF
//MAX7219 Intensity values
#define INTENSITY_1			0x00
#define INTENSITY_3			0x01
#define INTENSITY_5			0x02
#define INTENSITY_7			0x03
#define INTENSITY_9			0x04
#define INTENSITY_11		0x05
#define INTENSITY_13		0x06
#define INTENSITY_15		0x07
#define INTENSITY_17		0x08
#define INTENSITY_19		0x09
#define INTENSITY_21		0x0A
#define INTENSITY_23		0x0B
#define INTENSITY_25		0x0C
#define INTENSITY_27		0x0D
#define INTENSITY_29		0x0E
#define INTENSITY_31		0x0F
//MAX7219 Scan limit values
#define	DISP0				0x00
#define	DISP0_1				0x01
#define	DISP0_2				0x02
#define	DISP0_3				0x03
#define	DISP0_4				0x04
#define	DISP0_5				0x05
#define	DISP0_6				0x06
#define	DISP0_7				0x07
//No operation
#define NOP					0x00
/***********************************************///MAX7219 define constants end
/***********************************************///constants declarations begin
Time_t					_time[2];
extern const uint8_t	WeekDays[7][10];					//h,k,sz,cs,p,sz,v sz�vegesen
extern const uint8_t	Months[12][12];						//jan,feb.....dec sz�vegesen
AppConfig_t 			AppCfg;
/***********************************************///constants declarations end
/***********************************************///functions declaration begin
/**/
void CreateDateData(void);
/**/
void Rotate(uint8_t* Dest,uint8_t* Source);
/**/
void UpdateTimeOnDisplay(void);
/**/
void DateToDisplayDataArray(void);
/**/
void TextToDisplayDataArray(char* TextMessage);
/**/
void SendToDisplay(uint16_t from);
/**/
void MAX7219_Init(void);
/**/
void MAX7219_LoadPulse(void);
/**/
void MAX7219_Send(uint8_t ADDR, uint8_t CMD);
/**/
void MAX7219_SetIntensity(void);
/**/
void SendTimeToDisplay(void);
/**/
uint8_t BitSwapping(uint8_t ch);
/**/
void RemoteXY_InitAndRun(void);
/**/
void EEPROM_WriteFrame(void);
/**/
void EEPROM_ReadFrame(void);
/**/
void AppConfig_Init(void);
/**/
HAL_StatusTypeDef Init_Application(void);
/**/
void Run_Application(void);
/**/
void StateMachine(void);
/**/
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc);
/**/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
/**/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
/**/
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart);
/**/
void HAL_SYSTICK_Callback(void);

/***********************************************///functions declaration end
#endif
