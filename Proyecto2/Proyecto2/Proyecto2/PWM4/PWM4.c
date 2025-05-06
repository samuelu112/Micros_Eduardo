/*
 * PWM4.c
 *
 * Created: 30/04/2025 10:11:07
 *  Author: samur
 */ 

#include "PWM4.h"

void initPWMFastA_T1(uint8_t inv, uint16_t presc) {
	// 1) Pin PB1 (OC1A) como salida
	DDRB |= (1 << DDB1);

	// 2) *NO* limpiamos TCCR1A/B aquí, ya lo hizo PWM3.

	// 3) Aseguramos el bit WGM11 para Fast PWM (modo 14)
	TCCR1A |= (1 << WGM11);

	// 4) Configurar salida no invertida o invertida en OC1A
	if (inv == invert) {
		TCCR1A |= (1 << COM1A1) | (1 << COM1A0);
		} else {
		TCCR1A |= (1 << COM1A1);
		TCCR1A &= ~(1 << COM1A0);
	}

	// 5) Prescaler (ejemplo presc=8 para 50Hz)
	if (presc == 1)    TCCR1B |= (1 << CS10);
	else if (presc == 8)  TCCR1B |= (1 << CS11);
	else if (presc == 64) TCCR1B |= (1 << CS11) | (1 << CS10);
	else if (presc == 256)TCCR1B |= (1 << CS12);
	else if (presc == 1024)TCCR1B |= (1 << CS12) | (1 << CS10);
	else /*default*/      TCCR1B |= (1 << CS11);

	// 6) TOP (ICR1) ya configurado por PWM3
}

void updateDutyCycle4(uint16_t ticks) {
	OCR1A = ticks;
}
