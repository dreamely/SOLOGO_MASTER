/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

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
I2C_HandleTypeDef hi2c1;

IWDG_HandleTypeDef hiwdg;

TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_rx;

/* USER CODE BEGIN PV */
uint32_t systemTick = 0;
volatile uint32_t SYSTICK_Chk[SYSTICK_NUM_INT];	
uint8_t rx_dma_buf[LINE_BUFFER_MAX];
extern COM_RING_BUFF Com3Buf;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_IWDG_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint32_t microGetSystemTime(void);
void BSP_Init(void);

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

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
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_IWDG_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

	if(__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) == SET) {
		Printf_("Watch dog reset\r\n");
		 
		__HAL_RCC_CLEAR_RESET_FLAGS();
	}
	else	 {
	  Printf_("Power on reset\r\n");
	}

	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
	//__HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE);
	//__HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart3, rx_dma_buf, LINE_BUFFER_MAX);

	HAL_TIM_Base_Start_IT(&htim3);

	BSP_Init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		HAL_IWDG_Refresh(&hiwdg);

		applicationTick();
		Terminal_Services_Function();
		//processUart2Input();
		LORA_ServiceFunction();
		Modbus_Event();
		UART_RX_DMA_Handler();

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief IWDG Initialization Function
  * @param None
  * @retval None
  */
static void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */

  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
  hiwdg.Init.Reload = 4095;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */

  /* USER CODE END IWDG_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 84-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 1000;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

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
  huart2.Init.BaudRate = 9600;
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
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, RUN_LED_Pin|LORA_RESET_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, RTC_SCK_Pin|RTC_RST_Pin|TXD_LED_Pin|RXD_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : RUN_LED_Pin LORA_RESET_Pin */
  GPIO_InitStruct.Pin = RUN_LED_Pin|LORA_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : RTC_SCK_Pin RTC_RST_Pin TXD_LED_Pin RXD_LED_Pin */
  GPIO_InitStruct.Pin = RTC_SCK_Pin|RTC_RST_Pin|TXD_LED_Pin|RXD_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : RTC_IO_Pin */
  GPIO_InitStruct.Pin = RTC_IO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(RTC_IO_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
//********************************************************************
// USER APP INIT
//********************************************************************
void BSP_Init(void)
{
	TRX_LedInit();
	compileInfo(0,0);
	UartInit();
	LORA_BuffInit();
	loraGetBinding(BINDING_TABLE_SIZE);
	Modbus_Init();
}

//*************************************************************************
//EXIT timer set
//*************************************************************************
void ExitTimerSet(void)
{
  SYSTICK_Chk[SYSTICK_Delay_0] = 60000*1; //60000*1 
}

//*************************************************************************
//EXIT timer exit 
//*************************************************************************
uint8_t ExitTimerChekck(void)
{
  if(SYSTICK_Chk[SYSTICK_Delay_0] == 0) 
		return 1;
  else                                    
		return 0;
}

//********************************************************************
// SYSTEM TICK 
//********************************************************************
uint32_t microGetSystemTime(void)
{
  return systemTick;
}

//*************************************************************************
// EXIT timer reduce
//*************************************************************************
void SysTick_TimingDecrement(void)
{
	uint8_t  i;	
	for ( i = 0 ; i < SYSTICK_NUM_INT ; i++) {
		if (SYSTICK_Chk[i] != 0 ) {
			SYSTICK_Chk[i]--;
		}
	}		 
}

void HAL_SYSTICK_Callback(void)
{
	systemTick++;

	SysTick_TimingDecrement();
}

void HAL_3usDelay(uint16_t us)
{
	uint16_t i;
	uint16_t j;

	for(i=0; i<us; i++) {
		for(j=0; j<1; j++);
	}
}

void HAL_WatchDogReload(void)
{
	HAL_IWDG_Refresh(&hiwdg);
}

//*************************************************************************
// DMA 버퍼에 채워진 데이터의 위치를 계산하기 위한 매크로
//*************************************************************************
#define get_dma_data_length() huart3.hdmarx->Instance->NDTR
#define get_dma_total_size()  huart3.RxXferSize

#if 0
//*************************************************************************
// UART3 DMA RX TC INTERRUPT 
// Transfer Complete DMA event/flag
//*************************************************************************
uint16_t _HT_Count = 0;
uint16_t _TC_Count = 0;
uint16_t _IDLE_Count = 0;
uint8_t rcvFlag = 0;

//위에서 RxBuffer는 UART DMA RX 버퍼이고, wrPtr, rdPtr은 RxBuffer내의 valid한 데이터의 끝과 시작 위치를 가리킨다.

//RxBuf3은 UART3이 수신한 데이터를 저장하는 사용자 버퍼이다.

//다음은 UART IDLE Interrupt를 Enable하고 UART DMA RX를 개시하는 코드이다.
//main 에 선언
__HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);
HAL_UART_Receive_DMA(&huart3, RxBuffer, MAX_RX_BUF + 16);

