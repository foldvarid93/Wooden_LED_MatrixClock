/***************************************************************************************
 * Includes
 **************************************************************************************/
#include "esp8266.h"
/***************************************************************************************
 * Variables
 **************************************************************************************/
/*globals*/

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
	if(ESP8266_AT_SendAndReceiveWithTimeout("AT+RESTORE", "ready", LONG_PAUSE) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_AT_SendAndReceiveWithTimeout("AT", OK_STR, SHORT_PAUSE) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_AT_SendAndReceiveWithTimeout("ATE0", OK_STR, SHORT_PAUSE) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_AT_SendAndReceiveWithTimeout("AT+CWMODE=2", OK_STR, SHORT_PAUSE) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_AT_SendAndReceiveWithTimeout("AT+CWSAP=\"ESP\",\"password\",1,4", OK_STR,10000) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_AT_SendAndReceiveWithTimeout("AT+CIPMUX=1", OK_STR, SHORT_PAUSE) != HAL_OK)
	{
		  return HAL_ERROR;
	}

	if(ESP8266_AT_SendAndReceiveWithTimeout("AT+CIPAP=\"192.168.4.1\"", OK_STR, SHORT_PAUSE) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	if(ESP8266_AT_SendAndReceiveWithTimeout("AT+CIPSERVER=1,80", OK_STR, SHORT_PAUSE) != HAL_OK)
	{
		  return HAL_ERROR;
	}
	Uart_flush();
	while(1)
	{
		Get_HTML_Message();
	}
	return HAL_OK;
}
/**/
HAL_StatusTypeDef Get_HTML_Message(void)
{
	uint16_t head;
	uint16_t tail;
	uint16_t MsgLength;
	uint8_t MsgBuf[512];

	if (Wait_for(".-'S.-'T.-'A.-'R.-'T.-'"))
	{
		tail = rx_buffer.tail;
		if (Wait_for(".-'S.-'T.-'O.-'P.-'"))
		{
			head = rx_buffer.tail - strlen(".-'S.-'T.-'O.-'P.-'");
			MsgLength = head - tail;
			memset(MsgBuf, 0, sizeof(MsgBuf));
			strncpy((char*) MsgBuf, (char*) &(rx_buffer.buffer[tail]),MsgLength);
			MsgBuf[MsgLength] = '\0';
			char tmp[]="HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";

			if (ESP8266_AT_SendAndReceiveWithTimeout("AT+CIPSEND=0,44", OK_STR, SHORT_PAUSE)!= HAL_OK)
			{
				return HAL_ERROR;
			}
			UartPrintCharArray((char*)tmp,strlen(tmp));
			Uart_flush();
			asm("nop");
		}
	}
	return HAL_OK;
}
/**/
