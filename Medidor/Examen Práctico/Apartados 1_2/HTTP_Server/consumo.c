#include "stm32f4xx_hal.h"
#define RESOLUTION_12B 4096U
#define VREF 3.3f

static volatile uint8_t  AD_done;       /* AD conversion done flag            */
static volatile uint16_t AD_last;       /* Last converted value               */

float voltageCon = 0;

//Funcion que configura el ADC para la lectura del Consumo de tensión:
void ADC1_pins_F429ZI_config_consumo()
{
	  GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_ADC1_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
    
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

//Funcion que arranca la conversion para el Consumo de tensión:
int ADC_StartConversion_stm_consumo(ADC_HandleTypeDef *hadc, ADC_TypeDef  *ADC_Instance)
{
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)*/
  hadc->Instance = ADC_Instance;
  hadc->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc->Init.Resolution = ADC_RESOLUTION_12B;
  hadc->Init.ScanConvMode = DISABLE;
  hadc->Init.ContinuousConvMode = DISABLE;
  hadc->Init.DiscontinuousConvMode = DISABLE;
  hadc->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc->Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc->Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc->Init.NbrOfConversion = 1;
  hadc->Init.DMAContinuousRequests = DISABLE;
  hadc->Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(hadc) != HAL_OK)
  {
    return -1;
  }
	return 0;
}

//Funcion que proporciona el valor de la lectura del Consumo de tensión:
float ADC_getVoltage_stm_consumo(ADC_HandleTypeDef *hadc, uint32_t Channel)
{
		ADC_ChannelConfTypeDef sConfig = {0};
		HAL_StatusTypeDef status;

		uint32_t raw = 0;
		//float voltage = 0;
		
		/* Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.*/
		sConfig.Channel = Channel;
		sConfig.Rank = 1;
		sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
		if (HAL_ADC_ConfigChannel(hadc, &sConfig) != HAL_OK)
		{
			return -1;
		}
			
			HAL_ADC_Start(hadc);
			
			do (
				status = HAL_ADC_PollForConversion(hadc, 0)); //This funtions uses the HAL_GetTick(), then it only can be executed wehn the OS is running
		
			while(status != HAL_OK);
			
			raw = HAL_ADC_GetValue(hadc);
			
			voltageCon = raw*VREF/RESOLUTION_12B; 
			
			return voltageCon;
}
