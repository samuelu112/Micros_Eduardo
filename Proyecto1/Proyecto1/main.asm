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

; Programa Principal (código) 


.cseg
.org 0x0000
    RJMP START            ; Vector de reset

; Vector de interrupción por cambio en PORTC (para los pulsadores)
.org 0x0008
    RJMP ISR_PCINT1

; Vector de interrupción de Timer0 Overflow (ATmega328P: 0x0020)
.org 0x0020
    RJMP TMR0

.ORG 0X0100
TABLA7SEG: .DB 0x3F, 0X06, 0X5B, 0X4F, 0X66, 0X6D, 0X7D, 0X07, 0X7F, 0X67


START:
    ; Configuración de la pila
    LDI   r16, low(RAMEND)
    OUT   SPL, r16
    LDI   r16, high(RAMEND)
    OUT   SPH, r16

    cli ; Deshabilitar interrupciones
	
	;Configuracion de interrupciones de timer
	LDI		R16, (1<<TOIE0)
	STS		TIMSK0, R16
	LDI		R16, (1<<TOV0)
	STS		TIFR0, R16
	; Inicializar Timer0
    CALL	INIT_TMR0
    ; Configurar reloj: F_CPU = 1 MHz
    LDI   r16, (1<<CLKPCE)
    STS   CLKPR, r16
	LDI    R16, 0b00000100
    STS    CLKPR, R16

    ; Configurar PORTB:
    ; SALIDAS
    ; TRANSISTORES
	; PB0 minutos/dias unidades, PB1, minutos/dias decenas, PB2 Horas/meses unidades, PB3 horas/meses decenas
    LDI   r16, 0b00011111   ;
    OUT   DDRB, r16
    LDI   r16, 0b11111111
    OUT   PORTB, r16
	
    ; Habilitar interrupciones para PC0 y PC1 (corresponden a PCINT8 y PCINT9)
    LDI    R16, 0b00000011//(1<<PCINT8) | (1<<PCINT9)
    STS    PCMSK1, R16
; Configurar interrupciones por cambio para PORTC
    ; Habilitar interrupción por cambio para el grupo de PORTC (PCIE1 en PCICR)
    LDI    R16, 0b00000010//(1<<PCIE1)
    STS    PCICR, R16

	LDI		R16, 0B00000000
	OUT		DDRC, R16
	LDI		R16, 0B11111111
	OUT		PORTC, R16

    ; Configurar PORTD como salida (para los displays de 7 segmentos)
    LDI   r16, 0xFF
    OUT   DDRD, r16


    ; Inicializar contadores:
    ; Contador de pulsadores
    LDI   r24, 0x00
    ; Contador de overflows de Timer0 (para 1 s) en R5
	LDI		R16, 0
	MOV		R2, R16
	MOV		R3, R16
	MOV		R4, R16
    MOV		R5, R16
    ; Contador de segundos:
    ; R6: unidades (0–9) y r22: decenas (0–6)
    MOV		R6, R16
	MOV		R8, R16
	MOV		R9, R16
    LDI		r22, 0
    ; Condición de multiplexado en r23
    LDI		r23, 0
	LDI		R28, 0

    sei                      ; Habilitar interrupciones globales

LOOP:
	CPI		R28, 0X00
	BREQ	RELOJ
	;CPI		R28, 0X01
	;BREQ	FECHA
	CPI		R28, 0X02
	BREQ	CONHORA
	CPI		R28, 0X03
	;BREQ	CONFECHA
	;CPI		R28, 0X04
	;BREQ	ALARMA
	RJMP	LOOP

CONHORA:
	IN		R29, PINC
	SBRS	R29, 1
	INC		R3
	SBRS	R29, 2
	INC		R4

	SBRS	R27, 0
	INC		R23

	LDI		R22, 0X00
	OUT		PORTB, R22

	ANDI	R23, 0X03

    CPI		R23, 0X00
	BREQ	D1
	CPI		R23, 0X01
	BREQ	D2
	CPI		R23, 0X02
	BREQ	D3
	CPI		R23, 0X03
	BREQ	D4
	RJMP	LOOP


RELOJ:	
	IN		R27, TCNT0
	SBRS	R27, 0
	INC		R23
	
	LDI		R22, 0X00
	OUT		PORTB, R22

	ANDI	R23, 0X03

    CPI		R23, 0X00
	BREQ	D1
	CPI		R23, 0X01
	BREQ	D2
	CPI		R23, 0X02
	BREQ	D3
	CPI		R23, 0X03
	BREQ	D4
	RJMP	LOOP

