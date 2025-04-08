/*UNIVERSIDAD DEL VALLE DE GUATEMALA
*IE2023: Programación de microcontroladores
*Autor : Eduardo Samuel Urbina Pérez
*Proyecto : Postlaboratorio 4
*Hardware : ATmega 328

 * Created: 6/04/2025 18:02:55
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
volatile uint8_t show_mode = 0;
volatile uint8_t adc_value = 0;

void initADC() {
	ADMUX = (1 << REFS0) | (1 << ADLAR) | (1 << MUX1);
	ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADATE) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	DIDR0 = (1 << ADC2D);
}

ISR(ADC_vect) {
	adc_value = ADCH;
	display_digit[0] = seg7_table[(adc_value >> 4) & 0x0F];
	display_digit[1] = seg7_table[adc_value & 0x0F];
}

ISR(TIMER2_OVF_vect) {
	TCNT2 = 100; // Frecuencia de actualización
	
	// Apagar todos los elementos antes de cambiar
	PORTB &= ~0x0F;
	PORTD = 0x00;
	
	if (show_mode == 0) {
		// Mostrar displays 7 segmentos
		if (current_display == 0) {
			PORTD = display_digit[0];
			PORTB |= (1 << PB0);
			} else {
			PORTD = display_digit[1];
			PORTB |= (1 << PB1);
		}
		current_display = !current_display;
		} else {
		// Mostrar contador en LEDs
		PORTD = contador;
		PORTB |= (1 << PB2); // Activar transistor de LEDs
	}
	
	// Control del LED de comparación
	if (adc_value > contador) PORTB |= (1 << PB3);
	else PORTB &= ~(1 << PB3);
	
	show_mode = !show_mode; // Alternar
}

ISR(PCINT1_vect) {
	botones = PINC & 0x03;
	TCNT0 = 131;
	TCCR0B = (1 << CS02) | (1 << CS00);
	PCICR &= ~(1 << PCIE1);
}

ISR(TIMER0_OVF_vect) {
	TCCR0B = 0;
	uint8_t estadoa = PINC & 0x03;
	
	if (estadoa == botones) {
		if (!(estadoa & (1 << PC0))) contador++;
		if (!(estadoa & (1 << PC1))) contador--;
	}
	PCICR |= (1 << PCIE1);
}

int main(void) {
	DDRD = 0xFF;
	DDRB |= 0x0F;
	DDRC &= ~0x03;
	PORTC |= 0x03;

	// Configuración Timer2
	TCCR2A = 0;
	TCCR2B = (1 << CS22); // Prescaler 64
	TIMSK2 = (1 << TOIE2);
	TCNT2 = 100; // Overflow

	PCICR |= (1 << PCIE1);
	PCMSK1 |= (1 << PC0) | (1 << PC1);
	TIMSK0 |= (1 << TOIE0);
	
	initADC();
	sei();

	while (1) {
		}
}

