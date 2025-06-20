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
