/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "string.h"
#include "stdlib.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
#define DEBUG 1
#define AT_COMMAND_BUFFER_LENGTH 255
#define UART_RECEIVE_BUFFER_LENGTH 2048

uint8_t uartRxByte[1];
uint16_t i=0,j=0;
uint8_t *ptr;
uint8_t  c;
uint8_t AT_command[AT_COMMAND_BUFFER_LENGTH];
uint8_t uartReceive[UART_RECEIVE_BUFFER_LENGTH];
uint8_t uartReceiveEnd=0;
uint8_t HTTP_response[]="HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: Closed\r\n\r\n\r\n";
HAL_StatusTypeDef res;
uint8_t handleData=0;
uint8_t HTTP_request_length_string[10];
uint16_t HTTP_request_length=0;
uint8_t HTTP_request_length_valid=0;

extern uint8_t webpage[1271];


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	InitWebpage();


  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  	//HAL_UART_Receive_IT(&huart1, uartRxByte, 1);
	#ifdef DEBUG
		debugPrintf("\r\n");
		debugPrintf("\r\n");
		debugPrintf("######################\r\n");
		debugPrintf("######################\r\n");
		debugPrintf("######################\r\n");
	#endif

	//restore
	/*sprintf(AT_command,"%s","AT+RESTORE\r\n");
	HAL_UART_Transmit(&huart1,AT_command,strlen(AT_command),100);
	HAL_UART_Receive(&huart1,uartReceive,1024,100);
	//waitForUartResponse();
	//uartReceiveEnd=0;
	debugPrintf("ESP8266 restored to default\r\n");
	//debugPrintf(uartReceive);
	debugPrintf("-------------\r\n");*/

	//set uart parameters
	/*sprintf(AT_command,"%s","AT+UART_CUR=115200,8,1,0,0\r\n");
	HAL_UART_Transmit(&huart1,AT_command,strlen(AT_command),100);
	HAL_UART_Receive(&huart1,uartReceive,1024,100);
	//waitForUartResponse();
	//uartReceiveEnd=0;
	debugPrintf("ESP8266 uart parameters set\r\n");
	//debugPrintf(uartReceive);
	debugPrintf("-------------\r\n");*/

	//HAL_Delay(5000);

	//ATE0 switch off echo
	sprintf(AT_command,"%s","ATE0\r\n");
	HAL_UART_Transmit(&huart1,AT_command,strlen(AT_command),100);
	HAL_UART_Receive(&huart1,uartReceive,UART_RECEIVE_BUFFER_LENGTH,100);
	#ifdef DEBUG
		debugPrintf("ESP8266 echo switched off\r\n");
		//debugPrintf(uartReceive);
		debugPrintf("-------------\r\n");
	#endif


	//set wifi mode to Station mode (to be able to connect to a wifi router )
	sprintf(AT_command,"%s","AT+CWMODE_CUR=1\r\n");
	HAL_UART_Transmit(&huart1,AT_command,strlen(AT_command),100);
	HAL_UART_Receive(&huart1,uartReceive,UART_RECEIVE_BUFFER_LENGTH,100);
	#ifdef DEBUG
		debugPrintf("wifi mode set to Station mode: ");
		debugPrintf(uartReceive);
		debugPrintf("-------------\r\n");
	#endif

	//connect to a wifi network
	sprintf(AT_command,"%s%c%s%c%c%c%s%c%s","AT+CWJAP_CUR=",'"',"WIFI_network_name",'"',',','"',"WIFI_password",'"',"\r\n");  //replace WIFI_network_name and WIFI_password with your wifi network name and your password
	HAL_UART_Transmit(&huart1,AT_command,strlen(AT_command),100);
	debugPrintf("connecting to network...\r\n");
	HAL_UART_Receive(&huart1,uartReceive,UART_RECEIVE_BUFFER_LENGTH,10000); //give 10 seconds to connect to the wifi router
	#ifdef DEBUG
		debugPrintf(uartReceive);
		debugPrintf("-------------\r\n");
	#endif

	//get the mac address of ESP8266
	/*sprintf(AT_command,"%s","AT+CIPSTAMAC_CUR?\r\n");
	HAL_UART_Transmit(&huart1,AT_command,strlen(AT_command),100);
	HAL_UART_Receive(&huart1,uartReceive,255,100);
	debugPrintf("ESP8266 mac address: ");
	debugPrintf(uartReceive);
	debugPrintf("-------------\r\n");*/

	//get ip address of ESP8266
	sprintf(AT_command,"%s","AT+CIFSR\r\n");
	HAL_UART_Transmit(&huart1,AT_command,strlen(AT_command),100);
	HAL_UART_Receive(&huart1,uartReceive,UART_RECEIVE_BUFFER_LENGTH,100);
	#ifdef DEBUG
		debugPrintf("ESP8266 ip address: ");
		debugPrintf(uartReceive);
		debugPrintf("-------------\r\n");
	#endif

	//enable multiple connections
	sprintf(AT_command,"%s","AT+CIPMUX=1\r\n");
	HAL_UART_Transmit(&huart1,AT_command,strlen(AT_command),100);
	HAL_UART_Receive(&huart1,uartReceive,UART_RECEIVE_BUFFER_LENGTH,100);
	#ifdef DEBUG
		debugPrintf("single connection enabled: ");
		debugPrintf(uartReceive);
		debugPrintf("-------------\r\n");
	#endif

	//create server on port 80
	sprintf(AT_command,"%s","AT+CIPSERVER=1,80\r\n");
	HAL_UART_Transmit(&huart1,AT_command,strlen(AT_command),100);
	HAL_UART_Receive(&huart1,uartReceive,UART_RECEIVE_BUFFER_LENGTH,100);
	#ifdef DEBUG
		debugPrintf("Server started on port 80: ");
		debugPrintf(uartReceive);
		debugPrintf("-------------\r\n");
	#endif

	//start listening for HTTP requests
	HAL_UART_Receive_IT(&huart1, uartRxByte, 1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  /*HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_5);
	  HAL_Delay(200);*/

	  if (handleData==1){
		  debugPrintf(uartReceive);
		  debugPrintf("\r\n");
		  debugPrintf("\r\nhandling received data\r\n");
		  HandleReceivedData();
		  handleData=0;
		  HAL_UART_Receive_IT(&huart1, uartRxByte, 1);
	  }

	 // HAL_Delay(100);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /* Set transmission flag: transfer complete */
  //UartReady = SET;
	//HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);


//	 -------------------------------------------------
//	 	 If the esp8266 gets a reuest on port 80 it will send the incoming TCP data to the microcontorller with a header:
//		example:
//
//		+IPD,0,321:GET / HTTP/1.1
//		Host: 192.168.1.104
//		User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:66.0) Gecko/20100101 Firefox/66.0
//		Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
//		Accept-Language: en-US,en;q=0.5
//		Accept-Encoding: gzip, deflate
//		Connection: keep-alive
//		Upgrade-I..........

// 	The header is in the first line. +IPD,0,321 --> 0 is the link id, 321 is the length of the TCP data
//
//	 ---------------------------------------------------

	uartReceive[i]=uartRxByte[0];

	//if we got the : character we have the header so we can extract the length of the TCP data
	if (uartReceive[i]==':' && HTTP_request_length_valid==0){
		//find +IPD text in the string
		ptr=strstr(uartReceive,"+IPD");
		ptr+=7; //jump to the length in the string
		j=0;
		while ((*ptr) != ':'){
			HTTP_request_length_string[j]=*ptr;
			j++;
			ptr++;
		}
		HTTP_request_length_string[j]='\0';
		HTTP_request_length=atoi(HTTP_request_length_string); //convert string to int
		HTTP_request_length_valid=1;
	}

	if (HTTP_request_length==i && HTTP_request_length_valid==1){
			uartReceive[i+1]='\0'; //string end character
			i=0;
			handleData=1;
			HTTP_request_length_valid=0;
	}
	else{
			i++;
			HAL_UART_Receive_IT(&huart1, uartRxByte, 1);
	}

  /* Turn LED4 on: Transfer in reception process is correct */
  //BSP_LED_On(LED4);
}

