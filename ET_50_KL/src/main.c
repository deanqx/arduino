/*
 * Warnung: Boolische True-Werte müssen den Wert 1 haben
 *
 * Zuweisungsliste:
 *
 * Arduino | MCU Pin | Bauteil Pin | Bezeichnung
 * 5V        Vcc
 * GND       Vss
 * 13        PB5       RS            LCD
 * 12        PB4       R/~W          LCD
 * 11        PB3       E             LCD
 * 10        PB2       D0            LCD
 * 9         PB1       D1            LCD
 * 8         PB0       D2            LCD
 * 7         PD7       D3            LCD
 * 6         PD6       D4            LCD
 * 5         PD5       D5            LCD
 * 4         PD4       D6            LCD
 * 3         PD3       D7            LCD
 * 2         PD2       CLK           Datenübernehmen für 7 segment display
 * A0        PC0       S0            Taster in Negativlogik
 * A1        PC1       S1            Taster in Negativlogik
 * A2        PC2       S2            Taster in Negativlogik
 * A3        PC3       Q1            Relais Rechts
 * A4        PC4       Q2            Relais Links
 * A5        PC5
 * */

#define PIN_S0_AUS PC0
#define PIN_S1_RECHTS PC1
#define PIN_S2_LINKS PC2
#define PIN_Q1_RECHTS PC3
#define PIN_Q2_LINKS PC4

#define EXERCISE 11

#if EXERCISE == 8

#define INFO "ET_50_KL_1\r\n"
#define BAUD 19200UL

#include "uartAT328p.h"
#include <stdbool.h>

int main(void) {
  // GPIO Ausgänge setzen
  DDRB = 0x3F;
  DDRC = 0x18;
  DDRD = 0xFC;

  // UART initialisieren
  usart_init();

  // Version über UART senden
  usart_puts(INFO);

  // Port Eingänge in Variablen speichern
  // Taster S0 bis S2
  bool s0_aus = !((PINC >> PIN_S0_AUS) & 0x01);
  bool s1_rechts = !((PINC >> PIN_S1_RECHTS) & 0x01);
  bool s2_links = !((PINC >> PIN_S2_LINKS) & 0x01);

  // Taster Zustände senden
  usart_puts("S0=");
  usart_itoa(s0_aus);
  usart_puts("; S1=");
  usart_itoa(s1_rechts);
  usart_puts("; S2=");
  usart_itoa(s2_links);
  usart_puts("\r\n");
}

#elif EXERCISE == 9

#define INFO "ET_50_KL_2\r\n"
#define BAUD 19200UL

#include "uartAT328p.h"
#include <stdbool.h>

// Relais Zustand
bool q1_rechts = 0;
bool q2_links = 0;

int main(void) {
  // GPIO Ausgänge setzen
  DDRB = 0x3F;
  DDRC = 0x18;
  DDRD = 0xFC;

  // UART initialisieren
  usart_init();

  // Version über UART senden
  usart_puts(INFO);

  while (1) {
    // Port Eingänge in Variablen speichern
    // Taster S0 bis S2 (1=gedrückt)
    bool s0_aus = !((PINC >> PIN_S0_AUS) & 0x01);
    bool s1_rechts = !((PINC >> PIN_S1_RECHTS) & 0x01);
    bool s2_links = !((PINC >> PIN_S2_LINKS) & 0x01);

    if (s0_aus) {
      // Aus
      q1_rechts = 0;
      q2_links = 0;
    } else if (s1_rechts) {
      // Rechtsrum
      q1_rechts = 1;
      q2_links = 0;
    } else if (s2_links) {
      // Linksrum
      q1_rechts = 0;
      q2_links = 1;
    }

    // Taster Zustände senden
    usart_puts("S0=");
    usart_itoa(s0_aus);
    usart_puts("; S1=");
    usart_itoa(s1_rechts);
    usart_puts("; S2=");
    usart_itoa(s2_links);
    usart_puts("\r\n");

    if (q1_rechts) {
      usart_puts("Motor dreht rechtsrum\r\n");
    } else if (q2_links) {
      usart_puts("Motor dreht linksrum\r\n");
    }

    // Alle Relais Pins (PC3 und PC4) werden geleert und dann gesetzt
    PORTC = PORTC & ~(1 << PIN_Q1_RECHTS | 1 << PIN_Q2_LINKS) |
            q1_rechts << PIN_Q1_RECHTS | q2_links << PIN_Q2_LINKS;
  }
}

#elif EXERCISE == 10

#define INFO "ET_50_KL_3\r\n"
#define BAUD 19200UL

#include "uartAT328p.h"
#include <stdbool.h>

// Relais Zustand
bool q1_rechts = 0;
bool q2_links = 0;

int main(void) {
  // GPIO Ausgänge setzen
  DDRB = 0x3F;
  DDRC = 0x18;
  DDRD = 0xFC;

  // UART initialisieren
  usart_init();

  // Version über UART senden
  usart_puts(INFO);

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

    // Taster Zustände senden
    usart_puts("S0=");
    usart_itoa(s0_aus);
    usart_puts("; S1=");
    usart_itoa(s1_rechts);
    usart_puts("; S2=");
    usart_itoa(s2_links);
    usart_puts("\r\n");

    if (q1_rechts) {
      usart_puts("Motor dreht rechtsrum\r\n");
    } else if (q2_links) {
      usart_puts("Motor dreht linksrum\r\n");
    }

    // Alle Relais Pins (PC3 und PC4) werden geleert und dann gesetzt
    PORTC = PORTC & ~(1 << PIN_Q1_RECHTS | 1 << PIN_Q2_LINKS) |
            q1_rechts << PIN_Q1_RECHTS | q2_links << PIN_Q2_LINKS;
  }
}

#elif EXERCISE == 11

#define INFO "ET_50_KL_4\r\n"
#define BAUD 19200UL

#include "uartAT328p.h"
#include <stdbool.h>

// Relais Zustand
bool q1_rechts = 0;
bool q2_links = 0;

int main(void) {
  // GPIO Ausgänge setzen
  DDRB = 0x3F;
  DDRC = 0x18;
  DDRD = 0xFC;

  // UART initialisieren
  usart_init();

  // Version über UART senden
  usart_puts(INFO);

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

#elif EXERCISE == 12

#define INFO "ET_50_KL_5\r\n"
#define BAUD 19200UL

#include "uartAT328p.h"
#include <stdbool.h>

// Relais Zustand
bool q1_rechts = 0;
bool q2_links = 0;

void initLCD() {
    // Informationen bezüglich des HD44780 wurden dem Datenblatt entnommen:
    // https://cdn.sparkfun.com/assets/9/5/f/7/b/HD44780.pdf
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

#endif
