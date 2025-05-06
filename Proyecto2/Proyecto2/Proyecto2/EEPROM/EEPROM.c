/*
 * EEPROM.c
 *
 * Created: 5/05/2025 17:19:12
 *  Author: samur
 */ 
#include "EEPROM.h"

// Direcci�n base para almacenar las posiciones
#define EEPROM_BASE_ADDR 0x00

// Funci�n para leer un byte de la EEPROM
uint8_t EEPROM_Read(uint16_t address) {
	while(EECR & (1 << EEPE)); // Esperar a que termine una escritura previa
	EEAR = address;            // Establecer direcci�n
	EECR |= (1 << EERE);       // Iniciar lectura
	return EEDR;               // Devolver dato le�do
}

// Funci�n para escribir un byte en la EEPROM
void EEPROM_Write(uint16_t address, uint8_t data) {
	while(EECR & (1 << EEPE)); // Esperar escritura previa
	
	// Configurar direcci�n y dato
	EEAR = address;
	EEDR = data;
	
	// Habilitar escritura (EEMPE se desactiva autom�ticamente despu�s de 4 ciclos)
	EECR |= (1 << EEMPE);      // Habilitar maestro de escritura
	EECR |= (1 << EEPE);       // Iniciar escritura
	
	// Modo at�mico (3.4ms por defecto)
	EECR &= ~((1 << EEPM1) | (1 << EEPM0)); // EEPM1=0, EEPM0=0
}

// Inicializar EEPROM con ceros si est� vac�a
void EEPROM_Init() {
	if(EEPROM_Read(EEPROM_BASE_ADDR) == 0xFF) {
		ServoPosition init_pos = {0};
		for(uint8_t i = 0; i < MAX_POSITIONS; i++) {
			EEPROM_SavePosition(i, &init_pos);
		}
	}
}

// Guardar estructura en EEPROM (4 bytes por servo * 4 servos = 16 bytes por posici�n)
void EEPROM_SavePosition(uint8_t pos_index, ServoPosition* pos) {
	uint16_t addr = EEPROM_BASE_ADDR + (pos_index * sizeof(ServoPosition));
	uint8_t* data_ptr = (uint8_t*)pos;
	
	for(uint8_t i = 0; i < sizeof(ServoPosition); i++) {
		EEPROM_Write(addr + i, data_ptr[i]); // Escribe byte por byte
	}
}

// Cargar estructura desde EEPROM
void EEPROM_LoadPosition(uint8_t pos_index, ServoPosition* pos) {
	uint16_t addr = EEPROM_BASE_ADDR + (pos_index * sizeof(ServoPosition));
	uint8_t* data_ptr = (uint8_t*)pos;
	
	for(uint8_t i = 0; i < sizeof(ServoPosition); i++) {
		data_ptr[i] = EEPROM_Read(addr + i); // Lee byte por byte
	}
}