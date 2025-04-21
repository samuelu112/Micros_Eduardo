/*
 * Postlaboratorio_5.c
 *
 * Created: 20/04/2025 20:04:11
 * Author : samur
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "PWM/PWM.h"
#include "PWM2/PWM2.h"
#include "PWM3/PWM3.h"

// Prototipos ADC separados para cada canal:
void initADC(void);
uint8_t readADC_PC0(void);
uint8_t readADC_PC1(void);
uint8_t readADC_PC2(void);
int main(void) {
	cli();
	// Inicializamos PWM para servo 1 en PD6 (Timer0) y servo 2 en PD3 (Timer2)
	initPWMFastA(non_invert, 1024); // Para servo en PD6 (Timer0) ~16ms de período
	initPWMFastB(non_invert, 1024); // Para servo en PD3 (Timer2) ~16ms de período
	initPWM3(1999, 8);
	initADC();
	sei();

	while (1) {
		uint8_t adc_value0 = readADC_PC0();  // Lectura en PC0 para servo en PD6
		uint8_t adc_value1 = readADC_PC1();  // Lectura en PC1 para servo en PD3
		uint8_t adc3 = readADC_PC2();	// Lectura en PC2 para led
		// Mapeo de 0-255
		uint8_t pulse_ticks0 = 9 + ((uint16_t)adc_value0 * 28) / 255;
		uint8_t pulse_ticks1 = 9 + ((uint16_t)adc_value1 * 28) / 255;
		// Mapea 0–255 a 0–65535:
		//uint16_t duty = ((uint32_t)adc3 * 65535) / 255;
		updateDutyCycle(pulse_ticks0);   // Actualiza servo en PD6
		updateDutyCycle2(pulse_ticks1);  // Actualiza servo en PD3
		//setPWMDuty3(adc3);//duty);
		uint16_t duty = ((uint32_t)adc3 * 1999) / 255;
		setPWMDuty3(duty);
		_delay_ms(10);
	}
}

void initADC(void) {
	ADMUX  = (1 << REFS0) | (1 << ADLAR);
	// Configuración común del ADC se habilita y se deshabilitan entradas digitales en PC0 y PC1.
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prescaler 128
	DIDR0  = (1 << ADC0D) | (1 << ADC1D) | (1 << ADC2D); // Deshabilitar entrada digital en PC0, pc1 y pc2
}

uint8_t readADC_PC0(void) {
	ADMUX = (1 << REFS0) | (1 << ADLAR); // Selecciona canal ADC0 (PC0)
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADCH;
}

uint8_t readADC_PC1(void) {
	ADMUX = (1 << REFS0) | (1 << ADLAR) | (1 << MUX0); // Selecciona canal ADC1 (PC1)
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	//conversión
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADCH;
}

uint8_t readADC_PC2(void) {
	//ADMUX = 0;
	ADMUX = (1 << REFS0) | (1 << ADLAR) | (1 << MUX1); // Selecciona canal ADC1 (PC1)
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	//conversión
	//ADCSRA |= (1 << ADSC);
	//while (ADCSRA & (1 << ADSC));
	return ADCH;
}
