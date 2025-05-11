#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"
#include "bomba.h"

//Inicialización del pin de la Bomba de Agua:
void Configurar_pin_bomba(void)
{
    __HAL_RCC_GPIOE_CLK_ENABLE(); //Se configura el pin PE6.

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    // Apagar bomba al inicio (relé inactivo)
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);  
}
