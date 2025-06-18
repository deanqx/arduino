/*
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

#define EXERCISE 9

#if EXERCISE == 8

#define INFO "ET_50_KL_1\r\n"
#define BAUD 19200UL

#include "uartAT328p.h"
#include <stdbool.h>

// Taster S0 bis S2
bool s0, s1, s2;

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
  s0 = !((PORTC >> PB0) & 0x01);
  s1 = !((PORTC >> PB1) & 0x01);
  s2 = !((PORTC >> PB2) & 0x01);

  // Taster Zustände senden
  usart_puts("S0=");
  usart_itoa(s0);
  usart_puts("; S1=");
  usart_itoa(s1);
  usart_puts("; S2=");
  usart_itoa(s2);
  usart_puts("\r\n");
}

#elif EXERCISE == 9

#define INFO "ET_50_KL_2\r\n"
#define BAUD 19200UL

#include "uartAT328p.h"
#include <stdbool.h>

// Taster S0 bis S2
bool s0, s1, s2;
uint8_t port_b;
uint8_t port_c;
uint8_t port_d;

int main(void) {
  // GPIO Ausgänge setzen
  DDRB = 0x3F;
  DDRC = 0x18;
  DDRD = 0xFC;

  // UART initialisieren
  usart_init();

  // Version über UART senden
  usart_puts(INFO);

  while (1)
  {
    port_b = PORTB;
    port_c = PORTC;
    port_d = PORTD;

    // Port Eingänge in Variablen speichern
    s0 = !((port_c >> PB0) & 0x01);
    s1 = !((port_c >> PB1) & 0x01);
    s2 = !((port_c >> PB2) & 0x01);

    // Taster Zustände senden
    usart_puts("S0=");
    usart_itoa(s0);
    usart_puts("; S1=");
    usart_itoa(s1);
    usart_puts("; S2=");
    usart_itoa(s2);
    usart_puts("\r\n");

    if (s0)
    {
        // Rechts- und Linkslauf ausschalten
        port_c &= ~(1 << PB3 | 1 << PB4);
    }
    else if (s1)
    {
        // Erst beide richtung ausschalten
        port_c &= ~(1 << PB3 | 1 << PB4);
        // Dann Rechtslauf aktivieren
        port_c |= 1 << PB3;
    }
    else if (s2)
    {
        // Erst beide richtung ausschalten
        port_c &= ~(1 << PB3 | 1 << PB4);
        // Dann Rechtslauf aktivieren
        port_c |= 1 << PB4;
    }

    PORTB = port_b;
    PORTC = port_c;
    PORTD = port_d;
  }
}

#endif
