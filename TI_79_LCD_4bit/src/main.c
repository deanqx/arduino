/*
 * TI_74_AVRStudio_02.c
 * LED Test PB5 + uart Ausgabe
 *
 * Created: 11.06.2025 08:37:34
 * Author : JCSBK-User
 */

#define INFO "TI_74_AVRStudio_02\r\n"
#define BAUD 19200UL // Baudrate Arduino
#define P_UP PC0
#define P_DOWN PC1
#define P_SELECT PC2

#include "LCD_HD44780.h"
#include "uartAT328p.h" // Einbinden der UART-Bibliothek,
#include <avr/io.h>
#include <util/delay.h>
// diese muss sich in dem Verzeichnis der main.c befinden.

#define MENU_COUNT 3
#define MENU_WIDTH 17

// clang-format off
char menu[MENU_COUNT][MENU_WIDTH] = {
    "Willkommen      ",
    "1.              ",
    "2.              "
};
// clang-format on
int8_t current_menu = 0;

void draw_menu() {
  usart_puts("----");
  lcd_home();

  usart_puts(menu[current_menu]);
  usart_puts("\r\n");

  lcd_puts(menu[current_menu]);
  lcd_gotoxy(0, 1);

  if (current_menu + 1 < MENU_COUNT) {
    usart_puts(menu[current_menu + 1]);
    usart_puts("\r\n");

    lcd_puts(menu[current_menu + 1]);
  } else {
    usart_puts("\r\n");
    lcd_puts("                ");
  }

  lcd_gotoxy(15, 0);
}

int main(void) {
  // Pinbelegung für lcd
  DDRB = 0x3F;
  DDRD = 0xF8;

  // Enable Pull-Up resistors for buttons
  PORTC |= 1 << P_UP | 1 << P_DOWN | 1 << P_SELECT;

  usart_init(); // UART-Schnittstelle aktivieren PD0 = RX und PD1 = TX
  usart_puts(INFO);

  lcd_init();
  draw_menu();

  while (1) {
    // 1: down; -1: up
    int8_t move_menu = 0;

    if (!(PINC & (1 << P_UP))) {
      move_menu = -1;
    } else if (!(PINC & (1 << P_DOWN))) {
      move_menu = 1;
    } else if (!(PINC & (1 << P_SELECT))) {
    }

    if (move_menu != 0) {
      const int8_t new_menu = current_menu + move_menu;

      if (0 <= new_menu && new_menu < MENU_COUNT) {
        current_menu = new_menu;
        draw_menu();
      }
      _delay_ms(200);
    }
  }

loop:
  PORTB ^= (1 << PB5);
  _delay_ms(100);
  goto loop;
}
