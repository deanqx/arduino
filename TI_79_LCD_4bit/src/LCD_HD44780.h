/*	LCD_HD44780 Einfache Bibliothek fuer das
        Textdisplay HD44780 von Hitachi
*/

#define PINOUT 4 // 4 or 8 bit mode

#define lcd_DB0 PB2
#define lcd_DB1 PB1
#define lcd_DB2 PB0
#define lcd_DB3 PD7
#define lcd_DB4 PD6
#define lcd_DB5 PD5
#define lcd_DB6 PD4
#define lcd_DB7 PD3
#define lcd_RS PB5
#define lcd_Enable PB3
#define lcd_rw PB4

#define init_ms1 30
#define init_ms2 10

#define bit0 0
#define bit1 1
#define bit2 2
#define bit3 3
#define bit4 4
#define bit5 5
#define bit6 6
#define bit7 7

// Set DD RAM Address --------- 0b1xxxxxxx  (Display Data RAM)
#define LCD_SET_DDADR 0x80
// Zeilendefinitionen des verwendeten LCD
// Die Einträge hier sollten für ein LCD mit einer Zeilenlänge von 16 Zeichen pa
// sen Bei anderen Zeilenlängen müssen diese Einträge angepasst werden

#define LCD_DDADR_LINE1 0x00
#define LCD_DDADR_LINE2 0x40
#define LCD_DDADR_LINE3 0x10
#define LCD_DDADR_LINE4 0x50

#define LCD_Cursor_OFF 0
#define LCD_Cursor_ON 1
#define LCD_Cursor_ON_Blink 0x0F

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

void portAnpassung(uint8_t daten) { // Daten auf die PortPins schrieben
  // DB7	DB6	DB5	DB4	DB3	DB2	DB1	DB0
  // PD3	PD4	PD5	PD6	PD7	PB0	PB1	PB2
  if (daten & (1 << bit0)) {
    PORTB |= (1 << lcd_DB0);
  } else {
    PORTB &= ~(1 << lcd_DB0);
  }

  if (daten & (1 << bit1)) {
    PORTB |= (1 << lcd_DB1);
  } else {
    PORTB &= ~(1 << lcd_DB1);
  }

  if (daten & (1 << bit2)) {
    PORTB |= (1 << lcd_DB2);
  } else {
    PORTB &= ~(1 << lcd_DB2);
  }

  if (daten & (1 << bit3)) {
    PORTD |= (1 << lcd_DB3);
  } else {
    PORTD &= ~(1 << lcd_DB3);
  }

  if (daten & (1 << bit4)) {
    PORTD |= (1 << lcd_DB4);
  } else {
    PORTD &= ~(1 << lcd_DB4);
  }

  if (daten & (1 << bit5)) {
    PORTD |= (1 << lcd_DB5);
  } else {
    PORTD &= ~(1 << lcd_DB5);
  }

  if (daten & (1 << bit6)) {
    PORTD |= (1 << lcd_DB6);
  } else {
    PORTD &= ~(1 << lcd_DB6);
  }

  if (daten & (1 << bit7)) {
    PORTD |= (1 << lcd_DB7);
  } else {
    PORTD &= ~(1 << lcd_DB7);
  }
}

void lcd_enable(void) {
  // Daten/Befehl uebernehmen mit Enable
  PORTB |= (1 << lcd_Enable);
  _delay_ms(1); // Setze PB3 auf High und lass alle anderen Pins wie zuvor
  PORTB &= ~(1 << lcd_Enable);
  _delay_ms(1); // Setze PB3 auf High und lass alle anderen Pins wie zuvor
}

void lcd_putc(uint8_t daten) {
  portAnpassung(daten);   // Daten auf den LCD-PORT legen
  PORTB |= (1 << lcd_RS); // Daten dafuer muss das RS-Bit auf High liegen
  _delay_ms(init_ms2);
  lcd_enable();
}

