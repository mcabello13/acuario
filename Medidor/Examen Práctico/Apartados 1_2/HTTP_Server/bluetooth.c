#include "cmsis_os2.h"    
#include "Thread.h"                  
#include "main.h"
#include <Driver_USART.h>
#include "stm32f4xx_hal.h"
#include "luz.h"

osThreadId_t tid_ThreadMaster;
  
int estado;	
char bufferUART[70];
uint8_t buf[12];
char buf0[80]; 
int cuenta = 0;
uint8_t cmd; 
int p;
UART_HandleTypeDef huart3;
char txBufferGlobal[100];
char rx = 0;	
char txBuffer[30] = {0};
float tx[30] = {0};
char rxBuffer[30] = {0};
char msg[] = "Hola";

void Thread_master (void *argument);              


extern ARM_DRIVER_USART Driver_USART3; //Driver UART tres.
static ARM_DRIVER_USART * USARTuno = &Driver_USART3;

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
    huart3.Init.BaudRate = 38400;              // Velocidad para modo AT del HC-05
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

int Init_Thread_master (void) 
{
  tid_ThreadMaster = osThreadNew(Thread_master, NULL, NULL);
	
  if (tid_ThreadMaster == NULL) 
	{
    return(-1);
  }
 
  return(0);
}

void sendATCommand(UART_HandleTypeDef *huart, char *cmd) 
{	
		memset(txBuffer, 0, sizeof(txBuffer));	
	
    sprintf(txBuffer, "%s\r\n", cmd); // Los comandos AT terminan en \r\n
    HAL_UART_Transmit(huart, txBuffer, strlen(txBuffer), HAL_MAX_DELAY);
}

void receiveResponse(UART_HandleTypeDef *huart) 
{	
	  memset(rxBuffer, 0, sizeof(rxBuffer));
		HAL_UART_Receive(huart, rxBuffer, sizeof(rxBuffer), 1000);		
}

void configurar_master_bluetooth(void)
{
		//sendATCommand(&huart3, "AT");
		//receiveResponse(&huart3);
		//osDelay(1000);
		
		sendATCommand(&huart3, "AT+NAME=Master");
		receiveResponse(&huart3);
		osDelay(1000);
		//sendATCommand(&huart3, "AT+NAME?");
		//receiveResponse(&huart3);
		//osDelay(1000);
	
		sendATCommand(&huart3, "AT+PSWD=1212");
		receiveResponse(&huart3);
		osDelay(1000);
		//sendATCommand(&huart3, "AT+PSWD?");
		//receiveResponse(&huart3);
		//osDelay(100000);
		
		sendATCommand(&huart3, "AT+UART=9600,0,0");
		receiveResponse(&huart3);
		osDelay(1000);
		//sendATCommand(&huart3, "AT+UART?");
		//receiveResponse(&huart3);
		//osDelay(3000);
		
		//sendATCommand(&huart3, "AT+INIT");
		//receiveResponse(&huart3);
		//osDelay(1000);
		
		sendATCommand(&huart3, "AT+CMODE=0"); // conectar solo a esa dirección
		receiveResponse(&huart3);
		osDelay(1000);
		//sendATCommand(&huart3, "AT+CMODE?"); // conectar solo a esa dirección
		//osDelay(4000);
	
		sendATCommand(&huart3, "AT+ROLE=1");
		receiveResponse(&huart3);
		osDelay(1000);
		//sendATCommand(&huart3, "AT+ROLE?");
		//receiveResponse(&huart3);
		//osDelay(1000);
			
		sendATCommand(&huart3, "AT+BIND=98DA,50,03C6C6");
		//sendATCommand(&huart3, "AT+BIND?");
		receiveResponse(&huart3);
		osDelay(1000);

		/*sendATCommand(&huart3, "AT+INIT");
		sendATCommand(&huart3, "AT+BIND?");
		receiveResponse(&huart3);
		osDelay(1000);
		
   	sendATCommand(&huart3, "AT+IAC=9E8B33");
		receiveResponse(&huart3);
		osDelay(1000);

		sendATCommand(&huart3, "AT+CLASS=0");
		receiveResponse(&huart3);
		osDelay(1000);

		sendATCommand(&huart3, "AT+INQM=1,9,48");
		receiveResponse(&huart3);
		osDelay(1000);	
		
		sendATCommand(&huart3, "AT+INQ");
		receiveResponse(&huart3);
		osDelay(1000);	
		
		sendATCommand(&huart3, "AT+PAIR=98DA,50,03C6C6,20");
		receiveResponse(&huart3);
		osDelay(1000);
		
		sendATCommand(&huart3, "AT+LINK=98DA,50,03C6C6");
		receiveResponse(&huart3);
		osDelay(1000);*/	

		// Reinicio final
		//sendATCommand(&huart3, "AT+RESET");
		//receiveResponse(&huart3);
}

//Hilo que gestiona los envios del Master:
void Thread_master (void *argument) 
{ 
	while(1)
	{		
		osDelay(20000);
		//send(&huart3, datosLuz);
	}
}
