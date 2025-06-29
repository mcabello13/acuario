#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "rtc.h"
#include "sntp.h"
#include "Thread.h"
#include "rl_net.h"
#include "stm32f4xx.h"

//Variables:
GPIO_InitTypeDef GPIO_InitStruct;
RTC_HandleTypeDef RtcHandle; //RTC handler declaration.
RTC_DateTypeDef sdatestructure;
RTC_TimeTypeDef stimestructure;
RTC_AlarmTypeDef alarma; 
int al = 0;
char cadenaReloj [20+1];
char cadenaFecha [20+1];
int c=0;
int dia, mes, ano, horas, minutos, segundos;


//Funcion que inicializa el RTC:
void c_entry(void)
{
	/****************************************************************************************************************************
	 * --> OJO: por defecto viene configurado con el reloj HSE (rapido), pero hay que cambiarlo al LSE para que cuente bien!!!  *
	 ****************************************************************************************************************************/
	
  //HAL_PWR_EnableBkUpAccess(); 
  //__HAL_RCC_RTC_CONFIG(RCC_RTCCLKSOURCE_HSE_DIV8);
  //__HAL_RCC_RTC_ENABLE();	
	
  RtcHandle.Instance = RTC; 
  RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
  RtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
  RtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
  RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	__HAL_RTC_RESET_HANDLE_STATE(&RtcHandle);
	
  if(HAL_RTC_Init(&RtcHandle)!=HAL_OK)
  {
    
  }
	
	if (HAL_RTCEx_BKUPRead(&RtcHandle, RTC_BKP_DR1) != 0x32F2)
  {
    /* Configure RTC Calendar */
    RTC_setTime(); 
  }
	else
  {
    /* Check if the Power On Reset flag is set */
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET)
    {

    }
    /* Check if Pin Reset flag is set */
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET)
    {

    }
    /* Clear source Reset Flag */
   //__HAL_RCC_CLEAR_RESET_FLAGS();
  }
}

//Manejador de la alarma:
void RTC_Alarm_IRQHandler(void)
{	
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_11,1);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,1);		
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,0);

	osDelay(3000);

  HAL_RTC_AlarmIRQHandler(&RtcHandle); //...y bajamos el flag.
}

//Funcion que configura Fecha y Tiempo del RTC:
void RTC_setTime(void)
{	
	 /*##-1- Configure the Date #################################################*/
  sdatestructure.Year = tiempo_SNTP.tm_year+1900 /*23*/;
  sdatestructure.Month = tiempo_SNTP.tm_mon+1 /*RTC_MONTH_MARCH*/;
  sdatestructure.Date = tiempo_SNTP.tm_mday /*07*/;

	HAL_RTC_SetDate(&RtcHandle, &sdatestructure, RTC_FORMAT_BIN); //Tras configurar la fecha completa, hay que actualizar el RTC con "SetDate".
	
  /*##-2- Configure the Time #################################################*/
  stimestructure.Hours = tiempo_SNTP.tm_hour+1 /*16*/;
  stimestructure.Minutes = tiempo_SNTP.tm_min /*12*/;
  stimestructure.Seconds = tiempo_SNTP.tm_sec /*50*/;
  stimestructure.TimeFormat = /*RTC_HOURFORMAT12_AM*/ RTC_HOURFORMAT_24;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

	HAL_RTC_SetTime(&RtcHandle,&stimestructure,RTC_FORMAT_BIN); //Tambien hay que actualizarlo con "SetTime".
}

//Funcion que obtiene el Tiempo y Fecha del RTC y lo escribe en el LCD:
void RTC_getTime_Date(void)
{
	RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;
	
  HAL_RTC_GetTime(&RtcHandle, &stimestructureget, RTC_FORMAT_BIN); //Get the RTC current Time.  
  HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, RTC_FORMAT_BIN); //Get the RTC current Date.
	
	dia = sdatestructureget.Date;
	mes = sdatestructureget.Month; 
	ano = sdatestructureget.Year;
	horas = stimestructureget.Hours;
	minutos = stimestructureget.Minutes;
	segundos = stimestructureget.Seconds;
  
	sprintf(cadenaReloj,"%.2d:%.2d:%.2d", horas+1, minutos, segundos);	
	osDelay(100);
	sprintf(cadenaFecha,"%.2d/%.2d/%.2d", dia, mes, ano-48);
}

//Funcion que sobrescribe la misma funcion de la capa HAL para que el RTC utilice el reloj LSE en vez del HSE:
// ---> OJO: esto se realiza debido a que va muy rapido con el HSE, no hay que invocar la funcion para nada, simplemente colocarla aqui para que se sobreescriba!!!
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
  RCC_OscInitTypeDef        RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

  /*##-1- Enables the PWR Clock and Enables access to the backup domain ###################################*/
  /* To change the source clock of the RTC feature (LSE, LSI), You have to:
     - Enable the power clock using __HAL_RCC_PWR_CLK_ENABLE()
     - Enable write access using HAL_PWR_EnableBkUpAccess() function before to 
       configure the RTC clock source (to be done once after reset).
     - Reset the Back up Domain using __HAL_RCC_BACKUPRESET_FORCE() and 
       __HAL_RCC_BACKUPRESET_RELEASE().
     - Configure the needed RTc clock source */
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWR_EnableBkUpAccess();

  
  //##-2- Configure LSE as RTC clock source ###################################/
  RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  { 
    
  }
  
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  { 
   
  }
  
  //##-3- Enable RTC peripheral Clocks #######################################/
  /* Enable RTC Clock */
  __HAL_RCC_RTC_ENABLE();
}
