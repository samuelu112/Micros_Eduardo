/*
 * Postlaboratorio_5.c
 * Created: 20/04/2025 20:04:11
 * Author : samur
 */

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "PWM/PWM.h"     // Servo en PD6 (Timer0)
#include "PWM2/PWM2.h"   // Servo en PD3 (Timer2)
#include "PWM3/PWM3.h"   // LED en PB1 (Timer1 manual PWM)

// Variables globales para ADC
uint8_t adc0_value, adc1_value, adc2_value;
uint8_t current_channel = 0;

void initADC(void);

int main(void) {
	cli();

	// Servos:
	initPWMFastA(non_invert, 1024);   // Timer0 ? PD6 (~16 ms)
	initPWMFastB(non_invert, 1024);   // Timer2 ? PD3 (~16 ms)

	// LED PWM manual: top=255, prescaler=64 ? f?976 Hz, resolución=8 bits
	initPWM3(255, (1 << CS11) | (1 << CS10));

	// ADC con interrupciones (PC0, PC1, PC2)
	initADC();

	sei();

	while (1) {
		// Mapear para servos (ticks 9–37)
		uint8_t t0 = 9  + ((uint16_t)adc0_value * 28) / 255;
		uint8_t t1 = 9  + ((uint16_t)adc1_value * 28) / 255;
		updateDutyCycle(t0);
		updateDutyCycle2(t1);

		// LED: duty = adc2 directo (0–255)
		setPWMDuty3(adc2_value);

		_delay_ms(10);
	}
}

void initADC(void) {
	ADMUX  = (1 << REFS0) | (1 << ADLAR) | (current_channel & 0x07); // Canal inicial ADC0
	ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADATE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	ADCSRB = 0; // Free running mode
	DIDR0  = (1 << ADC0D) | (1 << ADC1D) | (1 << ADC2D);
	ADCSRA |= (1 << ADSC); // Iniciar primera conversión
}

ISR(ADC_vect) {
	switch(current_channel) {
		case 0:
		adc0_value = ADCH; // Guardar ADC0
		current_channel = 1;
		break;
		case 1:
		adc1_value = ADCH; // Guardar ADC1
		current_channel = 2;
		break;
		case 2:
		adc2_value = ADCH; // Guardar ADC2
		current_channel = 0;
		break;
	}
	ADMUX = (1 << REFS0) | (1 << ADLAR) | (current_channel & 0x07); // Cambiar canal
	ADCSRA |= (1 << ADSC); // Iniciar nueva conversión
}
