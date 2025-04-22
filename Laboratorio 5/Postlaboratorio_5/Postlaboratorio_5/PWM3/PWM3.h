/*
 * PWM3.h
 *
 * Created: 20/04/2025 20:11:45
 *  Author: samur
 */ 


#ifndef PWM3_H_
#define PWM3_H_

#include <avr/io.h>
#include <avr/interrupt.h>

void initPWM3_manual(uint16_t top, uint8_t prescalerBits);
void setPWM3_manual(uint8_t dutyValor);

#endif /* PWM3_H_ */
