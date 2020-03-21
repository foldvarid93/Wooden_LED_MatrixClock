#include "application.h"
/*********************************///
const uint8_t	DateText[] ={"A mai dátum: "};
const uint8_t	WeekDays[7][10]={
								{"hétfõ"},
								{"kedd"},
								{"szerda"},
								{"csütörtök"},
								{"péntek"},
								{"szombat"},
								{"vasárnap"}};
const uint8_t	Months[12][12]={
								{"január"},
								{"február"},
								{"március"},
								{"április"},
								{"május"},
								{"június"},
								{"július"},
								{"augusztus"},
								{"szeptember"},
								{"október"},
								{"november"},
								{"december"}};
/*********************************/				//Date functions begin
void CreateDateData(void){
	uint8_t	i=0;
	HAL_RTC_GetTime(&hrtc, &Time_Data, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &Date_Data, RTC_FORMAT_BIN);
	for(i=0;i<4;i++){							//16 szóköz
		TextArray[i]=' ';
	}
	for(uint8_t j=0;DateText[j]!='\0';i++,j++){	//A mai dátum:
		TextArray[i]=DateText[j];
	}
	TextArray[i++]='2';
	TextArray[i++]='0';
	TextArray[i++]=(Date_Data.Year/10)+'0';
	TextArray[i++]=(Date_Data.Year%10)+'0';	//évszám
	TextArray[i++]='.';							//pont
	TextArray[i++]=Date_Data.Month/10+'0';
	TextArray[i++]=Date_Data.Month%10+'0';	//hónap
	TextArray[i++]='.';							//pont
	TextArray[i++]=Date_Data.Date/10+'0';
	TextArray[i++]=Date_Data.Date%10+'0';	//nap
	TextArray[i++]='.';							//pont
	TextArray[i++]=',';							//vesszõ
	for(uint8_t j=0;WeekDays[Date_Data.WeekDay-1][j]!='\0';i++,j++){
		TextArray[i]=WeekDays[Date_Data.WeekDay-1][j];		//a hét napja
	}
	for(uint8_t j=0;j<4;i++,j++){				//16 szóköz
		TextArray[i]=' ';
	}
	TextArray[i]='\0';							//lezáró nulla
}
/*********************************/				//Date functions end
/*********************************/				//Time functions begin
void CreateFrameFromTime(void){
	HAL_RTC_GetTime(&hrtc, &Time_Data, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &Date_Data, RTC_FORMAT_BIN);

	uint8_t hour_ten_new = (Time_Data.Hours / 10);//newDataTime.hour() / 10;
	uint8_t hour_single_new = (Time_Data.Hours % 10);//newDataTime.hour() % 10;
	uint8_t min_ten_new = (Time_Data.Minutes / 10);//newDataTime.minute() / 10;
	uint8_t min_single_new = (Time_Data.Minutes % 10);//newDataTime.minute() % 10;
	uint8_t sec_ten_new = (Time_Data.Seconds / 10);//newDataTime.second() / 10;
	uint8_t sec_single_new = (Time_Data.Seconds % 10);//newDataTime.second() % 10;
	for (uint8_t i = 0; i < 5; i++) {
		if (hour_ten_new == 0) {
			DisplayData[i] = 0x00;
		} else {
			DisplayData[i] = BitSwapping(characters[hour_ten_new + '0'][i]);
		}
		DisplayData[i + 5] = BitSwapping(characters[hour_single_new + '0'][i]);
		DisplayData[i + 13] = BitSwapping(characters[min_ten_new + '0'][i]);
		DisplayData[i + 18] = BitSwapping(characters[min_single_new + '0'][i]);
	}
	DisplayData[10] = 0;
	if(Point){
		DisplayData[11] = 0x22;
	}
	else{
		DisplayData[11] = 0x00;
	}
	DisplayData[12] = 0;
	DisplayData[23] = 0;
	SendFrameToDisplay();
}
/*********************************/				//Time functions end
/*********************************/				//Text functions begin
void CreateDisplayDataArray(uint8_t *Text) {
  ScrollText = false;
  TextLength = strlen(Text);
  for (uint8_t i = 0; i < TextLength; i++) {
    for (uint8_t j = 0; j < 6; j++) {
      DisplayDataArray[(i * 6) + j] = BitSwapping(characters[Text[i]][j]);
    }
  }
  StartFrom = 0;
  ScrollEnd = false;
  ScrollText = true;
}
void SendToDisplay(uint16_t from) {
  SPI_Send(REG_SHTDWN, SHUTDOWN_MODE);
  for (uint8_t i = 1; i < 9; i++) {
	  HAL_GPIO_WritePin(MAX7219_CS_PORT,MAX7219_CS_PIN,GPIO_PIN_RESET);
    for (uint8_t j = 0; j < 3; j++) {
    	HAL_SPI_Transmit(&hspi2,&i,1,50);
    	HAL_SPI_Transmit(&hspi2,&(DisplayDataArray[from + ((2 - j) * 8) + (i - 1)]),1,50); //data
    }
    HAL_GPIO_WritePin(MAX7219_CS_PORT,MAX7219_CS_PIN,GPIO_PIN_SET);
  }
  SPI_Send(REG_SHTDWN, NORMAL_MODE);
}
/*********************************/				//Text functions end
void Init_MAX7219(void){
	SPI_Send(REG_NO_OP, NOP);
	SPI_Send(REG_SHTDWN, NORMAL_MODE);
	SPI_Send(REG_DECODE, NO_DECODE);
	SPI_Send(REG_SCANLIMIT, DISP0_7);
	SPI_Send(REG_INTENSITY, INTENSITY_31);
}
void SPI_Send(uint8_t ADDR, uint8_t CMD){
	uint8_t tmp[24];
	for(uint8_t i=0;i<DispNum;i++){
		tmp[2*i]=ADDR;
		tmp[(2*i)+1]=CMD;
	}
	HAL_GPIO_WritePin(MAX7219_CS_PORT,MAX7219_CS_PIN,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi2,tmp,24,50);
	HAL_GPIO_WritePin(MAX7219_CS_PORT,MAX7219_CS_PIN,GPIO_PIN_SET);
}
void SendFrameToDisplay(void){
	uint8_t tmp[24];
	for(uint8_t i=0;i<DispNum;i++){
		tmp[2*i]=REG_SHTDWN;
		tmp[(2*i)+1]=SHUTDOWN_MODE;
	}
	HAL_GPIO_WritePin(MAX7219_CS_PORT,MAX7219_CS_PIN,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi2,tmp,24,100);
	HAL_GPIO_WritePin(MAX7219_CS_PORT,MAX7219_CS_PIN,GPIO_PIN_SET);
	for(uint8_t i=8;i>0;i--){
		for(uint8_t j=0;j<DispNum;j++){
			tmp[2*j]=i;
			tmp[(2*j)+1]=DisplayData[DispLength-(8*j)-9+i];
		}
		HAL_GPIO_WritePin(MAX7219_CS_PORT, MAX7219_CS_PIN, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi2, tmp, 24, 100);
		HAL_GPIO_WritePin(MAX7219_CS_PORT, MAX7219_CS_PIN, GPIO_PIN_SET);
	}
	for(uint8_t i=0;i<DispNum;i++){
		tmp[2*i]=REG_SHTDWN;
		tmp[(2*i)+1]=NORMAL_MODE;
	}
	HAL_GPIO_WritePin(MAX7219_CS_PORT,MAX7219_CS_PIN,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi2,tmp,24,100);
	HAL_GPIO_WritePin(MAX7219_CS_PORT,MAX7219_CS_PIN,GPIO_PIN_SET);
}
uint8_t BitSwapping(uint8_t ch){
	uint8_t retval=0x00;
	if (ch&0B00000001) retval|=0B10000000;
	if (ch&0B00000010) retval|=0B01000000;
	if (ch&0B00000100) retval|=0B00100000;
	if (ch&0B00001000) retval|=0B00010000;
	if (ch&0B00010000) retval|=0B00001000;
	if (ch&0B00100000) retval|=0B00000100;
	if (ch&0B01000000) retval|=0B00000010;
	if (ch&0B10000000) retval|=0B00000001;
	return ~retval;
}
void RTCWrite(RTC_TimeTypeDef Time){
	Time.Hours=16;
	Time.Minutes=37;
	HAL_RTC_SetTime(&hrtc,&Time,RTC_FORMAT_BIN);
}
