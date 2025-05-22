/*
 * Proyecto2.c
 *
 * Created: 30/04/2025 09:47:02
<<<<<<< Updated upstream
 * Author : samur
=======
 * Author : Eduardo Urbina
>>>>>>> Stashed changes
 */
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include "PWM1/PWM1.h"
#include "PWM2/PWM2.h"
#include "PWM3/PWM3.h"
#include "PWM4/PWM4.h"
#include "EEPROM/EEPROM.h"

#define MODE_PIN   PD2
#define PC_SAVE    PC4
#define PC_RECALL  PC5
#define LED_PIN    PB0

#define BAUD     115200
#define UBRRVAL  ((F_CPU/16/BAUD)-1)

volatile uint8_t state = 0, flag_mode = 0, flag_save = 0, flag_recall = 0;
volatile uint8_t adc0, adc1;
volatile uint16_t adc2, adc3;
volatile uint8_t chan;
uint8_t save_idx = 0, recall_idx = 0;
char rxbuf[32];
uint8_t rxpos = 0;
volatile uint8_t line_ready = 0;

static uint8_t  last_ang1 = 90, last_ang2 = 90;
static uint16_t last_ang3 = 90, last_ang4 = 90;

void initADC(void);
void uart_init(void);
void uart_sendstr(const char*);

// --- ISRs ---
ISR(INT0_vect) {
	_delay_ms(10);
	if (!(PIND & (1 << MODE_PIN))) flag_mode = 1;
}
ISR(PCINT1_vect) {
	uint8_t p = PINC;
	if (!(p & (1 << PC_SAVE)))   flag_save = 1;
	if (!(p & (1 << PC_RECALL))) flag_recall = 1;
}
ISR(USART_RX_vect) {
	char c = UDR0;
	if (c == '\r' || c == '\n') {
		if (rxpos) {
			rxbuf[rxpos] = 0;
			line_ready = 1;
		}
		rxpos = 0;
		} else if (rxpos < sizeof(rxbuf) - 1) {
		rxbuf[rxpos++] = c;
	}
}
ISR(ADC_vect) {
	switch (chan) {
		case 0: adc0 = ADCH; chan = 1; break;
		case 1: adc1 = ADCH; chan = 2; break;
		case 2: adc2 = ADC;  chan = 3; break;
		default:adc3 = ADC;  chan = 0; break;
	}
	ADMUX = (1 << REFS0) | ((chan < 2) << ADLAR) | (chan & 7);
	ADCSRA |= (1 << ADSC);
}

int main(void) {
	cli();

	initPWMFastA(non_invert, 1024);
	initPWMFastB(non_invert, 1024);
	initPWMFastB_T1(non_invert, 8);
	initPWMFastA_T1(non_invert, 8);

	DDRB |= (1 << LED_PIN);
	PORTB &= ~(1 << LED_PIN);

	DDRD &= ~(1 << MODE_PIN);
	PORTD |= (1 << MODE_PIN);
	EICRA = (1 << ISC01);
	EIMSK = (1 << INT0);

	DDRC &= ~((1 << PC_SAVE) | (1 << PC_RECALL));
	PORTC |= (1 << PC_SAVE) | (1 << PC_RECALL);
	PCICR = (1 << PCIE1);
	PCMSK1 = (1 << PCINT12) | (1 << PCINT13);
	PCIFR = (1 << PCIF1);

	initADC();
	uart_init();
	sei();

	uart_sendstr("90,90,90,90\n");

	while (1) {
		if (flag_mode) {
			_delay_ms(50);
			state = (state + 1) % 4;
			flag_mode = 0;
			if (state == 2) PORTB |= (1 << LED_PIN);
			else            PORTB &= ~(1 << LED_PIN);
		}

		if (state == 1) {
			PORTB ^= (1 << LED_PIN);
			_delay_ms(200);
		}

		if (state == 0) {
			uint16_t t0 = 9   + (adc0 * 28UL) / 255;
			uint16_t t1 = 9   + (adc1 * 28UL) / 255;
			uint16_t t2 = 1200 + (adc2 * 3600UL) / 1023;
			uint16_t t3 = 1200 + (adc3 * 3600UL) / 1023;
			updateDutyCycle(t0);
			updateDutyCycle2(t1);
			updateDutyCycle3(t2);
			updateDutyCycle4(t3);
		}
		else if (state == 1) {
			if (flag_save) {
				ServoPosition sp = {
					.servo1 = 9 + (adc0 * 28UL) / 255,
					.servo2 = 9 + (adc1 * 28UL) / 255,
					.servo3 = 1200 + (adc2 * 3600UL) / 1023,
					.servo4 = 1200 + (adc3 * 3600UL) / 1023
				};
				EEPROM_SavePosition(save_idx, &sp);
				save_idx = (save_idx + 1) % MAX_POSITIONS;
				flag_save = 0;
				_delay_ms(200);
			}
			if (flag_recall) {
				ServoPosition sp;
				EEPROM_LoadPosition(recall_idx, &sp);
				recall_idx = (recall_idx + 1) % MAX_POSITIONS;
				updateDutyCycle(sp.servo1);
				updateDutyCycle2(sp.servo2);
				updateDutyCycle3(sp.servo3);
				updateDutyCycle4(sp.servo4);
				flag_recall = 0;
				_delay_ms(200);
			}
		}
		else {
			if (line_ready) {
				char s = rxbuf[0];
				uint16_t angle = atoi(&rxbuf[1]); 
				if (angle <= 180) {
					uint16_t pulse;

					switch (s) {
						case 'A':
						last_ang1 = angle;
						pulse = 9 + (angle * 28) / 255;
						updateDutyCycle(pulse);
						break;
						case 'B':
						last_ang2 = angle;
						pulse = 9 + (angle * 28) / 255;
						updateDutyCycle2(pulse);
						break;
						case 'C':
						last_ang3 = angle;
						pulse = 1200 + (angle * 3600) / 255;
						updateDutyCycle3(pulse);
						break;
						case 'D':
						last_ang4 = angle;
						pulse = 1200 + (angle * 3600) / 255;
						updateDutyCycle4(pulse);
						break;
					}
				}
				line_ready = 0;
			}

			// Enviar estado actual por UART
			char buf[32];
			sprintf(buf, "A%u,B%u,C%u,D%u\n",
			(unsigned)last_ang1,
			(unsigned)last_ang2,
			(unsigned)last_ang3,
			(unsigned)last_ang4);
			uart_sendstr(buf);
		}
	}
}

void initADC(void) {
	chan = 0;
	ADMUX = (1 << REFS0) | (1 << ADLAR) | (chan & 7);
	ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADATE)
	| (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	ADCSRB = 0;
	DIDR0 = (1 << ADC0D) | (1 << ADC1D) | (1 << ADC2D) | (1 << ADC3D);
	ADCSRA |= (1 << ADSC);
}

void uart_init(void) {
	UBRR0H = UBRRVAL >> 8;
	UBRR0L = UBRRVAL;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_sendstr(const char* t) {
	while (*t) {
		while (!(UCSR0A & (1 << UDRE0)));
		UDR0 = *t++;
	}
}

