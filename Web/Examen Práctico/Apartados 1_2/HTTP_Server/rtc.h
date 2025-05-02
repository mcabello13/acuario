#include "stm32f4xx_hal.h"
#include "rl_net.h"                    


/* Defines related to Clock configuration */
#define RTC_ASYNCH_PREDIV  0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FF /* LSE as RTC clock */

extern void c_entry(void);
extern void RTC_setTime(void);
extern void RTC_getTime_Date(void);

extern char cadenaReloj [20+1];
extern char cadenaFecha [20+1];

extern RTC_DateTypeDef sdatestructure;
extern RTC_TimeTypeDef stimestructure;
extern RTC_HandleTypeDef RtcHandle;
extern void RTC_setTime(void);

extern void SleepMode(void);
