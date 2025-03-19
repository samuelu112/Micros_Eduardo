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

; Programa Principal


.cseg
.org 0x0000
    RJMP START            ; Vector de reset

; Vector de interrupción por cambio en PORTC (para los pulsadores)
.org 0x0008
    RJMP ISR_PCINT1

; Vector de interrupción de Timer0 Overflow (ATmega328P: 0x0020)
.org 0x0020
    RJMP TMR0
.org 0x001C          ; Vector de interrupción de overflow del Timer2 (ATmega328P)
    RJMP TMR2
.ORG 0X0100		;Direccionamiento tabla de 7 segmentos
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
; --- Habilitar interrupción por overflow ---
    LDI   R16, (1<<TOIE2)        ; TOIE2: Habilitar interrupción por overflow
    STS   TIMSK2, R16
	LDI		R16, (1<<TOV2)
	STS		TIFR2, R16
	CALL	INIT_TMR2
    ; Configurar reloj: F_CPU = 1 MHz
    LDI		r16, (1<<CLKPCE)
    STS		CLKPR, r16
	LDI		R16, 0b00000100
    STS		CLKPR, R16

    ; Configurar PORTB:
    ; SALIDAS
    ; TRANSISTORES
	; PB0 minutos/dias unidades, PB1, minutos/dias decenas, PB2 Horas/meses unidades, PB3 horas/meses decenas
    LDI   r16, 0b00111111   ;
    OUT   DDRB, r16
    LDI   r16, 0b11011111
    OUT   PORTB, r16
	
; Configurar interrupciones por cambio para PORTC
    ; Habilitar interrupción por cambio para el grupo de PORTC (PCIE1 en PCICR)
    LDI    R16, 0b00000010//(1<<PCIE1)
    STS    PCICR, R16
    ; Habilitar interrupciones para PC0, PC1, PC2, PC3 y PC4
    LDI    R16, (1<<PCINT8) | (1<<PCINT9) | (1<<PCINT10) |(1<<PCINT11)|(1<<PCINT12)
    STS    PCMSK1, R16
	;Activar entradas y salida en puertoC
	LDI		R16, 0B00100000
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
    ; Contadores:
    ; Valores iniciales
    MOV		R6, R16
	MOV		R8, R16
	MOV		R9, R16
	MOV		R10, R16
	MOV		R11, R16
	MOV		R12, R16
	MOV		R13, R16
	MOV		R15, R16
	LDI		R17, 0X01
	MOV		R14, R17
	LDI		R18, 0X00
	LDI		R19, 0X00
	LDI		R20, 0X00
	LDI		R21, 0X00
    LDI		r22, 0
    ; Condición de multiplexado en r23
    LDI		r23, 0
	LDI		R28, 0

    sei     ; Habilitar interrupciones globales

;--------------LOOP principal-----------------
LOOP:
	CPI		R28, 0X00
	BREQ	RELOJ1		;Modo reloj
	CPI		R28, 0X01
	BREQ	FECHA1		;Modo fecha
	CPI		R28, 0X02
	BREQ	CONHORA		;Modo modificar hora
	CPI		R28, 0X03
	BREQ	CONFECHA1	;Modo modificar fecha
	CPI		R28, 0X04
	BREQ	ALARMA1		;Configuracion alarma
	CPI		R28, 0X05
	BREQ	ACTIVAR_ALARMA1 ;Activar alarma
	CPI		R28, 0X06
	BREQ	CONTADOR0	;Reiniciar los modos
	RJMP	LOOP

CONTADOR0:	;Reinciar los modos
	CLR		R28
	LDI		R16, (1<<CS01) | (1<<CS00);Activar timer0
    OUT		TCCR0B, R16
	LDI		R16, 100         ; Valor inicial para 10ms (1MHz)
	OUT		TCNT0, R16
	RJMP	LOOP
	;Saltos
CONFECHA1:		
	RJMP	CONFECHA
FECHA1:
	RJMP	FECHA
ALARMA1:
	RJMP	ALARMA