//다음으로 UART DMA RX의 Interrupt 처리 루틴을 추가한다. HT Interrupt 처리를 위한 HAL_UART_RxHalfCpltCallback(), TC Interrupt 처리를 위한 HAL_UART_RxCpltCallback(), UART IDLE Interrupt 처리를 위한 UART_IDLECallback() 함수를 다음과 같이 구성한다.

//main.c에 HAL_UART_RxHalfCpltCallback() 함수를 만들어 둔다.

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	_TC_Count++;
	rcvFlag = 1;
}

//*************************************************************************
// UART3 DMA RX HT INTERRUPT 
// Half-Transfer Complete DMA event/flag
//*************************************************************************
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
	_HT_Count++;
	rcvFlag = 1;
}

//*************************************************************************
// UART3 DMA RX IDLE INTERRUPT 
//*************************************************************************
void UART_IDLECallback(UART_HandleTypeDef *huart)
{
	_IDLE_Count++;
	rcvFlag = 1;
}

//위 두 개 함수는 weak 함수로 HAL 라이브러리에서 선언되어 있어서 따로 지정할 필요가 없지만 IDLE Interrupt 처리를 위한 UART_IDLECallback()은 함수명을 임의로 지정해도 된다.

//위 세개의 인터럽트 처리를 위한 Callback함수에서 인터럽트가 발생했다는 Flag(rcvFlag)를 설정하면, while() 루프내에서 UART_Data_Process를 호출하는데, 이 함수는 UART DMA RX 버퍼내의 수신 데이터를 사용자 버퍼로 복사하는 기능을 한다.

void UART_Data_Process(UART_HandleTypeDef *huart)
{
	wrPtr = ARRAY_LEN(RxBuffer) - huart->hdmarx->Instance->CNDTR;
	if(wrPtr != rdPtr)
	{
		memset(RxBuf3, 0, MAX_RX_BUF + 16);

		if (wrPtr > rdPtr)
		{
			rcvdLen = wrPtr - rdPtr;
			memcpy(RxBuf3, RxBuffer + rdPtr, rcvdLen);
		}else
		{
			rcvdLen = ARRAY_LEN(RxBuffer) - rdPtr;
			memcpy(RxBuf3, RxBuffer + rdPtr, rcvdLen);
			if(wrPtr > 0)
			{
				rcvdLen += wrPtr;
				memcpy(RxBuf3 + rcvdLen, RxBuffer, wrPtr);
			}
		}
		rdPtr = wrPtr;
	}
}

//다음은 UART IDLE Interrupt 발생시에 인터럽트 서비스 루틴에서 IDLE 인터럽트를 클리어하고 위에 있는 UART_IDLECallback 이 수행되도록 처리한다.

void USART3_IRQHandler(void)
{
  /* USER CODE BEGIN USART3_IRQn 0 */
  if(__HAL_UART_GET_FLAG(&huart3, UART_FLAG_IDLE) != RESET)
  {
	  __HAL_UART_CLEAR_IDLEFLAG(&huart3);
	  UART_IDLECallback(&huart3);
  }
  /* USER CODE END USART3_IRQn 0 */
  HAL_UART_IRQHandler(&huart3);
  /* USER CODE BEGIN USART3_IRQn 1 */

  /* USER CODE END USART3_IRQn 1 */
}

//다음은 main.c내의 while() 루프에서 1초마다 UART4 포트로 DMA TX 전송을 하는 것과 수신이 발생했는지를 체크하는 코드를 추가한다.

