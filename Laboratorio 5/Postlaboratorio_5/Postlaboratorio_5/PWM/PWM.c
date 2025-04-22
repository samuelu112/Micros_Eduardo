/*
 * PWM.c
 *
 * Created: 20/04/2025 20:10:45
 *  Author: samur
 */ 
#include "PWM.h"

void initPWMFastA(uint8_t invertido, uint16_t prescaler) {
	DDRD |= (1 << PORTD6); // PD6 como salida PWM
	
	// Modo Fast PWM
	TCCR0A |= (1 << WGM01) | (1 << WGM00);
	
	// Salida no invertida
	if (invertido == invert) {
		TCCR0A |= (1 << COM0A1) | (1 << COM0A0);
		} else {
		TCCR0A |= (1 << COM0A1);
	}
	
	// Configurar prescaler
	TCCR0B &= ~((1 << CS02) | (1 << CS01) | (1 << CS00));
	switch(prescaler) {
		case 1:    TCCR0B |= (1 << CS00); break;
		case 8:    TCCR0B |= (1 << CS01); break;
		case 64:   TCCR0B |= (1 << CS01) | (1 << CS00); break;
		case 256:  TCCR0B |= (1 << CS02); break;
		case 1024: TCCR0B |= (1 << CS02) | (1 << CS00); break;
		default:   TCCR0B |= (1 << CS02) | (1 << CS00); break;
	}
}

void updateDutyCycle(uint8_t ticks) {
	OCR0A = ticks; // Ciclo de trabajo
}