#ifndef _APPLICATION_H_
#define	_APPLICATION_H_
/***********************************************///includes begin
#include "stm32l4xx_hal.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include "rtc.h"
#include "characters.h"
#include "stdbool.h"
#include "string.h"
/***********************************************///includes end
/***********************************************///typedef begin
typedef struct RTC_DATA{
	uint8_t		sec;
	uint8_t		min;
	uint8_t		hour;
	uint8_t		day;
	uint8_t		date;
	uint8_t		month;
	uint8_t		year;
}RTC_DATA;
typedef struct TIME{
	uint8_t		hour_tens;
	uint8_t		hour_singles;
	uint8_t		min_tens;
	uint8_t		min_singles;
	uint8_t		sec_tens;
	uint8_t		sec_singles;
}TIME;
typedef struct DATE{
	uint8_t		day;
	uint8_t		date_tens;
	uint8_t		date_singles;
	uint8_t		month_tens;
	uint8_t		month_singles;
	uint8_t 	year_thousands;
	uint8_t		year_hundreds;
	uint8_t		year_tens;
	uint8_t		year_singles;
}DATE;
struct {
	unsigned char TIME_SET : 1;
	unsigned char DATE_TIME :1;
} MASK;
struct {
	unsigned char B0 :1;
	unsigned char B1 :1;
	unsigned char B2 :1;
	unsigned char B3 :1;
	unsigned char B4 :1;
	unsigned char B5 :1;
	unsigned char B6 :1;
	unsigned char B7 :1;
} MASKBYTE;
/***********************************************///typedef end
#define DispNum 			12
#define DispLength 			96
/***********************************************///MAX7219 define constants begin
#define ESP_RESET_PIN		GPIO_PIN_13
#define ESP_RESET_PORT		GPIOB
#define MAX7219_CS_PIN		GPIO_PIN_12			//Chip select pin
#define MAX7219_CS_PORT		GPIOB
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
RTC_DATA				RTC_Data;							//sec,min,hour,day,date,month,year
RTC_TimeTypeDef	 		Time_Data;							//idõt tároló struktúrapéldány
RTC_DateTypeDef			Date_Data;							//dátumot tároló struktórapéldány
uint8_t					DisplayData[96];					//kijelzõ oszlopainak adatai
bool					Point;
bool					ScrollText;
bool 					ScrollEnd;
uint8_t 				StartFrom;
uint8_t					TextLength;
uint8_t 				TextArray[256];
uint8_t 				DisplayDataArray[1536];
uint8_t 				UartBuff[5];						//HH:MM formátumhoz elég 5 byte
uint8_t 				TimeData[4];						//ebben van tárolva az idõ
uint8_t 				seconds;
enum 					mode{Time, Date} Mode;
extern const uint8_t	WeekDays[7][10];					//h,k,sz,cs,p,sz,v szövegesen
extern const uint8_t	Months[12][12];						//jan,feb.....dec szövegesen
/***********************************************///constants declarations end
/***********************************************///functions declaration begin
void Init_MAX7219(void);
void CreateDateData(void);
void CreateDisplayDataArray(uint8_t* Text);
void SendToDisplay(uint16_t from);
void SPI_Send(uint8_t ADDR, uint8_t CMD);
void SendFrameToDisplay(void);
void TestData(void);
void TestText(uint8_t ch);
uint8_t BitSwapping(uint8_t ch);
void ClearDisplay(void);
void ClearDisplayFromTo(uint8_t from, uint8_t to);
void RTC_Read(void);
void BinToTensAndSingles(uint8_t binary, uint8_t *tens, uint8_t *singles);
void ConvertRTCToDateAndTime(RTC_DATA *RTC_DATA,TIME *Time_Data, DATE *Date_Data);
void FormatDateToText(void);
void FormatTimeToText(void);
void CreateFrameFromTime(void);
void SendTextToDisplay(char *Text);
/***********************************************///functions declaration end
#endif
