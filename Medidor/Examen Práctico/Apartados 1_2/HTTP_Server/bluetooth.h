#include <Driver_USART.h>
#include "stm32f4xx_hal.h"

UART_HandleTypeDef huart3;

void send(UART_HandleTypeDef *huart, float cmd);
int Init_Thread_master (void);
int Init_Thread_slave (void);
void SleepMode(void);
int Init_ThreadCMSIS(void); 
void initUart(void);
	