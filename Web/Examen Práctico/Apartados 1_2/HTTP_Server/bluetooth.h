#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

#include "cmsis_os2.h"    
#include "Thread.h"                  
#include "main.h"
#include <Driver_USART.h>
#include "stm32f4xx_hal.h"

void receiveResponse(UART_HandleTypeDef *huart);
void sendATCommand(UART_HandleTypeDef *huart, char *cmd);
int Init_Thread_slave (void);

#endif
