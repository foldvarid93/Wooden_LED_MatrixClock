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
uint8_t *RxData=&TextArray[0];
uint8_t RxBufferIndex;
uint8_t AtCommandLength;
bool ReceiveComplete=false;
const uint8_t	ATRST[]="AT+RST";
const uint8_t	AT[]="ATE0";
const uint8_t	ESP8266RST[]="AT+RST";
/*********************************/				//ESP8266 functions begin
void SendAtCommandToEsp8266(uint8_t *String){
	uint8_t AtCommandLength=strlen(String);
	static uint8_t TmpArray[30];
	for(uint8_t i=0;i<AtCommandLength;i++){
		TmpArray[i]=String[i];
	}
	TmpArray[AtCommandLength++]='\r';
	TmpArray[AtCommandLength++]='\n';
	RxBufferIndex=0;
	HAL_UART_Transmit(&huart3, TmpArray, AtCommandLength,1);
	HAL_UART_Receive_IT(&huart3, RxData+RxBufferIndex, 1);
}
void Init_ESP8266(void){
	HAL_GPIO_WritePin(ESP_RESET_PORT,ESP_RESET_PIN,GPIO_PIN_RESET);
	HAL_Delay(3000);
	HAL_GPIO_WritePin(ESP_RESET_PORT,ESP_RESET_PIN,GPIO_PIN_SET);
}
/*********************************/				//ESP8266 functions begin
/*********************************/				//Date functions begin
void FormatDateToText(void){
	uint8_t	i=0;
	for(i=0;i<16;i++){							//16 szóköz
		TextArray[i]=' ';
	}
	for(uint8_t j=0;DateText[j]!='\0';i++,j++){	//A mai dátum:
		TextArray[i]=DateText[j];
	}
	TextArray[i++]=Date_Data.year_thousands+'0';
	TextArray[i++]=Date_Data.year_hundreds+'0';
	TextArray[i++]=Date_Data.year_tens+'0';
	TextArray[i++]=Date_Data.year_singles+'0';	//évszám
	TextArray[i++]='.';							//pont
	TextArray[i++]=Date_Data.month_tens+'0';
	TextArray[i++]=Date_Data.month_singles+'0';	//hónap
	TextArray[i++]='.';							//pont
	TextArray[i++]=Date_Data.date_tens+'0';
	TextArray[i++]=Date_Data.date_singles+'0';	//nap
	TextArray[i++]='.';							//pont
	TextArray[i++]=',';							//vesszõ
	for(uint8_t j=0;WeekDays[Date_Data.day-1][j]!='\0';i++,j++){
		TextArray[i]=WeekDays[Date_Data.day-1][j];		//a hét napja
	}
	for(uint8_t j=0;j<16;i++,j++){				//16 szóköz
		TextArray[i]=' ';
	}
	TextArray[i]='\0';							//lezáró nulla
}
/*********************************/				//Date functions end
/*********************************/				//Time functions begin
void FormatTimeToText(void){
	uint8_t i = 0;
	for (i = 0; i < 16; i++) {							//16 szóköz
		TextArray[i] = ' ';
	}
	TextArray[i++]=Time_Data.hour_tens+'0';
	TextArray[i++]=Time_Data.hour_singles+'0';	//óra
	TextArray[i++]=':';
	TextArray[i++]=Time_Data.min_tens+'0';
	TextArray[i++]=Time_Data.min_singles+'0';	//perc
	TextArray[i++]=':';
	TextArray[i++]=Time_Data.sec_tens+'0';
	TextArray[i++]=Time_Data.sec_singles+'0';
	for(uint8_t j=0;j<16;i++,j++){				//16 szóköz
		TextArray[i]=' ';
	}
	TextArray[i]='\0';
}
void CreateFrameFromTime(void){
	RTC_TimeTypeDef RTC_TimeData;

	static uint8_t Time[12];
	static uint8_t *TimeOldData = &Time[0];
	static uint8_t *TimeNewData = &Time[6];

	static uint8_t TmpArray[72];
	static uint8_t *OldDataPtr = &TmpArray[0];
	static uint8_t *NewDataPtr = &TmpArray[36];

	if (indx == 0) {
		HAL_RTC_GetTime(&hrtc, &RTC_TimeData, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, NULL, RTC_FORMAT_BIN);

		*TimeNewData = (RTC_TimeData.Hours / 10) + '0';
		*(TimeNewData + 1) = (RTC_TimeData.Hours % 10) + '0';
		*(TimeNewData + 2) = (RTC_TimeData.Minutes / 10) + '0';
		*(TimeNewData + 3) = (RTC_TimeData.Minutes % 10) + '0';
		*(TimeNewData + 4) = (RTC_TimeData.Seconds / 10) + '0';
		*(TimeNewData + 5) = (RTC_TimeData.Seconds % 10) + '0';

		for (uint8_t i = 0; i < 6; i++) {
			for (uint8_t j = 0; j < 6; j++) {
				*(NewDataPtr + (i * 6) + j) = BitSwapping(characters[*(TimeNewData + i)][j]);}
		}
	}
	if (*TimeNewData != *TimeOldData) {
		for (uint8_t i = 0; i < 6; i++) {
			*(OldDataPtr + i) <<= 1;
			*(OldDataPtr + i) &= 0B11111110;
			if (*(NewDataPtr + i) & 0B10000000) {*(OldDataPtr + i) |= 0B00000001;}
			*(NewDataPtr + i) <<= 1;
		}
	}
	if (*(TimeNewData + 1) != *(TimeOldData + 1)) {
		for (uint8_t i = 6; i < 12; i++) {
			*(OldDataPtr + i) <<= 1;
			*(OldDataPtr + i) &= 0B11111110;
			if (*(NewDataPtr + i) & 0B10000000) {*(OldDataPtr + i) |= 0B00000001;}
			*(NewDataPtr + i) <<= 1;
		}
	}
	if (*(TimeNewData + 2) != *(TimeOldData + 2)) {
		for (uint8_t i = 12; i < 18; i++) {
			*(OldDataPtr + i) <<= 1;
			*(OldDataPtr + i) &= 0B11111110;
			if (*(NewDataPtr + i) & 0B10000000) {*(OldDataPtr + i) |= 0B00000001;}
			*(NewDataPtr + i) <<= 1;
		}
	}
	if (*(TimeNewData + 3) != *(TimeOldData + 3)) {
		for (uint8_t i = 18; i < 24; i++) {
			*(OldDataPtr + i) <<= 1;
			*(OldDataPtr + i) &= 0B11111110;
			if (*(NewDataPtr + i) & 0B10000000) {*(OldDataPtr + i) |= 0B00000001;}
			*(NewDataPtr + i) <<= 1;
		}
	}
	if (*(TimeNewData + 4) != *(TimeOldData + 4)) {
		for (uint8_t i = 24; i < 30; i++) {
			*(OldDataPtr + i) <<= 1;
			*(OldDataPtr + i) &= 0B11111110;
			if (*(NewDataPtr + i) & 0B10000000) {*(OldDataPtr + i) |= 0B00000001;}
			*(NewDataPtr + i) <<= 1;
		}
	}
	if (*(TimeNewData + 5) != *(TimeOldData + 5)) {
		for (uint8_t i = 30; i < 36; i++) {
			*(OldDataPtr + i) <<= 1;
			*(OldDataPtr + i) &= 0B11111110;
			if (*(NewDataPtr + i) & 0B10000000) {*(OldDataPtr + i) |= 0B00000001;}
			*(NewDataPtr + i) <<= 1;
		}
	}
	for (uint8_t i = 0; i < 6; i++) {							//Kettõspontok
		DisplayData[i] = 0;
		DisplayData[i + 6] = 0;
		DisplayData[i + 12] = 0;
		DisplayData[i + 18] = 0;
		DisplayData[i + 24] = *(OldDataPtr + i);
		DisplayData[i + 30] = *(OldDataPtr + i + 6);
		DisplayData[i + 36] = DisplayData[i + 54] = BitSwapping(characters[':'][i]);
		DisplayData[i + 42] = *(OldDataPtr + i + 12);
		DisplayData[i + 48] = *(OldDataPtr + i + 18);
		DisplayData[i + 60] = *(OldDataPtr + i + 24);
		DisplayData[i + 66] = *(OldDataPtr + i + 30);
		DisplayData[i + 72] = 0;
		DisplayData[i + 78] = 0;
		DisplayData[i + 84] = 0;
		DisplayData[i + 90] = 0;
	}
	for (uint8_t i = 0; i < 6; i++) {
		DisplayData[i] = *(OldDataPtr + i + 30);
	}
	SendFrameToDisplay();
	indx++;
	if (indx == 8) {
		indx = 0;
		uint8_t *tmp = TimeOldData;
		TimeOldData = TimeNewData;
		TimeNewData = tmp;
	}
}
/*********************************/				//Time functions end
/*********************************/				//Text functions begin
void SendTextToDisplay(uint8_t *Text){
	uint8_t TextLength=0, ActualChar=0, column=0,kar_elt=0;	//length=strlength(text);
	while(Text[TextLength]!='\0'){					//Length of the text array
		TextLength++;
	}
	if(TextLength==0){								//ha 0 hosszú a szöveg rögtön return
		return;
	}
	if(TextLength<=16){								//16 karakter fér ki a kijelzõre, ha 16 vagy annál rövidebb akkor gond nélkül kifér
		for(uint8_t i=0;i<TextLength;i++){
			ActualChar=Text[i];
			for(uint8_t j=0;j<6;j++){
				DisplayData[column]=BitSwapping(characters[ActualChar][j]);
			}
		}
		SendFrameToDisplay();
		return;
	}
	else{											//Ha hosszabb mint 16 akkor shiftelni kell
		for(uint8_t i=0;i<16;i++){
			ActualChar=Text[i];
			for(uint8_t j=0;j<6;j++){
				DisplayData[i]=BitSwapping(characters[i][j]);
			}
		}
		SendFrameToDisplay();						//megjeleníti az elsõ 16 karaktert
		for (kar_elt = 16; kar_elt < TextLength; kar_elt++) {
			ActualChar = Text[kar_elt];
			for (uint8_t i=0; i < 6; i++) {
				for (uint8_t j = 0; j < (DispLength-1); j++) {
					DisplayData[j] = DisplayData[j + 1];
				}
				DisplayData[DispLength-1] = BitSwapping(characters[ActualChar][i]);
				SendFrameToDisplay();
				HAL_Delay(50);
			}
		}
		return;
	}
}
/*********************************/				//Text functions end
void Init_MAX7219(void){
	SPI_Send(REG_NO_OP, NOP);
	SPI_Send(REG_SHTDWN, NORMAL_MODE);
	SPI_Send(REG_DECODE, NO_DECODE);
	SPI_Send(REG_SCANLIMIT, DISP0_7);
	SPI_Send(REG_INTENSITY, INTENSITY_1);
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
void TestData(void){
	for(uint8_t i=0; i<DispLength;i++){
		DisplayData[i]=0xFF;
	}
}
void ClearDisplay(void){
	for(uint8_t i=0; i<DispLength;i++){
		DisplayData[i]=0;
	}
}
void ClearDisplayFromTo(uint8_t from, uint8_t to){
	for(uint8_t i=from;i<to;i++){
		DisplayData[i]=0;
	}
}
void TestText(uint8_t ch){
	for(uint8_t i=0;i<6;i++){
		DisplayData[i]=BitSwapping(characters[ch][i]);
	}
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
void RTC_Read(void){
	RTC_TimeTypeDef Time;
	RTC_DateTypeDef Date;
	HAL_RTC_GetTime(&hrtc, &Time, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc,&Date,RTC_FORMAT_BIN);
	RTC_Data.sec=Time.Seconds;
	RTC_Data.min=Time.Minutes;
	RTC_Data.hour=Time.Hours;
	RTC_Data.day=Date.WeekDay;
	RTC_Data.date=Date.Date;
	RTC_Data.month=Date.Month;
	RTC_Data.year=Date.Year;
	ConvertRTCToDateAndTime(&RTC_Data,&Time_Data,&Date_Data);
}
void BinToTensAndSingles(uint8_t binary, uint8_t *tens, uint8_t *singles){
	*tens=binary/10;
	*singles=binary%10;
}
void ConvertRTCToDateAndTime(RTC_DATA *RTC_Data,TIME *Time_Data, DATE *Date_Data){
	Time_Data->hour_tens=RTC_Data->hour/10;
	Time_Data->hour_singles=RTC_Data->hour%10;

	Time_Data->min_tens=RTC_Data->min/10;
	Time_Data->min_singles=RTC_Data->min%10;

	Time_Data->sec_tens=RTC_Data->sec/10;
	Time_Data->sec_singles=RTC_Data->sec%10;

	Date_Data->day=RTC_Data->day;

	Date_Data->date_tens=RTC_Data->date/10;
	Date_Data->date_singles=RTC_Data->date%10;

	Date_Data->month_tens=RTC_Data->month/10;
	Date_Data->month_singles=RTC_Data->month%10;

	Date_Data->year_thousands=2;
	Date_Data->year_hundreds=0;
	Date_Data->year_tens=RTC_Data->year/10;
	Date_Data->year_singles=RTC_Data->year%10;
}
