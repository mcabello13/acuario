#ifndef __PH_SENSOR_H
#define __PH_SENSOR_H

#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"

void pH_Thread(void *argument);
void MX_I2C2_Init(void);
void I2C2_GPIO_Config(void) ;
void pH_Sensor_Read(void);
void initI2C2(void);

extern float phAgua;

#endif
