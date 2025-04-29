/*
 * Postlaboratorio6.c
 *
 * Created: 28/04/2025 09:01:15
 * Author : Eduardo Urbina
 * Postlaboratorio 6
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>  // Para itoa()

// Prototipos
void setup(void);
void initUAT(void);
void writeChar(char c);
void writeString(const char* s);
void mostrarMenu(void);

volatile char comando = 0;  // Almacena el comando recibido

int main(void) {
	setup();
	writeString("Sistema listo\r\n");
	
	while(1) {
		mostrarMenu();
		
		// Esperar comando del usuario
		while(comando == 0);
		
		switch(comando) {
			case '1':
			
			break;
			
			case '2':
			
			break;
		}
		
		comando = 0;  // Resetear comando
	}
}

// Configuración inicial
void setup(void) {
	cli();
	DDRB = 0xFF;
	initUAT();
	sei();
}

// Inicializar USART (9600 bauds, 8N1)
void initUAT(void) {
	UBRR0 = 103;
	UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// Interrupción de recepción serial
ISR(USART_RX_vect) {
	comando = UDR0;  // Capturar comando del usuario
}

// Mostrar menú en terminal
void mostrarMenu(void) {
	writeString("\r\n=== Menu ===");
	writeString("\r\n1. Leer Potenciometro");
	writeString("\r\n2. Enviar ASCII");
	writeString("\r\nSeleccione opcion: ");
}


// Funciones básicas de USART
void writeChar(char c) {
	while(!(UCSR0A & (1 << UDRE0)));
	UDR0 = c;
}

void writeString(const char* s) {
	for(uint8_t i=0; s[i]!='\0'; i++) writeChar(s[i]);
}

