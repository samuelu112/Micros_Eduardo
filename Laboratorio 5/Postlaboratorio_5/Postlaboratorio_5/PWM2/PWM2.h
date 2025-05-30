/*
 * PWM2.h
 *
 * Created: 20/04/2025 20:11:24
 *  Author: samur
 */ 


#ifndef PWM2_H_
#define PWM2_H_

#include <avr/io.h>

#define invert 1
#define non_invert 0

void initPWMFastB(uint8_t invertido2, uint16_t prescaler2);
void updateDutyCycle2(uint8_t ticks2);

#endif /* PWM2_H_ */