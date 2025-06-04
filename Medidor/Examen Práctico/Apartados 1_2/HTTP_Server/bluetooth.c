#include "cmsis_os2.h"    
#include "Thread.h"                  
#include "main.h"
#include <Driver_USART.h>
#include "stm32f4xx_hal.h"
#include "luz.h"
#include "adc.h"
#include "ph.h"

//Hilos:
extern osThreadId_t TID_luz;
osThreadId_t tid_ThreadMaster;
osThreadId_t tid_ThreadSlave; 
void Thread_slave (void *argument);  

//Variables:
extern ARM_DRIVER_USART Driver_USART3; //Driver UART.
static ARM_DRIVER_USART * USARTtres = &Driver_USART3;

TIM_HandleTypeDef TimHandle;
osTimerId_t tim_id_s;
static uint32_t exec; 
GPIO_InitTypeDef GPIO_InitStruct;

char rx = 2;
extern float datosSensorTurbidez;
extern float datosSensorLuz;
extern float temperatura;
extern float consumoTension;
extern float consumoCorriente;
extern bool alimentacion;
char txBuffer[35] = {0};
//char buffer[70];
osThreadId_t tid_Thread; 
void Thread (void const *argument);

//Funciones utilizadas:
void myUSART_callback(uint32_t event);

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
        osThreadFlagsSet(tid_ThreadSlave, 0x01); // Señal de recepción
    }

    if (event & (ARM_USART_EVENT_SEND_COMPLETE | ARM_USART_EVENT_TX_COMPLETE | ARM_USART_EVENT_TRANSFER_COMPLETE))
    {
        osThreadFlagsSet(tid_Thread, 0x02); // Señal de transmisión
    }
    //osThreadFlagsSet(tid_Thread, 0x01); //Cada vez que se complete un envio, el callback mandara esta señal al hilo donde se gestiona la UART.
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

//Hilo que gestiona los envíos de las medidas del Maestro al Esclavo:
void Thread (void const *argument) 
{
  while (1) 
	{				
    int len = sprintf(txBuffer, "B%.2fW%.2fW%.2fW%.2fW%.2fW%.2fA", datosSensorLuz, phAgua, temperatura, datosSensorTurbidez, consumoTension, consumoCorriente);
		
    for(int i=0; i<len; i++)
    {
      USARTtres->Send(&txBuffer[i],1);
      osThreadFlagsWait(0x02, NULL, osWaitForever); //Se recibira la señal del callback y el hilo despertara de nuevo.
			//osDelay(200);
    }
    osDelay(1000);
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

//Hilo que gestiona la recepción de datos por parte del Maestro Bluetooth:
void Thread_slave (void *argument) 
{ 		
	while(1)
	{		
		USARTtres->Receive(&rx, 1); 		
		osThreadFlagsWait(0x01, NULL, osWaitForever);

		if(rx == 'C')
		{
			osThreadSuspend(tid_Thread);
			
			alimentacion = 1;
			rx = 0;
			
			osThreadResume(tid_Thread); 
		}
		else if(rx == 'O')
		{
			alimentacion = 0;			
		}
	}
}

void TIM6_delay(uint16_t ms)
{		
	__HAL_RCC_TIM6_CLK_ENABLE();

	TimHandle.Instance = TIM6;
	TimHandle.Init.Prescaler = (SystemCoreClock / 1000) - 1; // 1ms por cuenta
	TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
	TimHandle.Init.Period = ms - 1; // 5000 ms
	TimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

	HAL_TIM_Base_Init(&TimHandle);
	HAL_TIM_Base_Start_IT(&TimHandle); // Habilita interrupción

	HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
}

// Manejador de interrupción TIM6:
void TIM6_DAC_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TimHandle);
}

// Callback de HAL que se llama cuando termina la cuenta del timer:
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM6)
	{
		HAL_ResumeTick(); //Se levanta el sistema.
		osThreadResume(TID_luz);
	}
}

//Funcion para el Modo Sleep:
void SleepMode(void)
{
	__HAL_RCC_GPIOC_CLK_ENABLE(); //Se configura el Pulsador Azul...
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET); //LED Rojo ON.
	
	TIM6_delay(10000); //Timer para Bajo Consumo con Modo Sleep.
	
  HAL_SuspendTick(); //...y una vez configurado, se suspende el reloj del sistema.
  HAL_PWR_EnterSLEEPMode(0, PWR_SLEEPENTRY_WFI);
	//osDelay (10000);
	//HAL_ResumeTick(); //Se levanta el sistema.
	
	//Init_Thread_slave();
}

//Timer "One Shot" para el entrar en Modo Bajo Consumo:
//---------------------------------------------------------------------------------------
static void Timer_Callback (void const *arg) 
{
	SleepMode();
}

int Init_Timer (void) 
{
  // Create one-shoot timer
  exec = 1U;
  tim_id_s = osTimerNew((osTimerFunc_t)&Timer_Callback, osTimerPeriodic, &exec, NULL);
  return NULL;
}
//---------------------------------------------------------------------------------------



































