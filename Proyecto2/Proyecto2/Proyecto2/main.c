/*
 * Proyecto2.c
 *
 * Created: 30/04/2025 09:47:02
 * Author : samur
 */ 
/*
 * Proyecto2.c
 *
 * Created: 30/04/2025 09:47:02
 * Author : samur
 */
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "PWM1/PWM1.h"   // Servo1 (Timer0 OC0A / PD6)
#include "PWM2/PWM2.h"   // Servo2 (Timer2 OC2B / PD3)
#include "PWM3/PWM3.h"   // Servo3 (Timer1 OC1B / PB2)
#include "PWM4/PWM4.h"   // Servo4 (Timer1 OC1A / PB1)

// Variables ADC para 4 canales
volatile uint8_t  adc0_value, adc1_value;
volatile uint16_t adc2_value, adc3_value;
volatile uint8_t  current_channel = 0;

void initADC(void);

int main(void) {
    cli();

    // Inicializar 4 servos
    initPWMFastA(non_invert, 1024);    // Servo1 (Timer0 OC0A on PD6)
    initPWMFastB(non_invert, 1024);    // Servo2 (Timer2 OC2B on PD3)
    initPWMFastB_T1(non_invert, 8);    // Servo3 (Timer1 OC1B on PB2), prescaler 8
    initPWMFastA_T1(non_invert, 8);    // Servo4 (Timer1 OC1A on PB1), prescaler 8

    // ADC free-running con interrupciones en 4 canales
    initADC();

    sei();

    while (1) {
        // Mapea ADC (0–255) a ticks para servos 1&2 (9–37 ticks)
        uint16_t t0 =  9 + ((uint32_t)adc0_value * 28) / 255;
        uint16_t t1 =  9 + ((uint32_t)adc1_value * 28) / 255;

        // Mapea ADC (0–1023) a ticks para servos 3&4:
        // 600 µs–2400 µs => en ticks de 0.5 µs: 1200–4800
        uint16_t t2 = 1200 + ((uint32_t)adc2_value * 3600) / 1023;
        uint16_t t3 = 1200 + ((uint32_t)adc3_value * 3600) / 1023;

        updateDutyCycle(t0);   // Servo1
        updateDutyCycle2(t1);  // Servo2
        updateDutyCycle3(t2);  // Servo3
        updateDutyCycle4(t3);  // Servo4

        _delay_ms(20);
    }
}

void initADC(void) {
    // AVcc ref + left adjust, free-running
    ADMUX  = (1 << REFS0) | (1 << ADLAR) | (current_channel & 0x07);
    ADCSRA = (1 << ADEN)
           | (1 << ADIE)
           | (1 << ADATE)
           | (1 << ADPS2)|(1 << ADPS1)|(1 << ADPS0);
    ADCSRB = 0;
    DIDR0  = (1 << ADC0D)|(1 << ADC1D)|(1 << ADC2D)|(1 << ADC3D);
    ADCSRA |= (1 << ADSC);
}

// ADC ISR: rota canales 0?1?2?3 y guarda valores
ISR(ADC_vect) {
    switch (current_channel) {
    case 0:
        adc0_value = ADCH;
        current_channel = 1;
        break;
    case 1:
        adc1_value = ADCH;
        current_channel = 2;
        break;
    case 2:
        adc2_value = ADC;  // 10-bit result
        current_channel = 3;
        break;
    case 3:
        adc3_value = ADC;
        current_channel = 0;
        break;
    }
    // Prepara siguiente canal
    ADMUX = (1 << REFS0)
          | ((current_channel<2)? (1<<ADLAR):0)
          | (current_channel & 0x07);
    ADCSRA |= (1 << ADSC);
}
