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
            case '1': {
                ADCSRA |= (1 << ADSC);         // Iniciar conversión
                while (ADCSRA & (1 << ADSC));  // Esperar fin de conversión
                uint16_t adc_value = ADC;      // Leer valor
                
                char buffer[10];
                itoa(adc_value, buffer, 10);   // Convertir a cadena
                writeString("Valor del potenciometro: ");
                writeString(buffer);
                writeString("\r\n");
                break;
            }
            
            case '2': {
                writeString("\r\nIngrese un caracter: ");
                comando = 0;  // Reiniciar para capturar nuevo carácter
                while (comando == 0);  // Esperar nuevo carácter
                
                PORTB = comando;  // Mostrar ASCII en LEDs
                writeString("\r\nCaracter recibido: ");
                writeChar(comando);
                writeString("\r\n");
                break;
            }
        }
        
        comando = 0;  // Resetear comando para nueva selección
    }
}

// Configuración inicial
void setup(void) {
    cli();
    DDRB = 0xFF;        // Puerto B como salida
    // Configurar ADC
    ADMUX = (1 << REFS0); // AVcc como referencia, ADC0
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // ADC enable, prescaler 128
    initUAT();          // Inicializar USART
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

