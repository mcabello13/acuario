#include <Driver_USART.h>
#include "stm32f4xx_hal.h"

UART_HandleTypeDef huart3;

void send(UART_HandleTypeDef *huart, float cmd);
int Init_Thread_master (void);
	