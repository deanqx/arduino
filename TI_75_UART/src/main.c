/*
 * TI_74_AVRStudio_02.c
 * LED Test PB5 + uart Ausgabe
 *
 * Created: 11.06.2025 08:37:34
 * Author : JCSBK-User
 */

#define INFO "TI_74_AVRStudio_02\r\n"
#define BAUD 19200UL // Baudrate Arduino

#include "uartAT328p.h" // Einbinden der UART-Bibliothek,
#include <avr/io.h>
#include <util/delay.h>
// diese muss sich in dem Verzeichnis der main.c befinden.

int main(void) {
  DDRB = 0b00100000; // PB5 als Ausgang und alle anderen als Eingang in bin?r
                     // geht nicht beim 8051
  DDRB = 0x20;       // PB5 als Ausgang und alle anderen als Eingang in hex
  usart_init();      // UART-Schnittstelle aktivieren PD0 = RX und PD1 = TX
  _delay_ms(500);
  usart_puts(INFO);

  while (1) {
    usart_puts("Bitte ein Zeichen eingeben:\r\n");

    const char c = usart_getc();
    usart_putc(c);

    PORTB |= (1 << PB5); // PB5 auf high setzen
    usart_puts("LED 13 PB5 an\r\n");
    _delay_ms(5000);
    PORTB &= ~(1 << PB5); // PB5 auf low setzen
    usart_puts("LED 13 PB5 aus\r\n");
    _delay_ms(500);
  }

loop:
  PORTB ^= (1 << PB5); // PB5 auf high setzen
  _delay_ms(100);
  goto loop;
}