RELOJ1:
	RJMP	RELOJ
ACTIVAR_ALARMA1:
	RJMP	ACTIVAR_ALARMA
APAGARA:
	RJMP	LOOP
;-----------------MODO CONFIGURAR HORA-------------------
CONHORA:
	LDI		R16, 0	;Desactivar timer0
    OUT		TCCR0B, R16 
	LDS		R25, TCNT2 ;Usar timer2 para indicador para multiplexado
	SBRS	R25, 0
	INC		R23
	ANDI	R23, 0X03
	
	LDI		R22, 0X00
	OUT		PORTB, R22
	
	LDI		R16, 0XFF ;UNDERFLOW MINUTOS
	CP		R3, R16
	BREQ	MIN59

	LDI		R16, 0XFF
	CP		R5, R16
	BREQ	HOR24
;HORAS DECENAS
	LDI		R16, 0X02
	CP		R6, R16
	BREQ	CON24HORAS
;HORAS UNIDADES
	LDI		R16, 0X0A
	CP		R5, R16
	BREQ	LR5
;MINUTOS DECENAS
MINDECENAS:
	LDI		R16, 0X06
	CP		R4, R16
	BREQ	LR4
;MINUTOS UNIDADES
	LDI		R16, 0X0A
	CP		R3, R16
	BREQ	LR3
	RJMP	COMPROBAR
MIN59:
	LDI		R16, 0X00
	CP		R4, R16
	BREQ	MINMAX
	DEC		R4
	LDI		R16, 0X09
	MOV		R3, R16
	RJMP	COMPROBAR
MINMAX:
	LDI		R16, 0XFF
	CP		R3, R16
	BREQ	PONER59
	RJMP	COMPROBAR
PONER59:
	LDI		R16, 0X05
	MOV		R4, R16
	LDI		R16, 0X09
	MOV		R3, R16
	RJMP	COMPROBAR
HOR24:
	LDI		R16, 0X00
	CP		R6, R16
	BREQ	PONER24
	DEC		R6
	LDI		R16, 0X09
	MOV		R5, R16
	RJMP	COMPROBAR
PONER24:
	LDI		R16, 0XFF
	CP		R5, R16
	BREQ	PONER23
	RJMP	COMPROBAR
PONER23:
	LDI		R16, 0X02
	MOV		R6, R16
	LDI		R16, 0X03
	MOV		R5, R16
	RJMP	COMPROBAR
LR3:
	CLR		R3
	INC		R4
	RJMP	COMPROBAR
LR4:
	CLR		R4
	CLR		R3
	RJMP	COMPROBAR	
LR5:
	CLR		R5
	INC		R6
	RJMP	COMPROBAR
CON24HORAS:
	LDI		R16, 0X04
	CP		R5, R16
	BRLO	MINDECENAS
	CLR		R5
REG6:
	CLR		R6
;MULTIPLEXADO
COMPROBAR:
	CPI		R23, 0X00
	BREQ	D1
	CPI		R23, 0X01
	BREQ	D2
	CPI		R23, 0X02
	BREQ	D3
	CPI		R23, 0X03
	BREQ	D4
	RJMP	LOOP
;-------------------MODO HORA------------------------
RELOJ:	
	lds		R27, TCNT2
	SBRS	R27, 0
	INC		R23
	ANDI	R23, 0X03
	
	CPI		R21, 0X01
	BRNE	SEGUIR2
	CP		R3, R10
	BRNE	SEGUIR1
	CP		R4, R11
	BRNE	SEGUIR1
	CP		R5, R12
	BRNE	SEGUIR1
	CP		R6, R13
	BRNE	SEGUIR1
	SBI		PORTD, 7
	RJMP	SEGUIR2
SEGUIR1:
	CBI		PORTD, 7
SEGUIR2:
	LDI		R22, 0X00
	OUT		PORTB, R22

    CPI		R23, 0X00
	BREQ	D1
	CPI		R23, 0X01
	BREQ	D2
	CPI		R23, 0X02
	BREQ	D3
	CPI		R23, 0X03
	BREQ	D4
	RJMP	LOOP
