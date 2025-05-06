/*
 * EEPROM.h
 *
 * Created: 5/05/2025 17:19:31
 *  Author: samur
 */ 


#ifndef EEPROM_H
#define EEPROM_H

#include <avr/io.h>
#include <stdint.h>

#define EEPROM_SIZE 1024
#define MAX_POSITIONS 4

typedef struct {
	uint16_t servo1;
	uint16_t servo2;
	uint16_t servo3;
	uint16_t servo4;
} ServoPosition;

void EEPROM_Init();
void EEPROM_Write(uint16_t address, uint8_t data);
uint8_t EEPROM_Read(uint16_t address);
void EEPROM_SavePosition(uint8_t pos_index, ServoPosition* pos);
void EEPROM_LoadPosition(uint8_t pos_index, ServoPosition* pos);

#endif