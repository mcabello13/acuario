#ifndef __ALIM_PEZ_H
#define __ALIM_PEZ_H

#include "cmsis_os2.h"  
#include "stm32f4xx_hal.h"

int Init_Thread_alim_pez (void);
void function_th_alim_pez (void *argument);
void init_Digital_PIN_Out(void);
void Step1(void);
void Step2(void);
void Step3(void);
void Step4(void);
void Step5(void);
void Step6(void);
void Step7(void);
void Step8(void); 

#endif
