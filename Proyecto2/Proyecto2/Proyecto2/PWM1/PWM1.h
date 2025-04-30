/*
 * PWM1.h
 *
 * Created: 30/04/2025 10:09:23
 *  Author: samur
 */ 


#ifndef PWM1_H_
#define PWM1_H_

#include <avr/io.h>

#define invert 1
#define non_invert 0

void initPWMFastA(uint8_t invertido, uint16_t prescaler);
void updateDutyCycle(uint8_t ticks);

#endif