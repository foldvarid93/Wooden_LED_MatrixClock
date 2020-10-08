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
	for(uint8_t j=0;DateText[j]!='\0';i++,j++)
	{
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
	AppCfg.DisplayTextArray[i]='\0';			//lez?r? nulla
}
/*********************************/				//Date functions end
/*********************************/				//Time functions begin
/**/
void Rotate(uint8_t* Dest,uint8_t* Source)
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
void UpdateTimeOnDisplay(void)
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
	if(AppCfg.FirstRun == 1)
	{
		AppCfg.UpdateTime = 1;
	}
	if(AppCfg.UpdateTime == 1)
	{
		HAL_RTC_GetTime(&hrtc, &Time_Data, RTC_FORMAT_BIN);//read new time
		HAL_RTC_GetDate(&hrtc, &Date_Data, RTC_FORMAT_BIN); //rtcread_time(&time[0]);

		time[0].hour_tens=Time_Data.Hours / 10;
		time[0].hour_singles=Time_Data.Hours % 10;
		time[0].min_tens=Time_Data.Minutes / 10;
		time[0].min_singles=Time_Data.Minutes % 10;
		time[0].sec_tens=Time_Data.Seconds / 10;
		time[0].sec_singles=Time_Data.Seconds % 10;
		/**/
		if((AppCfg.TimeAnimation == 0)||(AppCfg.FirstRun == 1))
		{
			for (uint8_t i = 0; i < 5; i++)
			{
				if (time[0].hour_tens == 0)
				{
					AppCfg.DisplayData[i+HourTensStartIdx] = 0;
				}
				else
				{
					AppCfg.DisplayData[i+HourTensStartIdx] = BitSwapping(characters[time[0].hour_tens+ '0'][i]);
				}
				AppCfg.DisplayData[i + HourSinglesStartIdx] = BitSwapping(characters[time[0].hour_singles + '0'][i]);
				AppCfg.DisplayData[i + MinTensStartIdx] = BitSwapping(characters[time[0].min_tens + '0'][i]);
				AppCfg.DisplayData[i + MinSinglesStartIdx] = BitSwapping(characters[time[0].min_singles + '0'][i]);
				AppCfg.DisplayData[i + SecTensStartIdx] = BitSwapping(characters[time[0].sec_tens + '0'][i]);
				AppCfg.DisplayData[i + SecSinglesStartIdx] = BitSwapping(characters[time[0].sec_singles + '0'][i]);
			}
			/**/
			if(AppCfg.FirstRun == 1)
			{
				AppCfg.Point = 1;
			}
			else
			{
				AppCfg.Point=!AppCfg.Point;
			}
			if(AppCfg.Point == 1)
			{
				AppCfg.DisplayData[HourMinDoubleDot] = 0x22;
				AppCfg.DisplayData[MinSecDoubleDot] = 0x22;
			}
			else
			{
				AppCfg.DisplayData[HourMinDoubleDot] = 0;
				AppCfg.DisplayData[MinSecDoubleDot] = 0;
			}
			/**/
			SendTimeToDisplay();
			/**/
			if(AppCfg.FirstRun == 1)
			{
				AppCfg.FirstRun = 0;
			}
		}
		/**/
		else
		{
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
			/**/
			AppCfg.DisplayData[HourMinDoubleDot] = 0x22;
			AppCfg.DisplayData[MinSecDoubleDot] = 0x22;
			AppCfg.FlipCounter = 0;
		}
		time[1]=time[0];
		AppCfg.UpdateTime = 0;
	}
	/**/
	if( (AppCfg.TimeAnimation == 1) && (AppCfg.FlipCounter < 8) )
	{
		if (AppCfg.TimeDiffIndicator[0]){
			Rotate(&AppCfg.DisplayData[HourTensStartIdx],&AppCfg.NewTimeDataArray[0]);
		}
		if (AppCfg.TimeDiffIndicator[1]){
			Rotate(&AppCfg.DisplayData[HourSinglesStartIdx],&AppCfg.NewTimeDataArray[6]);
		}
		if (AppCfg.TimeDiffIndicator[2]){
			Rotate(&AppCfg.DisplayData[MinTensStartIdx],&AppCfg.NewTimeDataArray[12]);
		}
		if(AppCfg.TimeDiffIndicator[3]){
			Rotate(&AppCfg.DisplayData[MinSinglesStartIdx],&AppCfg.NewTimeDataArray[18]);
		}
		if(AppCfg.TimeDiffIndicator[4]){
			Rotate(&AppCfg.DisplayData[SecTensStartIdx],&AppCfg.NewTimeDataArray[24]);
		}
		if(AppCfg.TimeDiffIndicator[5]){
			Rotate(&AppCfg.DisplayData[SecSinglesStartIdx],&AppCfg.NewTimeDataArray[30]);
		}
		SendTimeToDisplay();
		AppCfg.FlipCounter ++;
	}
}
/**/
void TextToColumnDataArray(void)
{
	/*locals*/
	uint16_t StartIndx;
	AppCfg.TextLength = strlen((const char*)AppCfg.DisplayTextArray);
	/**/
	if((AppCfg.TextLength < SizeOf_WhiteSpaces) && (AppCfg.ScrollingMode == JustText))
	{
		AppCfg.TextScrolling = false;
	}
	else
	{
		AppCfg.TextScrolling = true;
	}
	/**/
	if(AppCfg.TextScrolling == true)
	{
		if(AppCfg.ScrollingMode == WallToWall)
		{
			for(uint8_t i=0;i<NumberOf_DisplayColumn;i++)
			{
				AppCfg.DisplayTextColumnArray[i]=0;
			}
			StartIndx = NumberOf_DisplayColumn;
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
			  AppCfg.DisplayTextColumnArray[StartIndx] = BitSwapping(characters[AppCfg.DisplayTextArray[i]][j]);
			  StartIndx++;
			}
		}
		/**/
		if(AppCfg.ScrollingMode == WallToWall)
		{
			for(uint8_t i=0;i<NumberOf_DisplayColumn;i++)
			{
				AppCfg.DisplayTextColumnArray[StartIndx+i]=0;
			}
			AppCfg.LastColumn = StartIndx + NumberOf_DisplayColumn;
		}
		else
		{
			AppCfg.LastColumn = StartIndx;
			AppCfg.TimeStamp = HAL_GetTick();
		}
	}
	else
	{
		for(uint8_t i=0;i<NumberOf_DisplayColumn;i++)
		{
			AppCfg.DisplayTextColumnArray[i]=0;
		}
		/*fill up with data*/
		StartIndx = 0;
		for (uint8_t i=0; i < AppCfg.TextLength; i++)
		{
			for (uint8_t j = 0; j < 6; j++)
			{
			  AppCfg.DisplayTextColumnArray[StartIndx] = BitSwapping(characters[AppCfg.DisplayTextArray[i]][j]);
			  StartIndx++;
			}
		}
		AppCfg.TimeStamp = HAL_GetTick();
	}

	/**/
	AppCfg.FirstColumn = 0;
}
/**/
void SendToDisplay(uint16_t from)
{
  /**/
  uint8_t tmp[192];
  for(uint8_t i=0;i<8;i++){
	  for(uint8_t j=0;j<NumberOf_Display;j++){
		  tmp[(i*24)+(2*j)]=8-i;
		  tmp[192-((i*24)+(2*j)+1)]=AppCfg.DisplayTextColumnArray[from+(j*8)+i];
	  }
  }
  /**/
  MAX7219_Send(REG_SHTDWN, SHUTDOWN_MODE);
  /**/
  for(uint8_t i=0;i<8;i++){
	  HAL_SPI_Transmit(&hspi2,&tmp[i*24],24,50);
		MAX7219_LoadPuse();
  }
  /**/
  MAX7219_Send(REG_SHTDWN, NORMAL_MODE);
  asm("nop");
}
/**/
void MAX7219_Init(void)
{
	MAX7219_Send(REG_NO_OP, NOP);
	MAX7219_Send(REG_SHTDWN, SHUTDOWN_MODE);
	MAX7219_Send(REG_DECODE, NO_DECODE);
	MAX7219_Send(REG_SCANLIMIT, DISP0_7);
	MAX7219_Send(REG_INTENSITY, INTENSITY_7);
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
void MAX7219_Send(uint8_t ADDR, uint8_t CMD)
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
void SendTimeToDisplay(void)
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
	for(uint8_t i=NumberOf_ColumnOfOneDisplay ; i>0 ; i--)
	{
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
				HAL_NVIC_DisableIRQ(RTC_Alarm_IRQn);
				if(HAL_RTC_SetTime(&hrtc,&HAL_Time,RTC_FORMAT_BIN) == HAL_OK)
				{
					if(HAL_RTC_SetDate(&hrtc,&HAL_Date,RTC_FORMAT_BIN) == HAL_OK)
					{
						HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
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
HAL_StatusTypeDef ESP8266_AccessPoint_InitAndRun(void){
	/*ESP8266 HW reset and enabling*/
	HAL_GPIO_WritePin(ESP8266_RST_GPIO_Port, ESP8266_RST_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ESP8266_EN_GPIO_Port, ESP8266_EN_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ESP8266_EN_GPIO_Port, ESP8266_EN_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(ESP8266_RST_GPIO_Port, ESP8266_RST_Pin, GPIO_PIN_SET);
	HAL_Delay(1000);
	/**/
	Ringbuf_init();
	/**/
	ESP8266_Serial_Init();
	/**/
	if(ESP8266_NTP_ATCommand("AT+RESTORE", "ready", LONG_PAUSE) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_NTP_ATCommand("AT", OK_STR, SHORT_PAUSE) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_NTP_ATCommand("ATE0", OK_STR, SHORT_PAUSE) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_NTP_ATCommand("AT+CWMODE=3", OK_STR, SHORT_PAUSE) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_NTP_ATCommand("AT+CIPMUX=1", OK_STR, SHORT_PAUSE) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_NTP_ATCommand("AT+CIPSERVER=1,23", OK_STR, SHORT_PAUSE) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_NTP_ATCommand("AT+CWSAP=\"ESP\",\"password\",1,4", OK_STR,10000) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	while(1){
		if(ESP8266_NTP_ATCommand("AT+CIPSEND=0,10", OK_STR,10000) != HAL_OK)
		{
			  return HAL_ERROR;
		}
	}
	return HAL_OK;
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
		if(RemoteXY.Btn_SSID_Send == 1)
		{
			if (EE_WriteCharArray(VirtAddr_SSID, (uint8_t*)(RemoteXY.TextBox_SSID)) != EE_OK) {
				Error_Handler();
			}

			if (EE_WriteCharArray(VirtAddr_PassWord, (uint8_t*)(RemoteXY.TextBox_PassWord)) != EE_OK) {
				Error_Handler();
			}
			RemoteXY.Btn_SSID_Send = 0;
		}
	}
}
/**/
void EEPROM_WriteFrame(void)
{
	//
	sprintf((char*)AppCfg.SSID,"foldvarid93");
	sprintf((char*)AppCfg.PassWord,"19701971");
	sprintf((char*)AppCfg.ScrollText,"6041, Kerekegyháza Tavasz u. 25.");
	AppCfg.TimeAnimation = 1;
	AppCfg.ScrollDateIntervalInSec = 10;
	AppCfg.ScrollTextIntervalInSec = 15;
	AppCfg.TextScrollingMode=JustText;
	AppCfg.DateScrollingMode=WallToWall;
	//
	EE_WriteCharArray(VirtAddr_SSID, (uint8_t*)(AppCfg.SSID));
	EE_WriteCharArray(VirtAddr_PassWord, (uint8_t*)(AppCfg.PassWord));
	EE_WriteCharArray(VirtAddr_ScrollText, (uint8_t*)(AppCfg.ScrollText));
	//
	EE_WriteVariable(VirtAddr_ScrollTextIntervalInSec, AppCfg.ScrollTextIntervalInSec);
	EE_WriteVariable(VirtAddr_ScrollDateIntervalInSec, AppCfg.ScrollDateIntervalInSec);
	//
	EE_WriteVariable(VirtAddr_TimeAnimation, AppCfg.TimeAnimation);
	EE_WriteVariable(VirtAddr_TextScrollingMode, AppCfg.TextScrollingMode);
	EE_WriteVariable(VirtAddr_DateScrollingMode, AppCfg.DateScrollingMode);
}
/**/
void EEPROM_ReadFrame(void)
{
	EE_ReadCharArray(VirtAddr_SSID,(uint8_t*)(AppCfg.SSID));
	EE_ReadCharArray(VirtAddr_PassWord,(uint8_t*)(AppCfg.PassWord));
	EE_ReadCharArray(VirtAddr_ScrollText,(uint8_t*)(AppCfg.ScrollText));
	//
	EE_ReadVariable(VirtAddr_ScrollTextIntervalInSec,&(AppCfg.ScrollTextIntervalInSec));
	EE_ReadVariable(VirtAddr_ScrollDateIntervalInSec, &(AppCfg.ScrollDateIntervalInSec));
	//
	EE_ReadVariable(VirtAddr_TimeAnimation, &(AppCfg.TimeAnimation));
	EE_ReadVariable(VirtAddr_TextScrollingMode, &(AppCfg.TextScrollingMode));
	EE_ReadVariable(VirtAddr_DateScrollingMode, &(AppCfg.DateScrollingMode));
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

	/*write eeprom*/
	//EEPROM_Write();

	/*read eeprom SSID and PassWord*/
	EEPROM_ReadFrame();
	/**/
	AppCfg.FirstRun=1;
	AppCfg.UpdateTime=0;
	AppCfg.FlipCounter=0;
	AppCfg.Point=false;
	AppCfg.DisplayMode=Time;
	AppCfg.LastScrolled=Date;
	AppCfg.ScrollSecCounter=0;
	/**/
	HAL_TIM_Base_Start_IT(&htim3);
	HAL_TIM_Base_Start_IT(&htim4);
	__HAL_RTC_EXTI_ENABLE_IT(RTC_IT_ALRA);
	/*RTC sync from NTP*/
	if(RTC_NTPSync(AppCfg.SSID,AppCfg.PassWord) !=HAL_OK)
	{
		//HAL_NVIC_SystemReset();
	}
	/**/
	//ESP8266_AccessPoint_InitAndRun();
	/**/
	return HAL_OK;
}
/**/
void Run_Application(void)
{
	ESP8266_RemoteXY_InitAndStart();
	while(1)
	{
		while(1)
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
			if(RemoteXY.Btn_SSID_Send == 1)
			{
				if (EE_WriteCharArray(VirtAddr_SSID, (uint8_t*)(RemoteXY.TextBox_SSID)) != EE_OK) {
					Error_Handler();
				}

				if (EE_WriteCharArray(VirtAddr_PassWord, (uint8_t*)(RemoteXY.TextBox_PassWord)) != EE_OK) {
					Error_Handler();
				}
				RemoteXY.Btn_SSID_Send = 0;
			}
		}

	}
}
/* Application Main Functions End ---------------------------------------------------------*/
/* Interrupt Callbacks Start ---------------------------------------------------------*/
/**/
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	if(AppCfg.DisplayMode == Time)
	{
		AppCfg.UpdateTime=1;
		/**/
		if(AppCfg.ScrollSecCounter >= AppCfg.ScrollTextIntervalInSec)
		{
			if(AppCfg.LastScrolled == Date)
			{
				AppCfg.ScrollingMode = AppCfg.TextScrollingMode;
				strcpy((char*)AppCfg.DisplayTextArray,(char*)AppCfg.ScrollText);
				TextToColumnDataArray();
				AppCfg.LastScrolled = Text;
			}
			else
			{
				AppCfg.ScrollingMode = AppCfg.DateScrollingMode;
				CreateDateData();
				TextToColumnDataArray();
				AppCfg.LastScrolled = Date;
			}
			AppCfg.ScrollSecCounter = 0;
			AppCfg.DisplayMode = Text;
		}
/*		if((AppCfg.LastScrolled == Date) && (AppCfg.ScrollSecCounter == AppCfg.ScrollTextIntervalInSec))
		{
			AppCfg.ScrollingMode = AppCfg.TextScrollingMode;
			strcpy((char*)AppCfg.DisplayTextArray,(char*)AppCfg.ScrollText);
			TextToColumnDataArray();
			AppCfg.DisplayMode = Text;
			AppCfg.ScrollSecCounter = 0;
		}
		if((AppCfg.LastScrolled == Text) && (AppCfg.ScrollSecCounter == AppCfg.ScrollDateIntervalInSec))
		{
			AppCfg.ScrollingMode = AppCfg.DateScrollingMode;
			CreateDateData();
			TextToColumnDataArray();
			AppCfg.DisplayMode = Date;
			AppCfg.ScrollSecCounter = 0;
		}
		*/
		AppCfg.ScrollSecCounter++;
	}
	/**/
	if(AppCfg.DisplayMode == Text)
	{
	}
	/**/
	if(AppCfg.DisplayMode == Date)
	{
	}
	/**/
	if(AppCfg.DisplayMode == TextDone)
	{
		AppCfg.DisplayMode = Time;
		AppCfg.FirstRun = 1;
		UpdateTimeOnDisplay();
	}
}
/**/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	UNUSED(htim);
	/*timer3 interrupt*/
	if(htim->Instance == TIM3)
	{
		/**/
		if(AppCfg.DisplayMode == Time)
		{
			UpdateTimeOnDisplay();
		}
		/**/
		if(AppCfg.DisplayMode == Text)
		{
			/*scrolling text on display*/
			if (AppCfg.TextScrolling == true)
			{
				if (AppCfg.FirstColumn == (AppCfg.LastColumn - 96))
				{
					/**/
					if(AppCfg.ScrollingMode == JustText)
					{
						if((HAL_GetTick()-AppCfg.TimeStamp)<1500)
						{
							/*wait*/
						}
						else
						{
							AppCfg.DisplayMode = TextDone;
						}

					}
					else
					{
						AppCfg.DisplayMode = TextDone;
					}
				}
				else
				{
					SendToDisplay(AppCfg.FirstColumn);
					if((AppCfg.ScrollingMode == JustText)&&(AppCfg.FirstColumn == 0))
					{
						if((HAL_GetTick()-AppCfg.TimeStamp)<1500)
						{
							/*wait*/
						}
						else
						{
							AppCfg.FirstColumn++;
						}

					}
					else{
						AppCfg.FirstColumn++;
						AppCfg.TimeStamp = HAL_GetTick();
					}
				}
			}
			/*static text on display*/
			else
			{
				SendToDisplay(AppCfg.FirstColumn);
				if((HAL_GetTick() - AppCfg.TimeStamp)<3000)
				{
					/*wait*/
				}
				else
				{
					AppCfg.DisplayMode = TextDone;
				}
			}
		}
	}
	/*timer4 interrupt*/
	if(htim->Instance == TIM4)
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