D4:
	LDI		R30, LOW(TABLA7SEG<<1)   ; Cargar la parte baja de la dirección de la tabla
	LDI		R31, HIGH(TABLA7SEG<<1)  ; Cargar la parte alta de la dirección de la tabla
	; Sumar el desplazamiento según el dígito a mostrar:
	ADD		R30, R6              ; Si RX es el dígito deseado, se ajusta la dirección (suponiendo que cada patrón es de 1 byte)
	LPM		R24, Z
	OUT		PORTD, R24          ; Enviar el patrón a PORTD para mostrarlo	LDI		R16, 0X04	LDI		R16, 0X08
	LDI		R22, 0x08
	OUT		PORTB, R22
	RJMP	LOOP
D1:
	LDI		R30, LOW(TABLA7SEG<<1)   ; Cargar la parte baja de la dirección de la tabla
	LDI		R31, HIGH(TABLA7SEG<<1)  ; Cargar la parte alta de la dirección de la tabla
	
	; Sumar el desplazamiento según el dígito a mostrar:
	ADD		R30, R3              ; Si RX es el dígito deseado, se ajusta la dirección (suponiendo que cada patrón es de 1 byte)
	LPM		R24, Z
	OUT		PORTD, R24          ; Enviar el patrón a PORTD para mostrarlo	LDI		R16, 0X04	LDI		R16, 0X01
	LDI		R22, 0X01
	OUT		PORTB, R22
	RJMP LOOP
D2:
	LDI		R30, LOW(TABLA7SEG<<1)   ; Cargar la parte baja de la dirección de la tabla
	LDI		R31, HIGH(TABLA7SEG<<1)  ; Cargar la parte alta de la dirección de la tabla
	
	; Sumar el desplazamiento según el dígito a mostrar:
	ADD		R30, R4              ; Si RX es el dígito deseado, se ajusta la dirección (suponiendo que cada patrón es de 1 byte)
	LPM		R24, Z
	OUT		PORTD, R24          ; Enviar el patrón a PORTD para mostrarlo
	LDI		R22, 0X02
	OUT		PORTB, R22
	RJMP LOOP
D3:
    LDI		R30, LOW(TABLA7SEG<<1)   ; Cargar la parte baja de la dirección de la tabla
	LDI		R31, HIGH(TABLA7SEG<<1)  ; Cargar la parte alta de la dirección de la tabla
	
	; Sumar el desplazamiento según el dígito a mostrar:
	ADD		R30, R5             ; Si RX es el dígito deseado, se ajusta la dirección (suponiendo que cada patrón es de 1 byte)
	LPM		R24, Z
	OUT		PORTD, R24          ; Enviar el patrón a PORTD para mostrarlo	LDI		R16, 0X04
	LDI		R22, 0X04
	OUT		PORTB, R22
    RJMP LOOP

;Subrutina para inicializar Timer0
INIT_TMR0:
    ;Configurar Timer0: prescaler de 64 para generar un overflow en 10ms
    LDI		R16, (1<<CS01) | (1<<CS00)
    OUT		TCCR0B, R16
    LDI		R16, 100
    OUT		TCNT0, R16
    RET

; ISR: Interrupción por cambio en PORTC
ISR_PCINT1:
	; Inicializar Timer0
    CALL	INIT_TMR0
    IN		R29, PINC       ; Leer el estado actual de PORTC
    ; Si PC0 está en 0, incrementa el contador
    SBRS	R29, 0          ; Si el bit0 está alto, salta la siguiente instrucción
    INC		R28
	CPI		R28, 0X06
	BREQ	REIN
	RETI
REIN:
	LDI		R28, 0X00
    RETI

; Timer0 Overflow
; Al llegar 100, actualiza el contador de segundos
; y realiza el multiplexado de los dos displays.
TMR0:
    LDI		R27, 100
    OUT		TCNT0, R27        ; RECARGAR TCNT0 PARA ~10MS
	
	INC		R8
	LDI		R16, 100
	CP		R8, R16
	BRLO	REINICIO
UNIMINUTOS:
	CLR		R8
	SBI		PINB, 4; PUNTOD DISPLAY
	INC		R2
	LDI		R16, 60
	CP		R2, R16 ; 1MINUTO
	BRLO	REINICIO
MINUTOS:
	CLR		R2
	INC		R3
	LDI		R16, 0X0A
	CP		R3, R16
	BRLO	REINICIO
RES:
	CLR		R3
	INC		R4
	LDI		R16, 0X06
	CP		R4, R16
	BRLO	REINICIO
HORAS:
	CLR		R4
	INC		R5
	LDI		R16, 0X0A
	CP		R5, R16
	BRLO	REINICIO
DECHORAS:
	CLR		R5
	INC		R6
	LDI		R16, 0X03
	CP		R6, R16
	BRLO	REINICIO
FINALH:
	CLR		R6
REINICIO:
	RETI