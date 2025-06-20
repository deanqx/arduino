#define INFO "ET_50_KL_7"
#define BAUD 19200UL
#define LCD_DELAY_MS 1

#include "LCD_HD44780.h"
#include "uartAT328p.h"
#include <stdbool.h>

// Relais Zustand
bool q1_rechts = 0;
bool q2_links = 0;

// Datenbits des Bus setzen. 0x11 => DB4=1; DB0=1
void set_data_bits(uint8_t data) {
  // Alle betroffenen Pins zurücksetzen und dann setzen
  // clang-format off
  PORTB = PORTB & ~(1 << lcd_DB0 | 1 << lcd_DB1 | 1 << lcd_DB2) 
    | ((data >> 0) & 1) << lcd_DB0 
    | ((data >> 1) & 1) << lcd_DB1
    | ((data >> 2) & 1) << lcd_DB2;

  PORTD = PORTD & ~(1 << lcd_DB3 | 1 << lcd_DB4 | 1 << lcd_DB5
      | 1 << lcd_DB6 | 1 << lcd_DB7)
    | ((data >> 3) & 1) << lcd_DB3
    | ((data >> 4) & 1) << lcd_DB4
    | ((data >> 5) & 1) << lcd_DB5
    | ((data >> 6) & 1) << lcd_DB6
    | ((data >> 7) & 1) << lcd_DB7;
  // clang-format on
}

// LCD Befehl senden
void command_lcd(uint8_t data) {
  // Informationen bezüglich des HD44780 wurden dem Datenblatt entnommen:
  // https://cdn.sparkfun.com/assets/9/5/f/7/b/HD44780.pdf

  // Instructions (S. 24)
  // Setze register select (RS) auf low zur Befehlannahme
  PORTB &= ~(1 << lcd_RS | 1 << lcd_rw);

  set_data_bits(data);
  _delay_ms(LCD_DELAY_MS);

  // Enable setzen
  PORTB |= 1 << lcd_Enable;
  _delay_ms(LCD_DELAY_MS);

  // Enable ausschalten
  PORTB &= ~(1 << lcd_Enable);
}

void initLCD() {
  // Informationen bezüglich des HD44780 wurden dem Datenblatt entnommen:
  // https://cdn.sparkfun.com/assets/9/5/f/7/b/HD44780.pdf

  // Anleitung befindet sich unter "Initializing by Instruction" (S. 45)

  // init delay after powerup
  _delay_ms(50);

  // Instruction Description (S. 25)
  // Sets interface data length (DL = 1 for 8 bits),
  // number of display lines (N = 1 for 2 lines),
  // and character font (F = 0 for 5x8 dots).
  const uint8_t function_set = 1 << 5 | 1 << 4 | 1 << 3;

  // 3x Function Set
  command_lcd(function_set);
  _delay_ms(6);
  command_lcd(function_set);
  _delay_ms(1);
  command_lcd(function_set);

  // Setup
  command_lcd(function_set);

  // Sets entire display (D = 1) on/off,
  // cursor on/off (C = 0),
  // and blinking of cursor position character (B = 0).
  command_lcd(1 << 3 | 1 << 2);

  // Clear display
  command_lcd(0x01);

  // Entry mode
  // Description: Sets cursor move direction and specifies display shift.
  // These operations are performed during data write and read.

  // Cursor automatisch weiterschieben: an
  command_lcd(1 << 2 | 1 << 1);
}

// Zeigt Zahl auf 7 Segment-Anzeige über 74HC573 (Register)
// @param led_states Binary layout: 0b{a,b,c,d,e,f,g,dp}
void set_7_segment_display(uint8_t led_states) {
  set_data_bits(led_states);
  _delay_ms(LCD_DELAY_MS);

  // Enable setzen
  PORTD |= 1 << PD2;
  _delay_ms(LCD_DELAY_MS);

  // Enable ausschalten
  PORTD &= ~(1 << PD2);
}

int main(void) {
  // GPIO Ausgänge setzen
  DDRB = 0x3F;
  DDRC = 0x18;
  DDRD = 0xFC;

  // UART initialisieren
  usart_init();

  // Version über UART senden
  usart_puts(INFO);
  usart_puts("\r\n");

  // LCD initialisieren
  initLCD();

  // Version anzeigen
  lcd_puts(INFO);

  while (1) {
    // Port Eingänge in Variablen speichern
    // Taster S0 bis S2 (1=gedrückt)
    const bool s0_aus = !((PINC >> PIN_S0_AUS) & 0x01);
    const bool s1_rechts = !((PINC >> PIN_S1_RECHTS) & 0x01);
    const bool s2_links = !((PINC >> PIN_S2_LINKS) & 0x01);

    /*
     * Motorsteuern über UART 
     * '0': ausschalten
     * 'r': Rechtslauf
     * 'l': Linkslauf
     */
    char uart_command = 0;

    // Liegt eine Eingabe am UART vor?
    if (kbhit()) {
      uart_command = usart_getc_free();
      usart_puts("UART Befehl erhalten: ");
      usart_putc(uart_command);
      usart_puts("\r\n");
    }

    if (uart_command == '0' || s0_aus) {
      // Aus
      q1_rechts = 0;
      q2_links = 0;
    } else if (uart_command == 'r' || s1_rechts) {
      // Rechtsrum
      q1_rechts = 1;
      q2_links = 0;
    } else if (uart_command == 'l' || s2_links) {
      // Linksrum
      q1_rechts = 0;
      q2_links = 1;
    }

    if (q1_rechts) {
      lcd_gotoxy(0, 1);
      lcd_puts("Rechtslauf      ");

      // 'r' => 0000 1010 => 0x0A
      set_7_segment_display(0x3A);
    } else if (q2_links) {
      lcd_gotoxy(0, 1);
      lcd_puts("Linkslauf       ");

      // 'l' => 0001 1100 => 0x1C
      set_7_segment_display(0x1C);
    } else {
      lcd_gotoxy(0, 1);
      lcd_puts("Ausgeschalten   ");

      // 'o' => 0011 1010 => 0x3A
      set_7_segment_display(0x3A);
    }

    if (uart_command == 'i') {
      // Taster Zustände senden
      usart_puts("S0=");
      usart_itoa(s0_aus);
      usart_puts("; S1=");
      usart_itoa(s1_rechts);
      usart_puts("; S2=");
      usart_itoa(s2_links);
      usart_puts("\r\n");

      usart_puts("Motorzustand: ");

      if (q1_rechts) {
        usart_putc('R');
      } else if (q2_links) {
        usart_putc('L');
      } else {
        usart_putc('0');
      }

      usart_puts("\r\n");
    }

    // Alle Relais Pins (PC3 und PC4) werden geleert und dann gesetzt
    PORTC = PORTC & ~(1 << PIN_Q1_RECHTS | 1 << PIN_Q2_LINKS) |
            q1_rechts << PIN_Q1_RECHTS | q2_links << PIN_Q2_LINKS;
  }
}
