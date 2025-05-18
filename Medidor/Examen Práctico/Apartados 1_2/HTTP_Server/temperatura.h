/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TEMPERATURA_H
#define __TEMPERATURA_H

#include "stm32f4xx_hal.h"
#include "math.h"

#define RESOLUTION_12B 4096U
#define VREF 3.3f
	
void 	ADC1_pins_F429ZI_config_temperatura(void);
int 	ADC_Init_Single_Conversion(ADC_HandleTypeDef *, ADC_TypeDef  *);
float ADC_getVoltage(ADC_HandleTypeDef * , uint32_t );
int 	Init_Thread_temp (void);
void 	Thread_temp (void *argument);

#endif