;----------------------Salidas a display
D1:
	LDI		R30, LOW(TABLA7SEG<<1)   ; Cargar la parte baja de la dirección de la tabla
	LDI		R31, HIGH(TABLA7SEG<<1)  ; Cargar la parte alta de la dirección de la tabla
	
	; Sumar el desplazamiento según el dígito a mostrar:
	ADD		R30, R3              ;se ajusta la dirección
	LPM		R24, Z
	OUT		PORTD, R24          ; Enviar el patrón a PORTD para mostrarlo	LDI		R16, 0X04	LDI		R16, 0X01
	LDI		R22, 0X01
	OUT		PORTB, R22
	RJMP LOOP
D2:
	LDI		R30, LOW(TABLA7SEG<<1)   ; Cargar la parte baja de la dirección de la tabla
	LDI		R31, HIGH(TABLA7SEG<<1)  ; Cargar la parte alta de la dirección de la tabla
	
	; Sumar el desplazamiento según el dígito a mostrar:
	ADD		R30, R4              ; se ajusta la dirección
	LPM		R24, Z
	OUT		PORTD, R24          ; Enviar el patrón a PORTD para mostrarlo
	LDI		R22, 0X02
	OUT		PORTB, R22
	RJMP LOOP
D3:
    LDI		R30, LOW(TABLA7SEG<<1)   ; Cargar la parte baja de la dirección de la tabla
	LDI		R31, HIGH(TABLA7SEG<<1)  ; Cargar la parte alta de la dirección de la tabla
	
	; Sumar el desplazamiento según el dígito a mostrar:
	ADD		R30, R5             ; se ajusta la dirección
	LPM		R24, Z
	OUT		PORTD, R24          ; Enviar el patrón a PORTD para mostrarlo	LDI		R16, 0X04
	LDI		R22, 0X04
	OUT		PORTB, R22
    RJMP LOOP
D4:
	LDI		R30, LOW(TABLA7SEG<<1)   ; Cargar la parte baja de la dirección de la tabla
	LDI		R31, HIGH(TABLA7SEG<<1)  ; Cargar la parte alta de la dirección de la tabla
	; Sumar el desplazamiento según el dígito a mostrar:
	ADD		R30, R6              ;se ajusta la dirección
	LPM		R24, Z
	OUT		PORTD, R24          ; Enviar el patrón a PORTD para mostrarlo	LDI		R16, 0X04	LDI		R16, 0X08
	LDI		R22, 0x08
	OUT		PORTB, R22
	RJMP	LOOP
;-------------------MODO FECHA----------------------------
FECHA:
	lds		R27, TCNT2
	SBRS	R27, 0
	INC		R23
	ANDI	R23, 0X03
	
	CPI		R21, 0X01
	BRNE	SEGUIR4
	CP		R3, R10
	BRNE	SEGUIR3
	CP		R4, R11
	BRNE	SEGUIR3
	CP		R5, R12
	BRNE	SEGUIR1
	CP		R6, R13
	BRNE	SEGUIR1
	SBI		PORTD, 7
	RJMP	SEGUIR4
SEGUIR3:
	CBI		PORTD, 7
SEGUIR4:
	LDI		R22, 0X20
	OUT		PORTB, R22
;-----------------MULTIPLEXADO--------------------------
COMPROBAR3:
    CPI		R23, 0X00
	BREQ	FD1
	CPI		R23, 0X01
	BREQ	FD2
	CPI		R23, 0X02
	BREQ	FD3
	CPI		R23, 0X03
	BREQ	FD4
	RJMP	LOOP
;---------------------Salidas display---------------------
FD1:
	LDI		R30, LOW(TABLA7SEG<<1)   ; Cargar la parte baja de la dirección de la tabla
	LDI		R31, HIGH(TABLA7SEG<<1)  ; Cargar la parte alta de la dirección de la tabla
	
	; Sumar el desplazamiento según el dígito a mostrar:
	ADD		R30, R14              ; se ajusta la dirección
	LPM		R24, Z
	OUT		PORTD, R24          ; Enviar el patrón a PORTD para mostrarlo	LDI		R16, 0X04	LDI		R16, 0X01
	LDI		R22, 0X21
	OUT		PORTB, R22
	RJMP LOOP
