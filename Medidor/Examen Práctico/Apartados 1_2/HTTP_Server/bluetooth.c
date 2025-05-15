#include "cmsis_os2.h"    
#include "Thread.h"                  
#include "main.h"
#include <Driver_USART.h>
#include "stm32f4xx_hal.h"
#include "luz.h"
#include "adc.h"
#include "ph.h"


//Hilos:
osThreadId_t tid_ThreadMaster;
osThreadId_t tid_ThreadSlave; 
void Thread_master (void /**argument*/);  
void Thread_slave (void *argument);  
  
//Variables:
char rx = 2;
uint8_t cmd; 
UART_HandleTypeDef huart3;
char txBuffer[20] = {0};
extern float datosSensorTurbidez;
extern float datosSensorLuz;
extern bool alimentacion;

       

//Funcion que configura la UART 3:
void initUart(void)
{
	 //Inicializamos UART1:
    __HAL_RCC_USART3_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Configurar PC10 (TX) y PC11 (RX)
    GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;  // USART3 AF7 para PC10/PC11
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // Configurar el UART
    huart3.Instance = USART3;
    huart3.Init.BaudRate = 9600;              // Velocidad para modo AT del HC-05
    huart3.Init.WordLength = UART_WORDLENGTH_8B;
    huart3.Init.StopBits = UART_STOPBITS_1;
    huart3.Init.Parity = UART_PARITY_NONE;
    huart3.Init.Mode = UART_MODE_TX_RX;
    huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart3.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart3) != HAL_OK) {
        //Error_Handler();
    }
}

void send(UART_HandleTypeDef *huart, char *cmd) 
{	
    //sprintf(txBuffer, "%f", cmd); 
    HAL_UART_Transmit(huart, cmd, 1, HAL_MAX_DELAY);
}

//Función para recibir comandos por parte del esclavo desde el master:
void receiveResponse(UART_HandleTypeDef *huart) 
{	  	
		HAL_UART_Receive(huart, &rx, 1, 100);		
	
		if(rx == 'C')
		{
			alimentacion = 1;
			rx = 0;
		}
		else
		{
			alimentacion = 0;
			//rx = 0;
		}
}

//Función que inicializa el hilo del Maestro Bluetooth:
/*int Init_Thread_master (void) 
{
  tid_ThreadMaster = osThreadNew(Thread_master, NULL, NULL);
	
  if (tid_ThreadMaster == NULL) 
	{
    return(-1);
  }
 
  return(0);
}*/

//Hilo que gestiona los envios del Maestro Bluetooth:
void Thread_master (void /**argument*/) 
{ 
	//while(1)
	//{	
		if(rx != 0)
		{
			sprintf(txBuffer, "A%.2fW%.2fW%.2f\n", datosSensorLuz, datosSensorTurbidez, phAgua);
			
			for(int i=0; i<20; i++)
			{
				send(&huart3, &txBuffer[i]);   
				osDelay(500);
			}
		}
	//}
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

//Hilo que gestiona la recepción de datos por parte del Maestro Bluetooth:
void Thread_slave (void *argument) 
{ 	
	while(1)
	{
		receiveResponse(&huart3);
		Thread_master();
	}
}
