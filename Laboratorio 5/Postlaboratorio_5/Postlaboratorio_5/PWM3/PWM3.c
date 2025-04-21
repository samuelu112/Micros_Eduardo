/*
 * PWM3.c
 *
 * Created: 20/04/2025 20:11:59
 *  Author: samur
 */ 
#include "PWM3.h"

uint8_t pwm3_counter = 0;
uint8_t pwm3_disparo = 0;

// Inicialización de Timer1 en modo Normal con overflow y compare-A interrupciones
void initPWM3(uint16_t top, uint16_t prescaler)
{
	
	// Pin PB1 como salida
	DDRB |= (1 << DDB1);

	// Modo Normal: WGM13:0 = 0
	TCCR1A = 0;
	TCCR1B = 0;
	
	// Habilitar interrupciones: overflow y compare match A
	TIMSK1 = (1 << TOIE1) | (1 << OCIE1A);
	
	OCR1A = top;
	// Selección de prescaler en TCCR1B
	switch (prescaler) {
		case 1:    TCCR1B |= (1 << CS10);                            break;
		case 8:    TCCR1B |= (1 << CS11);                            break;
		case 64:   TCCR1B |= (1 << CS11) | (1 << CS10);              break;
		case 256:  TCCR1B |= (1 << CS12);                            break;
		case 1024: TCCR1B |= (1 << CS12) | (1 << CS10);              break;
		default:   TCCR1B |= (1 << CS12) | (1 << CS10);              break;
	}
}

// Cambia el valor de OCR1A para ajustar el duty cycle
void setPWMDuty3(uint16_t duty)
{
	if (duty > OCR1A) duty = OCR1A;
	OCR1B = duty;
	//pwm3_disparo = duty;
	//OCR1A = duty;
}

// ISR: al desbordarse Timer1 (TCNT1=0) ? LED ON
ISR(TIMER1_OVF_vect)
{
	//pwm3_counter ++;
	//if(pwm3_counter == 0){
	PORTB |= (1 << PB1);
	//}
}

// ISR: al alcanzar OCR1A ? LED OFF
ISR(TIMER1_COMPA_vect)
{
	if(pwm3_counter >= pwm3_disparo){
		PORTB &= ~(1 << PB1);
	}
}