FD2:
	LDI		R30, LOW(TABLA7SEG<<1)   ; Cargar la parte baja de la dirección de la tabla
	LDI		R31, HIGH(TABLA7SEG<<1)  ; Cargar la parte alta de la dirección de la tabla
	
	; Sumar el desplazamiento según el dígito a mostrar:
	ADD		R30, R15              ; se ajusta la dirección
	LPM		R24, Z
	OUT		PORTD, R24          ; Enviar el patrón a PORTD para mostrarlo
	LDI		R22, 0X22
	OUT		PORTB, R22
	RJMP LOOP
FD3:
    LDI		R30, LOW(TABLA7SEG<<1)   ; Cargar la parte baja de la dirección de la tabla
	LDI		R31, HIGH(TABLA7SEG<<1)  ; Cargar la parte alta de la dirección de la tabla
	
	; Sumar el desplazamiento según el dígito a mostrar:
	ADD		R30, R17             ; se ajusta la dirección
	LPM		R24, Z
	OUT		PORTD, R24          ; Enviar el patrón a PORTD para mostrarlo	LDI		R16, 0X04
	LDI		R22, 0X24
	OUT		PORTB, R22
    RJMP LOOP
FD4:
	LDI		R30, LOW(TABLA7SEG<<1)   ; Cargar la parte baja de la dirección de la tabla
	LDI		R31, HIGH(TABLA7SEG<<1)  ; Cargar la parte alta de la dirección de la tabla
	; Sumar el desplazamiento según el dígito a mostrar:
	ADD		R30, R18              ; se ajusta la dirección
	LPM		R24, Z
	OUT		PORTD, R24          ; Enviar el patrón a PORTD para mostrarlo	LDI		R16, 0X04	LDI		R16, 0X08
	LDI		R22, 0x28
	OUT		PORTB, R22
	RJMP	LOOP
;------------MODO CONFIGURAR FECHA-----------
CONFECHA:
	LDI		R16, 0
    OUT		TCCR0B, R16
	LDS		R25, TCNT2
	SBRS	R25, 0
	INC		R23
	ANDI	R23, 0X03
	
	LDI		R22, 0X20
	OUT		PORTB, R22
	;hacer de 1 a 12
	MOV		R19, R17
	MOV		R20, R18
	LDI		R16, 10
	MUL		R20, R16
	ADD		R19, R0

	CPI		R17, 0XFF
	BREQ	SEPTIEMBRE
	CPI		R19, 0X0D
	BREQ	ENERO
	CPI		R19, 0X00
	BREQ	DICIEMBREC
	CPI		R19, 0X0A
	BREQ	OCTUBRE

	LDI		R16, 0X0A
	CP		R14, R16
	BREQ	PONERA101

	LDI		R16, 0XFF
	CP		R14, R16
	BREQ	VERMES

	LDI		R16, 0X09
	CP		R14, R16
	BREQ	ESFEBRERO
	
	LDI		R16, 0X00
	CP		R14, R16
	BREQ	VERMES1

	LDI		R16, 0X03
	CP		R15, R16
	BRNE	COMPROBAR31

	LDI		R16, 0X01
	CP		R14, R16
	BREQ	MESDE30DIAS
	LDI		R16, 0X02
	CP		R14, R16
	BREQ	ABRILREINICIO
	RJMP	COMPROBAR3
VERMES1:
	LDI		R16, 0X00
	CP		R15, R16
	BREQ	IRA
	RJMP	COMPROBAR3
	
ENERO:
	CLR		R18
	LDI		R17, 0X01
	RJMP	COMPROBAR3
SEPTIEMBRE:
	CPI		R18, 0X01
	BRNE	COMPROBAR31
	LDI		R17, 0X09
	CLR		R18
	RJMP	COMPROBAR3
