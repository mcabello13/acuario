#include "memoria.h"
#include "cmsis_os2.h"
#include <stdio.h>
#include <string.h>
#include "rtc.h"
#include "bluetooth.h"

#define ADDRESS_MEMORIA 0x50 // Dirección de la memoria EEPROM
#define EEPROM_PAGE_SIZE 64

osEventFlagsId_t flag_guardar_memoria;

//Variables:
extern ARM_DRIVER_I2C Driver_I2C1;
extern char cadenaReloj[21],cadenaFecha[21];
extern float datosLuzWeb, datospHWeb, datosTemperaturaWeb, datosTurbidezWeb, datosConsumoTensionWeb, datosConsumoCorrienteWeb;

static ARM_DRIVER_I2C *I2Cdrv = &Driver_I2C1;
static volatile uint32_t I2C_Event;
osThreadId_t tid_ThreadMemoria;
static uint16_t current_eeprom_address = 0;
static uint8_t indiceCircular = 0;
RegistroBinario reg;
uint16_t direccionEEPROM = 0;

//Funciones:
extern void Thread_Memoria(void *argument);
static void I2C_SignalEvent(uint32_t event);
void guardarRegistroEEPROM(void);
int8_t escrituraMemoria(uint8_t* datosEscribir, uint32_t longitudEscritura);
int8_t lecturaMemoria(uint8_t* direccionRegistro, uint8_t* datos, uint32_t longitudLectura);
void verificarUltimoRegistroEEPROM(uint16_t start_address, uint16_t length);

//Función que realiza el guardado en memori:
void guardarRegistroEEPROM(void) 
{
	RTC_getTime_Date();  // Actualiza cadenaFecha y cadenaReloj
	
	// Copia segura de las cadenas
	printf("cadenaFecha global: %s\n", cadenaFecha);
	printf("cadenaReloj global: %s\n", cadenaReloj);

	strncpy(reg.cadenaFecha, cadenaFecha, sizeof(reg.cadenaFecha));
	reg.cadenaFecha[sizeof(reg.cadenaFecha) - 1] = '\0';  // Terminar cadena

	strncpy(reg.cadenaReloj, cadenaReloj, sizeof(reg.cadenaReloj));
	reg.cadenaReloj[sizeof(reg.cadenaReloj) - 1] = '\0';

	printf("reg.cadenaFecha: %s\n", reg.cadenaFecha);
	printf("reg.cadenaReloj: %s\n", reg.cadenaReloj);

	// Copia de valores numéricos
	reg.luz = datosLuzWeb;
	reg.ph = datospHWeb;
	reg.turbidez = datosTurbidezWeb;
	reg.temperatura = datosTemperaturaWeb;
	reg.corriente = datosConsumoCorrienteWeb;

	// Dirección en memoria
	direccionEEPROM = indiceCircular * sizeof(RegistroBinario);
	uint8_t buffer[sizeof(RegistroBinario) + 2];

	buffer[0] = (direccionEEPROM >> 8) & 0xFF;
	buffer[1] = direccionEEPROM & 0xFF;
	memcpy(&buffer[2], &reg, sizeof(RegistroBinario));

	if (escrituraMemoria(buffer, sizeof(buffer)) == 0) {
			printf("[EEPROM] Guardado binario en 0x%04X OK\n", direccionEEPROM);
			verificarUltimoRegistroEEPROM(direccionEEPROM, sizeof(RegistroBinario));
			indiceCircular = (indiceCircular + 1) % NUM_REGISTROS;
	} else {
			printf("[EEPROM] Error al guardar binario en EEPROM\n");
	}
}

//Función que comprueba si se ha producido un evento del bus I2C:
static void I2C_SignalEvent(uint32_t event) 
{
  I2C_Event |= event;
}

//Función que inicializa el I2C:
int8_t I2C_Inicializacion(void) 
{
	uint8_t val;
	uint8_t registroPrueba[2] = {0x00, 0x00};

	I2Cdrv->Initialize(I2C_SignalEvent);
	I2Cdrv->PowerControl(ARM_POWER_FULL);
	I2Cdrv->Control(ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_FAST);
	I2Cdrv->Control(ARM_I2C_BUS_CLEAR, 0);

	return lecturaMemoria(registroPrueba, &val, 1);
}

//Hilo que gestiona el guardado en memoria:
void Memoria(void) 
{
	tid_ThreadMemoria = osThreadNew(Thread_Memoria, NULL, NULL);
  flag_guardar_memoria = osEventFlagsNew(NULL);
}

//Hilo que gestiona la memoria:
void Thread_Memoria(void *argument) 
{
	I2C_Inicializacion();
	buscarUltimaPosicionEEPROM();
	
	while (1) 
	{
		uint32_t flags = osEventFlagsWait(flag_guardar_memoria, FLAG_BT_DATA, osFlagsWaitAny, osWaitForever);
		if (flags & FLAG_BT_DATA) 
		{
				guardarRegistroEEPROM();
		}
	}
}

