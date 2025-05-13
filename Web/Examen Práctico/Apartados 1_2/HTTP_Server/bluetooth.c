#include "cmsis_os2.h"    
#include "Thread.h"                  
#include "main.h"
#include <Driver_USART.h>
#include "stm32f4xx_hal.h"

//Hilos:
osThreadId_t tid_ThreadSlave; 
void Thread_slave (void *argument);  

//Variables:
char rxBuffer[10] = {0};
UART_HandleTypeDef huart3;


//Funcion que configura la UART 3:
void initUart(void)
{
	// Habilitar el reloj para USART3 y los GPIOs necesarios
    __HAL_RCC_USART3_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Configurar PC10 (TX) y PC11 (RX)
    GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;  //USART3 AF7 para PC10/PC11
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // Configurar el UART
    huart3.Instance = USART3;
    huart3.Init.BaudRate = 9600; // Velocidad para modo AT del HC-05
    huart3.Init.WordLength = UART_WORDLENGTH_8B;
    huart3.Init.StopBits = UART_STOPBITS_1;
    huart3.Init.Parity = UART_PARITY_NONE;
    huart3.Init.Mode = UART_MODE_TX_RX;
    huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart3.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart3) != HAL_OK) 
		{
        //Error_Handler();
    }
}

//Función que inicializa el hilo para controlar el Esclavo bluetooth:
int Init_Thread_slave (void) 
{
  tid_ThreadSlave = osThreadNew(Thread_slave, NULL, NULL);
	
  if (tid_ThreadSlave == NULL) 
	{
    return(-1);
  }
 
  return(0);
}

//Función para recibir comandos por parte del esclavo desde el master:
void receiveResponse(UART_HandleTypeDef *huart) 
{
	  
		HAL_UART_Receive(huart, rxBuffer, sizeof(rxBuffer), 1000);		
	  osDelay(3000);
		memset(rxBuffer, 0, sizeof(rxBuffer));
}

//Hilo que gestiona la recepción de datos por parte del Esclavo Bluetooth:
void Thread_slave (void *argument) 
{ 
	UART_HandleTypeDef *huart;
	
	while(1)
	{
		receiveResponse(&huart3);
	}
}
