#ifndef __SNTP_H
#define __SNTP_H

#include "rl_net.h"                     
#include "time.h"
#include "time.h"
#include "stm32f4xx_hal.h"       
#include "stdio.h"
#include "main.h"

extern struct tm tiempo_SNTP;
extern void get_time (void);

#endif
