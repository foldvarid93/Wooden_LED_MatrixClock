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
/**/
void CreateDateData(void)
{
	uint8_t	i=0;
	HAL_RTC_GetTime(&hrtc, &Time_Data, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &Date_Data, RTC_FORMAT_BIN);
	for(i=0;i<4;i++){							//16 sz?k?z
		AppCfg.DisplayTextArray[i]=' ';
	}
	for(uint8_t j=0;DateText[j]!='\0';i++,j++){	//A mai d?tum:
		AppCfg.DisplayTextArray[i]=DateText[j];
	}
	AppCfg.DisplayTextArray[i++]='2';
	AppCfg.DisplayTextArray[i++]='0';
	AppCfg.DisplayTextArray[i++]=(Date_Data.Year/10)+'0';
	AppCfg.DisplayTextArray[i++]=(Date_Data.Year%10)+'0';	//?vsz?m
	AppCfg.DisplayTextArray[i++]='.';							//pont
	AppCfg.DisplayTextArray[i++]=Date_Data.Month/10+'0';
	AppCfg.DisplayTextArray[i++]=Date_Data.Month%10+'0';	//h?nap
	AppCfg.DisplayTextArray[i++]='.';							//pont
	AppCfg.DisplayTextArray[i++]=Date_Data.Date/10+'0';
	AppCfg.DisplayTextArray[i++]=Date_Data.Date%10+'0';	//nap
	AppCfg.DisplayTextArray[i++]='.';							//pont
	AppCfg.DisplayTextArray[i++]=',';							//vessz?
	for(uint8_t j=0;WeekDays[Date_Data.WeekDay-1][j]!='\0';i++,j++){
		AppCfg.DisplayTextArray[i]=WeekDays[Date_Data.WeekDay-1][j];		//a h?t napja
	}
	for(uint8_t j=0;j<4;i++,j++){				//16 sz?k?z
		AppCfg.DisplayTextArray[i]=' ';
	}
	AppCfg.DisplayTextArray[i]='\0';			//lez?r? nulla
	/**/
}
/*********************************/				//Date functions end
/*********************************/				//Time functions begin
/**/
void TimeAnimation(uint8_t* Dest,uint8_t* Source)
{
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
}
/**/
char concat(char b, char a)
{
	a>>=1;
	if(b&0x01){
		a|=0x80;
	}
	else{
		a&=0x7F;
	}
	return a;
}
/**/
void time_out(void)
{
#define StartIdx 			28
#define HourTensStartIdx 	StartIdx
#define HourSinglesStartIdx StartIdx+6
#define HourMinDoubleDot 	HourSinglesStartIdx+6
#define MinTensStartIdx 	HourMinDoubleDot+2
#define MinSinglesStartIdx 	MinTensStartIdx+6
#define MinSecDoubleDot 	MinSinglesStartIdx+6
#define SecTensStartIdx 	MinSecDoubleDot+2
#define SecSinglesStartIdx 	SecTensStartIdx+6

	if(AppCfg.FirstRun==1)
	{
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
				AppCfg.DisplayData[i+HourTensStartIdx] = 0;
			} else {
				AppCfg.DisplayData[i+HourTensStartIdx] = BitSwapping(characters[time[0].hour_tens+ '0'][i]);
			}
			AppCfg.DisplayData[i + HourSinglesStartIdx] = BitSwapping(characters[time[0].hour_singles + '0'][i]);
			AppCfg.DisplayData[i + MinTensStartIdx] = BitSwapping(characters[time[0].min_tens + '0'][i]);
			AppCfg.DisplayData[i + MinSinglesStartIdx] = BitSwapping(characters[time[0].min_singles + '0'][i]);
			AppCfg.DisplayData[i + SecTensStartIdx] = BitSwapping(characters[time[0].sec_tens + '0'][i]);
			AppCfg.DisplayData[i + SecSinglesStartIdx] = BitSwapping(characters[time[0].sec_singles + '0'][i]);
		}
		AppCfg.DisplayData[HourMinDoubleDot] = 0x22;
		AppCfg.DisplayData[MinSecDoubleDot] = 0x22;

		SendFrameToDisplay();

		time[1]=time[0];
		AppCfg.FirstRun=0;
	}
	if(AppCfg.UpdateTime==1)
	{
		HAL_RTC_GetTime(&hrtc, &Time_Data, RTC_FORMAT_BIN);//read new time
		HAL_RTC_GetDate(&hrtc, &Date_Data, RTC_FORMAT_BIN); //rtcread_time(&time[0]);

		time[0].hour_tens=Time_Data.Hours / 10;
		time[0].hour_singles=Time_Data.Hours % 10;
		time[0].min_tens=Time_Data.Minutes / 10;
		time[0].min_singles=Time_Data.Minutes % 10;
		time[0].sec_tens=Time_Data.Seconds / 10;
		time[0].sec_singles=Time_Data.Seconds % 10;

		if(time[0].sec_singles!=time[1].sec_singles){
			AppCfg.TimeDiffIndicator[5]=1;
			for(uint8_t i=0;i<5;i++){
				AppCfg.NewTimeDataArray[i + 30] = BitSwapping(characters[time[0].sec_singles + '0'][i]);
			}
		}
		else {
			AppCfg.TimeDiffIndicator[5]=0;
		}
		if(time[0].sec_tens!=time[1].sec_tens){
			AppCfg.TimeDiffIndicator[4]=1;
			for(uint8_t i=0;i<5;i++){
				AppCfg.NewTimeDataArray[i + 24] = BitSwapping(characters[time[0].sec_tens + '0'][i]);
			}
		}
		else {
			AppCfg.TimeDiffIndicator[4]=0;
		}
		if(time[0].min_singles!=time[1].min_singles){
			AppCfg.TimeDiffIndicator[3]=1;
			for(uint8_t i=0;i<5;i++){
				AppCfg.NewTimeDataArray[i + 18] = BitSwapping(characters[time[0].min_singles + '0'][i]);
			}
		}
		else {
			AppCfg.TimeDiffIndicator[3]=0;
		}
		if(time[0].min_tens!=time[1].min_tens){
			AppCfg.TimeDiffIndicator[2]=1;
			for(uint8_t i=0;i<5;i++){
				AppCfg.NewTimeDataArray[i + 12] = BitSwapping(characters[time[0].min_tens + '0'][i]);
			}
		}
		else {
			AppCfg.TimeDiffIndicator[2]=0;
		}
		if(time[0].hour_singles!=time[1].hour_singles){
			AppCfg.TimeDiffIndicator[1]=1;
			for(uint8_t i=0;i<5;i++){
				AppCfg.NewTimeDataArray[i + 6] = BitSwapping(characters[time[0].hour_singles + '0'][i]);
			}
		}
		else {
			AppCfg.TimeDiffIndicator[1]=0;
		}
		if(time[0].hour_tens!=time[1].hour_tens){
			AppCfg.TimeDiffIndicator[0]=1;
			for(uint8_t i=0;i<5;i++){
				AppCfg.NewTimeDataArray[i] = BitSwapping(characters[time[0].hour_tens + '0'][i]);
			}
		}
		else {
			AppCfg.TimeDiffIndicator[0]=0;
		}
		time[1]=time[0];
		AppCfg.UpdateTime=0;
	}
	if(AppCfg.TimeAnimation==1){
		if (AppCfg.TimeDiffIndicator[0]){
			TimeAnimation(&AppCfg.DisplayData[HourTensStartIdx],&AppCfg.NewTimeDataArray[0]);
		}
		if (AppCfg.TimeDiffIndicator[1]){
			TimeAnimation(&AppCfg.DisplayData[HourSinglesStartIdx],&AppCfg.NewTimeDataArray[6]);
		}
		if (AppCfg.TimeDiffIndicator[2]){
			TimeAnimation(&AppCfg.DisplayData[MinTensStartIdx],&AppCfg.NewTimeDataArray[12]);
		}
		if(AppCfg.TimeDiffIndicator[3]){
			TimeAnimation(&AppCfg.DisplayData[MinSinglesStartIdx],&AppCfg.NewTimeDataArray[18]);
		}
		if(AppCfg.TimeDiffIndicator[4]){
			TimeAnimation(&AppCfg.DisplayData[SecTensStartIdx],&AppCfg.NewTimeDataArray[24]);
		}
		if(AppCfg.TimeDiffIndicator[5]){
			TimeAnimation(&AppCfg.DisplayData[SecSinglesStartIdx],&AppCfg.NewTimeDataArray[30]);
		}
		SendFrameToDisplay();
		AppCfg.FlipCounter++;
		if(AppCfg.FlipCounter==8){
			AppCfg.FlipCounter=0;
			AppCfg.TimeAnimation=0;
		}
	}
}
/**/
void CreateDisplayDataArray(void)
{
	AppCfg.TextScrolling = false;
	AppCfg.TextLength = strlen((const char*)AppCfg.DisplayTextArray);
	/*set to zero*/
	for(uint16_t k=0;k<1536;k++)
	{
	  AppCfg.DisplayDataArray[k]=0;
	}
	/**/
	uint16_t StartIndx;
	if(AppCfg.TextScrollingMode == true)
	{
		StartIndx = 96;
	}
	else
	{
		StartIndx = 0;
	}
	/*fill up with data*/
	for (uint8_t i=0; i < AppCfg.TextLength; i++)
	{
		for (uint8_t j = 0; j < 6; j++)
		{
		  AppCfg.DisplayDataArray[StartIndx] = BitSwapping(characters[AppCfg.DisplayTextArray[i]][j]);
		  StartIndx++;
		}
	}
	/**/
	if(AppCfg.TextScrollingMode == true)
	{
		AppCfg.LastColumn = StartIndx + 96;
	}
	else
	{
		AppCfg.LastColumn = StartIndx;
	}
	/**/
	AppCfg.FirstColumn = 0;
	AppCfg.TextScrollEnd = false;
	AppCfg.TextScrolling = true;
}
/**/
void SendToDisplay(uint16_t from)
{
#define DispCount 12//
  //
  uint8_t tmp[192];
  for(uint8_t i=0;i<8;i++){
	  for(uint8_t j=0;j<12;j++){
		  tmp[(i*24)+(2*j)]=8-i;
		  tmp[192-((i*24)+(2*j)+1)]=AppCfg.DisplayDataArray[from+(j*8)+i];
	  }
  }
  /**/
  SPI_Send(REG_SHTDWN, SHUTDOWN_MODE);
  for(uint8_t i=0;i<8;i++){
	  HAL_SPI_Transmit(&hspi2,&tmp[i*24],24,50);
		MAX7219_LoadPuse();
  }
  SPI_Send(REG_SHTDWN, NORMAL_MODE);
  asm("nop");
}
/**/
void MAX7219_Init(void)
{
	SPI_Send(REG_NO_OP, NOP);
	SPI_Send(REG_SHTDWN, SHUTDOWN_MODE);
	SPI_Send(REG_DECODE, NO_DECODE);
	SPI_Send(REG_SCANLIMIT, DISP0_7);
	SPI_Send(REG_INTENSITY, INTENSITY_7);
}
/**/
void MAX7219_LoadPuse(void)
{
	HAL_GPIO_WritePin(MAX7219_CS_PORT,MAX7219_CS_PIN,GPIO_PIN_RESET);
	for(uint8_t i=0;i<10;i++){
		asm("nop");
	}
	HAL_GPIO_WritePin(MAX7219_CS_PORT,MAX7219_CS_PIN,GPIO_PIN_SET);
}
/**/
void SPI_Send(uint8_t ADDR, uint8_t CMD)
{
	uint8_t tmp[24];
	for(uint8_t i=0;i<DispNum;i++){
		tmp[2*i]=ADDR;
		tmp[(2*i)+1]=CMD;
	}
	/**/
	HAL_SPI_Transmit(&hspi2,tmp,24,50);
	MAX7219_LoadPuse();
}
/**/
void SendFrameToDisplay(void)
{
	/*locals*/
	uint8_t tmp1[24];
	uint8_t tmp2[24];
	uint8_t tmp3[8][24];
	/**/
	for(uint8_t i=0;i<DispNum;i++){
		tmp1[2*i]=REG_SHTDWN;
		tmp1[(2*i)+1]=SHUTDOWN_MODE;
		tmp2[2*i]=REG_SHTDWN;
		tmp2[(2*i)+1]=NORMAL_MODE;
	}
	/**/
	for(uint8_t i=8;i>0;i--){
		for(uint8_t j=0;j<DispNum;j++){
			tmp3[i-1][2*j]=i;
			tmp3[i-1][(2*j)+1]=AppCfg.DisplayData[DispLength-(8*j)-9+i];
		}
	}
	/*Shutdown drivers*/
	HAL_SPI_Transmit(&hspi2,tmp1,24,100);
	MAX7219_LoadPuse();
	/*Send out data*/
	for(uint8_t i=8;i>0;i--){
		HAL_SPI_Transmit(&hspi2, &tmp3[i-1][0], 24, 100);
		MAX7219_LoadPuse();
	}
	/*Turn back on display*/
	HAL_SPI_Transmit(&hspi2,tmp2,24,100);
	MAX7219_LoadPuse();
}
/**/
uint8_t BitSwapping(uint8_t ch)
{
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
/**/
HAL_StatusTypeDef RTC_NTPSync(const uint8_t * SSID, const uint8_t * PassWord)
{
	RTC_DataType DateTime={0,0,0,0,0,0,0};
	RTC_TimeTypeDef HAL_Time={0,0,0,0,0,0,RTC_DAYLIGHTSAVING_NONE,RTC_STOREOPERATION_RESET};
	RTC_DateTypeDef HAL_Date={0,0,0,0};
	uint8_t Attempt=0;
#define NumberOfAttepmts 	3
	/*Try to connect AP*/
	while(1)
	{
		Attempt++;
		if(Attempt<NumberOfAttepmts)
		{
			if(ESP8266_NTP_Init(SSID, PassWord) == HAL_OK)
			{
				break;
			}
		}
		else
		{
			return HAL_ERROR;
		}
	}
	/*Try to get NTP packet*/
	Attempt=0;
	while(1)
	{
		Attempt++;
		if(Attempt<NumberOfAttepmts)
		{
			if(ESP8266_NTP_GetDateTime(&DateTime) == HAL_OK)
			{
				HAL_Time.Hours = DateTime.hour;
				HAL_Time.Minutes = DateTime.min;
				HAL_Time.Seconds = DateTime.sec;
				HAL_Date.Year = DateTime.year;
				HAL_Date.Month = DateTime.month;
				HAL_Date.Date = DateTime.date;
				HAL_Date.WeekDay = DateTime.day;
				if(HAL_RTC_SetTime(&hrtc,&HAL_Time,RTC_FORMAT_BIN) == HAL_OK)
				{
					if(HAL_RTC_SetDate(&hrtc,&HAL_Date,RTC_FORMAT_BIN) == HAL_OK)
					{
						return HAL_OK;
					}
				}
			}
		}
		else
		{
			return HAL_ERROR;
		}
	}
}
/**/
void RemoteXY_InitAndRun(void)
{
	/*defines*/
#define	RemoteXY_Timeout 15000
	/*locals*/
	uint32_t StartTime=HAL_GetTick();
	/**/
	ESP8266_RemoteXY_InitAndStart();
	/*wait for connection timeout: 30sec*/
	while((HAL_GetTick()-StartTime) <= RemoteXY_Timeout)
	{
		ESP8266_RemoteXY_Handler();
		if(ESP8266_RemoteXY_IsConnected() == 1)
		{
			break;
		}
	}
	/*while connected: run application*/
	while(ESP8266_RemoteXY_IsConnected() == 1)
	{
		/*run application handler*/
		ESP8266_RemoteXY_Handler();
		/*if app sent data process and store*/
		if(RemoteXY.button_1==1)
		{
			if (EE_WriteCharArray(VirtAddr_SSID, (uint8_t*)(RemoteXY.edit_1)) != EE_OK) {
				Error_Handler();
			}
			if (EE_WriteCharArray(VirtAddr_PassWord, (uint8_t*)(RemoteXY.edit_2)) != EE_OK) {
				Error_Handler();
			}
			RemoteXY.button_1=0;
		}
	}
}
/* Application Main Functions Start ---------------------------------------------------------*/
HAL_StatusTypeDef Init_Application(void)
{
	/**/
	MAX7219_Init();
	/**/
	HAL_FLASH_Unlock();
	if (EE_Init() != HAL_OK)
	{
		return HAL_ERROR;
	}
	/*RemoteXY*/
	//RemoteXY_InitAndRun();

	/*read eeprom SSID and PassWord*/
	EE_ReadCharArray(VirtAddr_SSID,(uint8_t*)(AppCfg.SSID));
	EE_ReadCharArray(VirtAddr_PassWord,(uint8_t*)(AppCfg.PassWord));
	//EE_ReadCharArray(VirtAddr_ScrollText, (uint8_t)AppCfg.ScrollText);

	/*RTC sync from NTP*/
	//if(RTC_NTPSync(AppCfg.SSID,AppCfg.PassWord) !=HAL_OK)
	{
		//HAL_NVIC_SystemReset();
	}
	/**/
	HAL_RTC_MspInit(&hrtc);
	/**/
	AppCfg.FirstRun=1;
	AppCfg.UpdateTime=0;
	AppCfg.TimeAnimation=0;
	AppCfg.FlipCounter=0;
	AppCfg.Point=false;
	AppCfg.ScrollDateSecCounter=0;
	AppCfg.ScrollTextSecCounter=0;
	AppCfg.DisplayMode=Time;
	AppCfg.ScrollDateIntervalInSec=15;
	AppCfg.ScrollTextIntervalInSec=10;
	AppCfg.TextScrollingMode=true;
	strcpy((char*)AppCfg.ScrollText, "Ez egy futószöveg reklám: 6041, Kerekegyháza Tavasz u. 25.");
	/**/
	HAL_TIM_Base_Start_IT(&htim3);
	HAL_TIM_Base_Start_IT(&htim4);
	__HAL_RTC_EXTI_ENABLE_IT(RTC_IT_ALRA);
	return HAL_OK;
}
/**/
void Run_Application(void)
{
	while(1)
	{

	}
}
/* Application Main Functions End ---------------------------------------------------------*/
/* Interrupt Callbacks Start ---------------------------------------------------------*/
/**/
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	if(AppCfg.DisplayMode==Time)
	{
		AppCfg.Point=!AppCfg.Point;
		AppCfg.UpdateTime=1;
		AppCfg.TimeAnimation=1;
		AppCfg.ScrollDateSecCounter++;
		AppCfg.ScrollTextSecCounter++;
		if(AppCfg.ScrollDateSecCounter == AppCfg.ScrollDateIntervalInSec)
		{
			CreateDateData();
			CreateDisplayDataArray();
			AppCfg.DisplayMode=Date;
			AppCfg.ScrollDateSecCounter=0;
		}
		if(AppCfg.ScrollTextIntervalInSec == AppCfg.ScrollTextSecCounter)
		{
			strcpy((char*)AppCfg.DisplayTextArray,(char*)AppCfg.ScrollText);
			CreateDisplayDataArray();
			AppCfg.DisplayMode=Text;
			AppCfg.ScrollTextSecCounter=0;
		}
	}
	if(AppCfg.DisplayMode == Text)
	{

	}
	if(AppCfg.DisplayMode == Date)
	{

	}

	if(AppCfg.DisplayDateDone == true)
	{
		AppCfg.DisplayMode = Time;
		AppCfg.FirstRun = 1;
		time_out();
		AppCfg.DisplayDateDone = false;
	}

	if(AppCfg.DisplayTextDone == true)
	{
		AppCfg.DisplayMode = Time;
		AppCfg.FirstRun = 1;
		time_out();
		AppCfg.DisplayTextDone = false;
	}
}
/**/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	UNUSED(htim);
	/*timer3 interrupt*/
	if(htim->Instance==TIM3)
	{
		/**/
		if(AppCfg.DisplayMode == Time)
		{
			time_out();
		}
		/**/
		if((AppCfg.DisplayMode == Text) || (AppCfg.DisplayMode == Date) )
		{
			if (AppCfg.TextScrolling)
			{
				if (AppCfg.FirstColumn == (AppCfg.LastColumn - 96))
				{
					AppCfg.TextScrolling = false;
					/**/
					if(AppCfg.DisplayMode == Text)
					{
						AppCfg.DisplayTextDone = true;
					}
					/**/
					if(AppCfg.DisplayMode == Text)
					{
						AppCfg.DisplayDateDone = true;
					}
				}
				else
				{
					SendToDisplay(AppCfg.FirstColumn);
					AppCfg.FirstColumn++;
				}
			}
		}
	}
	/*timer4 interrupt*/
	if(htim->Instance==TIM4)
	{

	}
}
/**/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART2)
	{
		//HAL_UART_Receive_IT(&huart2,UartBuff,5);
	}
}
/**/
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	if(huart->ErrorCode == HAL_UART_ERROR_ORE)
	{
		//HAL_UART_Receive_IT(&huart2,UartBuff,5);
	}
}
/**/
void HAL_SYSTICK_Callback(void)
{
}
/* Interrupt Callbacks End ---------------------------------------------------------*/
