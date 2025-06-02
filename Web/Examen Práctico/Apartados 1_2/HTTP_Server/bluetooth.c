#include "cmsis_os2.h"    
#include "Thread.h"                  
#include "main.h"
#include <Driver_USART.h>
#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdlib.h>


extern ARM_DRIVER_USART Driver_USART3; //Driver UART.
static ARM_DRIVER_USART * USARTtres = &Driver_USART3;


//Variables:
char rx = 0;
char* numero;
char cadenaDatosWeb[50] = {0};
char rxBuffer[50] = {0};
char identificador = 0;
extern bool alimentacion;
float datosLuzWeb = 0;
float datospHWeb = 0;
float datosTurbidezWeb = 0;
float datosTemperaturaWeb = 0;
float datosConsumoTensionWeb = 0;
float datosConsumoCorrienteWeb = 0;
int cuenta = 0;
int p = 0;
char buffer[35];
uint8_t cmd; //Variable que almacena el comando recibido.
osThreadId_t tid_Thread; 
void Thread (void const *argument);

//Funciones utilizadas:
void myUSART_callback(uint32_t event);
void enviarDatosWeb(char cadenaDatosWeb[50]);

//Callback de la UART:
void myUSART_callback(uint32_t event)
{
  uint32_t mask;
  mask = ARM_USART_EVENT_RECEIVE_COMPLETE  |
         ARM_USART_EVENT_TRANSFER_COMPLETE |
         ARM_USART_EVENT_SEND_COMPLETE     |
         ARM_USART_EVENT_TX_COMPLETE       ;
	
  if (event & mask) 
	{
		if (event & ARM_USART_EVENT_RECEIVE_COMPLETE)
    {
        osThreadFlagsSet(tid_Thread, 0x01); // Señal de recepción
    }

    if (event & (ARM_USART_EVENT_SEND_COMPLETE | ARM_USART_EVENT_TX_COMPLETE | ARM_USART_EVENT_TRANSFER_COMPLETE))
    {
        osThreadFlagsSet(tid_Thread, 0x02); // Señal de transmisión
    }
  }
}

//Funcion que configura la UART 3:
void initUart(void)
{
	 USARTtres->Initialize(myUSART_callback);
   USARTtres->PowerControl(ARM_POWER_FULL);
   USARTtres->Control(
        ARM_USART_MODE_ASYNCHRONOUS | 
        ARM_USART_DATA_BITS_8 | 
        ARM_USART_PARITY_NONE | 
        ARM_USART_STOP_BITS_1 | 
			  ARM_USART_FLOW_CONTROL_NONE, 9600);
	 
  USARTtres->Control (ARM_USART_CONTROL_TX, 1);
  USARTtres->Control (ARM_USART_CONTROL_RX, 1);
}

//Funcion que inicializa hilos y timers:
int Init_ThreadCMSIS (void) 
{
  tid_Thread = osThreadNew (Thread, NULL, NULL);
  if (!tid_Thread) return(-1);
	
  return(0);
}

//Hilo
void Thread (void const *argument) 
{
	int alimentacionAnterior = 0;
	
  while(1)
  {    	
		if(alimentacion != alimentacionAnterior)
		{
			alimentacionAnterior = alimentacion;			
			
			if(alimentacion == 1)
			{
					identificador = 'C';			
			}
			else
			{
					identificador = 'O';
			}
			USARTtres->Send(&identificador, 1);
			osThreadFlagsWait(0x02, NULL, osWaitForever);
		}
		else
		{		
			p = 0;
			memset(rxBuffer, 0, sizeof(rxBuffer));

			for(p = 0; p < 32; p++) 
			{
				USARTtres->Receive(&rx, 1); 		
				osThreadFlagsWait(0x01, NULL, osWaitForever);
								
				if(rx != '\n') 
				{
					rxBuffer[p] = rx;			
				} 
				else
				{
					//rxBuffer[p] = '\0'; //Se finaliza la cadena antes de copiarla para enviarla a la web.
					strcpy(cadenaDatosWeb, rxBuffer);
					enviarDatosWeb(cadenaDatosWeb);
					osDelay(100);
					break;
				}				
				osDelay(200);
			}
		}
  }
}

//Función que particiona la cadena de datos recibida para representarlos en la Web:
void enviarDatosWeb(char cadenaDatosWeb[50])
{	
	numero = strtok(&cadenaDatosWeb[1], "W"); //Se empieza desde el segundo carácter para evitar el flag "B".
	datosLuzWeb = atof(numero);

	numero = strtok(NULL, "W");
	datospHWeb = atof(numero);

	numero = strtok(NULL, "W");
	datosTemperaturaWeb = atof(numero);
	
	numero = strtok(NULL, "W");
	datosTurbidezWeb = atof(numero);
	
	numero = strtok(NULL, "W");
	datosConsumoTensionWeb = atof(numero);
	
	numero = strtok(NULL, "W");
	datosConsumoCorrienteWeb = atof(numero);
}




















































