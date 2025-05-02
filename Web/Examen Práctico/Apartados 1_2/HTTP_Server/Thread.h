#include "cmsis_os2.h"  
#include "Driver_I2C.h"

#define representa 0x00000001
#define alarmaLED	 0x00000010
#define alarmaLED4 0x00000100
#define alarmaLED8 0x00001000
#define s1 0x00010000
#define s2 0x00100000
#define barrido 0x01000000
#define lux 0x10000000
#define uartUno 0x00000100
#define uartDos 0x00001000

extern int Init_Thread_lcd (void);
extern int Init_Thread_sntp (void);
extern void initI2C(void);
extern float realizarMedida(void);
extern void initUart(void);
