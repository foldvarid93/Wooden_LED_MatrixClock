/***************************************************************************************
 * Includes
 **************************************************************************************/
//#include "application.h"
#include "remotexy.h"
/***************************************************************************************
 * Variables
 **************************************************************************************/
/*globals*/
CRemoteXY remotexy;

Serial_t serial = { .read = NULL, .write = NULL, .available = NULL , .find = NULL, .println = NULL};

uint8_t RemoteXY_CONF[] = { 255, 48, 0, 0, 0, 66, 0, 10, 66, 1, 1, 1, 26, 63,
		12, 12, 36, 8, 83, 101, 116, 0, 7, 4, 2, 26, 59, 7, 2, 27, 38, 36, 129,
		0, 20, 34, 24, 6, 8, 78, 101, 116, 119, 111, 114, 107, 0, 129, 0, 17,
		51, 28, 6, 8, 80, 97, 115, 115, 119, 111, 114, 100, 0, 7, 4, 3, 42, 57,
		7, 2, 27, 2, 11 };

/*externs*/
extern UART_HandleTypeDef huart3;

extern ring_buffer *_rx_buffer;

/***************************************************************************************
 * Function declarations
 **************************************************************************************/
void ESP8266_RemoteXY_InitAndStart(void)
{
	/*HW reset*/
	HAL_GPIO_WritePin(ESP8266_RST_GPIO_Port, ESP8266_RST_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ESP8266_EN_GPIO_Port, ESP8266_EN_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ESP8266_EN_GPIO_Port, ESP8266_EN_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(ESP8266_RST_GPIO_Port, ESP8266_RST_Pin, GPIO_PIN_SET);
	/**/
	Ringbuf_init();
	/**/
	ESP8266_RemoteXY_Init(RemoteXY_CONF, &RemoteXY, REMOTEXY_ACCESS_PASSWORD,REMOTEXY_WIFI_SSID, REMOTEXY_WIFI_PASSWORD, REMOTEXY_SERVER_PORT);
}

void ESP8266_RemoteXY_Init(const void * _conf, void * _var,	const char * _accessPassword, const char * _wifiSsid,const char * _wifiPassword, uint16_t _port)
{
	ESP8266_Serial_Init();
	ESP8266_RemoteXY_InitAT();
	remotexy.wifiSsid = (char *) _wifiSsid;
	remotexy.wifiPassword = (char *) _wifiPassword;
	remotexy.port = _port;
	remotexy.connectCannel = 0;
	remotexy.connectAvailable = 0;
	remotexy.freeAvailable = 0;
	remotexy.ESP8266_RemoteXY_SendBytesAvailable = 0;
	remotexy.ESP8266_RemoteXY_SendBytesLater = 0;
	ESP8266_RemoteXY_InitRemoteXY(_conf, _var, _accessPassword);
	remotexy.moduleTestTimeout = HAL_GetTick();
}

uint8_t ESP8266_RemoteXY_InitModule(void) {

	uint8_t tryCount = 20;
	while (--tryCount > 0) {

		ESP8266_SendATCommand("AT", 0);
		if (ESP8266_WaitATAnswer(AT_ANSWER_OK, 1000))
			break;
	}
	if (tryCount == 0) {
		return 0;
	}
	ESP8266_SendATCommand("AT+RST", 0);
	if (!ESP8266_WaitATAnswer(AT_ANSWER_OK, 1000))
		return 0;
	if (!ESP8266_WaitATAnswer(AT_MESSAGE_READY, 5000))
		return 0;

	return ESP8266_RemoteXY_SetModule();
}

uint8_t ESP8266_RemoteXY_SetModule(void)
{
	char sport[6];
	rxy_itos(remotexy.port, sport);
	char stimeout[6];
	rxy_itos(REMOTEXY_SERVER_TIMEOUT / 1000, stimeout);

	remotexy.connectCannel = 0;
	remotexy.connectAvailable = 0;

	ESP8266_SendATCommand("ATE0", 0);
	if (!ESP8266_WaitATAnswer(AT_ANSWER_OK, 1000))
		return 0;
	ESP8266_SendATCommand("AT+CWMODE=2", 0);
	if (!ESP8266_WaitATAnswer(AT_ANSWER_OK, 2000))
		return 0;
	ESP8266_SendATCommand("AT+CWDHCP=0,1", 0);
	if (!ESP8266_WaitATAnswer(AT_ANSWER_OK, 2000))
		return 0;
	char crypt[2] = { *remotexy.wifiPassword ? '4' : '0', 0 };
	ESP8266_SendATCommand("AT+CWSAP=", "\"", remotexy.wifiSsid, "\",", "\"",
			remotexy.wifiPassword, "\",", "10,", crypt, 0);
	if (!ESP8266_WaitATAnswer(AT_ANSWER_OK, 5000))
		return 0;
	ESP8266_SendATCommand("AT+CIPMODE=0", 0);
	if (!ESP8266_WaitATAnswer(AT_ANSWER_OK, 2000))
		return 0;
	ESP8266_SendATCommand("AT+CIPMUX=1", 0);
	if (!ESP8266_WaitATAnswer(AT_ANSWER_OK, 1000))
		return 0;
	ESP8266_SendATCommand("AT+CIPSERVER=1,", sport, 0);
	if (!ESP8266_WaitATAnswer(AT_ANSWER_OK, 1000))
		return 0;
	ESP8266_SendATCommand("AT+CIPSTO=", stimeout, 0);
	if (!ESP8266_WaitATAnswer(AT_ANSWER_OK, 1000))
		return 0;
	remotexy.moduleTestTimeout = HAL_GetTick();
	return 1;
}

void ESP8266_RemoteXY_HandlerModule(void)
{
	while (serial.available() > 0) {
		if (remotexy.connectAvailable)
			break;
		if (remotexy.freeAvailable) {
			serial.read();
			remotexy.freeAvailable--;
		} else {
			readATMessage();
		}
		remotexy.moduleTestTimeout = HAL_GetTick();
	}
	if (HAL_GetTick()
			- remotexy.moduleTestTimeout> REMOTEXY_ESP8266_MODULETEST_TIMEOUT) {
		remotexy.moduleTestTimeout = HAL_GetTick();
		if (testATecho() == 2)
			ESP8266_RemoteXY_SetModule();
	}
}

void ESP8266_RemoteXY_ReadyAT(void)
{
	ESP8266_RemoteXY_SetModule();
}

void ESP8266_RemoteXY_ConnectAT(void)
{
	if (remotexy.connectCannel == 0) {
		remotexy.connectCannel = *(remotexy.params[0]);
		remotexy.connectAvailable = 0;
	}
}

void ESP8266_RemoteXY_ClosedAT(void)
{
	if (remotexy.connectCannel == *(remotexy.params[0]))
		remotexy.connectCannel = 0;
}

void ESP8266_RemoteXY_InputDataAT() {
	uint16_t size;
	size = getATParamInt(1);
	if (remotexy.connectCannel == *(remotexy.params[0]))
		remotexy.connectAvailable = size;
	else
		remotexy.freeAvailable = size;
}

void ESP8266_RemoteXY_SendStart(uint16_t len)
{
	char s[8];
	if (remotexy.connectCannel) {
		remotexy.ESP8266_RemoteXY_SendBytesLater = 0;
		if (len > REMOTEXY_ESP8266_MAX_SEND_BYTES) {
			remotexy.ESP8266_RemoteXY_SendBytesLater = len - REMOTEXY_ESP8266_MAX_SEND_BYTES;
			len = REMOTEXY_ESP8266_MAX_SEND_BYTES;
		}
		remotexy.ESP8266_RemoteXY_SendBytesAvailable = len;
		rxy_itos(len, s + 2);
		*s = remotexy.connectCannel;
		*(s + 1) = ',';
		ESP8266_SendATCommand("AT+CIPSEND=", s, 0);
		if (!ESP8266_WaitATAnswer(AT_ANSWER_GO, 1000))
			remotexy.ESP8266_RemoteXY_SendBytesAvailable = 0;
	}
}

void ESP8266_RemoteXY_SendByte(uint8_t b)
{
	if (remotexy.ESP8266_RemoteXY_SendBytesAvailable) {
		serial.write(b);
		remotexy.ESP8266_RemoteXY_SendBytesAvailable--;
		if (!remotexy.ESP8266_RemoteXY_SendBytesAvailable) {
			ESP8266_WaitATAnswer(AT_ANSWER_SEND_OK, 1000);
			if (remotexy.ESP8266_RemoteXY_SendBytesLater)
				ESP8266_RemoteXY_SendStart(remotexy.ESP8266_RemoteXY_SendBytesLater);
		}
	}
}

uint8_t ESP8266_RemoteXY_ReceiveByte(void)
{
	uint8_t b;
	if (remotexy.connectAvailable) {
		if (serial.available() > 0) {
			remotexy.connectAvailable--;
			b = serial.read();
			return b;
		}
	}
	return 0;
}

uint8_t ESP8266_RemoteXY_AvailableByte(void)
{
	if (remotexy.connectAvailable) {
		return serial.available() > 0;
	}
	return 0;
}

void ESP8266_RemoteXY_InitAT(void)
{
	remotexy.bufferATPos = 0;
}

void ESP8266_SendATCommand(const char * command, ...)
{

	char *p = (char*) command;
	va_list argptr;
	while (serial.available() > 0) {
		serial.read();
	}
	va_start(argptr, command);
	while (p) {
		uint8_t Len = strlen(p);
		for (uint8_t i = 0; i < Len; i++) {
			serial.write(*p);
			p++;
		}
		p = va_arg(argptr, char*);
	}
	va_end(argptr);
	serial.write('\r');
	serial.write('\n');
}

uint8_t ESP8266_WaitATAnswer(const char * answer, uint16_t delay)
{
	uint8_t b;
	uint32_t timeOut = HAL_GetTick();

	uint8_t k = 0;
	while (HAL_GetTick() - timeOut <= delay) {

		if (serial.available() > 0) {
			b = serial.read();
			if (b == 10)
				continue;
			if (b == 13) {
				remotexy.bufferAT[k++] = 0;
				remotexy.bufferATPos = 0;
				k = 0;
				if (strcmp(remotexy.bufferAT, answer) == 0)
					return 1;
				if (strcmp(remotexy.bufferAT, AT_ANSWER_ERROR) == 0)
					return 0;
				if (strcmp(cmpBufferAT(), AT_MESSAGE_READY) == 0)
					return 0;
				if (strcmp(remotexy.bufferAT, AT_MESSAGE_AT) == 0)
					remotexy.haveEcho = 1;
			} else {
				if (k < AT_BUFFER_STR_LENGTH)
					remotexy.bufferAT[k++] = b;
				if (b == '>') {
					if (strcmp(answer,AT_ANSWER_GO) == 0)
						return 1;
				}
			}
		}
	}
	return 0;
}

// echo test, returns
//  0 - no answer
//  1 - no echo
//  2 - have echo

uint8_t testATecho(void)
{
	remotexy.haveEcho = 0;
	ESP8266_SendATCommand("AT", 0);
	if (!ESP8266_WaitATAnswer(AT_ANSWER_OK, 1000))
		return 0;
	return (remotexy.haveEcho == 0 ? 1 : 2);
}

void readATMessage(void)
{
	uint8_t b;
	while (serial.available() > 0) {
		b = serial.read();
		if (b == 10)
			continue;
		if (b == 13) {
			remotexy.bufferAT[remotexy.bufferATPos] = 0;
			remotexy.bufferATPos = 0;
			if (!cmpBufferAT())
				return;
		} else {
			if (remotexy.bufferATPos < AT_BUFFER_STR_LENGTH)
				remotexy.bufferAT[remotexy.bufferATPos++] = b;
			if (b == ':') {
				remotexy.bufferAT[remotexy.bufferATPos] = 0;
				if (strcmpAT(remotexy.bufferAT, AT_MESSAGE_IPD) == 0) {
					remotexy.bufferATPos = 0;
					ESP8266_RemoteXY_InputDataAT();
					return;
				}
			}
		}
	}
}

const char * cmpBufferAT(void)
{
	if (strcmpAT(remotexy.bufferAT, AT_MESSAGE_CONNECT) == 0) {
		ESP8266_RemoteXY_ConnectAT();
		return AT_MESSAGE_CONNECT;
	}
	if (strcmpAT(remotexy.bufferAT, AT_MESSAGE_CLOSED) == 0) {
		ESP8266_RemoteXY_ClosedAT();
		return AT_MESSAGE_CLOSED;
	}
	if (strcmpAT(remotexy.bufferAT, AT_MESSAGE_CONNECT_FAIL) == 0) {
		ESP8266_RemoteXY_ClosedAT();
		return AT_MESSAGE_CONNECT_FAIL;
	}
	if (strcmpAT(remotexy.bufferAT, AT_MESSAGE_READY) == 0) {
		ESP8266_RemoteXY_ReadyAT();
		return AT_MESSAGE_READY;
	}
	return 0;
}

uint8_t strcmpAT(char * str, const char * temp)
{
	uint8_t k = 0;
	while (*temp) {
		if (!*str)
			return 1;
		switch (*temp) {
		case '?':
			remotexy.params[k] = str;
			remotexy.paramsLength[k] = 1;
			temp++;
			str++;
			k++;
			break;
		case '*':
			remotexy.params[k] = str;
			remotexy.paramsLength[k] = 0;
			temp++;
			while (*str != *temp) {
				if (!*str++)
					return 1;
				remotexy.paramsLength[k]++;
			}
			k++;
			break;
		default:
			if (*(str++) != *(temp++))
				return 1;
			break;
		}
	}
	if (*temp)
		return 1;
	return 0;
}

uint16_t getATParamInt(uint8_t k)
{
	uint16_t res = 0;
	char * p = remotexy.params[k];
	uint8_t i = remotexy.paramsLength[k];
	while (i--)
		res = res * 10 + (*p++) - '0';
	return res;
}

void ESP8266_Serial_Init(void)
{
	serial.write = &UartWrite;
	serial.read = &UartRead;
	serial.available = &UartAvailable;
	serial.find = &UartFind;
	serial.println = &UartPrintLn;
	serial.print = &UartPrint;
}

void UartWrite(uint8_t d)
{
	Uart_write(d);
}

uint8_t UartRead(void)
{
	return (uint8_t) Uart_read();
}

uint8_t UartAvailable(void)
{
	return (uint8_t) IsDataAvailable();
}

uint8_t UartFind(char *str)
{
	return (uint8_t) Look_for(str, (char*)_rx_buffer->buffer);
}

void UartPrintLn(char *str)
{
	while(*str!='\0')
	{
		Uart_write(*str++);
	}
	UartWrite('\r');
	UartWrite('\n');
}

void UartPrint(char *str)
{
	while(*str!='\0')
	{
		Uart_write(*str++);
	}
}

void UartPrintCharArray(char *str, uint8_t Length)
{
	uint16_t i=0;
	while(i<Length)
	{
		Uart_write(*(str+i));
		i++;
	}
}

char* rxy_itos(uint16_t i, char* s)
{
	uint8_t len = 0;
	char *p = s + 5;
	while (i) {
		*p-- = i % 10 + '0';
		i /= 10;
		len++;
	}
	for (i = 0; i < len; i++)
		*s++ = *(++p);
	*s = 0;
	return s;
}

uint8_t rxy_xctoi(char c)
{
	uint8_t b;
	if (c <= '9')
		b = c - '0';
	else if (c <= 'f')
		b = c - 'A' + 10;
	else
		b = c - 'a' + 10;
	return b & 0x0f;
}

void rxy_getMacAddr(char* s, uint8_t* m)
{
	uint8_t i, b;
	for (i = 6; i > 0; i--) {
		b = rxy_xctoi(*s++) << 4;
		b |= rxy_xctoi(*s++);
		*m++ = b;
		s++;
	}
}

void ESP8266_RemoteXY_InitRemoteXY(const void * _conf, void * _var, const char * _accessPassword) {
	uint8_t i;
	uint8_t* p = (uint8_t*) _conf;
	uint8_t b = ESP8266_RemoteXY_GetConfByte(p++);
	if (b == 0xff) {
		remotexy.inputLength = ESP8266_RemoteXY_GetConfByte(p++);
		remotexy.inputLength |= ESP8266_RemoteXY_GetConfByte(p++) << 8;
		remotexy.outputLength = ESP8266_RemoteXY_GetConfByte(p++);
		remotexy.outputLength |= ESP8266_RemoteXY_GetConfByte(p++) << 8;
	} else {
		remotexy.inputLength = b;
		remotexy.outputLength = ESP8266_RemoteXY_GetConfByte(p++);
	}
	remotexy.confLength = ESP8266_RemoteXY_GetConfByte(p++);
	remotexy.confLength |= ESP8266_RemoteXY_GetConfByte(p++) << 8;
	remotexy.conf = p;
	remotexy.confVersion = ESP8266_RemoteXY_GetConfByte(p);
	remotexy.var = (uint8_t*) _var;
	uint16_t varLength = remotexy.outputLength + remotexy.inputLength;
	remotexy.connect_flag = remotexy.var + varLength;
	*remotexy.connect_flag = 0;

	remotexy.accessPassword = (uint8_t*) _accessPassword;

	remotexy.receiveBufferLength = remotexy.inputLength;
	if ((*remotexy.accessPassword != 0)
			&& (remotexy.receiveBufferLength < REMOTEXY_PASSWORD_LENGTH_MAX))
		remotexy.receiveBufferLength = REMOTEXY_PASSWORD_LENGTH_MAX;
	remotexy.receiveBufferLength += 6;

	remotexy.receiveBuffer = (uint8_t*) malloc(remotexy.receiveBufferLength);

	p = remotexy.var;
	i = varLength;
	while (i--)
		*p++ = 0;

	ESP8266_RemoteXY_ResetWire();

	remotexy.moduleRunning = ESP8266_RemoteXY_InitModule();
}

uint8_t ESP8266_RemoteXY_GetConfByte(uint8_t* p) {
	return *(p);
}

void ESP8266_RemoteXY_ResetWire(void) {
	remotexy.receiveIndex = 0;
	remotexy.receiveCRC = ESP8266_RemoteXY_InitCRC();
	*remotexy.connect_flag = 0;
	remotexy.inputVarNeedSend = 1;
	remotexy.wireTimeOut = HAL_GetTick();
}

void ESP8266_RemoteXY_Handler(void)
{
	uint8_t *p;
	uint16_t i;
	uint8_t b;
	uint16_t packageLength;

	if (!remotexy.moduleRunning)
		return;

	ESP8266_RemoteXY_HandlerModule();

	while (ESP8266_RemoteXY_AvailableByte() > 0) {
		b = ESP8266_RemoteXY_ReceiveByte();

		if ((remotexy.receiveIndex == 0) && (b != REMOTEXY_PACKAGE_START_BYTE))
			continue;
		remotexy.receiveBuffer[remotexy.receiveIndex++] = b;
		ESP8266_RemoteXY_UpdateCRC(&remotexy.receiveCRC, b);
		if (remotexy.receiveIndex > remotexy.receiveBufferLength) {
			ESP8266_RemoteXY_SearchStartByte(1); //remotexy.receiveBuffer overflow
		}
		while (true) {
			if (remotexy.receiveIndex < 6)
				break;
			packageLength = remotexy.receiveBuffer[1]
					| (remotexy.receiveBuffer[2] << 8);
			if (packageLength > remotexy.receiveBufferLength)
				ESP8266_RemoteXY_SearchStartByte(1); // error
			else if (packageLength < 6)
				ESP8266_RemoteXY_SearchStartByte(1); // error
			else if (packageLength == remotexy.receiveIndex) {
				if (remotexy.receiveCRC == 0) {
					if (ESP8266_RemoteXY_HandleReceivePackage()) {
						remotexy.receiveIndex = 0;
						remotexy.receiveCRC = ESP8266_RemoteXY_InitCRC();
						break;
					}
				}
				ESP8266_RemoteXY_SearchStartByte(1); // error
			} else if (packageLength < remotexy.receiveIndex) {
				uint16_t crc = ESP8266_RemoteXY_InitCRC();
				p = remotexy.receiveBuffer;
				i = packageLength;
				while (i--)
					ESP8266_RemoteXY_UpdateCRC(&crc, *(p++));
				if (crc == 0) {
					if (ESP8266_RemoteXY_HandleReceivePackage()) {
						ESP8266_RemoteXY_SearchStartByte(packageLength);
						continue;
					}
				}
				ESP8266_RemoteXY_SearchStartByte(1);
			} else
				break;
		}
	}

	if (HAL_GetTick() - remotexy.wireTimeOut > REMOTEXY_TIMEOUT) {
		ESP8266_RemoteXY_ResetWire();
	}
}

uint16_t ESP8266_RemoteXY_InitCRC(void)
{
	return 0xffff;
}

void ESP8266_RemoteXY_UpdateCRC(uint16_t *crc, uint8_t b)
{
	*crc ^= b;
	for (uint8_t i = 0; i < 8; ++i) {
		if ((*crc) & 1)
			*crc = ((*crc) >> 1) ^ 0xA001;
		else
			*crc >>= 1;
	}
}

void ESP8266_RemoteXY_SendByteUpdateCRC(uint8_t b, uint16_t *crc)
{
	ESP8266_RemoteXY_SendByte(b);
	ESP8266_RemoteXY_UpdateCRC(crc, b);
}

void ESP8266_RemoteXY_SendPackage(uint8_t command, uint8_t *p, uint16_t length, uint8_t itConf)
{
	uint16_t crc = ESP8266_RemoteXY_InitCRC();
	uint16_t packageLength = length + 6;
	ESP8266_RemoteXY_SendStart(packageLength);
	ESP8266_RemoteXY_SendByteUpdateCRC(REMOTEXY_PACKAGE_START_BYTE, &crc);
	ESP8266_RemoteXY_SendByteUpdateCRC(packageLength, &crc);
	ESP8266_RemoteXY_SendByteUpdateCRC(packageLength >> 8, &crc);
	ESP8266_RemoteXY_SendByteUpdateCRC(command, &crc);
	uint8_t b;
	while (length--) {
		if (itConf)
			b = ESP8266_RemoteXY_GetConfByte(p++);
		else
			b = *p++;
		ESP8266_RemoteXY_SendByteUpdateCRC(b, &crc);
	}
	ESP8266_RemoteXY_SendByte(crc);
	ESP8266_RemoteXY_SendByte(crc >> 8);
}

void ESP8266_RemoteXY_SearchStartByte(uint16_t pos)
{
	uint8_t *p, *kp;
	uint16_t i;
	uint16_t ri = remotexy.receiveIndex;
	p = remotexy.receiveBuffer + pos;
	remotexy.receiveCRC = ESP8266_RemoteXY_InitCRC();
	for (i = pos; i < ri; i++) {
		if (*p == REMOTEXY_PACKAGE_START_BYTE) {
			kp = remotexy.receiveBuffer;
			remotexy.receiveIndex = remotexy.receiveIndex - i;
			while (i++ < ri) {
				ESP8266_RemoteXY_UpdateCRC(&remotexy.receiveCRC, *p);
				*(kp++) = *(p++);
			}
			return;
		}
		p++;
	}
	remotexy.receiveIndex = 0;
}

uint8_t ESP8266_RemoteXY_HandleReceivePackage(void)
{
	uint8_t command;
	uint16_t i;
	uint16_t length;
	uint8_t *p, *kp;
	uint8_t available;

	length = remotexy.receiveBuffer[1] | (remotexy.receiveBuffer[2] >> 8);
	length -= 6;
	command = remotexy.receiveBuffer[3];
	switch (command) {
	case 0x00:

		if (length == 0) {
			if (*remotexy.accessPassword == 0)
				available = 1;
			else
				available = 0;
		} else {
			uint8_t ch;
			available = 1;
			p = remotexy.receiveBuffer + 4;
			kp = remotexy.accessPassword;
			while (true) {
				ch = *kp++;
				if (ch != *p++)
					available = 0;
				if (!ch)
					break;
			}
		}
		if (available != 0) {
			ESP8266_RemoteXY_SendPackage(command, remotexy.conf, remotexy.confLength, 1);
			*remotexy.connect_flag = 1;
		} else {
			uint8_t buf[4];
			p = buf;
			kp = remotexy.conf;
			i = remotexy.confVersion >= 5 ? 3 : 2;
			length = i + 1;
			while (i--)
				*p++ = ESP8266_RemoteXY_GetConfByte(kp++);
			*p++ = 0xf0;
			ESP8266_RemoteXY_SendPackage(command, buf, length, 0);
		}
		break;
	case 0x40:
		remotexy.inputVarNeedSend = 0;
		ESP8266_RemoteXY_SendPackage(command, remotexy.var,
				remotexy.inputLength + remotexy.outputLength, 0);
		break;
	case 0x80:
		if ((length == remotexy.inputLength)
				&& (remotexy.inputVarNeedSend == 0)) {
			p = remotexy.receiveBuffer + 4;
			kp = remotexy.var;
			i = remotexy.inputLength;
			while (i--)
				*kp++ = *p++;
		}
		ESP8266_RemoteXY_SendPackage(command, 0, 0, 0);
		break;
	case 0xC0:
		if (remotexy.inputVarNeedSend != 0)
			command |= 0x01;
		ESP8266_RemoteXY_SendPackage(command, remotexy.var + remotexy.inputLength,
				remotexy.outputLength, 0);
		break;
	default:
		return 0;
	}

	remotexy.wireTimeOut = HAL_GetTick();

	return 1;
}

uint8_t ESP8266_RemoteXY_IsConnected(void)
{
	return *remotexy.connect_flag;
}

// transmit the input vars to smartphone, while input variables are transmitted the output variables are not sent

void ESP8266_RemoteXY_SendInputVariables(void)
{
	remotexy.inputVarNeedSend = 1;
}

//check if input variables were sent, return 1 if sent

uint8_t ESP8266_RemoteXY_DidSendInputVariables(void)
{
	if (remotexy.inputVarNeedSend == 0)
		return 1;
	else
		return 0;
}

/**************************************************************************************/
