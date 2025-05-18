#ifndef __ALIM_PEZ_H
#define __ALIM_PEZ_H

#include "cmsis_os2.h"  
#include "stm32f4xx_hal.h"

int Init_Thread_alim_pez (void);
void function_th_alim_pez (void *argument);
void init_Digital_PIN_Out(void);

#endif