void lcd_befehl(uint8_t befehl) {
#if PINOUT == 8
  portAnpassung(befehl);   // 0b00000001   => LCD-Clear
  PORTB &= ~(1 << lcd_RS); // Befehl
  _delay_ms(init_ms2);
  lcd_enable();
#elif PINOUT == 4
  // Higher nibble
  portAnpassung(befehl & 0xF0); // 0b00000001   => LCD-Clear
  PORTB &= ~(1 << lcd_RS);      // Befehl
  _delay_ms(init_ms2);
  lcd_enable();

  // Lower nibble
  portAnpassung(befehl << 4); // 0b00000001   => LCD-Clear
  PORTB &= ~(1 << lcd_RS);    // Befehl
  _delay_ms(init_ms2);
  lcd_enable();
#endif
}

void lcd_clear() {  // Displayinhalt loeschen => lcdclear
  lcd_befehl(0x01); // 0b00000001   => LCD-Clear
}

void lcd_init() {      // LCD 8bit Initialisierung
  _delay_ms(40); // Wartezeit bis das LCD betriebsbereit ist
#if PINOUT == 8
  lcd_befehl(0x38); // 0b00111000;	8-Bitmodus 2-zeilig 5x7 font
  _delay_ms(5);
  lcd_befehl(0x38); // 0b00111000;	8-Bitmodus 2-zeilig 5x7 font
  _delay_ms(1);
  lcd_befehl(0x38); // 0b00111000;	8-Bitmodus 2-zeilig 5x7 font
#elif PINOUT == 4
  lcd_befehl(0x28); // 0b00111000;	8-Bitmodus 2-zeilig 5x7 font
  _delay_ms(5);
  lcd_befehl(0x28); // 0b00111000;	8-Bitmodus 2-zeilig 5x7 font
  _delay_ms(1);
  lcd_befehl(0x28); // 0b00111000;	8-Bitmodus 2-zeilig 5x7 font
#endif
  lcd_befehl(LCD_Cursor_ON_Blink); // 0b00001111	// Display an, Cursor
                                   // an, Cursor blink
  lcd_clear();                     // Displayinhalt loeschen => lcdclear
  lcd_befehl(0x06);                // 0b00000110	Cursor increment => cursor wird
                                   // automatisch nach rechts geschoben
}

void lcd_puts(char text[]) // char Array mit unbestimmter Groesse => es ist
                           // eigentlich ein Pointer
{
  for (int8_t i = 0; text[i] != 0; i++) {
    lcd_putc(text[i]);
  }
}

void lcd_gotoxy(uint8_t x, uint8_t y) { // x = Spalte , y = Zeile
                                        // x=0 erste Stelle y=0 erste Zeile
  uint8_t data = 0;
  switch (y) {
  case 0:
    data = LCD_SET_DDADR + LCD_DDADR_LINE1 + x;
    break; // 1. Zeile
  case 1:
    data = LCD_SET_DDADR + LCD_DDADR_LINE2 + x;
    break; // 2. Zeile
  case 2:
    data = LCD_SET_DDADR + LCD_DDADR_LINE3 + x;
    break; // 3. Zeile
  case 3:
    data = LCD_SET_DDADR + LCD_DDADR_LINE4 + x;
    break; // 4. Zeile
  }
  lcd_befehl(data);
}

void lcd_stringxy(uint8_t x, uint8_t y, char text[]) {
  lcd_gotoxy(x, y);
  lcd_puts(text);
}

void lcd_home() { lcd_befehl(0x02); }

void lcd_cursor(
    uint8_t on) { // 0 = Cursor aus, 1 = Cursor an, 2 = Cursor Blinken
  // Hex-Werte sind aus dem Datasheet des LCD
  switch (on) {
  case LCD_Cursor_OFF:
    lcd_befehl(0x08 + 0x04);
    break;
  case LCD_Cursor_ON:
    lcd_befehl(0x08 + 0x04 + 0x02);
    break;
  case LCD_Cursor_ON_Blink:
    lcd_befehl(0x08 + 0x04 + 0x02 + 0x01);
    break;
  default:
    lcd_befehl(0x08 + 0x04);
  }
}
