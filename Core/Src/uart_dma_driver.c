
#include "main.h"

uint8_t rx_dma_buf[LINE_BUFFER_MAX];
extern COM_RING_BUFF Com3Buf;
extern UART_HandleTypeDef huart3;


//*************************************************************************
// DMA ���ۿ� ä���� �������� ��ġ�� ����ϱ� ���� ��ũ��
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

//������ RxBuffer�� UART DMA RX �����̰�, wrPtr, rdPtr�� RxBuffer���� valid�� �������� ���� ���� ��ġ�� ����Ų��.

//RxBuf3�� UART3�� ������ �����͸� �����ϴ� ����� �����̴�.

//������ UART IDLE Interrupt�� Enable�ϰ� UART DMA RX�� �����ϴ� �ڵ��̴�.
//main �� ����
__HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);
HAL_UART_Receive_DMA(&huart3, RxBuffer, MAX_RX_BUF + 16);

//�������� UART DMA RX�� Interrupt ó�� ��ƾ�� �߰��Ѵ�. HT Interrupt ó���� ���� HAL_UART_RxHalfCpltCallback(), TC Interrupt ó���� ���� HAL_UART_RxCpltCallback(), UART IDLE Interrupt ó���� ���� UART_IDLECallback() �Լ��� ������ ���� �����Ѵ�.

//main.c�� HAL_UART_RxHalfCpltCallback() �Լ��� ����� �д�.

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

//�� �� �� �Լ��� weak �Լ��� HAL ���̺귯������ ����Ǿ� �־ ���� ������ �ʿ䰡 ������ IDLE Interrupt ó���� ���� UART_IDLECallback()�� �Լ����� ���Ƿ� �����ص� �ȴ�.

//�� ������ ���ͷ�Ʈ ó���� ���� Callback�Լ����� ���ͷ�Ʈ�� �߻��ߴٴ� Flag(rcvFlag)�� �����ϸ�, while() ���������� UART_Data_Process�� ȣ���ϴµ�, �� �Լ��� UART DMA RX ���۳��� ���� �����͸� ����� ���۷� �����ϴ� ����� �Ѵ�.

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

//������ UART IDLE Interrupt �߻��ÿ� ���ͷ�Ʈ ���� ��ƾ���� IDLE ���ͷ�Ʈ�� Ŭ�����ϰ� ���� �ִ� UART_IDLECallback �� ����ǵ��� ó���Ѵ�.

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

//������ main.c���� while() �������� 1�ʸ��� UART4 ��Ʈ�� DMA TX ������ �ϴ� �Ͱ� ������ �߻��ߴ����� üũ�ϴ� �ڵ带 �߰��Ѵ�.

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
	//dma ���ۿ��� �̹� ������ �������� ��ġ�� ������ ����
	static uint32_t old_position = 0;
	//dma ���ۿ� ���� �����Ͱ� ä���� ��ġ�� ������ ����
	uint32_t new_position;
	uint16_t i;

	//dma ���ۿ��� ������ ������ ����
	uint8_t rx_buf[LINE_BUFFER_MAX];
	uint32_t rx_size;

	//������ �����͸� ����ϱ� ���� ������
	uint8_t ch;

	//DMA ���ۿ��� ������ �������� ũ�⸦ ������ ����
	rx_size = 0;

	//dma ���ۿ� �����Ͱ� ä���� ��ġ ���
	new_position = get_dma_total_size() - get_dma_data_length();

	//���� ��ġ�� ���� ��ġ�� �ٸ��ٸ�... (ó���� �����Ͱ� �ִ�)
	if(new_position != old_position) {
		//���� ��ġ�� ������ ������ ��ġ���� ũ�ٸ� ������ ��Ȳ���� ���� ��ġ���� 
		//������ġ���� DMA ���ۿ��� ������ ������.
		if(new_position > old_position) {

			memcpy(rx_buf, &rx_dma_buf[old_position], new_position - old_position);
		
			// DMA ���ۿ��� ������ ������ ũ�� ����
			rx_size += new_position - old_position;
		}
		else {
			//���� ��ġ�� ���� ��ġ���� �Ʒ��� �����Ѵٸ� DMA ���۰� 
			//�ѹ����� �Ѿ ����(Overflow)
			//�ϴ� ������ ������ ��ġ���� ������ ������ ��� �����´�.
			memcpy(rx_buf, &rx_dma_buf[old_position], get_dma_total_size() - old_position);
			// ������ ������ ũ�� ����.
			rx_size += get_dma_total_size() - old_position;

			//������ �� �����Դµ� 
			//���� ��ġ�� 0���� ũ�ٸ� �߰��� ���������� ���� �ִٴ� �����
			//DMA ������ 0��°���� ���� ��ġ���� �߰��� ������.
			if(new_position > 0){
				//rx_buf ��ġ ����!!
				memcpy(&rx_buf[get_dma_total_size() - old_position], &rx_dma_buf[0], new_position);
				// �߰��� ������ ������ ũ�� �߰�
				rx_size += new_position;
			}
		}
		//���� ��ġ�� ������ġ�� ����
		old_position = new_position;


#if 0
		//������ �����Ͱ� �ִٸ�
		if(rx_size > 0) {
			//ũ�� ��ŭ ���� ���!
			for(i = 0;i<rx_size;i++) {
				ch = rx_buf[i];
				Printf_("%c", ch);
				// �� ��! (to do?)

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


