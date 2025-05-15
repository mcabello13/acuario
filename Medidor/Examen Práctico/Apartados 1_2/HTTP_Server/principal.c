//-------------------------------------------
//PROGRAMA PRINCIPAL DEL MEDIDOR DEL ACUARIO.
//-------------------------------------------

#include <stdio.h>
#include "main.h"               
#include "stm32f4xx_hal.h"              
#include "leds.h"
#include "Thread.h"
#include "turbidez.h"
#include "luz.h"
#include "ph.h"

// Main stack size must be multiple of 8 Bytes
#define APP_MAIN_STK_SZ (1024U)
uint64_t app_main_stk[APP_MAIN_STK_SZ / 8];
const osThreadAttr_t app_main_attr = {
  .stack_mem  = &app_main_stk[0],
  .stack_size = sizeof(app_main_stk)
};

//Identificadores de Hilos:
osThreadId_t TID_turbidez;
osThreadId_t TID_luz;

extern osThreadId_t TID_turbidez;
extern osThreadId_t TID_luz;
osThreadId_t th_alim_pez;

static GPIO_InitTypeDef GPIO_InitStruct;

//Variables:
bool LEDrun;
char lcd_text[2][20+1];
ADC_HandleTypeDef adchandle; 
float value = 0;
float datosSensorLuz = 0;
float datosSensorTurbidez = 0;
bool alimentacion = 0;


//Funciones:
__NO_RETURN void app_main (void *arg);
void Thread_turbidez(void *argument);
void Thread_luz(void *argument);
void function_th_alim_pez (void *argument); 
void Thread_master(void *argument);
void creacion_hilos(void);


/********************************************************************
 *               INICIALIZACION DE LOS HILOS                        *
 ********************************************************************/


int Init_Thread_turbidez (void) 
{
  TID_turbidez = osThreadNew(Thread_turbidez, NULL, NULL);
	
  if (TID_turbidez == NULL) 
	{
    return(-1);
  }
 
  return(0);
}

int Init_Thread_luz (void) 
{
  TID_luz = osThreadNew(Thread_luz, NULL, NULL);
	
  if (TID_luz == NULL) 
	{
    return(-1);
  }
 
  return(0);
}

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

/********************************************************************
 *                        HILOS Y FUNCIONES                         *
 ********************************************************************/

//Hilo que gestiona el Sensor de Turbidez:
void Thread_turbidez (void *argument) 
{ 		
	while(1)
	{	

		ADC_StartConversion_stm(&adchandle, ADC1); 
		value = ADC_getVoltage_stm(&adchandle, 10);
		datosSensorTurbidez = value;
	}
}

//Hilo que gestiona el Sensor de Luz:
void Thread_luz (void *argument) 
{ 		
	while(1)
	{	
		datosSensorLuz = realizarMedida();
	}
}

//Hilo que gestiona la Alimentación de los Peces:
/***********************************************************************
 * EXPLICACION: el motor tiene un paso básico de 64 pasos por vuelta,  *
 * por lo tanto 64*8 half-steps por paso completo = 512, es decir      *
 * 512*64 = 4096. 																										 *
 ***********************************************************************/		
void function_th_alim_pez (void *argument) 
{
  while(1)
	{
		if(alimentacion == 1)
		{				
			for (int i = 0; i < 4096; i++)
			{
				if (alimentacion == 0)
          break;  
				
				Step8(); 
				osDelay(2);
				Step7(); 
				osDelay(2);
				Step6(); 
				osDelay(2);
				Step5(); 
				osDelay(2);
				Step4(); 
				osDelay(2);
				Step3(); 
				osDelay(2);
				Step2(); 
				osDelay(2);
				Step1(); 
				osDelay(2);
			}
			osThreadSuspend(NULL);
		}	
		else if(alimentacion == 0)
		{
			osThreadSuspend(NULL);
		}
		
  }
}

//Funcion para crear los hilos utilizados:
void creacion_hilos(void)
{
	TID_turbidez = osThreadNew (Thread_turbidez,  NULL, NULL);
	TID_luz = osThreadNew (Thread_luz,  NULL, NULL);
}

__NO_RETURN void app_main (void *arg) 
{
	(void)arg;
}