//Función que realiza la escritura en memoria:
int8_t escrituraMemoria(uint8_t* datosEscribir, uint32_t longitudEscritura)
{
    I2C_Event = 0U;
    I2Cdrv->MasterTransmit(ADDRESS_MEMORIA, datosEscribir, longitudEscritura, false);
    while ((I2C_Event & ARM_I2C_EVENT_TRANSFER_DONE) == 0U);
    if ((I2C_Event & ARM_I2C_EVENT_TRANSFER_INCOMPLETE) != 0U) return -1;
    return 0;
}

//Función que realiza la lectura de la memoria:
int8_t lecturaMemoria(uint8_t* direccionRegistro, uint8_t* datos, uint32_t longitudLectura) {
    I2C_Event = 0U;
    printf("[I2C] Leyendo %lu bytes desde 0x%02X%02X\n", longitudLectura, direccionRegistro[0], direccionRegistro[1]);

    I2Cdrv->MasterTransmit(ADDRESS_MEMORIA, direccionRegistro, 2, true);
    while ((I2C_Event & ARM_I2C_EVENT_TRANSFER_DONE) == 0U);

    if ((I2C_Event & ARM_I2C_EVENT_TRANSFER_INCOMPLETE) != 0U) 
		{
			printf("[I2C] Transmit fallido\n");
			return -1;
    }

    I2C_Event = 0U;
    I2Cdrv->MasterReceive(ADDRESS_MEMORIA, datos, longitudLectura, false);
    while ((I2C_Event & ARM_I2C_EVENT_TRANSFER_DONE) == 0U);

    if ((I2C_Event & ARM_I2C_EVENT_TRANSFER_INCOMPLETE) != 0U)
		{
			printf("[I2C] Receive fallido\n");
			return -1;
    }

    return 0;
}

//Funicón que busca la última posición utilizada:
//----------Si reiniciamos buscamos donde nos quedamos escribiendo---------//
static void buscarUltimaPosicionEEPROM(void) 
{
	uint8_t direccion[2];
	RegistroBinario reg;

	for (uint16_t i = 0; i < NUM_REGISTROS; i++)
	{
			uint16_t addr = i * sizeof(RegistroBinario);
			direccion[0] = (addr >> 8) & 0xFF;
			direccion[1] = addr & 0xFF;

			if (lecturaMemoria(direccion, (uint8_t *)&reg, sizeof(RegistroBinario)) != 0) 
			{
				break;
			}

			//Si la fecha está vacía o es inválida que no hay registro
			if ( reg.luz == 0.0f) 
			{
				indiceCircular = i;
				return;
			}
	}

	//Si todos están válidos, volvemos al principio
	indiceCircular = 0;
}

//Función de verificación de la memoria:
void verificarUltimoRegistroEEPROM(uint16_t start_address, uint16_t length) {
    uint8_t direccion[2];
    direccion[0] = (start_address >> 8) & 0xFF;
    direccion[1] = start_address & 0xFF;

    uint8_t buffer_leido[128] = {0};

    printf("[EEPROM] Verificando dirección: 0x%04X (%u bytes)\n", start_address, length);

    osDelay(10);  // <-- AÑADE ESTA LÍNEA

    if (lecturaMemoria(direccion, buffer_leido, length) != 0)
		{
			printf("[EEPROM] ? Error al leer verificación EEPROM.\n");
			return;
    }

    printf("[EEPROM] Contenido bruto leído:\n");
    for (int i = 0; i < length; i++) 
		{
       printf("%02X ", buffer_leido[i]);
    }
    printf("\n");
}

//Función que realiza el borrado de la memoria:
void borrarEEPROM(void) {
    uint8_t buffer[EEPROM_PAGE_SIZE + 2];
    memset(&buffer[2], 0xFF, EEPROM_PAGE_SIZE);  // Rellenar con 0xFF

    for (uint16_t addr = 0; addr < NUM_REGISTROS * sizeof(RegistroBinario); addr += EEPROM_PAGE_SIZE) 
		{
        buffer[0] = (addr >> 8) & 0xFF;  // MSB
        buffer[1] = addr & 0xFF;         // LSB

        if (escrituraMemoria(buffer, EEPROM_PAGE_SIZE + 2) != 0) 
				{
            printf("[EEPROM] Error al borrar en dirección 0x%04X\n", addr);
        } else 
				{
            printf("[EEPROM] Borrado correcto en dirección 0x%04X\n", addr);
        }
        osDelay(10);  // Retardo de escritura
    }

    indiceCircular = 0;  // Reiniciar índice tras borrado
}
