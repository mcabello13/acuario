///* Define to prevent recursive inclusion -------------------------------------*/
//#ifndef __TEMPERATURA_H
//#define __TEMPERATURA_H

//#include "stm32f4xx_hal.h"
//#include "math.h"

//#define RESOLUTION_12B 4096U
//#define VREF 3.3f
//	
//void 	ADC1_pins_F429ZI_config_consumo(void);
//int 	ADC_Init_Single_Conversion_consumo(ADC_HandleTypeDef *, ADC_TypeDef  *);
//float ADC_getVoltage_consumo(ADC_HandleTypeDef * , uint32_t );
//int 	Init_Thread_consumo (void);
//void 	Thread_consumo (void *argument);

//#endif





#include "stm32f4xx_hal.h"

#ifndef __ADC_H
	void ADC1_pins_F429ZI_config_consumo(void);
	int ADC_StartConversion_stm_consumo(ADC_HandleTypeDef *hadc, ADC_TypeDef  *ADC_Instance);
	float ADC_getVoltage_stm_consumo(ADC_HandleTypeDef *hadc, uint32_t Channel);
#endif

