/*	LCD_HD44780 Einfache Bibliothek fuer das
        Textdisplay HD44780 von Hitachi
*/

#define PINOUT 4 // 4 or 8 bit mode

#define LCD_DB0 PB2
#define LCD_DB1 PB1
#define LCD_DB2 PB0
#define LCD_DB3 PD7
#define LCD_DB4 PD6
#define LCD_DB5 PD5
#define LCD_DB6 PD4
#define LCD_DB7 PD3
#define LCD_RS PB5
#define LCD_Enable PB3
#define LCD_rw PB4

#define LCD_PORT_DB0 PORTB
#define LCD_PORT_DB1 PORTB
#define LCD_PORT_DB2 PORTB
#define LCD_PORT_DB3 PORTD
#define LCD_PORT_DB4 PORTD
#define LCD_PORT_DB5 PORTD
#define LCD_PORT_DB6 PORTD
#define LCD_PORT_DB7 PORTD
#define LCD_PORT_RS PORTB
#define LCD_PORT_Enable PORTB
#define LCD_PORT_rw PORTB

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

void lcd_set_data(uint8_t data) { // Daten auf die PortPins schrieben
  // Alle betroffenen Pins zurücksetzen und dann setzen
  LCD_PORT_DB0 = LCD_PORT_DB0 & ~(1 << LCD_DB0) | ((data >> 0) & 1) << LCD_DB0;
  LCD_PORT_DB1 = LCD_PORT_DB1 & ~(1 << LCD_DB1) | ((data >> 1) & 1) << LCD_DB1;
  LCD_PORT_DB2 = LCD_PORT_DB2 & ~(1 << LCD_DB2) | ((data >> 2) & 1) << LCD_DB2;
  LCD_PORT_DB3 = LCD_PORT_DB3 & ~(1 << LCD_DB3) | ((data >> 3) & 1) << LCD_DB3;
  LCD_PORT_DB4 = LCD_PORT_DB4 & ~(1 << LCD_DB4) | ((data >> 4) & 1) << LCD_DB4;
  LCD_PORT_DB5 = LCD_PORT_DB5 & ~(1 << LCD_DB5) | ((data >> 5) & 1) << LCD_DB5;
  LCD_PORT_DB6 = LCD_PORT_DB6 & ~(1 << LCD_DB6) | ((data >> 6) & 1) << LCD_DB6;
  LCD_PORT_DB7 = LCD_PORT_DB7 & ~(1 << LCD_DB7) | ((data >> 7) & 1) << LCD_DB7;
}

void lcd_enable(void) {
  // Daten/Befehl uebernehmen mit Enable
  LCD_PORT_Enable |= (1 << LCD_Enable);
  _delay_ms(10); // Setze PB3 auf High und lass alle anderen Pins wie zuvor
  LCD_PORT_Enable &= ~(1 << LCD_Enable);
  _delay_ms(10); // Setze PB3 auf High und lass alle anderen Pins wie zuvor
}

void lcd_putc(uint8_t daten) {
  LCD_PORT_RS |= (1 << LCD_RS); // Daten dafuer muss das RS-Bit auf High liegen

  lcd_set_data(daten & 0xF0); // Daten auf den LCD-PORT legen
  _delay_ms(10);
  lcd_enable();

  _delay_ms(10);

  lcd_set_data(daten << 4);
  _delay_ms(10);
  lcd_enable();
}

void lcd_befehl(uint8_t befehl) {
#if PINOUT == 8
  portAnpassung(befehl);         // 0b00000001   => LCD-Clear
  LCD_PORT_RS &= ~(1 << LCD_RS); // Befehl
  _delay_ms(1);
  lcd_enable();
#elif PINOUT == 4
  LCD_PORT_RS &= ~(1 << LCD_RS); // Befehl

  // Higher nibble
  lcd_set_data(befehl & 0xF0); // 0b00000001   => LCD-Clear
  _delay_ms(10);
  lcd_enable();

  _delay_ms(10);

  // Lower nibble
  lcd_set_data(befehl << 4); // 0b00000001   => LCD-Clear
  _delay_ms(10);
  lcd_enable();
#endif
}

void lcd_clear() {  // Displayinhalt loeschen => lcdclear
  lcd_befehl(0x01); // 0b00000001   => LCD-Clear
}

void lcd_init() { // LCD 8bit Initialisierung
  _delay_ms(40);  // Wartezeit bis das LCD betriebsbereit ist
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
