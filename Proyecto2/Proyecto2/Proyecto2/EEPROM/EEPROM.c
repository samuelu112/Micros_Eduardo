/*
 * EEPROM.c
 *
 * Created: 5/05/2025 17:19:12
 *  Author: samur
 */ 

#include "EEPROM.h"

uint8_t EEPROM_Read(uint16_t address) {
	// Esperar escritura previa
	while (EECR & (1 << EEPE));
	// Ajustar dirección
	EEARL = (uint8_t)address;
	EEARH = (uint8_t)(address >> 8);
	// Iniciar lectura
	EECR |= (1 << EERE);
	return EEDR;
}

void EEPROM_Write(uint16_t address, uint8_t data) {
	// Esperar escritura previa
	while (EECR & (1 << EEPE));
	// Ajustar dirección y dato
	EEARL = (uint8_t)address;
	EEARH = (uint8_t)(address >> 8);
	EEDR  = data;
	// Maestro de escritura
	EECR |= (1 << EEMPE);
	// Iniciar escritura
	EECR |= (1 << EEPE);
}

void EEPROM_SavePosition(uint8_t pos_index, const ServoPosition* pos) {
	uint16_t base = pos_index * sizeof(ServoPosition);
	const uint8_t* p = (const uint8_t*)pos;
	for (uint8_t i = 0; i < sizeof(ServoPosition); i++) {
		EEPROM_Write(base + i, p[i]);
	}
}

void EEPROM_LoadPosition(uint8_t pos_index, ServoPosition* pos) {
	uint16_t base = pos_index * sizeof(ServoPosition);
	uint8_t* p = (uint8_t*)pos;
	for (uint8_t i = 0; i < sizeof(ServoPosition); i++) {
		p[i] = EEPROM_Read(base + i);
	}
}