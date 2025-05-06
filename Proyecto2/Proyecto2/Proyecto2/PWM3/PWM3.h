/*
 * PWM3.h
 *
 * Created: 30/04/2025 10:10:38
 *  Author: samur
 */ 


#ifndef PWM3_H_
#define PWM3_H_
#include <avr/io.h>
#define invert     1
#define non_invert 0
void initPWMFastB_T1(uint8_t invertido, uint16_t prescaler);
void updateDutyCycle3(uint16_t ticks);
#endif
