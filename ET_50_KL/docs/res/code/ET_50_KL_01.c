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
