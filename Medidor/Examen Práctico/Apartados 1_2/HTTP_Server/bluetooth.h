#include <Driver_USART.h>
#include "stm32f4xx_hal.h"

UART_HandleTypeDef huart3;

void sendATCommand(UART_HandleTypeDef *huart, char *cmd);
void receiveResponse(UART_HandleTypeDef *huart) ;
void configurar_master_bluetooth(void);
int Init_Thread_master (void);
	