#include "stm32f4xx_hal.h"

#ifndef __ADC_H
	void ADC3_pins_F429ZI_config();
	int ADC_StartConversion_stm(ADC_HandleTypeDef *hadc, ADC_TypeDef  *ADC_Instance);
	float ADC_getVoltage_stm(ADC_HandleTypeDef *hadc, uint32_t Channel);
  void deInit_Digital_PIN_Out_turbidez(void);
  void Init_Digital_PIN_Out(void);
#endif
