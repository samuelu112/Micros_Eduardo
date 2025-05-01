/*
 * Prelab_4.c
 *
 * Created: 2/04/2025 08:40:55
 * Author : Eduardo Samuel Urbina Perez
 Prelaboratorio 4
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t contador = 0;
volatile uint8_t botones;

int main(void) {
	// Configurar PORTD como salida
	DDRD = 0xFF;
	PORTD = 0x00;

	// Configurar PC0 y PC1 como entradas con pull-up
	DDRC &= ~((1 << PC0) | (1 << PC1));
	PORTC |= (1 << PC0) | (1 << PC1);

	// Habilitar interrupciones por cambio en PC0 y PC1
	PCICR |= (1 << PCIE1);
	PCMSK1 |= (1 << PC0) | (1 << PC1);

	// Configurar Timer0
	TIMSK0 |= (1 << TOIE0);  // Habilitar interrupción por overflow
	TCCR0B = 0;              // Detener timer inicialmente

	// Habilitar interrupciones globales
	sei();

	while (1) { }
}

ISR(PCINT1_vect) {
	// Leer estado actual de los botones
	botones = PINC & ((1 << PC0) | (1 << PC1));
	
	// Configurar Timer0 para 15ms
	TCNT0 = 138;             // Valor inicial para 15ms con prescaler 1024
	TCCR0B = (1 << CS02)|(1 << CS00); // Prescaler 1024 e iniciar timer
	
	// Deshabilitar interrupciones durante el antirebote
	PCICR &= ~(1 << PCIE1);
}

ISR(TIMER0_OVF_vect) {
	// Detener el timer
	TCCR0B = 0;
	
	// Leer estado actual de los botones
	uint8_t estadoa = PINC & ((1 << PC0) | (1 << PC1));
	
	// Verificar si el estado
	if (estadoa == botones) {
		// Incremento
		if ((estadoa & (1 << PC0)) == 0) {
			contador++;
		}
		
		// Decremento
		if ((estadoa & (1 << PC1)) == 0) {
			contador--;
		}
		
		// Actualizar salida
		PORTD = contador;
	}
	
	// Reactivar interrupciones de botones
	PCICR |= (1 << PCIE1);
}

