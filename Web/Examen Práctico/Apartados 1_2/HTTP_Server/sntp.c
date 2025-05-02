#include "rl_net.h"                     
#include "lcd.h"
#include "time.h"
#include "rtc.h"
#include "sntp.h"
#include "time.h"
#include "stm32f4xx_hal.h"       
#include "stdio.h"
#include "main.h"

//const NET_ADDR4 ntp_server = { NET_ADDR_IP4, 0, 130, 206, 3, 166 }; //--> Otra forma de pasar la direccion IP del servidor...
                                                                     //...si utilizamos esta, no hace falta poner la IP en el archivo "Net_Config_ETH_0".
static void time_callback (uint32_t seconds, uint32_t seconds_fraction);

//Variables:
uint32_t s = 0;
struct tm tiempo_SNTP;

//Funcion que realiza la peticion al servidor NTP:
void get_time (void) 
{
  if (netSNTPc_GetTime (/*(NET_ADDR *)&ntp_server*/NULL, time_callback) == netOK) 
	{
		osDelay(3000);
  }
  else 
	{
    //printf ("SNTP not ready or bad parameters.\n");
  }
}
 
//Funcion callback que actualiza el tiempo del RTC al que llega desde el servidor NTP:
static void time_callback (uint32_t seconds, uint32_t seconds_fraction) 
{
	s = seconds;
	
  if (s == 0) 
	{
    //printf ("Server not responding or bad response.\n");
  }
  else 
	{
		tiempo_SNTP = *localtime(&s);
		RTC_setTime();
  }
}
