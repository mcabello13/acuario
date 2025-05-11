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
osThreadId_t TID_ph;

extern osThreadId_t TID_turbidez;
extern osThreadId_t TID_luz;
extern osThreadId_t TID_ph;

static GPIO_InitTypeDef GPIO_InitStruct;

//Variables:
bool LEDrun;
char lcd_text[2][20+1];
ADC_HandleTypeDef adchandle; 
float value = 0;

//Funciones:
__NO_RETURN void app_main (void *arg);
void Thread_turbidez(void *argument);
void Thread_luz(void *argument);
void Thread_ph(void *argument);
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

int Init_Thread_ph (void) 
{
  TID_ph = osThreadNew(Thread_ph, NULL, NULL);
	
  if (TID_ph == NULL) 
	{
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
	}
}

//Hilo que gestiona el Sensor de Luz:
void Thread_luz (void *argument) 
{ 		
	while(1)
	{	
		realizarMedida();
		osDelay(1000);
	}
}

//Hilo que gestiona el Sensor de pH:
void Thread_ph (void *argument) 
{ 		
	while(1)
	{	
		pH_Sensor_Read();
		osDelay(2000);
	}
}

//Funcion para crear los hilos utilizados:
void creacion_hilos(void)
{
	TID_turbidez = osThreadNew (Thread_turbidez,  NULL, NULL);
	TID_luz = osThreadNew (Thread_luz,  NULL, NULL);
	TID_ph = osThreadNew (Thread_ph,  NULL, NULL);
	//TID_Led     = osThreadNew (BlinkLed, NULL, NULL);
  //TID_Display = osThreadNew (Display,  NULL, NULL);
}

__NO_RETURN void app_main (void *arg) 
{
	(void)arg;
}