void main() {
if(rcvFlag)
	{
		rcvFlag = 0;
		UART_Data_Process(&huart3);
		totalRcvdBytes += rcvdLen;
		printf("RxBuf3: [%d] bytes, %s\r\n", strlen(RxBuf3), RxBuf3);
		printf("_HT_Count: %d, _TC_Count: %d, _IDLE_Count: %d, rdPtr: %d, wrPtr: %d, rcvdLen: %d, totalSentBytes: %d, totalRcvdBytes: %d\r\n",
				_HT_Count, _TC_Count, _IDLE_Count, rdPtr, wrPtr, rcvdLen, totalSentBytes, totalRcvdBytes);
	}
}
#endif

void UART_RX_DMA_Handler(void)
{
	//dma 버퍼에서 이미 가져온 데이터의 위치를 저장할 변수
	static uint32_t old_position = 0;
	//dma 버퍼에 현재 데이터가 채워진 위치를 저장할 변수
	uint32_t new_position;
	uint16_t i;

	//dma 버퍼에서 가져다 저장할 변수
	uint8_t rx_buf[LINE_BUFFER_MAX];
	uint32_t rx_size;

	//가져온 데이터를 출력하기 위한 변수들
	uint8_t ch;

	//DMA 버퍼에서 가져온 데이터의 크기를 저장할 변수
	rx_size = 0;

	//dma 버퍼에 데이터가 채워진 위치 얻기
	new_position = get_dma_total_size() - get_dma_data_length();

	//이전 위치와 현재 위치가 다르다면... (처리할 데이터가 있다)
	if(new_position != old_position) {
		//현재 위치가 이전에 저장한 위치보다 크다면 보통의 상황으로 이전 위치부터 
		//현재위치까지 DMA 버퍼에서 데이터 가져옴.
		if(new_position > old_position) {

			memcpy(rx_buf, &rx_dma_buf[old_position], new_position - old_position);
		
			// DMA 버퍼에서 가져온 데이터 크기 저장
			rx_size += new_position - old_position;
		}
		else {
			//현재 위치가 이전 위치보다 아래에 존재한다면 DMA 버퍼가 
			//한바퀴가 넘어간 상태(Overflow)
			//일단 이전에 가져온 위치부터 버퍼의 끝까지 모두 가져온다.
			memcpy(rx_buf, &rx_dma_buf[old_position], get_dma_total_size() - old_position);
			// 가져온 데이터 크기 저장.
			rx_size += get_dma_total_size() - old_position;

			//끝까지 다 가져왔는데 
			//현재 위치가 0보다 크다면 추가로 가져가야할 것이 있다는 얘기임
			//DMA 버퍼의 0번째부터 현재 위치까지 추가로 가져옴.
			if(new_position > 0){
				//rx_buf 위치 주의!!
				memcpy(&rx_buf[get_dma_total_size() - old_position], &rx_dma_buf[0], new_position);
				// 추가로 가져온 데이터 크기 추가
				rx_size += new_position;
			}
		}
		//현재 위치를 이전위치로 변경
		old_position = new_position;

#if 0
		//가져온 데이터가 있다면
		if(rx_size > 0) {
			//크기 만큼 루프 출력!
			for(i = 0;i<rx_size;i++) {
				ch = rx_buf[i];
				Printf_("%c", ch);
				// 할 것! (to do?)

			}
			//Printf_("\r\n");
		}
#endif
		
		if(rx_size > 0) {
			for(i = 0;i<rx_size;i++) {
				//store data in rx ring buffer
				 Com3Buf.rx_buf[Com3Buf.tail++] = rx_buf[i];
				
				//ring line count increase
				Com3Buf.buffering_cnt++;
				//if buffer fill, ring line count is 0
				if(Com3Buf.tail >= LINE_BUFFER_MAX) Com3Buf.tail = 0;  
			}
		}
	}	
}

#if 0
if ((__HAL_UART_GET_FLAG(&huart3, UART_FLAG_RXNE) != RESET) && 
		(__HAL_UART_GET_IT_SOURCE(&huart3, UART_IT_RXNE) != RESET)) { 

	//store data in rx ring buffer
	 Com3Buf.rx_buf[Com3Buf.tail++] = (uint8_t)(huart3.Instance->DR & (uint8_t)0x00FF);
	
	//ring line count increase
	Com3Buf.buffering_cnt++;
	//if buffer fill, ring line count is 0
	if(Com3Buf.tail >= LINE_BUFFER_MAX) Com3Buf.tail = 0;  
}
#endif

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
