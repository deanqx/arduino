/*
 * TI_74_AVRStudio_02.c
 * LED Test PB5 + uart Ausgabe
 *
 * Created: 11.06.2025 08:37:34
 * Author : JCSBK-User
 */

#define INFO "TI_74_AVRStudio_02\r\n"
#define BAUD 19200UL // Baudrate Arduino

#include "LCD_HD44780.h"
#include "uartAT328p.h" // Einbinden der UART-Bibliothek,
#include <avr/io.h>
#include <util/delay.h>
// diese muss sich in dem Verzeichnis der main.c befinden.

int main(void) {
  // Pinbelegung für lcd
  DDRB = 0x3F;
  DDRD = 0xF8;

  usart_init(); // UART-Schnittstelle aktivieren PD0 = RX und PD1 = TX
  lcd_init();
  while (1) {
    lcd_init();
    _delay_ms(2000);
  }

  lcd_home();
  lcd_puts("test");
  usart_puts(INFO);

  while (1) {
  }

loop:
  PORTB ^= (1 << PB5);
  _delay_ms(100);
  goto loop;
}
