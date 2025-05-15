#include "cmsis_os2.h"    
#include "Thread.h"                  
#include "main.h"
#include "rtc.h"
#include <stdio.h>
#include "luz.h"


//Driver I2C:
extern ARM_DRIVER_I2C Driver_I2C1;
static ARM_DRIVER_I2C *I2Cdev = &Driver_I2C1;
uint16_t luminosidad = 0; //Variable que almacena el valor medido en ASCII.
float datosLuz = 0; //Variable que almacena la variable en decimal y ya en tanto por ciento.


//Funcion que inicializa el bus I2C de forma general para la aplicacion completa:
void initI2C(void)
{
	static uint8_t Parametros[]={0XA0,0X03}; //Valores para poder realizar el ON del sensor.
		
	I2Cdev->Initialize (NULL);//No establecemos ninguna función de callback.
	I2Cdev->PowerControl(ARM_POWER_FULL);//Arrancamos el control de energía.
	I2Cdev->Control(ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_FAST); //Establecemos velocidad de transmision de 100 KHz (Frec. señal de reloj SCL).
	I2Cdev->Control(ARM_I2C_BUS_CLEAR,0);	//Limpiamos el bus.
	
	I2Cdev->MasterTransmit(TSL,Parametros,2,false); //Se realiza el envio al registro TSL.
	while(I2Cdev->GetStatus().busy){}
}

//Funcion que realiza una medida:
float realizarMedida(void)
{
		static uint8_t registroLIR = CH0; //0xB4 --> El sensor almacena la medida en dos canales, el cero y el uno... 																	   
		static uint8_t registroIR = CH1; //0xB6      ...y las transferencias tienen doble buffer.
	  uint8_t DatosLIR[2] = {0,0}; //Dos bytes por cada Canal.
	  uint8_t DatosIR[2] = {0,0};
		
		//APUNTE (Pag 8, figura 12 del manual):
		/**********************************************************************************************************************
		* El dispositivo combina un fotodiodo de banda ancha (visible más infrarrojo) y un fotodiodo                          *
		* de respuesta infrarroja en un solo circuito integrado CMOS. Dos ADC integrados convierten las                       *
		* corrientes de los fotodiodos en una salida digital que representa la irradiancia medida en cada canal.              *
		* Esta salida digital se puede ingresar a un microprocesador donde la iluminancia (nivel de luz ambiental) en lux se  *
		* deriva utilizando una fórmula empírica para aproximar la respuesta del ojo humano.																	*
		***********************************************************************************************************************/
		
		//APUNTE(Pag 17 del manual):
		/***************************************************************************************************************************
		 * Para minimizar el skew entre el Canal 0 y el Canal 1 se recomienda leer los cuatro bytes de ADC en secuencia, es decir, *
		 * realizando una operacion de lectura I2C de cuatro bytes "(DatosLIR y DatosIR)".																				 *
		 ***************************************************************************************************************************/
		
		I2Cdev->MasterTransmit(TSL,&registroLIR,1,true);
		while(I2Cdev->GetStatus().busy);
		I2Cdev->MasterReceive(TSL,DatosLIR,2,false);//Leemos del registro. False es para indicar que la comunicacion se ha acabado. 
		while(I2Cdev->GetStatus().busy);
		
		I2Cdev->MasterTransmit(TSL,&registroIR,1,true);
		while(I2Cdev->GetStatus().busy);
		I2Cdev->MasterReceive(TSL,DatosIR,2,false);//Leemos del registro. False es para indicar que la comunicacion se ha acabado. 
		while(I2Cdev->GetStatus().busy);
		
		luminosidad = (DatosLIR[0]|(DatosLIR[1]<<8))-(DatosIR[0]|(DatosIR[1]<<8));
		datosLuz = (luminosidad*100)/619.0;          
		
		//APUNTE:
		/********************************************************************************************************************************************
		 * El resultado recibido se multiplica por cien para pasarlo a "%" y se divide entre 619 (la respuesta esta normalizada, de ahi la division)*
		 * debido a lo expuesto en la grafica de la figura 12 del manual del sensor. A partir de 600 nm se percibe el espectro infrarrojo, y        *
		 * escogemos hasta el primer pico de bajada del Canal 0, que coincide con la subida del Canal 1 a un nivel mas alto.                        *
		 ********************************************************************************************************************************************/
		
		if(datosLuz>100) //Acotamos la medida entre 0% y 100%.
		{
			datosLuz = 100;
		}
		if(datosLuz<=0)
		{
			datosLuz = 0;
		}	
		
		//printf("La luz medida es: %.2f%%\n", datosLuz);
    osDelay(3000);
		return datosLuz;
}
