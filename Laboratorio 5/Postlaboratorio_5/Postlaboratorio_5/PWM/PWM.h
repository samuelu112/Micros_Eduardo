/*
 * PWM.h
 *
 * Created: 20/04/2025 20:10:25
 *  Author: samur
 */ 


#ifndef PWM_H_
#define PWM_H_

#include <avr/io.h>

#define invert 1
#define non_invert 0

void initPWMFastA(uint8_t invertido, uint16_t prescaler);
void updateDutyCycle(uint8_t ticks);

#endif