////Hilos:
//osThreadId_t tid_ThreadMaster;
//osThreadId_t tid_ThreadSlave; 
//extern osThreadId_t TID_luz;
//void Thread_master (void /**argument*/);  
//void Thread_slave (void *argument);  
//  
////Funciones:	
//int Init_Timer (void);
//	
////Variables:
//char rx = 2;
//uint8_t cmd; 
//UART_HandleTypeDef huart3;
//char txBuffer[50] = {0};
//extern float datosSensorTurbidez;
//extern float datosSensorLuz;
//extern float temperatura;
//extern float consumoTension;
//extern float consumoCorriente;
//extern bool alimentacion;
//TIM_HandleTypeDef TimHandle;
//osTimerId_t tim_id_s;
//static uint32_t exec;  
//GPIO_InitTypeDef GPIO_InitStruct;


////Funcion que configura la UART 3:
//void initUart(void)
//{
//	 //Inicializamos UART1:
//    __HAL_RCC_USART3_CLK_ENABLE();
//    __HAL_RCC_GPIOC_CLK_ENABLE();

//    GPIO_InitTypeDef GPIO_InitStruct = {0};

//    // Configurar PC10 (TX) y PC11 (RX)
//    GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
//    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;  // USART3 AF7 para PC10/PC11
//    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

//    // Configurar el UART
//    huart3.Instance = USART3;
//    huart3.Init.BaudRate = 9600;              // Velocidad para modo AT del HC-05
//    huart3.Init.WordLength = UART_WORDLENGTH_8B;
//    huart3.Init.StopBits = UART_STOPBITS_1;
//    huart3.Init.Parity = UART_PARITY_NONE;
//    huart3.Init.Mode = UART_MODE_TX_RX;
//    huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
//    huart3.Init.OverSampling = UART_OVERSAMPLING_16;
//    if (HAL_UART_Init(&huart3) != HAL_OK) {
//        //Error_Handler();
//    }
//}

//void send(UART_HandleTypeDef *huart, char *cmd) 
//{	    
//    HAL_UART_Transmit(huart, cmd, 1, 1000);
//}

////Función para recibir comandos por parte del esclavo desde el master:
//void receiveResponse(UART_HandleTypeDef *huart) 
//{	
//	HAL_UART_Receive(huart, &rx, 1, 100);		

//	if(rx == 'C')
//	{
//		alimentacion = 1;
//		rx = 0;
//	}
//	else if(rx == 'O')
//	{
//		alimentacion = 0;			
//	}
//}

////Hilo que gestiona los envios del Maestro Bluetooth:
//void Thread_master (void) 
//{ 
//	if(rx != 0)
//	{
//		sprintf(txBuffer, "B%.2fW%.2fW%.2fW%.2fW%.2fW%.2f\n", datosSensorLuz, phAgua, temperatura, datosSensorTurbidez, consumoTension, consumoCorriente);
//		
//		for(int i=0; i<35; i++)
//		{
//			send(&huart3, &txBuffer[i]);   
//			osDelay(250);
//		}
//	}
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

////Hilo que gestiona la recepción de datos por parte del Maestro Bluetooth:
//void Thread_slave (void *argument) 
//{ 		
//	while(1)
//	{		
//		receiveResponse(&huart3);
//		Thread_master();
//	}
//}

//void TIM6_delay(uint16_t ms)
//{		
//	__HAL_RCC_TIM6_CLK_ENABLE();

//	TimHandle.Instance = TIM6;
//	TimHandle.Init.Prescaler = (SystemCoreClock / 1000) - 1; // 1ms por cuenta
//	TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
//	TimHandle.Init.Period = ms - 1; // 5000 ms
//	TimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
//	TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

//	HAL_TIM_Base_Init(&TimHandle);
//	HAL_TIM_Base_Start_IT(&TimHandle); // Habilita interrupción

//	HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 0, 0);
//	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
//}

//// Manejador de interrupción TIM6:
//void TIM6_DAC_IRQHandler(void)
//{
//    HAL_TIM_IRQHandler(&TimHandle);
//}

//// Callback de HAL que se llama cuando termina la cuenta del timer:
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//	if (htim->Instance == TIM6)
//	{
//		HAL_ResumeTick(); //Se levanta el sistema.
//		osThreadResume(TID_luz);
//	}
//}

////Funcion para el Modo Sleep:
//void SleepMode(void)
//{
//	__HAL_RCC_GPIOC_CLK_ENABLE(); //Se configura el Pulsador Azul...
//  GPIO_InitStruct.Pin = GPIO_PIN_13;
//  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
//	
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET); //LED Rojo ON.
//	
//	TIM6_delay(10000); //Timer para Bajo Consumo con Modo Sleep.
//	
//  HAL_SuspendTick(); //...y una vez configurado, se suspende el reloj del sistema.
//  HAL_PWR_EnterSLEEPMode(0, PWR_SLEEPENTRY_WFI);
//	//osDelay (10000);
//	//HAL_ResumeTick(); //Se levanta el sistema.
//	
//	//Init_Thread_slave();
//}

////Timer "One Shot" para el entrar en Modo Bajo Consumo:
////---------------------------------------------------------------------------------------
//static void Timer_Callback (void const *arg) 
//{
//	SleepMode();
//}

//int Init_Timer (void) 
//{
//  // Create one-shoot timer
//  exec = 1U;
//  tim_id_s = osTimerNew((osTimerFunc_t)&Timer_Callback, osTimerPeriodic, &exec, NULL);
//  return NULL;
//}
////---------------------------------------------------------------------------------------
