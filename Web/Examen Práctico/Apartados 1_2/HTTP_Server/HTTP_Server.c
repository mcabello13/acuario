//-------------------------------------------
//PROGRAMA PRINCIPAL DE LA WEB DEL ACUARIO.
//-------------------------------------------

#include <stdio.h>
#include "main.h"
#include "rl_net.h"                   
#include "stm32f4xx_hal.h"              
#include "leds.h"
#include "lcd.h"
#include "adc.h"
#include "rtc.h"
#include "Thread.h"
#include "sntp.h"
#include "bluetooth.h"

// Main stack size must be multiple of 8 Bytes
#define APP_MAIN_STK_SZ (1024U)
uint64_t app_main_stk[APP_MAIN_STK_SZ / 8];
const osThreadAttr_t app_main_attr = {
  .stack_mem  = &app_main_stk[0],
  .stack_size = sizeof(app_main_stk)
};

//Hilos:
extern osThreadId_t TID_Display;
extern osThreadId_t TID_Led;
extern osThreadId_t TID_RTC; 

//Declaraciones de los hilos:
static void BlinkLed (void *arg);
static void Display  (void *arg);

//Identificadores de hilos:
osThreadId_t TID_Display;
osThreadId_t TID_Led;
osThreadId_t TID_RTC;
osThreadId_t tid_Threadrandom; 

static GPIO_InitTypeDef GPIO_InitStruct;

//Variables:
float datosRandom = 0;
float datosRandomTurbidez = 0;
bool LEDrun;
bool o;
bool u = 1;
bool bar = 0;
char lcd_text[2][20+1];
int i;
RTC_AlarmTypeDef alarma2;
uint8_t tiempo;
uint8_t tempo = 0x0A;


//Declaración de Funciones:
__NO_RETURN void app_main (void *arg);
void Thread_random (void *argument); 


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
	(void)arg;
	LEDrun = false;
	static char buf[24];

	// ---> Comentamos estas dos lineas porque representamos el reloj con RTC y al entrar en el menu del server del LCD crea conflicto.
	//init(); //IMPORTANTE --> inicializar y resetear el LCD aqui y no en el main del micro o en el del servidor.
	//LCD_reset();

	 // --> Otra forma de hacerlo:
	/***************************/
	//escribeLinea1(lcd_text[0]);
	//escribeLinea2(lcd_text[1]);
	
	EscribeFraseL1(lcd_text[0]);
	EscribeFraseL2(lcd_text[1]);

  while(1) 
	{
		osThreadFlagsWait (0x01U, osFlagsWaitAny, osWaitForever); //Se utiliza el hilo que se proporciona por defecto. La señal proviene del archivo HTTP_Server_CGI.
		
		sprintf (buf, "%-20s", lcd_text[0]);
		sprintf (buf, "%-20s", lcd_text[1]);
		
   	// --> Otra forma de hacerlo:
	 /***************************/
		//LCD_clear_L1();
		//escribeLinea1(lcd_text[0]); 
   	//LCD_clear_L2();
		//escribeLinea2(lcd_text[1]);*/
		
		limpiardisplay();
		EscribeFraseL1(lcd_text[0]);		
		EscribeFraseL2(lcd_text[1]);
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

//////////////FUNCION QUE INICIALIZA LOS LED NORMALES Y EL RGB:
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











/**************************************************
HILO PARA SIMULAR VALORES RANDOM DE LUZ Y PH
**************************************************/
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
	
	
	
	
	//Inicializacion de los LED normales mas el RGB para ejemplo de ESTUDIO:
	//LED_Init();
	//LED_Initialize_stm();
	netInitialize(); //Se inicializan los LED, el ADC y la red.
	//init(); //Inicializacion del LCD.
	//LCD_reset();
	c_entry();
	initUart();
	
	Init_Thread_lcd();
	Init_Thread_sntp();	
	
	configurar_esclavo_bluetooth();
	Init_Thread_slave();
	
  TID_Led     = osThreadNew (BlinkLed, NULL, NULL);
  TID_Display = osThreadNew (Display,  NULL, NULL);
	
  osThreadExit();
}
