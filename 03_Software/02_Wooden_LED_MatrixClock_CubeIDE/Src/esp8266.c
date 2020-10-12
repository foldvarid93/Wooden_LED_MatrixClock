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
	if(ESP8266_AT_SendAndReceiveWithTimeout("AT+CWSAP=\"ESP\",\"password\",1,4", AT_OK,10000) != HAL_OK)
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
	while(1)
	{
		if(HTML_GetMessage(HTML_Message) == HAL_OK){
			HTML_Interpreter(HTML_Message);
		}
	}
	return HAL_OK;
}
/**/
HAL_StatusTypeDef HTML_GetMessage(uint8_t * Message)
{
	uint16_t head;
	uint16_t tail;
	uint16_t MsgLength;

	if (Wait_for(".-'S.-'T.-'A.-'R.-'T.-'"))
	{
		tail = rx_buffer.tail;
		if (Wait_for(".-'S.-'T.-'O.-'P.-'"))
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
			return HAL_OK;
		}
		else
		{
			MsgLen = StopPtr - MsgPtr;

			strncpy((char*)MSG_START,(char*)StartPtr,MSG_ID_LENGTH);
			MSG_START[MSG_ID_LENGTH] = '\0';

			strncpy((char*)MSG,(char*)MsgPtr,MsgLen);
			MSG[MsgLen] = '\0';

			strncpy((char*)MSG_STOP,(char*)StopPtr,MSG_ID_LENGTH);
			MSG_STOP[MSG_ID_LENGTH] = '\0';

			/*Message ID1*/
			if(((strcmp((char*) MSG_START,(char*)MSG_ID01_START)) == 0) && ((strcmp((char*) MSG_STOP,(char*)MSG_ID01_STOP)) == 0))
			{
				if(strlen((char*)MSG) <= sizeof(AppCfg.SSID))
				{
					strcpy((char*)AppCfg.SSID,(char*)MSG);
					EE_WriteCharArray(VirtAddr_SSID, (uint8_t*)(AppCfg.SSID));
				}
			}
			/*Message ID2*/
			if(((strcmp((char*) MSG_START,(char*)MSG_ID02_START)) == 0) && ((strcmp((char*) MSG_STOP,(char*)MSG_ID02_STOP)) == 0))
			{
				if(strlen((char*)MSG) <= sizeof(AppCfg.PassWord))
				{
					strcpy((char*)AppCfg.PassWord,(char*)MSG);
					EE_WriteCharArray(VirtAddr_PassWord, (uint8_t*)(AppCfg.PassWord));
				}
			}
			/*Message ID3*/
			if(((strcmp((char*) MSG_START,(char*)MSG_ID03_START)) == 0) && ((strcmp((char*) MSG_STOP,(char*)MSG_ID03_STOP)) == 0))
			{
				if(strlen((char*)MSG) <= sizeof(AppCfg.ScrollText))
				{
					strcpy((char*)AppCfg.ScrollText,(char*)MSG);
					EE_WriteCharArray(VirtAddr_ScrollText, (uint8_t*)(AppCfg.ScrollText));
				}
			}
			/*Message ID4*/
			if(((strcmp((char*) MSG_START,(char*)MSG_ID04_START)) == 0) && ((strcmp((char*) MSG_STOP,(char*)MSG_ID04_STOP)) == 0))
			{
				RTC_TimeTypeDef HAL_Time={0,0,0,0,0,0,RTC_DAYLIGHTSAVING_NONE,RTC_STOREOPERATION_RESET};
				RTC_DateTypeDef HAL_Date={0,0,0,0};
				uint8_t i=0;
				char TmpBuf[20];
				/*year*/
				for(uint8_t j=0 ; MSG[i] != '.' ; i++ , j++)
				{
					TmpBuf[j] = MSG[i];
					TmpBuf[j+1] = '\0';
				}
				i++;
				HAL_Date.Year =  atoi((const char*)TmpBuf) - 2000;
				/*month*/
				for(uint8_t j=0 ; MSG[i] != '.' ; i++ , j++)
				{
					TmpBuf[j] = MSG[i];
					TmpBuf[j+1] = '\0';
				}
				i++;
				HAL_Date.Month =  atoi((const char*)TmpBuf);
				/*date*/
				for(uint8_t j=0 ; MSG[i] != '.' ; i++ , j++)
				{
					TmpBuf[j] = MSG[i];
					TmpBuf[j+1] = '\0';
				}
				i++;
				HAL_Date.Date =  atoi((const char*)TmpBuf);
				/*Day*/
				//TODO: set day
				/*hour*/
				for(uint8_t j=0 ; MSG[i] != '.' ; i++ , j++)
				{
					TmpBuf[j] = MSG[i];
					TmpBuf[j+1] = '\0';
				}
				i++;
				HAL_Time.Hours =  atoi((const char*)TmpBuf);
				/*Minute*/
				for(uint8_t j=0 ; MSG[i] != '.' ; i++ , j++)
				{
					TmpBuf[j] = MSG[i];
					TmpBuf[j+1] = '\0';
				}
				HAL_Time.Minutes =  atoi((const char*)TmpBuf);
				/**/
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
			/*Message ID5*/
			if(((strcmp((char*) MSG_START,(char*)MSG_ID05_START)) == 0) && ((strcmp((char*) MSG_STOP,(char*)MSG_ID05_STOP)) == 0))
			{
				if(strlen((char*)MSG) == 1)
				{
					if(strcmp((char*)MSG,"1") == 0)
					{
						AppCfg.TimeAnimation = 1;
						EE_WriteVariable(VirtAddr_TimeAnimation, AppCfg.TimeAnimation);
					}
					else
					{
						AppCfg.TimeAnimation = 0;
						EE_WriteVariable(VirtAddr_TimeAnimation, AppCfg.TimeAnimation);
					}
				}
			}
			/*Message ID6*/
			if(((strcmp((char*) MSG_START,(char*)MSG_ID06_START)) == 0) && ((strcmp((char*) MSG_STOP,(char*)MSG_ID06_STOP)) == 0))
			{
				asm("nop");
			}
			/*Message ID7*/
			if(((strcmp((char*) MSG_START,(char*)MSG_ID07_START)) == 0) && ((strcmp((char*) MSG_STOP,(char*)MSG_ID07_STOP)) == 0))
			{
				asm("nop");
			}
			/*Message ID8*/
			if(((strcmp((char*) MSG_START,(char*)MSG_ID08_START)) == 0) && ((strcmp((char*) MSG_STOP,(char*)MSG_ID08_STOP)) == 0))
			{
				asm("nop");
			}
			/*Message ID9*/
			if(((strcmp((char*) MSG_START,(char*)MSG_ID09_START)) == 0) && ((strcmp((char*) MSG_STOP,(char*)MSG_ID09_STOP)) == 0))
			{
				asm("nop");
			}
			/*Message ID10*/
			if(((strcmp((char*) MSG_START,(char*)MSG_ID10_START)) == 0) && ((strcmp((char*) MSG_STOP,(char*)MSG_ID10_STOP)) == 0))
			{
				asm("nop");
			}
			/*Message ID11*/
			if(((strcmp((char*) MSG_START,(char*)MSG_ID11_START)) == 0) && ((strcmp((char*) MSG_STOP,(char*)MSG_ID11_STOP)) == 0))
			{
				asm("nop");
			}
			/*Message ID12*/
			if(((strcmp((char*) MSG_START,(char*)MSG_ID12_START)) == 0) && ((strcmp((char*) MSG_STOP,(char*)MSG_ID12_STOP)) == 0))
			{
				asm("nop");
			}
			/*Message ID13*/
			if(((strcmp((char*) MSG_START,(char*)MSG_ID13_START)) == 0) && ((strcmp((char*) MSG_STOP,(char*)MSG_ID13_STOP)) == 0))
			{
				asm("nop");
			}
			/*Message ID14*/
			if(((strcmp((char*) MSG_START,(char*)MSG_ID14_START)) == 0) && ((strcmp((char*) MSG_STOP,(char*)MSG_ID14_STOP)) == 0))
			{
				asm("nop");
			}
			/*Message ID15*/
			if(((strcmp((char*) MSG_START,(char*)MSG_ID15_START)) == 0) && ((strcmp((char*) MSG_STOP,(char*)MSG_ID15_STOP)) == 0))
			{
				asm("nop");
			}
			/*Message ID16*/
			if(((strcmp((char*) MSG_START,(char*)MSG_ID16_START)) == 0) && ((strcmp((char*) MSG_STOP,(char*)MSG_ID16_STOP)) == 0))
			{
				asm("nop");
			}
		}
		Message = (uint8_t*) StopPtr + MSG_ID_LENGTH;
		MessageLength = strlen((char*)Message);
	}
	return HAL_OK;
}
/**/
