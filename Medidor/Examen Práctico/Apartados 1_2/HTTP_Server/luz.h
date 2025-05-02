#include "cmsis_os2.h"  
#include "Driver_I2C.h"

//Sensor TSL2591
#define TSL 0x29 //Direcciones del Sensor de Luz para el bus I2C.
#define CH0 0xB4 //Canal 0.
#define CH1 0xB6 //Canal 1.

extern float realizarMedida(void);
extern void initI2C(void);

extern float datosLuz;