OCTUBRE:
	LDI		R18, 0X01
	CLR		R17
	RJMP	COMPROBAR3
DICIEMBREC:
	LDI		R18, 0X01
	LDI		R17, 0X02
	RJMP	COMPROBAR3
PONERA101:
	RJMP	PONERA10
ESFEBRERO:
	RJMP	ESFEBRERO1
MESDE30DIAS:
	RJMP	MESDE30DIAS1
ABRILREINICIO:
	RJMP	ABRILREINICIO1
VERMES:
	LDI		R16, 0
	CP		R15, R16
	BRNE	MENOSR15
IRA:
	CPI		R19, 2
	BREQ	FEB
	CPI		R19, 4
	BREQ	ABR
	CPI		R19, 6
	BREQ	ABR
	CPI		R19, 9
	BREQ	ABR
	CPI		R19, 11
	BREQ	ABR
	LDI		R16, 0X01
	MOV		R14, R16
	LDI		R16, 0X03
	MOV		R15, R16
	RJMP	COMPROBAR3
FEB:
	LDI		R16, 0X08
	MOV		R14, R16
	LDI		R16, 0X02
	MOV		R15, R16
	RJMP	COMPROBAR3
COMPROBAR31:
	RJMP	COMPROBAR3
ABR:
	LDI		R16, 0X00
	MOV		R14, R16
	LDI		R16, 0X03
	MOV		R15, R16
	RJMP	COMPROBAR3
MENOSR15:
	DEC		R15
	LDI		R16, 0X09
	MOV		R14, R16
	RJMP	COMPROBAR3
MESDE30DIAS1:
	CPI		R19, 0X04
	BREQ	ABRILREINICIO
	CPI		R19, 0X06
	BREQ	ABRILREINICIO
	CPI		R19, 0X09
	BREQ	ABRILREINICIO
	CPI		R19, 0X0B
	BREQ	ABRILREINICIO
	RJMP	COMPROBAR3
ABRILREINICIO1:
	CLR		R15
	LDI		R16, 0X01
	MOV		R14, R16
	RJMP	COMPROBAR3
PONERA10:
	INC		R15
	CLR		R14
	RJMP	COMPROBAR3
ESFEBRERO1:
	LDI		R16, 0X02
	CP		R15, R16
	BRNE	COMPROBAR31
	CPI		R19, 0X02
	BRNE	COMPROBAR31
	LDI		R16, 0X01
	MOV		R14, R16
	CLR		R15
	RJMP	COMPROBAR3
;------------MODO CONFIGURAR ALARMA----------	
ALARMA:
	LDI   R16, 0
    OUT   TCCR0B, R16
	LDS		R25, TCNT2
	SBRS	R25, 0
	INC		R23
	ANDI	R23, 0X03
	
	LDI		R22, 0X20
	OUT		PORTB, R22
	
	LDI		R16, 0XFF ;UNDERFLOW MINUTOS
	CP		R10, R16
	BREQ	AMIN59

	LDI		R16, 0XFF
	CP		R12, R16
	BREQ	AHOR24
;HORAS DECENAS
	LDI		R16, 0X02
	CP		R13, R16
	BREQ	ACON24HORAS
;HORAS UNIDADES
	LDI		R16, 0X0A
	CP		R12, R16
	BREQ	ALR5
;MINUTOS DECENAS
AMINDECENAS:
	LDI		R16, 0X06
	CP		R11, R16
	BREQ	ALR4
;MINUTOS UNIDADES
	LDI		R16, 0X0A
	CP		R10, R16
	BREQ	ALR3
	RJMP	COMPROBAR2
AMIN59:
	LDI		R16, 0X00
	CP		R11, R16
	BREQ	AMINMAX
	DEC		R11
	LDI		R16, 0X09
	MOV		R10, R16
	RJMP	COMPROBAR2
AMINMAX:
	LDI		R16, 0XFF
	CP		R10, R16
	BREQ	APONER59
	RJMP	COMPROBAR2