void debugPrintf(uint8_t *msg){
	HAL_UART_Transmit(&huart2,msg,strlen(msg),100);
}

void HandleReceivedData(){
	int j;
	uint8_t *ptr;
	uint8_t some_response[255];

	//If we can find a special command in the request then we don't have to send the full webpage again. Just send some response.
	//in our case this special command will be MYCOMMAND because if we click on a button in the html page we send this command first

	//When we click on the button on the webpage the HTTP request will look like this:
	//	+IPD,0,305:GET /MYCOMMAND_LED=0&nocache=210479.98243957412 HTTP/1.1
	//	Host: 192.168.1.104
	//	User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:66.0) Gecko/20100101 Firefox/66.0
	//	Accept: */*
	//	Accept-Language: en-US,en;q=0.5
	//	Accept-Encoding: gzip, deflate
	//	Referer: http://192.168.1.104/
	//	Con..............


	ptr=strstr(uartReceive,"MYCOMMAND");

	if (ptr!=NULL){ //we found the MYCOMMAND in the request
		// if we can found the LED=1 string in the request then we have to turn on the led
		ptr=strstr(uartReceive,"LED=1");
		if (ptr!=NULL){ //we have found it, turn on the led
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);
			//send back a response that the LED is on
			sprintf(some_response,"%s","The led is on");
			sprintf(AT_command,"%s%d%s","AT+CIPSEND=0,",strlen(some_response),"\r\n"); //CIPSEND can send max 2048 bytes
			HAL_UART_Transmit(&huart1,AT_command,strlen(AT_command),100);
			HAL_UART_Receive(&huart1,uartReceive,UART_RECEIVE_BUFFER_LENGTH,100);
			#ifdef DEBUG
				debugPrintf("start sending data ");
				debugPrintf(uartReceive);
				debugPrintf("-------------\r\n");
			#endif
			HAL_UART_Transmit(&huart1,some_response,strlen(some_response),100); //send the response that the led is on
			HAL_UART_Receive(&huart1,uartReceive,UART_RECEIVE_BUFFER_LENGTH,500);

			//give some time to send the data
			//debugPrintf("-------------\r\n");
			HAL_Delay(100);

			//close connection
			sprintf(AT_command,"%s","AT+CIPCLOSE=0\r\n");
			HAL_UART_Transmit(&huart1,AT_command,strlen(AT_command),100);
			HAL_UART_Receive(&huart1,uartReceive,UART_RECEIVE_BUFFER_LENGTH,500);
			#ifdef DEBUG
				debugPrintf("close the connection ");
				debugPrintf(uartReceive);
				debugPrintf("-------------\r\n");
			#endif
		}

		// if we can found the LED=0 string in the request then we have to turn on the led
		ptr=strstr(uartReceive,"LED=0");
		if (ptr!=NULL){ //we have found it, turn off the led
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET);
			//send back a response that the LED is off
			sprintf(some_response,"%s","The led is off");
			sprintf(AT_command,"%s%d%s","AT+CIPSEND=0,",strlen(some_response),"\r\n"); //CIPSEND can send max 2048 bytes
			HAL_UART_Transmit(&huart1,AT_command,strlen(AT_command),100);
			HAL_UART_Receive(&huart1,uartReceive,UART_RECEIVE_BUFFER_LENGTH,100);
			#ifdef DEBUG
				debugPrintf("start sending data ");
				debugPrintf(uartReceive);
				debugPrintf("-------------\r\n");
			#endif
			HAL_UART_Transmit(&huart1,some_response,strlen(some_response),100); //send the response that the led is off
			HAL_UART_Receive(&huart1,uartReceive,UART_RECEIVE_BUFFER_LENGTH,500);

			//give some time to send the data
			//debugPrintf("-------------\r\n");
			HAL_Delay(100);

			//close connection
			sprintf(AT_command,"%s","AT+CIPCLOSE=0\r\n");
			HAL_UART_Transmit(&huart1,AT_command,strlen(AT_command),100);
			HAL_UART_Receive(&huart1,uartReceive,UART_RECEIVE_BUFFER_LENGTH,500);
			#ifdef DEBUG
				debugPrintf("close the connection ");
				debugPrintf(uartReceive);
				debugPrintf("-------------\r\n");
			#endif
		}


	}
	else{ //we have not found MYCOMMAND in the request, so it is a simple http request, we send the whole webpage
			sprintf(AT_command,"%s%d%s","AT+CIPSEND=0,",strlen(HTTP_response)+strlen(webpage),"\r\n"); //CIPSEND can send max 2048 bytes
			HAL_UART_Transmit(&huart1,AT_command,strlen(AT_command),100);
			HAL_UART_Receive(&huart1,uartReceive,UART_RECEIVE_BUFFER_LENGTH,100);
			#ifdef DEBUG
				debugPrintf("start sending data ");
				debugPrintf(uartReceive);
				debugPrintf("-------------\r\n");
			#endif
			HAL_UART_Transmit(&huart1,HTTP_response,strlen(HTTP_response),300); //send an OK http response
			HAL_UART_Transmit(&huart1,webpage,strlen(webpage),1000); //send the full webpage
			HAL_UART_Receive(&huart1,uartReceive,UART_RECEIVE_BUFFER_LENGTH,500);
			#ifdef DEBUG
				debugPrintf("data sent ");
				debugPrintf(uartReceive);
				debugPrintf("-------------\r\n");
			#endif

			for (j=0; j<1024;j++){
					uartReceive[j]='\0';
				}

			//give some time to send the data
			//debugPrintf("-------------\r\n");
			HAL_Delay(100);

			//close connection
			sprintf(AT_command,"%s","AT+CIPCLOSE=0\r\n");
			HAL_UART_Transmit(&huart1,AT_command,strlen(AT_command),100);
			HAL_UART_Receive(&huart1,uartReceive,UART_RECEIVE_BUFFER_LENGTH,1000);
			#ifdef DEBUG
				debugPrintf("close the connection ");
				debugPrintf(uartReceive);
				debugPrintf("-------------\r\n");
			#endif
	}

}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
