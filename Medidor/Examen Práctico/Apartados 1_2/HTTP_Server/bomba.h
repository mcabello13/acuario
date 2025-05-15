#ifndef BOMBA_H
#define BOMBA_H

#include "stm32f4xx_hal.h"

// Definición del puerto y pin de la Bomba de Agua
#define BOMBA_GPIO_PORT      GPIOE
#define BOMBA_GPIO_PIN       GPIO_PIN_6

//Funciones:
void Configurar_pin_bomba(void);
void Thread_Bomba(void);

#endif // BOMBA_H