APONER59:
	LDI		R16, 0X05
	MOV		R11, R16
	LDI		R16, 0X09
	MOV		R10, R16
	RJMP	COMPROBAR2

AHOR24:
	LDI		R16, 0X00
	CP		R13, R16
	BREQ	APONER24
	DEC		R13
	LDI		R16, 0X09
	MOV		R12, R16
	RJMP	COMPROBAR2
APONER24:
	LDI		R16, 0XFF
	CP		R12, R16
	BREQ	APONER23
	RJMP	COMPROBAR2
APONER23:
	LDI		R16, 0X02
	MOV		R13, R16
	LDI		R16, 0X03
	MOV		R12, R16
	RJMP	COMPROBAR2
ALR3:
	CLR		R10
	INC		R11
	RJMP	COMPROBAR2
ALR4:
	CLR		R11
	CLR		R10
	RJMP	COMPROBAR2	
ALR5:
	CLR		R12
	INC		R13
	RJMP	COMPROBAR2
ACON24HORAS:
	LDI		R16, 0X04
	CP		R12, R16
	BRLO	AMINDECENAS
	CLR		R12
AREG6:
	CLR		R13
;-------------MULTIPLEXADO 2-----------
COMPROBAR2:
	CPI		R23, 0X00
	BREQ	DA1
	CPI		R23, 0X01
	BREQ	DA2
	CPI		R23, 0X02
	BREQ	DA3
	CPI		R23, 0X03
	BREQ	DA4
	RJMP	LOOP
;-------------Salidas display------------
DA1:
	LDI		R30, LOW(TABLA7SEG<<1)   ; Cargar la parte baja de la dirección de la tabla
	LDI		R31, HIGH(TABLA7SEG<<1)  ; Cargar la parte alta de la dirección de la tabla
	
	; Sumar el desplazamiento según el dígito a mostrar:
	ADD		R30, R10              ; se ajusta la dirección
	LPM		R24, Z
	OUT		PORTD, R24          ; Enviar el patrón a PORTD para mostrarlo	LDI		R16, 0X04	LDI		R16, 0X01
	LDI		R22, 0X21
	OUT		PORTB, R22
	RJMP LOOP
DA2:
	LDI		R30, LOW(TABLA7SEG<<1)   ; Cargar la parte baja de la dirección de la tabla
	LDI		R31, HIGH(TABLA7SEG<<1)  ; Cargar la parte alta de la dirección de la tabla
	
	; Sumar el desplazamiento según el dígito a mostrar:
	ADD		R30, R11              ; se ajusta la dirección
	LPM		R24, Z
	OUT		PORTD, R24          ; Enviar el patrón a PORTD para mostrarlo
	LDI		R22, 0X22
	OUT		PORTB, R22
	RJMP LOOP
DA3:
    LDI		R30, LOW(TABLA7SEG<<1)   ; Cargar la parte baja de la dirección de la tabla
	LDI		R31, HIGH(TABLA7SEG<<1)  ; Cargar la parte alta de la dirección de la tabla
	
	; Sumar el desplazamiento según el dígito a mostrar:
	ADD		R30, R12             ; se ajusta la dirección
	LPM		R24, Z
	OUT		PORTD, R24          ; Enviar el patrón a PORTD para mostrarlo	LDI		R16, 0X04
	LDI		R22, 0X24
	OUT		PORTB, R22
    RJMP LOOP
DA4:
	LDI		R30, LOW(TABLA7SEG<<1)   ; Cargar la parte baja de la dirección de la tabla
	LDI		R31, HIGH(TABLA7SEG<<1)  ; Cargar la parte alta de la dirección de la tabla
	; Sumar el desplazamiento según el dígito a mostrar:
	ADD		R30, R13              ; se ajusta la dirección
	LPM		R24, Z
	OUT		PORTD, R24          ; Enviar el patrón a PORTD para mostrarlo	LDI		R16, 0X04	LDI		R16, 0X08
	LDI		R22, 0x28
	OUT		PORTB, R22
	RJMP	LOOP  
