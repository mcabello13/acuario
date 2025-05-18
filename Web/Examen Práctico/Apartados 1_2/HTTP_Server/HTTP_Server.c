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

//Identificadores de los Hilos:
extern osThreadId_t TID_Led;
extern osThreadId_t TID_RTC; 
osThreadId_t TID_Led;
osThreadId_t TID_RTC;
osThreadId_t tid_ThreadModoBajoConsumo;
osThreadId_t tid_Thread_Bomba;


static GPIO_InitTypeDef GPIO_InitStruct;


//Variables:
bool LEDrun;
bool limpiezaActiva = 0;
bool bomba = 0;
bool bar = 0;
char lcd_text[2][20+1];
int i;
RTC_AlarmTypeDef alarma2;
uint8_t tiempo;
uint8_t tempo = 0x0A;


//Declaración de Funciones:
__NO_RETURN void app_main (void *arg);
void Thread_Bomba(void);


/* IP address change notification */
void netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len) {

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

/*----------------------------------------------------------------------------
  Main Thread 'main': Run Network
 *---------------------------------------------------------------------------*/
__NO_RETURN void app_main (void *arg) 
{
	(void)arg;

	LED_Init();
	//LED_Initialize_stm();
	netInitialize(); 
	c_entry();
	initUart();
	
	Init_Thread_sntp();	
	//init_Digital_PIN_Out();
	Init_Thread_slave();
	
  TID_Led     = osThreadNew (BlinkLed, NULL, NULL);
	
  osThreadExit();
}
