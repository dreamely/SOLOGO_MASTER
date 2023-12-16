
#include "main.h"

uint8_t rx_dma_buf[LINE_BUFFER_MAX];
extern COM_RING_BUFF Com3Buf;
extern UART_HandleTypeDef huart3;


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
#else
		
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
#endif
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


