#include "PWM3.h"

// Contador
static volatile uint8_t pwm3_counter = 0;
// Señal
static volatile uint8_t pwm3_duty    = 0;

void initPWM3_manual(uint16_t top, uint8_t prescalerBits)
{
	// Configura PB1 como salida del LED
	DDRB |= (1 << DDB1);  

	// Modo CTC: WGM13:0 = 0100, WGM12 = 1
	TCCR1A = 0;
	TCCR1B = (1 << WGM12) | prescalerBits;  

	// Fija TOP en OCR1A = periodo de interrupción
	OCR1A = top;  

	// Habilita interrupción Compare Match A
	TIMSK1 = (1 << OCIE1A);  

	// Inicializa el contador de software
	pwm3_counter = 0;
	pwm3_duty    = 0;
}

void setPWM3_manual(uint8_t dutyValor)
{
	// duty a 0–255
	pwm3_duty = dutyValor;
}

//se llama cada vez que TCNT1 == OCR1A
ISR(TIMER1_COMPA_vect)
{
	// Incrementa ciclo de software (0–255)
	pwm3_counter++;
	// Comienza en 0 y cada vez que es 0 se enciende
	if (pwm3_counter == 0) {
		PORTB |= (1 << PB1);
	}
	// Si alcanza el valor de la señal, apaga el LED
	else if (pwm3_counter == pwm3_duty) {
		PORTB &= ~(1 << PB1);
	}
}
