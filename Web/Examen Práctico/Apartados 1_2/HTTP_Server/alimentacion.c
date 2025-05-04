#include "alimentacion.h"                        

//Hilos:
osThreadId_t th_alim_pez; // thread id

//Variables:
uint8_t activo = 0; 

//Funciones:
void function_th_alim_pez (void *argument);
 
 
 
 //Función de inicialización del hilo encargado de la alimentación de los peces:
int Init_Thread_alim_pez (void) 
{  
	init_Digital_PIN_Out();
  th_alim_pez = osThreadNew(function_th_alim_pez, NULL, NULL);
  if (th_alim_pez == NULL) {
    return(-1);
  }
 
  return(0);
}
 
//Función que gestiona los LED para el Modo Alimentación:
void function_th_alim_pez (void *argument) 
{
  while(1)
	{
    osThreadFlagsWait(0x01, NULL, osWaitForever);
		
		if(activo == 0)
		{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOG, GPIO_PIN_2, GPIO_PIN_SET);
			activo = 1;
		}
		else
		{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOG, GPIO_PIN_2, GPIO_PIN_RESET);
			activo = 0;
		}
		
    osThreadYield();  
  }
}

//Función que inicializa el pin digital para el Modo Alimentación:
void init_Digital_PIN_Out(void)
{	
	  GPIO_InitTypeDef GPIO_InitStruct; // ----> El pin PG2 se configura como salida digital para activar la trampilla.
	
	__HAL_RCC_GPIOG_CLK_ENABLE(); //Configuración Salida PIN Digital.
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	
	GPIO_InitStruct.Pin = GPIO_PIN_2;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
}
