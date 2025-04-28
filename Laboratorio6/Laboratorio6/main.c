/*
 * Laboratorio6.c
 *
 * Created: 23/04/2025 16:04:13
 * Author : samur
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

void setup(void);         // Declarar antes de usar
void initUAT(void);
void writeChar(char);
void writeString(char* texto);
int main(void)
{
	setup();
	//writeChar('H');
	writeString("Muchas gracias");
	while (1)
	{
	}
}

void setup(){
	cli();
	DDRB = 0XFF;
	//Se decide el Baud rate
	//9600
	//Modo asincrono
	//En doble speed con prescaler 1
	//UBRR en 103
	initUAT();
	sei();
}

// Inicialización del módulo USART
void initUAT() {
	DDRD |= (1 << DDD1);   // PD1 (TX) como salida
	DDRD &= ~(1 << DDD0);  // PD0 (RX) como entrada

	UCSR0A = 0;            // Modo normal
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0); // Habilita interrupciones, TX y RX
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 bits de datos, 1 bit de parada

	UBRR0 = 103;           // Baud rate 9600 F_CPU = 16MHz
}

// Envía un carácter por USART
void writeChar(char caracter) {
	while (!(UCSR0A & (1 << UDRE0))); // Espera hasta que el buffer TX esté listo
	UDR0 = caracter;                   // Escribe el carácter en el buffer
}

void writeString(char* texto){
	for (uint8_t i=0; *(texto+i) != '\0'; i++){
		writeChar(*(texto+i));
	}
}

// Interrupción de recepción USART
ISR(USART_RX_vect) {
	char recibido = UDR0;  // Lee el carácter recibido
	writeChar(recibido);//PORTB = recibido;      // Muestra el carácter en el Puerto B
}

