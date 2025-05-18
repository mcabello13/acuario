#ifndef __PH_SENSOR_H
#define __PH_SENSOR_H

#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"

void pH_Thread(void *argument);
void MX_I2C2_Init(void);
void I2C2_GPIO_Config(void) ;
void initI2C2(void);
int Init_Thread_pH (void);

extern float phAgua;

#endif
