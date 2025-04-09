/*
 * Prelaboratorio 5.c
 *
 * Created: 9/04/2025 08:12:46
 * Author : samur
 */ 
/*
 * Prelaboratorio 5.c
 */
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "PWM/PWM.h"

void initADC(void);
uint8_t readADC(void);

int main(void) {
	cli();
	initPWMFastA(non_invert, 1024); // Prescaler 1024 para período de ~16ms
	initADC();
	sei();

	while (1) {
		uint8_t adc_value = readADC();
		// Mapeo de 0-255 a 9-37 ticks (600-2400µs)
		uint8_t pulse_ticks = 9 + ((uint16_t)adc_value * 28) / 255;
		updateDutyCycle(pulse_ticks);
		_delay_ms(20);
	}
}

void initADC(void) {
	ADMUX = (1 << REFS0) | (1 << ADLAR); // Canal ADC0 (PC0), alineación izquierda
	ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prescaler 128
	DIDR0 = (1 << ADC0D); // Deshabilitar entrada digital en PC0
}

uint8_t readADC(void) {
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADCH; // Valor de 8 bits (0-255)
}