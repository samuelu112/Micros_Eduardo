/*
 * PWM4.h
 *
 * Created: 30/04/2025 10:10:52
 *  Author: samur
 */ 


#ifndef PWM4_H_
#define PWM4_H_
#include <avr/io.h>
#define invert     1
#define non_invert 0
void initPWMFastA_T1(uint8_t invertido, uint16_t prescaler);
void updateDutyCycle4(uint16_t ticks);
#endif
