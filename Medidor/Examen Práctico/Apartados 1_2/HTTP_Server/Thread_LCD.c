#include "cmsis_os2.h"                        
#include "rtc.h"
#include "Thread.h"                      
 
osThreadId_t tid_ThreadLcd; 

void Thread_lcd (void *argument);             
 
int contador = 0;
 
int Init_Thread_lcd (void) 
{ 
  tid_ThreadLcd = osThreadNew(Thread_lcd, NULL, NULL);
	
  if (tid_ThreadLcd == NULL) 
	{
    return(-1);
  }
 
  return(0);
}
 
//Hilo que gestiona la representacion en el LCD de la fecha y la hora:
void Thread_lcd (void *argument) 
{ 
  while (1) 
	{
		osThreadFlagsWait(representa, osFlagsWaitAny, osWaitForever);
		
		//RTC_getTime_Date(); //Se actualiza el RTC con lo que llega del servidor NTP. Se representa en esta funcion.
    
    osThreadYield();                        
  }
}
