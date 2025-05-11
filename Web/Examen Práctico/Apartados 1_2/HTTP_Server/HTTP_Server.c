//-------------------------------------------
//PROGRAMA PRINCIPAL DE LA WEB DEL ACUARIO.
//-------------------------------------------

#include <stdio.h>
#include "main.h"
#include "rl_net.h"                   
#include "stm32f4xx_hal.h"              
#include "leds.h"
#include "adc.h"
#include "rtc.h"
#include "Thread.h"
#include "sntp.h"
#include "bluetooth.h"
#include "alimentacion.h"
#include "bomba.h"

// Main stack size must be multiple of 8 Bytes
#define APP_MAIN_STK_SZ (1024U)
uint64_t app_main_stk[APP_MAIN_STK_SZ / 8];
const osThreadAttr_t app_main_attr = {
  .stack_mem  = &app_main_stk[0],
  .stack_size = sizeof(app_main_stk)
};

//Declaraciones de los hilos:
static void BlinkLed (void *arg);
static void Display  (void *arg);

//Identificadores de los Hilos:
//extern osThreadId_t TID_Display;
extern osThreadId_t TID_Led;
extern osThreadId_t TID_RTC; 
osThreadId_t TID_Display;
osThreadId_t TID_Led;
osThreadId_t TID_RTC;
osThreadId_t tid_ThreadModoBajoConsumo;
osThreadId_t tid_Threadrandom; 
osThreadId_t th_alim_pez;
osThreadId_t tid_Thread_Bomba;


static GPIO_InitTypeDef GPIO_InitStruct;


//Variables:
float datosRandom = 0;
float datosRandomTurbidez = 0;
bool LEDrun;
bool limpiezaActiva = 0;
bool alimentacion = 0;
bool bomba = 0;
bool bar = 0;
char lcd_text[2][20+1];
int i;
RTC_AlarmTypeDef alarma2;
uint8_t tiempo;
uint8_t tempo = 0x0A;


//Declaración de Funciones:
__NO_RETURN void app_main (void *arg);
void Thread_random (void *argument); 
void Thread_Bomba(void);


/* IP address change notification */
void netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len) {

  /*(void)if_num;
  (void)val;
  (void)len;

  if (option == NET_DHCP_OPTION_IP_ADDRESS) {
    //IP address change, trigger LCD update 
    osThreadFlagsSet (TID_Display, 0x01);
  }*/
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
static __NO_RETURN void Display (void *arg) 
{
  while(1) 
	{
	
  }
}

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
static __NO_RETURN void BlinkLed (void *arg) 
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
}

//Función que inicializa los LED de la placa y los RGB:
void LED_Init(void)
{
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_7 | GPIO_PIN_14; 
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_RESET);
	
	//LED RGB:
	__HAL_RCC_GPIOD_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_13 |  GPIO_PIN_12 | GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_11,GPIO_PIN_RESET);
	
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_11,1); //De partida, apagados.
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,1);		
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,1);
	
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);	
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
 
/*----------------------------------------------------------------------------
  Alimentación de los Peces
 *---------------------------------------------------------------------------*/
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

//Hilo que controla la Bomba de Agua:
void Thread_Bomba(void)
{
    while (1)
    {
			if(bomba == 0)
			{
				printf("Bomba OFF\n");
				
				//Apagar la bomba 
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);
				osDelay(5000); //Encendida 5 segundos
			}
			else if(bomba == 1)
			{
				printf("Bomba ON\n");
			
				// Encender la bomba  (relé inactivo)
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);
				osDelay(5000); //Apagada 5 segundos
			}
    }
}




















/***************************************************************************
HILO PARA SIMULAR VALORES RANDOM DE LUZ Y PH HASTA NO TENER EL BLUETOOTH
****************************************************************************/
int Init_Thread_random (void) 
{
  tid_Threadrandom = osThreadNew(Thread_random, NULL, NULL);
	
  if (tid_Threadrandom == NULL) 
	{
    return(-1);
  }
 
  return(0);
}
void Thread_random (void *argument) 
{ 	
	while(1)
	{
			datosRandom = (datosRandom * 3.01f + 1.01f);  // Constantes arbitrarias
			//datosRandom = (datosRandom*100)/619.01;    
		
			//datosRandomTurbidez = (datosRandomTurbidez * 2.01f + 1.01f);

			while (datosRandom > 100.00f)
			{
					datosRandom -= 100.00f;  //Mantener dentro del rango [0,100)
			}			
			/*while (datosRandomTurbidez > 3.3f)
			{            
					datosRandomTurbidez -= 3.3f;  //Mantener dentro del rango [0 ; 3.3)
			}*/
			
			// Truncar a 2 decimales
			datosRandom = (int)(datosRandom * 100) / 100.0f;
			
			osDelay(6000);
	}
}



















/*----------------------------------------------------------------------------
  Main Thread 'main': Run Network
 *---------------------------------------------------------------------------*/
__NO_RETURN void app_main (void *arg) 
{
	(void)arg;

	
	
	Init_Thread_random();
	
	
	
	

	LED_Init();
	//LED_Initialize_stm();
	netInitialize(); //Se inicializan los LED, el ADC y la red.
	//init(); //Inicializacion del LCD.
	//LCD_reset();
	c_entry();
	initUart();
	
	Init_Thread_sntp();	
	Init_Thread_alim_pez();
	init_Digital_PIN_Out();
	Configurar_pin_bomba();
	Init_Thread_Bomba();
	
	configurar_esclavo_bluetooth();
	Init_Thread_slave();
	
  TID_Led     = osThreadNew (BlinkLed, NULL, NULL);
  //TID_Display = osThreadNew (Display,  NULL, NULL);
	
  osThreadExit();
}
