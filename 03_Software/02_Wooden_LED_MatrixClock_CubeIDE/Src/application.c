/*********************************///
#include "application.h"
/*********************************///
//Flash variables
bool FlashWriteEnabled=true;
uint16_t VirtAddVarTab;//[NB_OF_VAR] = {0x0001};
extern uint8_t HTML_Message[SizeOf_HTML_Message];
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
/*********************************/
/**/
void CreateDateData(void)
{
	uint8_t	i=0;
	RTC_TimeTypeDef	 		Time_Data;
	RTC_DateTypeDef			Date_Data;
	/**/
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
	/*locals*/
	RTC_TimeTypeDef	 		Time_Data;
	RTC_DateTypeDef			Date_Data;
	/**/
	if(AppCfg.FirstRun == 1)
	{
		AppCfg.UpdateTime = 1;
	}
	if(AppCfg.UpdateTime == 1)
	{
		HAL_RTC_GetTime(&hrtc, &Time_Data, RTC_FORMAT_BIN);//read new time
		HAL_RTC_GetDate(&hrtc, &Date_Data, RTC_FORMAT_BIN); //rtcread_time(&_time[0]);

		_time[0].hour_tens=Time_Data.Hours / 10;
		_time[0].hour_singles=Time_Data.Hours % 10;
		_time[0].min_tens=Time_Data.Minutes / 10;
		_time[0].min_singles=Time_Data.Minutes % 10;
		_time[0].sec_tens=Time_Data.Seconds / 10;
		_time[0].sec_singles=Time_Data.Seconds % 10;
		/**/
		if((AppCfg.TimeAnimation == 0)||(AppCfg.FirstRun == 1))
		{
			for (uint8_t i = 0; i < 5; i++)
			{
				if (_time[0].hour_tens == 0)
				{
					AppCfg.DisplayData[i+HourTensStartIdx] = 0;
				}
				else
				{
					AppCfg.DisplayData[i+HourTensStartIdx] = BitSwapping(characters[_time[0].hour_tens+ '0'][i]);
				}
				AppCfg.DisplayData[i + HourSinglesStartIdx] = BitSwapping(characters[_time[0].hour_singles + '0'][i]);
				AppCfg.DisplayData[i + MinTensStartIdx] = BitSwapping(characters[_time[0].min_tens + '0'][i]);
				AppCfg.DisplayData[i + MinSinglesStartIdx] = BitSwapping(characters[_time[0].min_singles + '0'][i]);
				AppCfg.DisplayData[i + SecTensStartIdx] = BitSwapping(characters[_time[0].sec_tens + '0'][i]);
				AppCfg.DisplayData[i + SecSinglesStartIdx] = BitSwapping(characters[_time[0].sec_singles + '0'][i]);
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
			if(_time[0].sec_singles!=_time[1].sec_singles){
				AppCfg.TimeDiffIndicator[5]=1;
				for(uint8_t i=0;i<5;i++){
					AppCfg.NewTimeDataArray[i + 30] = BitSwapping(characters[_time[0].sec_singles + '0'][i]);
				}
			}
			else {
				AppCfg.TimeDiffIndicator[5]=0;
			}
			if(_time[0].sec_tens!=_time[1].sec_tens){
				AppCfg.TimeDiffIndicator[4]=1;
				for(uint8_t i=0;i<5;i++){
					AppCfg.NewTimeDataArray[i + 24] = BitSwapping(characters[_time[0].sec_tens + '0'][i]);
				}
			}
			else {
				AppCfg.TimeDiffIndicator[4]=0;
			}
			if(_time[0].min_singles!=_time[1].min_singles){
				AppCfg.TimeDiffIndicator[3]=1;
				for(uint8_t i=0;i<5;i++){
					AppCfg.NewTimeDataArray[i + 18] = BitSwapping(characters[_time[0].min_singles + '0'][i]);
				}
			}
			else {
				AppCfg.TimeDiffIndicator[3]=0;
			}
			if(_time[0].min_tens!=_time[1].min_tens){
				AppCfg.TimeDiffIndicator[2]=1;
				for(uint8_t i=0;i<5;i++){
					AppCfg.NewTimeDataArray[i + 12] = BitSwapping(characters[_time[0].min_tens + '0'][i]);
				}
			}
			else {
				AppCfg.TimeDiffIndicator[2]=0;
			}
			if(_time[0].hour_singles!=_time[1].hour_singles){
				AppCfg.TimeDiffIndicator[1]=1;
				for(uint8_t i=0;i<5;i++){
					AppCfg.NewTimeDataArray[i + 6] = BitSwapping(characters[_time[0].hour_singles + '0'][i]);
				}
			}
			else {
				AppCfg.TimeDiffIndicator[1]=0;
			}
			if(_time[0].hour_tens!=_time[1].hour_tens){
				AppCfg.TimeDiffIndicator[0]=1;
				for(uint8_t i=0;i<5;i++){
					AppCfg.NewTimeDataArray[i] = BitSwapping(characters[_time[0].hour_tens + '0'][i]);
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
		_time[1]=_time[0];
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
	if((AppCfg.TextLength < SizeOf_WhiteSpaces) && (AppCfg.ScrollingMode == TM_MessageScroll))
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
		if(AppCfg.ScrollingMode == TM_MessageScrollInAndOut)
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
		if(AppCfg.ScrollingMode == TM_MessageScrollInAndOut)
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
	  MAX7219_LoadPulse();
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
void MAX7219_LoadPulse(void)
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
	for(uint8_t i=0;i<NumberOf_Display;i++){
		tmp[2*i]=ADDR;
		tmp[(2*i)+1]=CMD;
	}
	/**/
	HAL_SPI_Transmit(&hspi2,tmp,24,50);
	MAX7219_LoadPulse();
}
/**/
void SendTimeToDisplay(void)
{
	/*locals*/
	uint8_t tmp1[24];
	uint8_t tmp2[24];
	uint8_t tmp3[8][24];
	/**/
	for(uint8_t i=0;i<NumberOf_Display;i++)
	{
		tmp1[2*i]=REG_SHTDWN;
		tmp1[(2*i)+1]=SHUTDOWN_MODE;
		tmp2[2*i]=REG_SHTDWN;
		tmp2[(2*i)+1]=NORMAL_MODE;
	}
	/**/
	for(uint8_t i=8;i>0;i--)
	{
		for(uint8_t j=0;j<NumberOf_Display;j++)
		{
			tmp3[i-1][2*j]=i;
			tmp3[i-1][(2*j)+1]=AppCfg.DisplayData[NumberOf_DisplayColumn-(8*j)-9+i];
		}
	}
	/*Shutdown drivers*/
	HAL_SPI_Transmit(&hspi2,tmp1,24,100);
	MAX7219_LoadPulse();
	/*Send out data*/
	for(uint8_t i=NumberOf_ColumnOfOneDisplay ; i>0 ; i--)
	{
		HAL_SPI_Transmit(&hspi2, &tmp3[i-1][0], 24, 100);
		MAX7219_LoadPulse();
	}
	/*Turn back on display*/
	HAL_SPI_Transmit(&hspi2,tmp2,24,100);
	MAX7219_LoadPulse();
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
void StateMachine(void)
{
	if((AppCfg.Date_Enabled == false) && (AppCfg.Text_Enabled == false))
	{
		//AppCfg.SM_AppStatus = AS_Time;
		AppCfg.SM_NextState = AS_Time;
	}
	/**/
	if(AppCfg.SM_NextState == AS_Time)
	{
		AppCfg.SM_NextState = AS_None;//none
		AppCfg.SM_AppStatus = AS_Time;
	}
	if(AppCfg.SM_NextState == AS_Text)
	{
		AppCfg.SM_NextState = AS_None;//none
		AppCfg.SM_AppStatus = AS_Text;
	}
	if(AppCfg.SM_NextState == AS_Date)
	{
		AppCfg.SM_NextState = AS_None;//none
		AppCfg.SM_AppStatus = AS_Date;
	}
	/**/
	switch (AppCfg.SM_AppStatus)
	{
	case AS_None:
		{
			break;
		}
	case AS_Time:
		{
			if((AppCfg.Date_Enabled == true) && (AppCfg.Text_Enabled == false))
			{
				AppCfg.RTCIntSecCounter++;
				if((AppCfg.RTCIntSecCounter - AppCfg.Date_LastTimeStamp) >= AppCfg.Date_ScrollIntervalInSec)
				{
					AppCfg.SM_AppStatus = AS_Date;
				}
			}
			if((AppCfg.Date_Enabled == false) && (AppCfg.Text_Enabled == true))
			{
				AppCfg.RTCIntSecCounter++;
				if((AppCfg.RTCIntSecCounter - AppCfg.Text_LastTimeStamp) >= AppCfg.Text_ScrollIntervalInSec)
				{
					AppCfg.SM_AppStatus = AS_Text;
				}
			}
			if( (AppCfg.Date_Enabled == true) && (AppCfg.Text_Enabled == true) )
			{
				AppCfg.RTCIntSecCounter++;
				if(AppCfg.LastScrolled == AS_Text)
				{
					if((AppCfg.RTCIntSecCounter - AppCfg.Date_LastTimeStamp) >= AppCfg.Date_ScrollIntervalInSec)
					{
						AppCfg.SM_AppStatus = AS_Date;
					}
				}
				if(AppCfg.LastScrolled == AS_Date)
				{
					if((AppCfg.RTCIntSecCounter - AppCfg.Text_LastTimeStamp) >= AppCfg.Text_ScrollIntervalInSec)
					{
						AppCfg.SM_AppStatus = AS_Text;
					}
				}
			}
			AppCfg.UpdateTime=1;
			AppCfg.DisplayMode = AS_Time;
			break;
		}
	case AS_Date:
		{
			AppCfg.ScrollingMode = AppCfg.Date_ScrollingMode;
			CreateDateData();
			TextToColumnDataArray();
			AppCfg.LastScrolled = AS_Date;
			AppCfg.SM_AppStatus = AS_TextRunning;
			AppCfg.DisplayMode = AS_Text;
			break;
		}
	case AS_Text:
		{
			AppCfg.ScrollingMode = AppCfg.Text_ScrollingMode;
			strcpy((char*)AppCfg.DisplayTextArray,(char*)AppCfg.Text_Message);
			TextToColumnDataArray();
			AppCfg.LastScrolled = AS_Text;
			AppCfg.SM_AppStatus = AS_TextRunning;
			AppCfg.DisplayMode = AS_Text;
			break;
		}
	case AS_TextRunning:
		{

			break;
		}
	case AS_TextDone:
		{
			AppCfg.Text_LastTimeStamp = AppCfg.RTCIntSecCounter;
			AppCfg.Date_LastTimeStamp = AppCfg.RTCIntSecCounter;
			AppCfg.FirstRun = 1;
			UpdateTimeOnDisplay();
			AppCfg.SM_AppStatus = AS_Time;
			AppCfg.DisplayMode = AS_Time;
			break;
		}

	}
	/**/
	if(AppCfg.NTP_SyncEnabled)
	{
		if(AppCfg.NTP_SyncTimerCounter == 0)
		{
			AppCfg.SM_WiFiStatus = WiFi_NTP;
			AppCfg.NTP_SyncTimerCounter =  (AppCfg.NTP_SyncInterval * NUMBEROFSECONDS_HOUR);
		}
		else
		{
			AppCfg.NTP_SyncTimerCounter--;
		}
	}
}
/**/
void EEPROM_WriteFrame(void)
{
	/*NTP*/
	EE_WriteCharArray(VA_NTP_SSID, AppCfg.NTP_SSID);
	EE_WriteCharArray(VA_NTP_PassWord, AppCfg.NTP_PassWord);
	EE_WriteVariable(VA_NTP_SyncEnabled, (AppCfg.NTP_SyncEnabled));
	EE_WriteVariable(VA_NTP_SyncInterval, (AppCfg.NTP_SyncInterval));
	/*Text*/
	EE_WriteCharArray(VA_Text_Message, AppCfg.Text_Message);
	EE_WriteVariable(VA_Text_Enabled, AppCfg.Text_Enabled);
	EE_WriteVariable(VA_Text_ScrollingMode, AppCfg.Text_ScrollingMode);
	EE_WriteVariable(VA_Text_ScrollIntervalInSec, AppCfg.Text_ScrollIntervalInSec);
	/*Date*/
	EE_WriteVariable(VA_Date_Enabled, AppCfg.Date_Enabled);
	EE_WriteVariable(VA_Date_ScrollingMode, AppCfg.Date_ScrollingMode);
	EE_WriteVariable(VA_Date_ScrollIntervalInSec, AppCfg.Date_ScrollIntervalInSec);
	/*Other*/
	EE_WriteVariable(VA_TimeAnimation, AppCfg.TimeAnimation);
	EE_WriteVariable(VA_DisplayBrightnessMode, AppCfg.DisplayBrightnessMode);
	EE_WriteVariable(VA_DisplayBrightness, AppCfg.DisplayBrightness);
}
/**/
void EEPROM_ReadFrame(void)
{
	/*NTP*/
	EE_ReadCharArray(VA_NTP_SSID, (uint8_t*)(AppCfg.NTP_SSID));
	EE_ReadCharArray(VA_NTP_PassWord, (uint8_t*)(AppCfg.NTP_PassWord));
	EE_ReadVariable(VA_NTP_SyncEnabled, &(AppCfg.NTP_SyncEnabled));
	EE_ReadVariable(VA_NTP_SyncInterval, &(AppCfg.NTP_SyncInterval));
	/*Text*/
	EE_ReadCharArray(VA_Text_Message, (uint8_t*)(AppCfg.Text_Message));
	EE_ReadVariable(VA_Text_Enabled, &(AppCfg.Text_Enabled));
	EE_ReadVariable(VA_Text_ScrollingMode, (uint16_t*)(AppCfg.Text_ScrollingMode));
	EE_ReadVariable(VA_Text_ScrollIntervalInSec, &(AppCfg.Text_ScrollIntervalInSec));
	/*Date*/
	EE_ReadVariable(VA_Date_Enabled, &(AppCfg.Date_Enabled));
	EE_ReadVariable(VA_Date_ScrollingMode, (uint16_t*)(AppCfg.Date_ScrollingMode));
	EE_ReadVariable(VA_Date_ScrollIntervalInSec, &(AppCfg.Date_ScrollIntervalInSec));
	/*Other*/
	EE_ReadVariable(VA_TimeAnimation, &(AppCfg.TimeAnimation));
	EE_ReadVariable(VA_DisplayBrightnessMode, &(AppCfg.DisplayBrightnessMode));
	EE_ReadVariable(VA_DisplayBrightness, &(AppCfg.DisplayBrightness));
}
/**/
void AppConfig_Init(void)
{
	/*NTP*/
	memset(AppCfg.NTP_SSID,0,SO_NTP_SSID);
	memset(AppCfg.NTP_PassWord,0,SO_NTP_PassWord);
	AppCfg.NTP_SyncEnabled = 0;
	AppCfg.NTP_SyncInterval = 0;
	/*Text*/
	memset(AppCfg.Text_Message,0,SO_Text_Message);
	AppCfg.Text_Enabled = 0;
	AppCfg.Text_ScrollingMode = 0;
	AppCfg.Text_ScrollIntervalInSec = 0;
	/*Date*/
	AppCfg.Date_Enabled = 0;
	AppCfg.Date_ScrollingMode = DM_DateNoScroll;
	AppCfg.Date_ScrollIntervalInSec = 0;
	/*Other*/
	AppCfg.TimeAnimation = 0;
	AppCfg.DisplayBrightnessMode = 0;
	AppCfg.DisplayBrightness = 0;
	//
	AppCfg.FirstRun = 1;
	AppCfg.LastScrolled = AS_Text;
	AppCfg.SM_AppStatus = AS_Time;
	AppCfg.SM_WiFiStatus = WiFi_AP;
	AppCfg.NTP_SyncTimerCounter = 0;
}
/* Application Main Functions Start ---------------------------------------------------------*/
HAL_StatusTypeDef Init_Application(void)
{
	AppConfig_Init();
	/**/
	MAX7219_Init();
	/**/
	HAL_FLASH_Unlock();
	if (EE_Init() != HAL_OK)
	{
		return HAL_ERROR;
	}
	/**/
	EEPROM_ReadFrame();
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
		switch(AppCfg.SM_WiFiStatus)
		{
			case WiFi_None:
			{
				break;
			}
			case WiFi_NTP:
			{
				if(RTC_NTPSync(AppCfg.NTP_SSID,AppCfg.NTP_PassWord) == HAL_OK)
				{
					AppCfg.NTP_Connected = 1;
				}
				else
				{
					AppCfg.NTP_Connected = 0;
				}
				AppCfg.SM_WiFiStatus = WiFi_AP;
				break;
			}
			case WiFi_AP:
			{
				while(ESP8266_AccessPoint_InitAndRun() != HAL_OK)
				{
					HAL_Delay(8000);
				}
				while(AppCfg.SM_WiFiStatus == WiFi_AP)
				{
					if(HTML_GetMessage(HTML_Message) == HAL_OK)
					{
						HTML_Interpreter(HTML_Message);
					}
				}
				break;
			}
		}
	}
}
/* Application Main Functions End ---------------------------------------------------------*/
/* Interrupt Callbacks Start ---------------------------------------------------------*/
/**/
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	StateMachine();
}
/**/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	UNUSED(htim);
	/*timer3 interrupt*/
	if(htim->Instance == TIM3)
	{
		/**/
		if(AppCfg.DisplayMode == AS_Time)
		{
			UpdateTimeOnDisplay();
		}
		/**/
		if(AppCfg.DisplayMode == AS_Text)
		{
			/*scrolling text on display*/
			if (AppCfg.TextScrolling == true)
			{
				if (AppCfg.FirstColumn == (AppCfg.LastColumn - NumberOf_DisplayColumn))
				{
					/**/
					if(AppCfg.ScrollingMode == TM_MessageScroll)
					{
						if((HAL_GetTick()-AppCfg.TimeStamp)<1500)
						{
							/*wait*/
						}
						else
						{
							//AppCfg.DisplayMode = TextDone;
							AppCfg.SM_AppStatus = AS_TextDone;
						}

					}
					else
					{
						//AppCfg.DisplayMode = TextDone;
						AppCfg.SM_AppStatus = AS_TextDone;
					}
				}
				else
				{
					SendToDisplay(AppCfg.FirstColumn);
					if((AppCfg.ScrollingMode == TM_MessageScroll)&&(AppCfg.FirstColumn == 0))
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
				uint32_t tmpp = HAL_GetTick();
				if((tmpp - AppCfg.TimeStamp)<3000)
				{
					/*wait*/
				}
				else
				{
					//AppCfg.DisplayMode = TextDone;
					AppCfg.SM_AppStatus = AS_TextDone;
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
