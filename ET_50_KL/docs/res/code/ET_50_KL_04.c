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
