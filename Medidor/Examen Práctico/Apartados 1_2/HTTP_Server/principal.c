//-------------------------------------------
//PROGRAMA PRINCIPAL DEL MEDIDOR DEL ACUARIO.
//-------------------------------------------

#include <stdio.h>
#include "main.h"               
#include "stm32f4xx_hal.h"              
#include "leds.h"
#include "lcd.h"
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

//extern osThreadId_t TID_Display;
//extern osThreadId_t TID_Led;
extern osThreadId_t TID_turbidez;
extern osThreadId_t TID_luz;
extern osThreadId_t TID_ph;

//Declaraciones de los Hilos:
//static void BlinkLed (void *arg);
//static void Display  (void *arg);

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


/*----------------------------------------------------------------------------
  LCD display handler
 *---------------------------------------------------------------------------*/
/***************************************************************************************************
* EXPLICACION: en esta funcion se realiza la representacion de los textos escritos en la web. Es   *
* importante destacar que la inicializacion y el reset del LCD se realiza en esta funcion y no     *
* en el main del servidor o en el main del microprocesador (si se hace esto, se bloquea). Se       *
* utiliza la señal 0x01U proporcionada por el proyecto base, que cada vez que se introduce un      *
* texto desde la web, se envia desde el cgi hasta esta funcion, que actualizara el LCD.            *
****************************************************************************************************/

//Funcion que realiza las representaciones en el LCD:
/*static __NO_RETURN void Display (void *arg) 
{
	(void)arg;
	LEDrun = false;
	static char buf[24];
	
	EscribeFraseL1(lcd_text[0]);
	EscribeFraseL2(lcd_text[1]);

  while(1) 
	{
		osThreadFlagsWait (0x01U, osFlagsWaitAny, osWaitForever); //Se utiliza el hilo que se proporciona por defecto. La señal proviene del archivo HTTP_Server_CGI.
		
		sprintf (buf, "%-20s", lcd_text[0]);
		sprintf (buf, "%-20s", lcd_text[1]);
		
		limpiardisplay();
		EscribeFraseL1(lcd_text[0]);		
		EscribeFraseL2(lcd_text[1]);
  }
}*/

/*----------------------------------------------------------------------------
  Blink the LEDs on an eval board
 *---------------------------------------------------------------------------*/
/**********************************************************************************************
 * EXPLICACION: en esta funcion se realiza el barrido de los LED, primeramente se han         *
 * modificado los valores de led_val para el correcto encendido-apagado y el resto de la      *
 * funcion se ha mantenido como en el ejemplo de partida. Cabe destacar la funcion            *
 * LED_SetOut, que es la encargada de realizar el ON y OFF de los LED. Recibe como            *
 * parametro el contador para saber que LED debe encenderse.																	*
 **********************************************************************************************/

//Funcion que realiza el barrido de los LEDs:
/*static __NO_RETURN void BlinkLed (void *arg) 
{
  const uint8_t led_val[6] = {0x01,0x02,0x04};
  int cnt = 0;

  (void)arg;

  LEDrun = false;
  while(1) 
  {    
    if (LEDrun == true) //Cada 100 ms.
    {
      LED_SetOut_stm (led_val[cnt]);
			
      if (++cnt >= sizeof(led_val)) 
      {
        cnt = 0;
      }
    }
    osDelay (100);
  }
}*/
