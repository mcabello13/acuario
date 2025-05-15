#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"
#include <string.h>
#include <stdio.h>
#include "Thread.h"                  

#define PH_RX_LEN 20

I2C_HandleTypeDef hi2c2;  // Declaraci�n externa (ya inicializado en otro archivo)
int32_t convert_24bit_to_int32(uint8_t msb, uint8_t mid, uint8_t lsb);
float adc_code_to_voltage(int32_t adc_code, float vref);
float calcular_pH(float voltage_mV);
float voltageAgua = 0.0f;
float phAgua = 0;

//void pH_Thread(void *argument) {
//    char comando = 'R';
//    uint8_t buffer[PH_RX_LEN];

//    while (1) {
//        // 1. Enviar comando 'R' para solicitar lectura de pH
//        if (HAL_I2C_Master_Transmit(&hi2c1, PH_SENSOR_I2C_ADDR, (uint8_t *)&comando, 1, HAL_MAX_DELAY) != HAL_OK) {
//            printf("***** I2C ERROR (Transmisi�n) *****\n");
//            osDelay(2000);
//            continue;
//        }

//        // 2. Esperar a que el sensor prepare la respuesta (1000ms recomendado)
//        osDelay(1000);

//        // 3. Leer la respuesta
//        memset(buffer, 0, PH_RX_LEN);
//        if (HAL_I2C_Master_Receive(&hi2c1, PH_SENSOR_I2C_ADDR, buffer, PH_RX_LEN, HAL_MAX_DELAY) != HAL_OK) {
//            printf("***** I2C ERROR (Recepci�n) *****\n");
//        } else {
//            printf("Lectura de pH: %s\n", buffer);
//        }

//        osDelay(2000);  // Tiempo entre lecturas
//    }
//}

//Funci�n que inicializa el I2C:
void MX_I2C2_Init(void)
{

    __HAL_RCC_I2C2_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // Configuraci�n de pines PB10 (SCL) y PB11 (SDA)
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    hi2c2.Instance = I2C2;
    hi2c2.Init.ClockSpeed = 100000;
    hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c2.Init.OwnAddress1 = 0;
    hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c2.Init.OwnAddress2 = 0;
    hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    HAL_I2C_Init(&hi2c2);

}
#define PH_SENSOR_I2C_ADDR (0x24 << 1)
#define ADC_VREF 2.5f
#define ADC_RES 536870912.0f  // 2^29

float offset_mV = +05.0f;    //30.0f// Voltaje en pH 7 (aj�stalo seg�n tu medici�n)
float slope_mV_per_pH = 59.0f;   // Cambia por la pendiente real


typedef union 
{
    int32_t LT_INT32;
    uint32_t LT_UINT32;
    uint8_t LT_BYTE[4];
} LT_UNION_INT32_4BYTES;

// Correcto armado del c�digo ADC con desplazamiento y signo igual a Arduino
int32_t convert_24bit_to_int32(uint8_t lsb, uint8_t mid, uint8_t msb) {
    LT_UNION_INT32_4BYTES data;

    data.LT_BYTE[3] = msb;          // msb recibido �ltimo
    data.LT_BYTE[2] = mid;          // mid
    data.LT_BYTE[1] = lsb;          // lsb recibido primero
    data.LT_BYTE[0] = 0x00;         // lo que hace Arduino

    data.LT_UINT32 >>= 2;           // desplaza 2 bits a la derecha
    data.LT_BYTE[3] &= 0x3F;        // limpia los dos bits superiores

    return data.LT_INT32;
}

// Igual que Arduino
float adc_code_to_voltage(int32_t adc_code, float vref) 
{
    adc_code -= 0x20000000;  // Offset binary ? signed binary
    return ((float)adc_code / ADC_RES) * vref;
}

//Funci�n que calibra el sensor:
float calcular_pH_calibrado(float voltage_mV, float offset_mV, float slope_mV_per_pH) 
{
    phAgua = 7.0f - ((voltage_mV - offset_mV) / slope_mV_per_pH);;
    return phAgua;
}

//Funci�n que imprime por pantalla los valores medidos:
void mostrar_pH_y_voltaje(float voltage_mV) 
{
    float ph = calcular_pH_calibrado(voltage_mV* 1000.0f, offset_mV, slope_mV_per_pH);
    //printf("Voltaje PH: %.2f mV | pH: %.2f\n", voltage_mV * 1000.0f, ph);
}

//Funci�n que realiza las lecturas del Sensor de pH:
void pH_Sensor_Read(void) 
{
    uint8_t rx[3];
    int32_t adc_code = 0;

    while (1) 
		{
        if (HAL_I2C_Master_Receive(&hi2c2, PH_SENSOR_I2C_ADDR, rx, 3, HAL_MAX_DELAY) == HAL_OK) 
				{
            adc_code = convert_24bit_to_int32(rx[2], rx[1], rx[0]);  // LSB first!
            voltageAgua = adc_code_to_voltage(adc_code, ADC_VREF);
					  mostrar_pH_y_voltaje(voltageAgua);
             //float ph = calcular_pH_calibrado(voltage, offset_mV, slope_mV_per_pH);
            //printf("%ld BNC Input Voltage: %.2f mV\n", adc_code, voltageAgua * 1000.0f);
        } 
				else 
				{
            //printf("***** I2C ERROR (Lectura) *****\n");
        }
        osDelay(1000);
    }
}