/*********************************///
#include "application.h"
/*********************************///
//Flash variables
bool FlashWriteEnabled=true;
uint16_t VirtAddVarTab;//[NB_OF_VAR] = {0x0001};
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
void TimeAnimation(uint8_t* Dest,uint8_t* Source){
	for(uint8_t i=0;i<6;i++){
		(Dest[i])<<=1;
		if ((Source[i])&0x80){
			(Dest[i])|=0x01;
		}
		else{
			(Dest[i])&=0xFE;
		}
		Source[i]<<=1;
	}
/*	if(Iteration==1){
		Old<<=1;
	}
	if(b&0x01){
		a|=0x80;
	}
	else{
		a&=0x7F;
	}
	return a;*/
}
char concat(char b, char a){
	a>>=1;
	if(b&0x01){
		a|=0x80;
	}
	else{
		a&=0x7F;
	}
	return a;
}
void time_out(void){
#define StartIdx 28
#define HourTensStartIdx 	StartIdx
#define HourSinglesStartIdx StartIdx+6
#define HourMinDoubleDot 	HourSinglesStartIdx+6
#define MinTensStartIdx 	HourMinDoubleDot+2
#define MinSinglesStartIdx 	MinTensStartIdx+6
#define MinSecDoubleDot 	MinSinglesStartIdx+6
#define SecTensStartIdx 	MinSecDoubleDot+2
#define SecSinglesStartIdx 	SecTensStartIdx+6

	if(FirstRun==1){
		HAL_RTC_GetTime(&hrtc, &Time_Data, RTC_FORMAT_BIN);//read new time
		HAL_RTC_GetDate(&hrtc, &Date_Data, RTC_FORMAT_BIN); //rtcread_time(&time[0]);

		time[0].hour_tens=Time_Data.Hours / 10;
		time[0].hour_singles=Time_Data.Hours % 10;
		time[0].min_tens=Time_Data.Minutes / 10;
		time[0].min_singles=Time_Data.Minutes % 10;
		time[0].sec_tens=Time_Data.Seconds / 10;
		time[0].sec_singles=Time_Data.Seconds % 10;

		for (uint8_t i = 0; i < 5; i++) {
			if (time[0].hour_tens == 0) {
				DisplayData[i+HourTensStartIdx] = 0;
			} else {
				DisplayData[i+HourTensStartIdx] = BitSwapping(characters[time[0].hour_tens+ '0'][i]);
			}
			DisplayData[i + HourSinglesStartIdx] = BitSwapping(characters[time[0].hour_singles + '0'][i]);
			DisplayData[i + MinTensStartIdx] = BitSwapping(characters[time[0].min_tens + '0'][i]);
			DisplayData[i + MinSinglesStartIdx] = BitSwapping(characters[time[0].min_singles + '0'][i]);
			DisplayData[i + SecTensStartIdx] = BitSwapping(characters[time[0].sec_tens + '0'][i]);
			DisplayData[i + SecSinglesStartIdx] = BitSwapping(characters[time[0].sec_singles + '0'][i]);
		}
		DisplayData[HourMinDoubleDot] = 0x22;
		DisplayData[MinSecDoubleDot] = 0x22;

		SendFrameToDisplay();

		time[1]=time[0];
		FirstRun=0;
	}
	if(UpdateTime==1){
		HAL_RTC_GetTime(&hrtc, &Time_Data, RTC_FORMAT_BIN);//read new time
		HAL_RTC_GetDate(&hrtc, &Date_Data, RTC_FORMAT_BIN); //rtcread_time(&time[0]);

		time[0].hour_tens=Time_Data.Hours / 10;
		time[0].hour_singles=Time_Data.Hours % 10;
		time[0].min_tens=Time_Data.Minutes / 10;
		time[0].min_singles=Time_Data.Minutes % 10;
		time[0].sec_tens=Time_Data.Seconds / 10;
		time[0].sec_singles=Time_Data.Seconds % 10;

		if(time[0].sec_singles!=time[1].sec_singles){
			TimeDiffIndicator[5]=1;
			for(uint8_t i=0;i<5;i++){
				NewTimeDataArray[i + 30] = BitSwapping(characters[time[0].sec_singles + '0'][i]);
			}
		}
		else {
			TimeDiffIndicator[5]=0;
		}
		if(time[0].sec_tens!=time[1].sec_tens){
			TimeDiffIndicator[4]=1;
			for(uint8_t i=0;i<5;i++){
				NewTimeDataArray[i + 24] = BitSwapping(characters[time[0].sec_tens + '0'][i]);
			}
		}
		else {
			TimeDiffIndicator[4]=0;
		}
		if(time[0].min_singles!=time[1].min_singles){
			TimeDiffIndicator[3]=1;
			for(uint8_t i=0;i<5;i++){
				NewTimeDataArray[i + 18] = BitSwapping(characters[time[0].min_singles + '0'][i]);
			}
		}
		else {
			TimeDiffIndicator[3]=0;
		}
		if(time[0].min_tens!=time[1].min_tens){
			TimeDiffIndicator[2]=1;
			for(uint8_t i=0;i<5;i++){
				NewTimeDataArray[i + 12] = BitSwapping(characters[time[0].min_tens + '0'][i]);
			}
		}
		else {
			TimeDiffIndicator[2]=0;
		}
		if(time[0].hour_singles!=time[1].hour_singles){
			TimeDiffIndicator[1]=1;
			for(uint8_t i=0;i<5;i++){
				NewTimeDataArray[i + 6] = BitSwapping(characters[time[0].hour_singles + '0'][i]);
			}
		}
		else {
			TimeDiffIndicator[1]=0;
		}
		if(time[0].hour_tens!=time[1].hour_tens){
			TimeDiffIndicator[0]=1;
			for(uint8_t i=0;i<5;i++){
				NewTimeDataArray[i] = BitSwapping(characters[time[0].hour_tens + '0'][i]);
			}
		}
		else {
			TimeDiffIndicator[0]=0;
		}
		time[1]=time[0];
		UpdateTime=0;
	}
	if(Flip==1){
		if (TimeDiffIndicator[0]){
			TimeAnimation(&DisplayData[HourTensStartIdx],&NewTimeDataArray[0]);
		}
		if (TimeDiffIndicator[1]){
			TimeAnimation(&DisplayData[HourSinglesStartIdx],&NewTimeDataArray[6]);
		}
		if (TimeDiffIndicator[2]){
			TimeAnimation(&DisplayData[MinTensStartIdx],&NewTimeDataArray[12]);
		}
		if(TimeDiffIndicator[3]){
			TimeAnimation(&DisplayData[MinSinglesStartIdx],&NewTimeDataArray[18]);
		}
		if(TimeDiffIndicator[4]){
			TimeAnimation(&DisplayData[SecTensStartIdx],&NewTimeDataArray[24]);
		}
		if(TimeDiffIndicator[5]){
			TimeAnimation(&DisplayData[SecSinglesStartIdx],&NewTimeDataArray[30]);
		}
		SendFrameToDisplay();
		FlipCounter++;
		if(FlipCounter==8){
			FlipCounter=0;
			Flip=0;
		}
	}
/*
	HAL_RTC_GetTime(&hrtc, &Time_Data, RTC_FORMAT_BIN);//read new time
	HAL_RTC_GetDate(&hrtc, &Date_Data, RTC_FORMAT_BIN); //rtcread_time(&time[0]);

	time[0].hour_tens=Time_Data.Hours / 10;
	time[0].hour_singles=Time_Data.Hours % 10;
	time[0].min_tens=Time_Data.Minutes / 10;
	time[0].min_singles=Time_Data.Minutes % 10;
	time[0].sec_tens=Time_Data.Seconds / 10;
	time[0].sec_singles=Time_Data.Seconds % 10;

	if(time[0].sec_singles!=time[1].sec_singles){
		TimeDiffIndicator[5]=1;
		for(uint8_t i=0;i<5;i++){
			NewTimeDataArray[i + 30] = BitSwapping(characters[time[0].sec_singles + '0'][i]);
		}
	}
	else {
		TimeDiffIndicator[5]=0;
	}
	if(time[0].sec_tens!=time[1].sec_tens){
		TimeDiffIndicator[4]=1;
		for(uint8_t i=0;i<5;i++){
			NewTimeDataArray[i + 24] = BitSwapping(characters[time[0].sec_tens + '0'][i]);
		}
	}
	else {
		TimeDiffIndicator[4]=0;
	}
	if(time[0].min_singles!=time[1].min_singles){
		TimeDiffIndicator[3]=1;
		for(uint8_t i=0;i<5;i++){
			NewTimeDataArray[i + 18] = BitSwapping(characters[time[0].min_singles + '0'][i]);
		}
	}
	else {
		TimeDiffIndicator[3]=0;
	}
	if(time[0].min_tens!=time[1].min_tens){
		TimeDiffIndicator[2]=1;
		for(uint8_t i=0;i<5;i++){
			NewTimeDataArray[i + 12] = BitSwapping(characters[time[0].min_tens + '0'][i]);
		}
	}
	else {
		TimeDiffIndicator[2]=0;
	}
	if(time[0].hour_singles!=time[1].hour_singles){
		TimeDiffIndicator[1]=1;
		for(uint8_t i=0;i<5;i++){
			NewTimeDataArray[i + 6] = BitSwapping(characters[time[0].hour_singles + '0'][i]);
		}
	}
	else {
		TimeDiffIndicator[1]=0;
	}
	if(time[0].hour_tens!=time[1].hour_tens){
		TimeDiffIndicator[0]=1;
		for(uint8_t i=0;i<5;i++){
			NewTimeDataArray[i] = BitSwapping(characters[time[0].hour_tens + '0'][i]);
		}
	}
	else {
		TimeDiffIndicator[0]=0;
	}
	for(uint8_t i=0;i<8;i++){
		if (TimeDiffIndicator[0]){
			TimeAnimation(&DisplayData[HourTensStartIdx],&NewTimeDataArray[0]);
		}
		if (TimeDiffIndicator[1]){
			TimeAnimation(&DisplayData[HourSinglesStartIdx],&NewTimeDataArray[6]);
		}
		if (TimeDiffIndicator[2]){
			TimeAnimation(&DisplayData[MinTensStartIdx],&NewTimeDataArray[12]);
		}
		if(TimeDiffIndicator[3]){
			TimeAnimation(&DisplayData[MinSinglesStartIdx],&NewTimeDataArray[18]);
		}
		if(TimeDiffIndicator[4]){
			TimeAnimation(&DisplayData[SecTensStartIdx],&NewTimeDataArray[24]);
		}
		if(TimeDiffIndicator[5]){
			TimeAnimation(&DisplayData[SecSinglesStartIdx],&NewTimeDataArray[30]);
		}
		SendFrameToDisplay();
	}

	time[1]=time[0];*/
}
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
			DisplayData[i] = 0;
		} else {
			DisplayData[i] = BitSwapping(characters[hour_ten_new + '0'][i]);
		}
		DisplayData[i + 5] = BitSwapping(characters[hour_single_new + '0'][i]);
		DisplayData[i + 13] = BitSwapping(characters[min_ten_new + '0'][i]);
		DisplayData[i + 18] = BitSwapping(characters[min_single_new + '0'][i]);
		DisplayData[i + 26] = BitSwapping(characters[sec_ten_new + '0'][i]);
		DisplayData[i + 31] = BitSwapping(characters[sec_single_new + '0'][i]);
	}
	DisplayData[10] = 0;
	if(Point){
		DisplayData[11] = 0x22;
		DisplayData[24] = 0x22;
	}
	else{
		DisplayData[11] = 0;
		DisplayData[24] = 0;
	}
	DisplayData[12] = 0;
	DisplayData[23] = 0;
	DisplayData[25] = 0;
	SendFrameToDisplay();
}
/*********************************/				//Time functions end
/*********************************/				//Text functions begin
void CreateDisplayDataArray(uint8_t *Text) {
  ScrollText = false;
  TextLength = strlen((const char*)Text);
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
#define DispCount 12//
  //
  uint8_t tmp[192];
  for(uint8_t i=0;i<8;i++){
	  for(uint8_t j=0;j<12;j++){
		  tmp[(i*24)+(2*j)]=8-i;
		  tmp[192-((i*24)+(2*j)+1)]=DisplayDataArray[from+(j*8)+i];
	  }
  }
  //
  SPI_Send(REG_SHTDWN, SHUTDOWN_MODE);
  for(uint8_t i=0;i<8;i++){
	  HAL_GPIO_WritePin(MAX7219_CS_PORT,MAX7219_CS_PIN,GPIO_PIN_RESET);
	  HAL_SPI_Transmit(&hspi2,&tmp[i*24],24,50);
	  HAL_GPIO_WritePin(MAX7219_CS_PORT,MAX7219_CS_PIN,GPIO_PIN_SET);
  }
  /*
  for (uint8_t i = 1; i < 9; i++) {
	HAL_GPIO_WritePin(MAX7219_CS_PORT,MAX7219_CS_PIN,GPIO_PIN_RESET);
    for (uint8_t j = 0; j < DispCount; j++) {
    	HAL_SPI_Transmit(&hspi2,&i,1,50);
    	HAL_SPI_Transmit(&hspi2,&(DisplayDataArray[from + ((DispCount - j) * 8) + (i - 1)]),1,50); //data
    }
    HAL_GPIO_WritePin(MAX7219_CS_PORT,MAX7219_CS_PIN,GPIO_PIN_SET);
  }
  */
  SPI_Send(REG_SHTDWN, NORMAL_MODE);
  asm("nop");
}
/*********************************/				//Text functions end
void Init_MAX7219(void){
	SPI_Send(REG_NO_OP, NOP);
	SPI_Send(REG_SHTDWN, SHUTDOWN_MODE);
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
	uint8_t tmp1[24];
	uint8_t tmp2[24];
	uint8_t tmp3[8][24];
	for(uint8_t i=0;i<DispNum;i++){
		tmp1[2*i]=REG_SHTDWN;
		tmp1[(2*i)+1]=SHUTDOWN_MODE;
		tmp2[2*i]=REG_SHTDWN;
		tmp2[(2*i)+1]=NORMAL_MODE;
	}
	for(uint8_t i=8;i>0;i--){
		for(uint8_t j=0;j<DispNum;j++){
			tmp3[i-1][2*j]=i;
			tmp3[i-1][(2*j)+1]=DisplayData[DispLength-(8*j)-9+i];
		}
	}
	HAL_GPIO_WritePin(MAX7219_CS_PORT,MAX7219_CS_PIN,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi2,tmp1,24,100);
	HAL_GPIO_WritePin(MAX7219_CS_PORT,MAX7219_CS_PIN,GPIO_PIN_SET);
	for(uint8_t i=8;i>0;i--){
		HAL_GPIO_WritePin(MAX7219_CS_PORT, MAX7219_CS_PIN, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi2, &tmp3[i-1][0], 24, 100);
		HAL_GPIO_WritePin(MAX7219_CS_PORT, MAX7219_CS_PIN, GPIO_PIN_SET);
	}
	HAL_GPIO_WritePin(MAX7219_CS_PORT,MAX7219_CS_PIN,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi2,tmp2,24,100);
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
/* ESP8266 Functions Start ---------------------------------------------------------*/
void Init_ESP8266(void)
{
	HAL_GPIO_WritePin(ESP8266_RST_GPIO_Port, ESP8266_RST_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ESP8266_EN_GPIO_Port, ESP8266_EN_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ESP8266_EN_GPIO_Port, ESP8266_EN_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(ESP8266_RST_GPIO_Port, ESP8266_RST_Pin, GPIO_PIN_SET);
	Ringbuf_init();
	RemoteXY_Init();
}
/* ESP8266 Functions End ---------------------------------------------------------*/

/* Application Main Functions Start ---------------------------------------------------------*/
void Init_Application(void)
{
	/**/
	HAL_FLASH_Unlock();
	if (EE_Init() != EE_OK) {
		Error_Handler();
	}
	/*
	uint8_t Arr[]="Hello";
	if (EE_WriteCharArray(0x0100, &Arr) != EE_OK) {
		Error_Handler();
	}
	*/
	Init_MAX7219();
	//HAL_UART_Receive_IT(&huart2,UartBuff,5);
	/**/
	Init_ESP8266();
	/**/
	FirstRun=1;
	UpdateTime=0;
	Flip=0;
	FlipCounter=0;
	Point=false;
	seconds=0;
	Mode=Time;
	/**/
	HAL_TIM_Base_Start_IT(&htim3);
	HAL_TIM_Base_Start_IT(&htim4);
	__HAL_RTC_EXTI_ENABLE_IT(RTC_IT_ALRA);

}

void Run_Application(void)
{
	char Array[0xFF];
	while(1)
	{
		RemoteXY_Handler();
		if(RemoteXY.button_1==1)
		{
			//Uart_sendstring("SSID: ", &huart2);
			//Uart_sendstring(strcat(RemoteXY.edit_1,"\n"), &huart2);
			//Uart_sendstring("PASS: ", &huart2);
			//Uart_sendstring(strcat(RemoteXY.edit_2,"\n"), &huart2);
			/**/
			if('0'<=RemoteXY.edit_2[0]&&RemoteXY.edit_2[0]<='9' &&'0'<=RemoteXY.edit_2[1]&&RemoteXY.edit_2[1]<='9'&&RemoteXY.edit_2[2]==':'&&
					'0'<=RemoteXY.edit_2[3]&&RemoteXY.edit_2[3]<='9'&&'0'<=RemoteXY.edit_2[4]&&RemoteXY.edit_2[4]<='9')
			{
				RTC_TimeTypeDef Time;
				RTC_DateTypeDef Date;
				Time.Hours=(RemoteXY.edit_2[0]-'0')*10+(RemoteXY.edit_2[1]-'0');
				Time.Minutes=(RemoteXY.edit_2[3]-'0')*10+(RemoteXY.edit_2[4]-'0');
				Time.DayLightSaving=RTC_DAYLIGHTSAVING_NONE;
				HAL_RTC_SetTime(&hrtc,&Time,RTC_FORMAT_BIN);
				HAL_RTC_GetDate(&hrtc,&Date,RTC_FORMAT_BIN);
				HAL_RTC_GetTime(&hrtc,&Time,RTC_FORMAT_BIN);
			}
			strcpy(Array, "SSID:");
			strcat(Array,RemoteXY.edit_1);
			if (EE_WriteCharArray(0x0100, (uint8_t*)Array) != EE_OK) {
				Error_Handler();
			}
			strcpy(Array, "PASS: ");
			strcat(Array,RemoteXY.edit_2);
			if (EE_WriteCharArray(0x0200, (uint8_t*)Array) != EE_OK) {
				Error_Handler();
			}
			/**/
		    RemoteXY.button_1=0;
		}
	}

}
/* Application Main Functions End ---------------------------------------------------------*/
/* Interrupt Callbacks Start ---------------------------------------------------------*/
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	Point=!Point;
	UpdateTime=1;
	time_out();
	Flip=1;
	if(Mode==Time){
		//time_out();//CreateFrameFromTime();
		seconds++;
	}
/*	if(seconds==10){
		CreateDateData();
		CreateDisplayDataArray(TextArray);
		Mode=Date;
		seconds=0;
	}*/
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART2)	{
		if('0'<=UartBuff[0]&&UartBuff[0]<='9' &&'0'<=UartBuff[1]&&UartBuff[1]<='9'&&UartBuff[2]==':'&&
				'0'<=UartBuff[3]&&UartBuff[3]<='9'&&'0'<=UartBuff[4]&&UartBuff[4]<='9')
		{
			RTC_TimeTypeDef Time;
			RTC_DateTypeDef Date;
			Time.Hours=(UartBuff[0]-'0')*10+(UartBuff[1]-'0');
			Time.Minutes=(UartBuff[3]-'0')*10+(UartBuff[4]-'0');
			Time.DayLightSaving=RTC_DAYLIGHTSAVING_NONE;
			HAL_RTC_SetTime(&hrtc,&Time,RTC_FORMAT_BIN);
			HAL_RTC_GetDate(&hrtc,&Date,RTC_FORMAT_BIN);
			HAL_RTC_GetTime(&hrtc,&Time,RTC_FORMAT_BIN);
		}
		HAL_UART_Receive_IT(&huart2,UartBuff,5);
	}
}
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	if(huart->ErrorCode == HAL_UART_ERROR_ORE){
		HAL_UART_Receive_IT(&huart2,UartBuff,5);
	}
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	UNUSED(htim);
	if(htim->Instance==TIM3){
		if(Flip==1){
			time_out();
		}
		if (ScrollText) {
			if (StartFrom == ((TextLength * 6) - 24)) {
				ScrollText = false;
				Mode=Time;
				CreateFrameFromTime();
			}
			else {
				SendToDisplay(StartFrom);
				StartFrom++;
			}
		}
	}
	if(htim->Instance==TIM4){
	}
}
void HAL_SYSTICK_Callback(void)
{
}
/* Interrupt Callbacks End ---------------------------------------------------------*/
