#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "rtc.h"
#include "lcd.h"
#include "sntp.h"
#include "Thread.h"
#include "rl_net.h"


//Variables:
RTC_HandleTypeDef RtcHandle; //RTC handler declaration.
RTC_DateTypeDef sdatestructure;
RTC_TimeTypeDef stimestructure;
RTC_AlarmTypeDef alarma; 
int al = 0;
char cadenaReloj [20+1];
char cadenaFecha [20+1];
int c=0;
int dia, mes, ano, horas, minutos, segundos;
int modoSleep = 0;


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

	//Configuracion de la Alarma:
	
	/******************************************************************************************************************************************
	 * EXPLICACION: se configura la alarma habilitando primeramente con NVIC, el manejador de las interrupciones.                             *
	 * A continuacion configuramos el registro Seconds porque queremos que salte cada cinco segundos "0x05", el resto                         *
	 * de parametros se configuran como vienen de base, pero cabe destacar la mascara, que debe estar configurada para                        *
	 * que salten los segundos, de ahi que --> alarma.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY|RTC_ALARMMASK_HOURS|RTC_ALARMMASK_MINUTES. Por    *
	 * ultimo se configura "SetAlarm" pasando como parametro la estructura "alarma" con toda la configuracion completa.                       *
	 ******************************************************************************************************************************************/
	
	/*HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
	alarma.AlarmTime.Seconds = 0x05; // ---> Cada cinco segundos.
	alarma.AlarmTime.TimeFormat = RTC_HOURFORMAT12_AM;
  alarma.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	alarma.Alarm = RTC_ALARM_A;
  alarma.AlarmTime.StoreOperation = RTC_STOREOPERATION_SET;
  alarma.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY|RTC_ALARMMASK_HOURS|RTC_ALARMMASK_MINUTES;
  alarma.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  alarma.AlarmDateWeekDay = 0x1;
	HAL_RTC_SetAlarm_IT(&RtcHandle, &alarma, FORMAT_BIN);*/
}

//Manejador de la alarma:
void RTC_Alarm_IRQHandler(void)
{
	//al = 1; //Se activa esta variable para hacer ver que la alarma ha entrado en el manejador...
	//DE LA PRACTICA 2 ---> osThreadFlagsSet(tid_ThreadAlarma, alarmaLED); //...lanzamos la señal al hilo de la alarma que gestiona el LED verde...
	
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_11,1);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,1);		
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,0);

	osDelay(3000);
	osThreadFlagsSet(tid_ThreadAlarma, 0x00000100);

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
  stimestructure.Hours = tiempo_SNTP.tm_hour /*16*/;
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
	
	if(modoSleep == 1) //En este "if" se realiza la comprobacion de si el sistema viene del Modo Sleep...
	{
		enciendeLCD_consumo(); //...si es asi, se resetea la variable "modoSleep" y se apaga el LED rojo.
		modoSleep = 0;		
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14); 
	}
  
	sprintf(cadenaReloj,"%.2d:%.2d:%.2d", horas+1, minutos, segundos);	
	EscribeTiempo(cadenaReloj);
	osDelay(100);
	sprintf(cadenaFecha,"%.2d/%.2d/%.2d", dia, mes, ano-48);
	EscribeFecha(cadenaFecha);
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

/***************************************************************************************
 * EXPLICACION: para que el sistema entre en Modo Sleep, se apaga el Systick porque es *
 * el reloj del sistema.																															 *
 ***************************************************************************************/

//Funcion para el Modo Sleep:
void SleepMode(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
	
	//Configuracion del LED Rojo:
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Pin = GPIO_PIN_14;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	
	
	modoSleep = 1; //Se activa el Modo Sleep...
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14); //...se enciende el LED Rojo...
	apagarLCD_consumo(); //...y se apaga el LCD.
	
  __HAL_RCC_GPIOC_CLK_ENABLE(); //Se configura el Pulsador Azul...
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  HAL_SuspendTick(); //...y una vez configurado, se suspende el reloj del sistema.
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWR_EnterSLEEPMode(0, PWR_SLEEPENTRY_WFI);
}
