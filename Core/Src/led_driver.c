#include "main.h"

void TRX_LedInit(void)
{
	RX_LedOnOff(FALSE);
	TX_LedOnOff(FALSE);
}

void RX_LedOnOff(uint8_t onOff)
{
	if(onOff) {
		HAL_GPIO_WritePin(RXD_LED_GPIO_Port, RXD_LED_Pin, GPIO_PIN_RESET);
	}
	else {
		HAL_GPIO_WritePin(RXD_LED_GPIO_Port, RXD_LED_Pin, GPIO_PIN_SET);
	}
}	

void TX_LedOnOff(uint8_t onOff)
{
	if(onOff) {
		HAL_GPIO_WritePin(TXD_LED_GPIO_Port, TXD_LED_Pin, GPIO_PIN_RESET);
	}
	else {
		HAL_GPIO_WritePin(TXD_LED_GPIO_Port, TXD_LED_Pin, GPIO_PIN_SET);
	}
}	

void HeartBeatToggle(void)
{
	HAL_GPIO_TogglePin(RUN_LED_GPIO_Port, RUN_LED_Pin);
}


