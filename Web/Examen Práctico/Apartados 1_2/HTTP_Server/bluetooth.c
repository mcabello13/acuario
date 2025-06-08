#include "cmsis_os2.h"    
#include "Thread.h"                  
#include "main.h"
#include <Driver_USART.h>
#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdlib.h>


extern ARM_DRIVER_USART Driver_USART3; //Driver UART.
static ARM_DRIVER_USART * USARTtres = &Driver_USART3;


//Variables:
char rx = 0;
char* numero;
char cadenaDatosWeb[50] = {0};
char rxBuffer[50] = {0};
char identificador = 0;
extern bool alimentacion;
float datosLuzWeb = 0;
float datospHWeb = 0;
float datosTurbidezWeb = 0;
float datosTemperaturaWeb = 0;
float datosConsumoTensionWeb = 0;
float datosConsumoCorrienteWeb = 0;
int cuenta = 0;
int p = 0;
char buffer[35];
uint8_t cmd; //Variable que almacena el comando recibido.
osThreadId_t tid_Thread; 
void Thread (void const *argument);

//Funciones utilizadas:
void myUSART_callback(uint32_t event);
void enviarDatosWeb(char cadenaDatosWeb[50]);

//Callback de la UART:
void myUSART_callback(uint32_t event)
{
  uint32_t mask;
  mask = ARM_USART_EVENT_RECEIVE_COMPLETE  |
         ARM_USART_EVENT_TRANSFER_COMPLETE |
         ARM_USART_EVENT_SEND_COMPLETE     |
         ARM_USART_EVENT_TX_COMPLETE       ;
	
  if (event & mask) 
	{
		if (event & ARM_USART_EVENT_RECEIVE_COMPLETE)
    {
        osThreadFlagsSet(tid_Thread, 0x01); // Señal de recepción
    }

    if (event & (ARM_USART_EVENT_SEND_COMPLETE | ARM_USART_EVENT_TX_COMPLETE | ARM_USART_EVENT_TRANSFER_COMPLETE))
    {
        osThreadFlagsSet(tid_Thread, 0x02); // Señal de transmisión
    }
  }
}

//Funcion que configura la UART 3:
void initUart(void)
{
	 USARTtres->Initialize(myUSART_callback);
   USARTtres->PowerControl(ARM_POWER_FULL);
   USARTtres->Control(
        ARM_USART_MODE_ASYNCHRONOUS | 
        ARM_USART_DATA_BITS_8 | 
        ARM_USART_PARITY_NONE | 
        ARM_USART_STOP_BITS_1 | 
			  ARM_USART_FLOW_CONTROL_NONE, 9600);
	 
  USARTtres->Control (ARM_USART_CONTROL_TX, 1);
  USARTtres->Control (ARM_USART_CONTROL_RX, 1);
}

//Funcion que inicializa hilos y timers:
int Init_ThreadCMSIS (void) 
{
  tid_Thread = osThreadNew (Thread, NULL, NULL);
  if (!tid_Thread) return(-1);
	
  return(0);
}

//Hilo
void Thread (void const *argument) 
{
	int alimentacionAnterior = 0;
	
  while(1)
  {    	
		if(alimentacion != alimentacionAnterior)
		{
			alimentacionAnterior = alimentacion;			
			
			if(alimentacion == 1)
			{
					identificador = 'C';			
			}
			else
			{
					identificador = 'O';
			}
			USARTtres->Send(&identificador, 1);
			osThreadFlagsWait(0x02, NULL, osWaitForever);
		}
		else
		{		
			p = 0;
			memset(rxBuffer, 0, sizeof(rxBuffer));

      while (p < sizeof(rxBuffer) - 1)
      {
        USARTtres->Receive(&rx, 1);
        osThreadFlagsWait(0x01, NULL, osWaitForever);

        if (rx == 'A') 
        {
            rxBuffer[p] = '\0'; 
            strcpy(cadenaDatosWeb, rxBuffer);
            enviarDatosWeb(cadenaDatosWeb);
            break;
        }
        else
        {
            rxBuffer[p++] = rx;
        }
      }
		}
  }
}

//Función que particiona la cadena de datos recibida para representarlos en la Web:
void enviarDatosWeb(char cadenaDatosWeb[50])
{	
	numero = strtok(&cadenaDatosWeb[1], "W"); //Se empieza desde el segundo carácter para evitar el flag "B".
	datosLuzWeb = atof(numero);

	numero = strtok(NULL, "W");
	datospHWeb = atof(numero);

	numero = strtok(NULL, "W");
	datosTemperaturaWeb = atof(numero);
	
	numero = strtok(NULL, "W");
	datosTurbidezWeb = atof(numero);
	
	numero = strtok(NULL, "W");
	datosConsumoTensionWeb = atof(numero);
	
	numero = strtok(NULL, "W");
	datosConsumoCorrienteWeb = atof(numero);
}
