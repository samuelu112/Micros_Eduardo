#include "PWM3.h"

void initPWM3(uint16_t top, uint8_t prescalerBits) {
	DDRB |= (1 << DDB1); // PB1 como salida
	TCCR1A = 0;
	TCCR1B = (1 << WGM12); // Modo CTC
	OCR1A = top;
	TIMSK1 = (1 << OCIE1A) | (1 << OCIE1B);
	TCCR1B |= prescalerBits;
}

void setPWMDuty3(uint16_t dutyValor) {
	OCR1B = (dutyValor > OCR1A) ? OCR1A : dutyValor;
}

ISR(TIMER1_COMPA_vect) { PORTB |= (1 << PB1); } // Encender LED
ISR(TIMER1_COMPB_vect) { PORTB &= ~(1 << PB1); } // Apagar LED
