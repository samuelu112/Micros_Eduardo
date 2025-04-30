/*
 * PWM2.c
 *
 * Created: 30/04/2025 10:10:01
 *  Author: samur
 */ 
#include "PWM2.h"

void initPWMFastB(uint8_t invertido2, uint16_t prescaler2) {
	// Configurar PD3 (OC2B) como salida.
	DDRD |= (1 << DDD3);
	
	// Limpiar registros de Timer2
	TCCR2A = 0;
	TCCR2B = 0;
	
	// Configurar modo Fast PWM para Timer2:
	// Se activa el modo Fast PWM con TOP
	TCCR2A |= (1 << WGM21) | (1 << WGM20);
	
	// Configurar la salida en el canal OC2B (PD3)
	if (invertido2 == invert) {
		TCCR2A |= (1 << COM2B1) | (1 << COM2B0);
		} else {
		TCCR2A |= (1 << COM2B1);
		TCCR2A &= ~(1 << COM2B0);
	}
	
	// Limpiar bits de prescaler
	TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20));
	// Configuración adecuada según el prescaler deseado:
	switch(prescaler2) {
		case 1: TCCR2B |= (1 << CS20);        // 001 = clock/1
		break;
		case 8: TCCR2B |= (1 << CS21);        // 010 = clock/8
		break;
		case 32: TCCR2B |= (1 << CS21) | (1 << CS20); // 011 = clock/32
		break;
		case 64: TCCR2B |= (1 << CS22);        // 100 = clock/64
		break;
		case 128: TCCR2B |= (1 << CS22) | (1 << CS20); // 101 = clock/128
		break;
		case 256: TCCR2B |= (1 << CS22) | (1 << CS21); // 110 = clock/256
		break;
		case 1024: TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); // 111 = clock/1024
		break;
		default: TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); // Predeterminado: 1024
		break;
	}
}

void updateDutyCycle2(uint8_t ticks2) {
	// Actualiza el OCR2B, OC2B = PD3.
	OCR2B = ticks2;
}