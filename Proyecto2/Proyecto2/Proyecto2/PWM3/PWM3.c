/*
 * PWM3.c
 *
 * Created: 30/04/2025 10:10:17
 *  Author: samur
 */ 
#include "PWM3.h"

void initPWMFastB_T1(uint8_t inv, uint16_t presc) {
	// 1) Pin PB2 (OC1B) como salida
	DDRB |= (1 << DDB2);

	// 2) Limpiar registros antes de configurar todo
	TCCR1A = 0;
	TCCR1B = 0;

	// 3) Modo Fast PWM con ICR1 como TOP (modo 14)
	TCCR1A |= (1 << WGM11);
	TCCR1B |= (1 << WGM13) | (1 << WGM12);

	// 4) Configurar salida no invertida o invertida en OC1B
	if (inv == invert) {
		TCCR1A |= (1 << COM1B1) | (1 << COM1B0);
		} else {
		TCCR1A |= (1 << COM1B1);
		TCCR1A &= ~(1 << COM1B0);
	}

	// 5) Prescaler (ejemplo presc=8 para 50Hz)
	switch (presc) {
		case 1:    TCCR1B |= (1 << CS10); break;
		case 8:    TCCR1B |= (1 << CS11); break;
		case 64:   TCCR1B |= (1 << CS11) | (1 << CS10); break;
		case 256:  TCCR1B |= (1 << CS12); break;
		case 1024: TCCR1B |= (1 << CS12) | (1 << CS10); break;
		default:   TCCR1B |= (1 << CS11); break; // default 8
	}

	// 6) TOP para 20ms (50Hz): ICR1 = F_CPU/prescaler/50Hz - 1 = 16MHz/8/50 -1 = 39999
	ICR1 = 39999;
}

void updateDutyCycle3(uint16_t ticks) {
	OCR1B = ticks;
}

