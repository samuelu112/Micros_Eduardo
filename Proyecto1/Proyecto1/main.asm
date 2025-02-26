; UNIVERSIDAD DEL VALLE DE GUATEMALA
; IE2023: Programación de microcontroladores
; Proyecto1.asm
;
; Autor : Eduardo Samuel Urbina Pérez
; Proyecto : Proyecto 1
; Hardware : ATmega 328
; Creado: 12/02/2025
; Descripción:

.include "M328PDEF.inc"
.cseg
.org 0x0000
    JMP		START             ; Salto a la rutina de inicio

; Vector para interrupciones por cambio en PORTC (PCINT1)
.org 0x0008
    JMP    ISR_PCINT1

.org 0x0020
	RJMP		TMR0 ;Salta a rutina de timer0 overflow
.dseg
.org 0X0100
TABLA7SEG: .DB 0B00111111, 0B00000110, 0B01011011, 0B01001111, 0B01100110, 0B01101101, 0B01111101, 0B00000111, 0B01111111, 0B01100111
.cseg

START:
    ; Configuración de la pila
    LDI    R16, LOW(RAMEND)
    OUT    SPL, R16
    LDI    R16, HIGH(RAMEND)
    OUT    SPH, R16

    ; Deshabilitar interrupciones mientras se configura
    CLI
	;Configuracion de interrupciones de timer
	LDI		R16, (1<<TOIE0)
	STS		TIMSK0, R16
	LDI		R16, (1<<TOV0)
	STS		TIFR0, R16

	; Inicializar Timer0
    CALL	INIT_TMR0

	; Configurar el prescaler principal para obtener F_CPU = 1MHz
    LDI    R16, (1<<CLKPCE)
    STS    CLKPR, R16
    LDI    R16, 0b00000100
    STS    CLKPR, R16

	; Habilitar interrupciones
	SEI

MAIN_LOOP:
    ; En el loop principal solo se actualiza la salida de los Leds
	RJMP	MAIN_LOOP

INIT_TMR0:
    ;Configurar Timer0: prescaler de 64 para generar un overflow en 10ms
    LDI		R17, (1<<CS01) | (1<<CS00)
    OUT		TCCR0B, R17
    LDI		R17, 100
    OUT		TCNT0, R17
    RET