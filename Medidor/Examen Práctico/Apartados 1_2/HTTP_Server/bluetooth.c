#include "cmsis_os2.h"    
#include "Thread.h"                  
#include "main.h"
#include <Driver_USART.h>
#include "stm32f4xx_hal.h"
#include "luz.h"
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

//Funcion que inicializa el hilo de la transmisión:
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

//Timer utilizado para comprobar la medida de luz para salir del Modo de Bajo Consumo:
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
    HAL_TIM_IRQHandler(&TimHandle); //Cuando salta el timer, llega aquí...
}

// Callback de HAL que se llama cuando termina la cuenta del timer:
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{	
	if (htim->Instance == TIM6) //...y termina aquí.
	{
		HAL_ResumeTick(); //Se levanta el sistema...
		osThreadResume(TID_luz); //...y se rehabilita el hilo medidor de luz.
		datosSensorLuz = 20;
	}
}

//Funcion para el Modo de Bajo Consumo Sleep:
void SleepMode(void)
{	
	TIM6_delay(20000); //Timer para Bajo Consumo con Modo Sleep.
	
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU); //Se limpian los flag de cualquier otra interrupción.
  HAL_SuspendTick(); //...y una vez configurado, se suspende el reloj del sistema.
	__DSB(); 
	__WFI(); 
	HAL_NVIC_DisableIRQ(EXTI15_10_IRQn); //Deshabilitación de la interrupción del Pulsador Azul.
  HAL_PWR_EnterSLEEPMode(0, PWR_SLEEPENTRY_WFI);
}