////Hilos:
//osThreadId_t tid_ThreadSlave; 
//void Thread_slave (void *argument);  

////Variables:
//UART_HandleTypeDef huart3;
//char rx = 0;
//char identificador = 0;
//char rxBuffer[50] = {0};
//char cadenaDatosWeb[50] = {0};
//char cadena = {0};
//int p = 0;
//extern bool alimentacion;
//char* numero;
//float datosLuzWeb = 0;
//float datospHWeb = 0;
//float datosTurbidezWeb = 0;
//float datosTemperaturaWeb = 0;
//float datosConsumoTensionWeb = 0;
//float datosConsumoCorrienteWeb = 0;



////Funcion que configura la UART 3:
//void initUart(void)
//{
//	// Habilitar el reloj para USART3 y los GPIOs necesarios
//    __HAL_RCC_USART3_CLK_ENABLE();
//    __HAL_RCC_GPIOC_CLK_ENABLE();

//    GPIO_InitTypeDef GPIO_InitStruct = {0};

//    // Configurar PC10 (TX) y PC11 (RX)
//    GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
//    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;  //USART3 AF7 para PC10/PC11
//    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

//    // Configurar el UART
//    huart3.Instance = USART3;
//    huart3.Init.BaudRate = 9600; // Velocidad para modo AT del HC-05
//    huart3.Init.WordLength = UART_WORDLENGTH_8B;
//    huart3.Init.StopBits = UART_STOPBITS_1;
//    huart3.Init.Parity = UART_PARITY_NONE;
//    huart3.Init.Mode = UART_MODE_TX_RX;
//    huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
//    huart3.Init.OverSampling = UART_OVERSAMPLING_16;
//    if (HAL_UART_Init(&huart3) != HAL_OK) 
//		{
//        //Error_Handler();
//    }
//}

////Función que particiona la cadena de datos recibida para representarlos en la Web:
//void enviarDatosWeb(char cadenaDatosWeb[50])
//{	
//	numero = strtok(&cadenaDatosWeb[1], "W"); //Se empieza desde el segundo carácter para evitar el flag "A".
//	datosLuzWeb = atof(numero);

//	numero = strtok(NULL, "W");
//	datospHWeb = atof(numero);

//	numero = strtok(NULL, "W");
//	datosTemperaturaWeb = atof(numero);
//	
//	numero = strtok(NULL, "W");
//	datosTurbidezWeb = atof(numero);
//	
//	numero = strtok(NULL, "W");
//	datosConsumoTensionWeb = atof(numero);
//	
//	numero = strtok(NULL, "W");
//	datosConsumoCorrienteWeb = atof(numero);
//}

////Función que realiza el envío del Esclavo al Maestro:
//void send(UART_HandleTypeDef *huart, char *cmd) 
//{	
//    HAL_UART_Transmit(huart, cmd, 1, 100);
//}

////Función para recibir comandos por parte del esclavo desde el master:
//void receiveResponse(UART_HandleTypeDef *huart) 
//{	  	
//		HAL_UART_Receive(huart, &rx, 1, 500);		
//	
//		if(rx == 'B')
//		{
//			p = 0;
//			
//			for(p=0; p<35; p++)
//			{			
//				HAL_UART_Receive(huart, &rx, 1, 500);			
//				
//				if(rx != '\n')
//				{
//					rxBuffer[p] = rx;			
//				}
//				else if(rx == '\n')
//				{
//					strcpy(cadenaDatosWeb, rxBuffer);
//					enviarDatosWeb(cadenaDatosWeb);
//					rxBuffer[p] = '\0';
//					osDelay(100);
//					p = 0;
//					memset(rxBuffer, 0, sizeof(rxBuffer));
//					break;
//				}				
//				osDelay(100);
//			}
//		}
//}

////Función que inicializa el hilo para controlar el Esclavo bluetooth:
//int Init_Thread_slave (void) 
//{
//  tid_ThreadSlave = osThreadNew(Thread_slave, NULL, NULL);
//	
//  if (tid_ThreadSlave == NULL) 
//	{
//    return(-1);
//  }
// 
//  return(0);
//}

////Hilo que gestiona la recepción de datos por parte del Esclavo Bluetooth:
//void Thread_slave (void *argument) 
//{ 	
//	while(1)
//	{
//		if(alimentacion == 1)
//		{
//			identificador = 'C';
//			send(&huart3, &identificador);
//		}
//		else
//		{
//			identificador = 'O';
//			alimentacion = 0;			
//			send(&huart3, &identificador);	
//			receiveResponse(&huart3);
//		}
//	}
//}
