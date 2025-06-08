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
#include "temperatura.h"
#include "consumo.h"

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
osThreadId_t tid_Thread_Bomba;
osThreadId_t th_temp_sensor; 
osThreadId_t th_consumo; 


//Variables:
bool LEDrun;
char lcd_text[2][20+1];
ADC_HandleTypeDef adchandle; 
ADC_HandleTypeDef adchandle2;
float value = 0;
float datosSensorLuz = 1;
float datosSensorTurbidez = 0;
float temperatura = 0; 
float voltajeSal = 0;
float consumoTension = 0;
float consumoCorriente = 0;
float voltageConsumo = 0;
bool alimentacion = 0;


//Funciones:
__NO_RETURN void app_main (void *arg);
void Thread_turbidez(void *argument);
void Thread_luz(void *argument);
void function_th_alim_pez (void *argument); 
void Step1(void);
void Step2(void);
void Step3(void);
void Step4(void);
void Step5(void);
void Step6(void);
void Step7(void);
void Step8(void); 
void Thread_Bomba(void);
void Thread_master(void *argument);
void Thread_consumo (void *argument); 
void creacion_hilos(void);
//int Init_Timer (void);
void SleepMode(void);


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

//Función de inicialización del hilo encargado de la Bomba de Agua:
int Init_Thread_Bomba (void) 
{  
	tid_Thread_Bomba = osThreadNew(Thread_Bomba, NULL, NULL);
	
  if (tid_Thread_Bomba == NULL) 
	{
    return(-1);
  }
 
  return(0);
}

//Función de inicialización del hilo encargado de la temperatura del acuario: 
int Init_Thread_temp (void) 
{
 
  th_temp_sensor = osThreadNew(Thread_temp, NULL, NULL);
  if (th_temp_sensor == NULL) {
    return(-1);
  }
 
  return(0);
}

//Función de inicialización del hilo encargado de la medición del consumo: 
int Init_Thread_consumo (void) 
{
 
  th_consumo = osThreadNew(Thread_consumo, NULL, NULL);
  if (th_consumo == NULL) {
    return(-1);
  }
 
  return(0);
}

/********************************************************************
 *                        HILOS Y FUNCIONES                         *
 ********************************************************************/

//Hilo que gestiona el Sensor de Luz:
void Thread_luz (void *argument) 
{ 		
	while(1)
	{			
		if(datosSensorLuz <= 0.2)
		{
			//datosSensorLuz = 0;
			TID_luz = osThreadNew(Thread_luz, NULL, NULL);
			osThreadSuspend(TID_luz);
			SleepMode();
		}
		else
    {
			datosSensorLuz = realizarMedida();
		}
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

//Hilo que controla la Bomba de Agua (Simulada con un Ventilador):
void Thread_Bomba(void)
{
    while (1)
    {
			//printf("Bomba ON\n");
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);		
    }
}

//Hilo que gestiona la lectura de la temperatura:
void Thread_temp (void *argument) 
{
	
  ADC_HandleTypeDef adchandle; 
	ADC1_pins_F429ZI_config_temperatura();
	ADC_Init_Single_Conversion(&adchandle , ADC3);

  while (1) 
  {
		/* Ecuación que relaciona el voltaje con temperatura */
		/*	        y = -4,5603x2 - 15,504x + 52,6					 */
		
		voltajeSal = ADC_getVoltage(&adchandle, 9); // Revisar PIN en el proyecto final
		temperatura = (-4.5603*voltajeSal*voltajeSal)+(-15.504*voltajeSal)+52.6;
		
		osDelay(700); 
  }
}

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

//Hilo que gestiona la lectura del consumo:
void Thread_consumo (void *argument) 
{  
  while(1)
  {
  		ADC_StartConversion_stm_consumo(&adchandle2, ADC1); 
      consumoTension = ADC_getVoltage_stm_consumo(&adchandle2, 13);
      consumoCorriente = consumoTension / 0.321; //Para obtener la corriente se divide entre "Rshunt" utilizada.
      osDelay(500); 
  }
}

__NO_RETURN void app_main (void *arg) 
{
	(void)arg;
}
