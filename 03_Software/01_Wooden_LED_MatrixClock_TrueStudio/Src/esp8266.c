/***************************************************************************************
 * Includes
 **************************************************************************************/
#include "esp8266.h"
/***************************************************************************************
 * Variables
 **************************************************************************************/
/*globals*/
uint8_t HTML_Message[SizeOf_HTML_Message];
/*externs*/
extern ring_buffer rx_buffer;
/***************************************************************************************
 * Function declarations
 **************************************************************************************/
/**/
void ESP8266_AT_Send(const char * command, ...)
{

	char *p = (char*) command;
	va_list argptr;
	while (IsDataAvailable() > 0) {
		Uart_read();
	}
	va_start(argptr, command);
	while (p) {
		uint8_t Len = strlen(p);
		for (uint8_t i = 0; i < Len; i++) {
			Uart_write(*p);
			p++;
		}
		p = va_arg(argptr, char*);
	}
	va_end(argptr);
	Uart_write('\r');
	Uart_write('\n');
}
/**/
HAL_StatusTypeDef ESP8266_AT_ReceiveWithTimeout(const char * Answer, uint16_t Timeout)
{
	char tmp[256];
	int len = strlen (Answer);
	uint16_t Diff=0;
	uint16_t TmpIdx=0;
	uint32_t timeOut = HAL_GetTick();//save start timestamp
	uint8_t StateMachineStatus=0;
	while (HAL_GetTick() - timeOut <= Timeout)
	{
		/*if new data available*/
		if(IsDataAvailable() > 0 )
		{
			/*check the tail and head index*/
			if(rx_buffer.head > rx_buffer.tail)
			{
				Diff=rx_buffer.head - rx_buffer.tail;
			}
			else if(rx_buffer.tail > rx_buffer. head)
			{
				Diff= (UART_BUFFER_SIZE - rx_buffer.tail) +rx_buffer.head;
			}
			else
			{
				Diff=0;
			}
			/*if enough caracter arrived to check*/
			if( Diff >= len)
			{
				if(StateMachineStatus == 0)
				{
					if(Uart_peek() != Answer[0])
					{
						rx_buffer.tail = (unsigned int)(rx_buffer.tail + 1) % UART_BUFFER_SIZE;
					}
					else
					{
						StateMachineStatus = 1;//go to the state 1
					}
				}
				else if(StateMachineStatus == 1)
				{
					TmpIdx=rx_buffer.tail;
					/*if the first character mach, continue*/
					for(uint16_t i=0;i<len;i++)
					{
						tmp[i]=rx_buffer.buffer[TmpIdx];
						TmpIdx = (TmpIdx + 1) % UART_BUFFER_SIZE;
						tmp[i+1]='\0';
					}
					/*if equal, return 1*/
					if (strcmp((char*)tmp,(char*)Answer) == 0 )
					{
						rx_buffer.tail=TmpIdx;
						return HAL_OK;// return witn 1, success
					}
					else
					{
						rx_buffer.tail = (unsigned int)(rx_buffer.tail + 1) % UART_BUFFER_SIZE;// increment the source buffer position
					}
				}
				else
				{
					asm("nop");//debug nop
				}
			}
			/*else do nothing, wait*/
			else
			{
				asm("nop");//debug nop
			}
		}
		/*no available data wait for data or timeout*/
		else
		{
			asm("nop");//debug nop
		}
	}
	return HAL_ERROR;//return with 0, fail or timeout
}
/*Sending AT command and wait for answer*/
HAL_StatusTypeDef ESP8266_AT_SendAndReceiveWithTimeout(const char * Command, const char * Respond, uint32_t Timeout)
{
	/*send*/
	ESP8266_AT_Send(Command, 0);
	/*receive*/
	if((ESP8266_AT_ReceiveWithTimeout(Respond, Timeout)) == HAL_OK)
	{
		return HAL_OK;
	}
	return HAL_ERROR;
}
/**/
HAL_StatusTypeDef ESP8266_AccessPoint_InitAndRun(void)
{
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
	if(ESP8266_AT_SendAndReceiveWithTimeout("AT+RESTORE", AT_READY, LONG_PAUSE) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_AT_SendAndReceiveWithTimeout("AT", AT_OK, SHORT_PAUSE) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_AT_SendAndReceiveWithTimeout("ATE0", AT_OK, SHORT_PAUSE) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_AT_SendAndReceiveWithTimeout("AT+CWMODE=2", AT_OK, SHORT_PAUSE) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_AT_SendAndReceiveWithTimeout("AT+CWSAP=\"FlipClock\",\"FL1pCL0ck\",1,4", AT_OK,10000) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_AT_SendAndReceiveWithTimeout("AT+CIPMUX=1", AT_OK, SHORT_PAUSE) != HAL_OK)
	{
		  return HAL_ERROR;
	}

	if(ESP8266_AT_SendAndReceiveWithTimeout("AT+CIPAP=\"192.168.4.1\"", AT_OK, SHORT_PAUSE) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_AT_SendAndReceiveWithTimeout("AT+CIPSERVER=1,80", AT_OK, SHORT_PAUSE) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	Uart_flush();
	return HAL_OK;
}
/**/
HAL_StatusTypeDef HTML_GetMessage(uint8_t * Message)
{
	uint16_t head;
	uint16_t tail;
	uint16_t MsgLength;

	//if (Wait_for_timeout(".-'S.-'T.-'A.-'R.-'T.-'",1000))
	if( ESP8266_AT_ReceiveWithTimeout(".-'S.-'T.-'A.-'R.-'T.-'",1000) == HAL_OK)
	{
		tail = rx_buffer.tail;
		//if (Wait_for_timeout(".-'S.-'T.-'O.-'P.-'",1000))
		if( ESP8266_AT_ReceiveWithTimeout(".-'S.-'T.-'O.-'P.-'",1000) == HAL_OK)
		{
			head = rx_buffer.tail - strlen(".-'S.-'T.-'O.-'P.-'");
			MsgLength = head - tail;
			memset(Message, 0, SizeOf_HTML_Message);
			strncpy((char*) Message, (char*) &(rx_buffer.buffer[tail]),MsgLength);
			Message[MsgLength] = '\0';

			/*HTML respond: OK*/
			if (ESP8266_AT_SendAndReceiveWithTimeout("AT+CIPSEND=0,63", AT_OK, SHORT_PAUSE)!= HAL_OK)
			{
				return HAL_ERROR;
			}
			if (ESP8266_AT_SendAndReceiveWithTimeout(HTML_OK, AT_OK, SHORT_PAUSE)!= HAL_OK)
			{
				return HAL_ERROR;
			}
			/*Close connection*/
			if (ESP8266_AT_SendAndReceiveWithTimeout("AT+CIPCLOSE=0", AT_OK, SHORT_PAUSE)!= HAL_OK)
			{
				return HAL_ERROR;
			}
			//UartPrintCharArray(HTML_OK,strlen(HTML_OK));

			Uart_flush();
			asm("nop");
		}
	}
	return HAL_OK;
}
/**/
HAL_StatusTypeDef HTML_Interpreter(uint8_t * Message)
{
	uint16_t MessageLength=strlen((char*)Message);
#define MSG_ID_LENGTH			(14)
#define MSG_ID_FRAME_LENGTH		(2*MSG_ID_LENGTH)
	char MSG_START[MSG_ID_LENGTH + 1];
	char MSG[SizeOf_HTML_Message];
	char MSG_STOP[MSG_ID_LENGTH + 1];
	uint8_t* StartPtr;
	uint8_t* StopPtr;
	uint8_t* MsgPtr;
	uint16_t MsgLen;

	while(MessageLength > 0)
	{
		StartPtr = (uint8_t*) strstr((const char*)Message, (char*)"MsG_STRT-");
		MsgPtr = StartPtr + MSG_ID_LENGTH;
		StopPtr = (uint8_t*) strstr((const char*)Message, (char*)"MsG_STOP-");
		if(MsgPtr == StopPtr)
		{
			//return HAL_OK;
		}

		MsgLen = StopPtr - MsgPtr;

		strncpy((char*)MSG_START,(char*)StartPtr,MSG_ID_LENGTH);
		MSG_START[MSG_ID_LENGTH] = '\0';

		strncpy((char*)MSG,(char*)MsgPtr,MsgLen);
		MSG[MsgLen] = '\0';

		strncpy((char*)MSG_STOP,(char*)StopPtr,MSG_ID_LENGTH);
		MSG_STOP[MSG_ID_LENGTH] = '\0';

		/* Group 0 ***********************************************************************************************************/
		/*Message ID00*/
		/* NTP Sync ON/OFF */
		if(((strcmp((char*) MSG_START,(char*)MSG_ID00_START)) == 0) && ((strcmp((char*) MSG_STOP,(char*)MSG_ID00_STOP)) == 0))
		{
			if(strcmp((char*) MSG, (char*) "true") == 0)
			{
				AppCfg.NTP_SyncEnabled = 1;
			}
			else if(strcmp((char*) MSG, (char*) "false") == 0)
			{
				AppCfg.NTP_SyncEnabled = 0;
			}
			else
			{
				AppCfg.NTP_SyncEnabled = 0;
			}
		}
		/* Message ID01 */
		/*NTP_SSID*/
		if(((strcmp((char*) MSG_START,(char*)MSG_ID01_START)) == 0) && ((strcmp((char*) MSG_STOP,(char*)MSG_ID01_STOP)) == 0))
		{
			if(strlen((char*)MSG) <= sizeof(AppCfg.NTP_SSID))
			{
				strcpy((char*)AppCfg.NTP_SSID,(char*)MSG);
			}
		}
		/* Message ID02 */
		/* Password */
		if(((strcmp((char*) MSG_START,(char*)MSG_ID02_START)) == 0) && ((strcmp((char*) MSG_STOP,(char*)MSG_ID02_STOP)) == 0))
		{
			if(strlen((char*)MSG) <= sizeof(AppCfg.NTP_PassWord))
			{
				strcpy((char*)AppCfg.NTP_PassWord,(char*)MSG);
			}
		}
		/*Message ID03*/
		/*Ntp sync interval*/
		if(((strcmp((char*) MSG_START,(char*)MSG_ID03_START)) == 0) && ((strcmp((char*) MSG_STOP,(char*)MSG_ID03_STOP)) == 0))
		{
			uint8_t i=0;
			while(MSG[i] != 'h'){
				i++;
			}
			MSG[i] = '\0';
			AppCfg.NTP_SyncInterval = atoi((char*) MSG);
			AppCfg.NTP_SyncTimerCounter = (AppCfg.NTP_SyncInterval * NUMBEROFSECONDS_HOUR);
		}
		/*Message ID04*/
		/*Date Time manual set*/
		if(((strcmp((char*) MSG_START,(char*)MSG_ID04_START)) == 0) && ((strcmp((char*) MSG_STOP,(char*)MSG_ID04_STOP)) == 0))
		{
			if (Convert_CharArrayToDateTime((const char*) MSG) == HAL_ERROR)
			{
				return HAL_ERROR;
			}
			//AppCfg.SM_NextState = Date;
		}
		/* Group 1 ***********************************************************************************************************/
		/*Message ID10*/
		/*text message enabled*/
		if(((strcmp((char*) MSG_START,(char*)MSG_ID10_START)) == 0) && ((strcmp((char*) MSG_STOP,(char*)MSG_ID10_STOP)) == 0))
		{
			if(strcmp((char*) MSG, (char*) "true") == 0)
			{
				AppCfg.Text_Enabled = 1;
			}
			else if(strcmp((char*) MSG, (char*) "false") == 0)
			{
				AppCfg.Text_Enabled = 0;
			}
			else
			{
				AppCfg.Text_Enabled = 0;
			}
		}
		/*Message ID11*/
		/*text message display mode*/
		if(((strcmp((char*) MSG_START,(char*)MSG_ID11_START)) == 0) && ((strcmp((char*) MSG_STOP,(char*)MSG_ID11_STOP)) == 0))
		{
			if(strcmp((char*) MSG, (char*)"No Scroll Message") == 0)
			{
				AppCfg.Text_ScrollingMode = JustText;
				AppCfg.TextScrolling = false;
			}
			else if(strcmp((char*) MSG, (char*)"Scrolling Just Message") == 0)
			{
				AppCfg.Text_ScrollingMode = ScrollInAndOut;
				AppCfg.TextScrolling = false;
			}
			else if(strcmp((char*) MSG, (char*)"Scrolling In and Out Message") == 0)
			{
				AppCfg.Text_ScrollingMode = ScrollInAndOut;
				AppCfg.TextScrolling = true;
			}
			else
			{
				asm("nop");
			}
		}
		/*Message ID12*/
		/* Text scroll Interval*/
		if(((strcmp((char*) MSG_START,(char*)MSG_ID12_START)) == 0) && ((strcmp((char*) MSG_STOP,(char*)MSG_ID12_STOP)) == 0))
		{
			AppCfg.Text_ScrollIntervalInSec = atoi((char*) MSG);
		}
		/*Message ID13*/
		/*Scrolltext text message*/
		if(((strcmp((char*) MSG_START,(char*)MSG_ID13_START)) == 0) && ((strcmp((char*) MSG_STOP,(char*)MSG_ID13_STOP)) == 0))
		{
			if(strlen((char*)MSG) <= sizeof(AppCfg.Text_Message))
			{
				strcpy((char*)AppCfg.Text_Message,(char*)MSG);
			}
			//AppCfg.SM_NextState = Text;
		}
		/*Message ID14*/
		/*date message enabled*/
		if(((strcmp((char*) MSG_START,(char*)MSG_ID14_START)) == 0) && ((strcmp((char*) MSG_STOP,(char*)MSG_ID14_STOP)) == 0))
		{
			if(strcmp((char*) MSG, (char*) "true") == 0)
			{
				AppCfg.Date_Enabled = 1;
			}
			else if(strcmp((char*) MSG, (char*) "false") == 0)
			{
				AppCfg.Date_Enabled = 0;
			}
			else
			{
				AppCfg.Date_Enabled = 0;
			}
		}
		/*Message ID15*/
		/*text message display mode*/
		if(((strcmp((char*) MSG_START,(char*)MSG_ID15_START)) == 0) && ((strcmp((char*) MSG_STOP,(char*)MSG_ID15_STOP)) == 0))
		{
			if(strcmp((char*) MSG, (char*)"Just Date No Scroll") == 0)
			{
				AppCfg.Date_ScrollingMode = JustText;
			}
			else if(strcmp((char*) MSG, (char*)"Just Date Scroll") == 0)
			{
				AppCfg.Date_ScrollingMode = ScrollInAndOut;
			}
			else if(strcmp((char*) MSG, (char*)"Scroll Date Message") == 0)
			{
				AppCfg.Date_ScrollingMode = ScrollInAndOut;
			}
			else
			{
				asm("nop");
			}
		}
		/*Message ID16*/
		/* Date scroll Interval*/
		if(((strcmp((char*) MSG_START,(char*)MSG_ID16_START)) == 0) && ((strcmp((char*) MSG_STOP,(char*)MSG_ID16_STOP)) == 0))
		{
			AppCfg.Date_ScrollIntervalInSec = atoi((char*) MSG);
		}
		/* Group 2 ***********************************************************************************************************/



		/* Group 3 ***********************************************************************************************************/
		/*Message ID30*/
		/*TimeAnimation*/
		if(((strcmp((char*) MSG_START,(char*)MSG_ID30_START)) == 0) && ((strcmp((char*) MSG_STOP,(char*)MSG_ID30_STOP)) == 0))
		{
			if(strcmp((char*) MSG, (char*) "true") == 0)
			{
				AppCfg.TimeAnimation = 1;
			}
			else if(strcmp((char*) MSG, (char*) "false") == 0)
			{
				AppCfg.TimeAnimation = 0;
			}
			else
			{
				AppCfg.TimeAnimation = 0;
			}
		}
		/*Message ID31*/
		/*Brightness*/
		if(((strcmp((char*) MSG_START,(char*)MSG_ID31_START)) == 0) && ((strcmp((char*) MSG_STOP,(char*)MSG_ID31_STOP)) == 0))
		{
			if(strcmp((char*) MSG, (char*) "true") == 0)
			{
				AppCfg.DisplayBrightnessMode = 1;
			}
			else if(strcmp((char*) MSG, (char*) "false") == 0)
			{
				AppCfg.DisplayBrightnessMode = 0;
			}
			else
			{
				AppCfg.DisplayBrightnessMode = 0;
			}
		}
		/*Message ID32*/
		/*Brightness*/
		if(((strcmp((char*) MSG_START,(char*)MSG_ID32_START)) == 0) && ((strcmp((char*) MSG_STOP,(char*)MSG_ID32_STOP)) == 0))
		{
			uint16_t Tmp= (uint16_t) (atof (MSG));

			switch (Tmp)
			{
			case 0: AppCfg.DisplayBrightness = INTENSITY_1; break;
			case 1: AppCfg.DisplayBrightness = INTENSITY_1; break;
			/**/
			case 2: AppCfg.DisplayBrightness = INTENSITY_3; break;
			case 3: AppCfg.DisplayBrightness = INTENSITY_3; break;
			/**/
			case 4: AppCfg.DisplayBrightness = INTENSITY_5; break;
			case 5: AppCfg.DisplayBrightness = INTENSITY_5; break;
			/**/
			case 6: AppCfg.DisplayBrightness = INTENSITY_7; break;
			case 7: AppCfg.DisplayBrightness = INTENSITY_7; break;
			/**/
			case 8: AppCfg.DisplayBrightness = INTENSITY_9; break;
			case 9: AppCfg.DisplayBrightness = INTENSITY_9; break;
			/**/
			case 10: AppCfg.DisplayBrightness = INTENSITY_11; break;
			case 11: AppCfg.DisplayBrightness = INTENSITY_11; break;
			/**/
			case 12: AppCfg.DisplayBrightness = INTENSITY_13; break;
			case 13: AppCfg.DisplayBrightness = INTENSITY_13; break;
			/**/
			case 14: AppCfg.DisplayBrightness = INTENSITY_15; break;
			case 15: AppCfg.DisplayBrightness = INTENSITY_15; break;
			/**/
			case 16: AppCfg.DisplayBrightness = INTENSITY_17; break;
			case 17: AppCfg.DisplayBrightness = INTENSITY_17; break;
			/**/
			case 18: AppCfg.DisplayBrightness = INTENSITY_19; break;
			case 19: AppCfg.DisplayBrightness = INTENSITY_19; break;
			/**/
			case 20: AppCfg.DisplayBrightness = INTENSITY_21; break;
			case 21: AppCfg.DisplayBrightness = INTENSITY_21; break;
			/**/
			case 22: AppCfg.DisplayBrightness = INTENSITY_23; break;
			case 23: AppCfg.DisplayBrightness = INTENSITY_23; break;
			/**/
			case 24: AppCfg.DisplayBrightness = INTENSITY_25; break;
			case 25: AppCfg.DisplayBrightness = INTENSITY_25; break;
			/**/
			case 26: AppCfg.DisplayBrightness = INTENSITY_27; break;
			case 27: AppCfg.DisplayBrightness = INTENSITY_27; break;
			/**/
			case 28: AppCfg.DisplayBrightness = INTENSITY_29; break;
			case 29: AppCfg.DisplayBrightness = INTENSITY_29; break;
			/**/
			case 30: AppCfg.DisplayBrightness = INTENSITY_31; break;
			case 31: AppCfg.DisplayBrightness = INTENSITY_31; break;
			/**/
			default: AppCfg.DisplayBrightness = INTENSITY_15;
			}
			MAX7219_Send(REG_INTENSITY, AppCfg.DisplayBrightness);
		}
		/* Group 4 ***********************************************************************************************************/
		/*Message ID40*/
		/*Load*/
		if(((strcmp((char*) MSG_START,(char*)MSG_ID40_START)) == 0) && ((strcmp((char*) MSG_STOP,(char*)MSG_ID40_STOP)) == 0))
		{
			asm("nop");
		}
		/*Message ID41*/
		/*Save*/
		if(((strcmp((char*) MSG_START,(char*)MSG_ID41_START)) == 0) && ((strcmp((char*) MSG_STOP,(char*)MSG_ID41_STOP)) == 0))
		{
			EEPROM_WriteFrame();
		}
		Message = (uint8_t*) StopPtr + MSG_ID_LENGTH;
		MessageLength = strlen((char*)Message);
	}
	return HAL_OK;
}
/**/