;---------------Modo activar alarma------------
ACTIVAR_ALARMA:
	LDI		R16, 0
    OUT		TCCR0B, R16

	CPI		R21, 0X02
	BREQ	REINICIAR21
	CPI		R21, 0XFF
	BREQ	REINICIARA1
	CPI		R21, 0X01
	BRNE	PONERB0
	LDI		R22, 0XEF
	OUT		PORTB, R22
	RJMP	LOOP;COMPROBAR2
PONERB0:
	LDI		R22, 0X00
	OUT		PORTB, R22
	RJMP	LOOP;COMPROBAR2
REINICIAR21:
	LDI		R21, 0X01
	RJMP	LOOP;COMPROBAR2
REINICIARA1:
	LDI		R21, 0X00
	RJMP	LOOP;COMPROBAR2
;----------------------------------------------------------
;Subrutina para inicializar Timer0
INIT_TMR0:
    ;Configurar Timer0: prescaler de 64 para generar un overflow en 10ms
    LDI		R16, (1<<CS01) | (1<<CS00)
    OUT		TCCR0B, R16
    LDI		R16, 100
    OUT		TCNT0, R16
    RET
INIT_TMR2:
	; --- Inicializar el contador ---
	LDI   R16, (1<<CS22)         ; Prescaler = 64 (CS22=1, CS21=0, CS20=0)
    STS   TCCR2B, R16
    LDI   R16, 100                ; Iniciar desde 0
    STS   TCNT2, R16
	RET
;--------------------------------------------------------------------------------
; ISR: Interrupción por cambio en PORTC
ISR_PCINT1:
    IN		R29, PINC       ; Leer el estado actual de PORTC
    ; Si PC0 está en 0, incrementa el contador
    SBRS	R29, 0          ; Si el bit0 está alto, salta la siguiente instrucción
    INC		R28

	CPI		R28, 0X00 ;Hora
	BREQ	VERHORA
	CPI		R28, 0X01 ;Configurar hora
	BREQ	VERFECHA
	CPI		R28, 0X02 ;Configurar hora
	BREQ	PHORA
	CPI		R28, 0X03 ;Configurar fecha
	BREQ	PFECHA
	CPI		R28, 0X04 ;Configurar alarma
	BREQ	PALARMA
	CPI		R28, 0X05
	BREQ	PACALARMA
	RETI
VERHORA:	;Modo hora
	SBI		PORTC, 5
	CBI		PORTB, 5
	RETI
VERFECHA:	;Modo fecha
	CBI		PORTC, 5
	LDI		R16, 0X20
	OUT		PORTB, R16
	RETI
PHORA:		;Modo configurar hora
	SBI		PORTC, 5
	CBI		PORTB, 5
	SBRS	R29, 1
	INC		R5
	SBRS	R29, 2
	DEC		R5
	SBRS	R29, 3
	INC		R3
	SBRS	R29, 4
	DEC		R3
	RETI
PFECHA:		;Modo configurar fecha
	CBI		PORTC, 5
	LDI		R16, 0X20
	OUT		PORTB, R16
	SBRS	R29, 1
	INC		R17
	SBRS	R29, 2
	DEC		R17
	SBRS	R29, 3
	INC		R14
	SBRS	R29, 4
	DEC		R14
	RETI
PALARMA:	;Modo configurar alarma
	SBI		PORTC, 5
	LDI		R16, 0X20
	OUT		PORTB, R16

	SBRS	R29, 1
	INC		R12
	SBRS	R29, 2
	DEC		R12
	SBRS	R29, 3
	INC		R10
	SBRS	R29, 4
	DEC		R10
	RETI
PACALARMA:	;Modo activar alarma
	CBI		PORTC, 5
	LDI		R16, 0X00
	OUT		PORTB, R16
	SBRS	R29, 1
	INC		R21
	SBRS	R29, 2
	DEC		R21
    RETI


