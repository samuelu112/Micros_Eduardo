/*
 * Prelab_4.c
 *
 * Created: 2/04/2025 08:40:55
 * Author : Eduardo Samuel Urbina Perez
 Prelaboratorio 4
 */ 
#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t seg7_table[] = {
	0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
	0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71
};

volatile uint8_t display_digit[2] = {0};
volatile uint8_t contador = 0;
volatile uint8_t botones;
volatile uint8_t current_display = 0;

void initADC() {
	ADMUX = (1 << REFS0) | (1 << ADLAR) | (1 << MUX1);  // ADC2 (PC2), 8-bit
	ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADATE) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	DIDR0 = (1 << ADC2D);
}

ISR(ADC_vect) {
	//uint8_t adc_value = ADCH;  // Leer valor ADC de 8 bits
	display_digit[0] = seg7_table[(ADCH >> 4) & 0x0F];  // Nibble alto
	display_digit[1] = seg7_table[ADCH & 0x0F];         // Nibble bajo
}

ISR(TIMER2_OVF_vect) {
	TCNT2 = 131;  // Recarga para 1ms @16MHz con prescaler 64
	
	PORTB &= ~0x03;  // Apagar ambos displays
	
	if(current_display == 0) {
		PORTD = display_digit[0];  // Mostrar dígito alto
		PORTB |= (1 << PB0);
		current_display = 1;
		} else {
		PORTD = display_digit[1];  // Mostrar dígito bajo
		PORTB |= (1 << PB1);
		current_display = 0;
	}
}

ISR(PCINT1_vect) {
	botones = PINC & ((1 << PC0) | (1 << PC1));
	TCNT0 = 138;
	TCCR0B = (1 << CS02) | (1 << CS00);
	PCICR &= ~(1 << PCIE1);
}

ISR(TIMER0_OVF_vect) {
	TCCR0B = 0;
	uint8_t estadoa = PINC & ((1 << PC0) | (1 << PC1));
	
	if (estadoa == botones) {
		if ((estadoa & (1 << PC0))) contador++;
		if ((estadoa & (1 << PC1))) contador--;
		}
	PCICR |= (1 << PCIE1);
}

int main(void) {
	DDRD = 0xFF;    // Salida para segmentos
	DDRB |= 0x03;   // PB0 y PB1 como salidas
	DDRC &= ~0x03;  // PC0 y PC1 como entradas
	PORTC |= 0x03;  // Pull-ups en PC0 y PC1

	// Configurar Timer2 para multiplexado
	TCCR2A = 0;
	TCCR2B = (1 << CS22);  // Prescaler 64
	TIMSK2 = (1 << TOIE2);
	TCNT2 = 131;

	// Configurar interrupciones
	PCICR |= (1 << PCIE1);
	PCMSK1 |= (1 << PC0) | (1 << PC1);
	TIMSK0 |= (1 << TOIE0);
	
	initADC();
	sei();

	while (1) { }
}

