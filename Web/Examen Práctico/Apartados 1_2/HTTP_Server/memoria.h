#ifndef __MEMORIA_H
#define __MEMORIA_H
		#include "cmsis_os2.h" 	
    #include "Driver_I2C.h"	
    #include "rtc.h"
		
		/*---------------- CONSTANTES ----------------*/
		#define TIMER_ESCRITURA 0x03 //Constante que nos informa de que se ha realizado la escritura en la memoria
		#define NUM_REGISTROS 50
		#define TAM_MAX_REGISTRO 64 
				
		
		extern osEventFlagsId_t flag_guardar_memoria;
    #define FLAG_BT_DATA (1U << 0)
		
		typedef struct {
    uint16_t direccion;
    char contenido[TAM_MAX_REGISTRO];
    } RegistroEEPROM;
  
/*---------------- STRUCT PARA DATOS BINARIOS ----------------*/
typedef struct {
    char cadenaFecha[21];
    char cadenaReloj[21];
    float luz;
    float ph;
    float turbidez;
    float temperatura;
    float corriente;
} RegistroBinario;
		/*---------------- VARIABLES ----------------*/
		
		/*---------------- FUNCIONES ----------------*/
			//Función para inicializar el puerto I2C
		int8_t I2C_Inicializacion (void); 
			//Funcion para inicializar el timer que luego utilizaremos para esperar que se ha realizado la escritura
		int8_t Init_Timer(void); 
			//Función para leer de la memoria
		int8_t lecturaMemoria( uint8_t* direccionRegistro, uint8_t* datos, uint32_t longitudLectura); 
			//Función para escribir en la memoria
		int8_t escrituraMemoria( uint8_t* datosEscribir, uint32_t longitudEscritura); 
		static void buscarUltimaPosicionEEPROM(void);
    extern void borrarEEPROM(void);
		void test_EEPROM(void);
		void Memoria(void);
		void Thread_Memoria(void *argument);
		void guardarRegistroBluetooth(char *cadenaBluetooth);
#endif