; Timer0 Overflow
; Al llegar 100, actualiza el contador de segundos
; y realiza el multiplexado de los dos displays.
TMR2:
	LDI		R16, 100                ; Iniciar desde 0
    STS		TCNT2, R16
	INC		R26
	CPI		R26, 100
	BRLO	REINICIO1
	CLR		R26
	RETI
TMR0:
    LDI		R16, 100
    OUT		TCNT0, R16       ; RECARGAR TCNT0 PARA ~10MS
	
	INC		R8
	LDI		R16, 100
	CP		R8, R16
	BRLO	REINICIO1
UNIMINUTOS:
	SBI		PINB, 4; PUNTOD DISPLAY
	CLR		R8
	INC		R2
	LDI		R16, 60
	CP		R2, R16 ; 1MINUTO
	BRLO	REINICIO1
MINUTOS:
	CLR		R2
	INC		R3
	LDI		R16, 0X0A
	CP		R3, R16
	BRLO	REINICIO1
RES:
	CLR		R3
	INC		R4
	LDI		R16, 0X06
	CP		R4, R16
	BRLO	REINICIO1
HORAS:
	CLR		R4
	INC		R5
	LDI		R16, 0X02
	CP		R6, R16
	BREQ	L24HORAS
		
	LDI		R16, 0X0A
	CP		R5, R16
	BRLO	REINICIO

	CLR		R5
	INC		R6
REINICIO1:
	RJMP	REINICIO
L24HORAS:
	LDI		R16, 0X03
	CP		R5, R16
	BRLO	REINICIO
	CLR		R5
	CLR		R6
; --- Incrementar día ---
;VERIFICAR MES
	;hacer de 1 a 12
	MOV		R19, R17
	MOV		R20, R18
	LDI		R16, 10
	MUL		R20, R16
	ADD		R19, R0

	CPI		R19, 0X02
	BREQ	FEBRERO
	CPI		R19, 0X04
	BREQ	ABRIL
	CPI		R19, 0X06
	BREQ	ABRIL
	CPI		R19, 0X09
	BREQ	ABRIL
	CPI		R19, 0X0B
	BREQ	ABRIL
	;MESES 31 DIAS
	LDI		R16, 0X03
	CP		R15, R16
	BREQ	MESES31
	RJMP	SFEB
MESES31:
	LDI		R16, 0X01
	CP		R14, R16
	BRLO	SFEB
	CPI		R19, 0X0C
	BREQ	DICIEMBRE
	CLR		R15
	INC		R17
	LDI		R16, 0X01
	MOV		R14, R16
	CPI		R17, 0X0A
	BREQ	MES
	RJMP	REINICIO
SFEB:
    INC		R14                ; Incrementar unidades de día
	LDI		R16, 10
    CP		R14, R16            ; ¿Unidades >= 10?
    BRLO	REINICIO      ; No, verificar máximo
    CLR		R14                ; Resetear unidades
    INC		R15                ; Incrementar decenas de día 
REINICIO:
    RETI
FEBRERO:
	LDI		R16, 0X02
	CP		R15, R16
	BREQ	FEB28
	RJMP	SFEB
FEB28:
	LDI		R16, 0X08
	CP		R14, R16
	BRLO	SFEB
	CLR		R15
	INC		R17
	LDI		R16, 0X01
	MOV		R14, R16
	CPI		R17, 0X0A
	BREQ	MES
	RJMP	REINICIO
ABRIL:
	LDI		R16, 0X03
	CP		R15, R16
	BREQ	AVIEJO
	RJMP	SFEB
AVIEJO:
	LDI		R16, 0X00
	CP		R14, R16
	BRLO	SFEB
	CLR		R15 
	INC		R17
	LDI		R16, 0X01
	MOV		R14, R16
	CPI		R17, 0X0A
	BREQ	MES
	RJMP	REINICIO
MES:   
	CLR		R17
	INC		R18
	RJMP	REINICIO
DICIEMBRE:
	CLR		R15
	CLR		R18
	LDI		R17, 1
	LDI		R16, 0X01
	MOV		R14, R16
	RJMP	REINICIO