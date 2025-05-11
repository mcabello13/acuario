#include "cmsis_os2.h"                 
#include "sntp.h"
#include "Thread.h"
#include "rtc.h" 

osThreadId_t tid_ThreadSntp;

void Thread_sntp (void *argument);             
 
int Init_Thread_sntp (void) 
{ 
  tid_ThreadSntp = osThreadNew(Thread_sntp, NULL, NULL);
	
  if (tid_ThreadSntp == NULL) 
	{
    return(-1);
  }
 
  return(0);
}
 
//Hilo que gestiona las peticiones al servidor NTP:
void Thread_sntp (void *argument) 
{ 
  while (1) 
	{		
    get_time(); //Se realiza la peticion.
		RTC_getTime_Date();
    osThreadYield();           
  }
}
