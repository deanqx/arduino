/*
 * '\xF5' == 'ü'
 */

#define INFO "TI_80_AVRStudio_07_Menue\r\n"
#define INFO_LCD_1 "TI_80_AVRStudio_"
#define INFO_LCD_2 "07_Menue"
#define BAUD 19200UL // Baudrate Arduino
#define P_UP PC0
#define P_SELECT PC1
#define P_DOWN PC2

#include "LCD_HD44780.h"
#include "uartAT328p.h" // Einbinden der UART-Bibliothek,
#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>
// diese muss sich in dem Verzeichnis der main.c befinden.

#define MENU_COUNT 4
#define MENU_WIDTH 17

// clang-format off
char menu[MENU_COUNT][MENU_WIDTH] = {
    "Willkommen      ",
    "Men\xF5 1          ",
    "Men\xF5 2          ",
    "Men\xF5 3          "
};
// clang-format on

int8_t current_menu = 0;
bool is_selected = 0;

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
    if (is_selected) {
      if (!(PINC >> P_SELECT & 1)) {
        is_selected = false;
        draw_menu();
        _delay_ms(200); // Wartezeit damit Taster losgelassen werden kann
      }

      continue;
    }

    if (!(PINC >> P_UP & 1)) {
      if (current_menu == 0) {
        continue;
      }

      current_menu--;
      draw_menu();
      _delay_ms(200); // Wartezeit damit Taster losgelassen werden kann
    } else if (!(PINC >> P_DOWN & 1)) {
      if (current_menu == MENU_COUNT - 1) {
        continue;
      }

      current_menu++;
      draw_menu();
      _delay_ms(200); // Wartezeit damit Taster losgelassen werden kann
    } else if (!(PINC >> P_SELECT & 1)) {
      is_selected = true;

      usart_puts("Menü ");
      usart_itoa(current_menu);
      usart_puts(" wurde ausgewählt.\r\n");

      lcd_clear();

      switch (current_menu) {
      case 0:
        usart_puts(INFO);

        lcd_puts(INFO_LCD_1);
        lcd_gotoxy(0, 1);
        lcd_puts(INFO_LCD_2);
        break;
      case 1:
        lcd_puts("Men\xF5punkt 1");
        break;
      case 2:
        lcd_puts("Men\xF5punkt 2");
        break;
      case 3:
        lcd_puts("Men\xF5punkt 3");
        break;
      }

      _delay_ms(200); // Wartezeit damit Taster losgelassen werden kann
    }
  }
}
