/*
        RS232 Bibliothek von MS 07.06.19
        fuer die ATMegareihe.
        ATMega328P
*/

#include <avr/io.h>
#include <stdio.h>

// #define BAUD 115200UL          // Baudrate
#ifndef BAUD
#error BAUD not defined
#define BAUD 19200UL // Baudrate Arduino
#endif

// Berechnungen
#define UBRR_VAL ((F_CPU + BAUD * 8) / (BAUD * 16) - 1) // clever runden
#define BAUD_REAL (F_CPU / (16 * (UBRR_VAL + 1)))       // Reale Baudrate
#define BAUD_ERROR                                                             \
  ((BAUD_REAL * 1000) / BAUD) // Fehler in Promille, 1000 = kein Fehler.

#if ((BAUD_ERROR < 960) || (BAUD_ERROR > 1040))
#error Systematischer Fehler der Baudrate groesser 4% und damit zu hoch!
#endif

void usart_init(void) { // set baud rate
  UBRR0H = (uint8_t)(UBRR_VAL >> 8);
  UBRR0L = (uint8_t)(UBRR_VAL);

  //	UBRRH = (uint8_t)(UART_BAUD_CALC(UART_BAUD_RATE,F_OSC)>>8);
  //	UBRRL = (uint8_t)UART_BAUD_CALC(UART_BAUD_RATE,F_OSC);
  // Enable receiver and transmitter; enable RX interrupt
  UCSR0B = (1 << RXEN0) | (1 << TXEN0); //  | (1 << RXCIE);
  // asynchronous 8N1
  UCSR0C = (1 << USBS0) | (3 << UCSZ00);
}

/* Zusaetzlich zur Baudrateneinstellung und der weiteren Initialisierung: */
void usart_EnableRX(void) { UCSR0B |= (1 << RXEN0); }

/* Zeichen empfangen */
uint8_t usart_getc(void) {
  while (!(UCSR0A & (1 << RXC0)))
    ;          // warten bis Zeichen verfuegbar
  return UDR0; // Zeichen aus UDR an Aufrufer zurueckgeben
}

uint8_t usart_getc_free(void) {
  if ((UCSR0A & (1 << RXC0)))
    return UDR0; // warten bis Zeichen verfuegbar
  return 0;      // Zeichen aus UDR an Aufrufer zurueckgeben
}

uint8_t kbhit(void) {
  if ((UCSR0A & (1 << RXC0)))
    return 1;
  return 0;
}

void usart_putc(uint8_t c) { // wait until UDR ready
  while (!(UCSR0A & (1 << UDRE0)))
    ;
  UDR0 = c; // send character
}

/*  Pointer in C
    Pointer ist ein Zeiger auf eine Speicherstelle im MC, diese Speicherstelle
   hat je nach MC-Typ eine feste Größe  z.B. 16Bit => Adr  00000000 00000000 -
   11111111 11111111 0                 - 65535 0x0000            - 0xFFFF char
   *s   d.h. Es wird ein Pointer übergeben und die Anzahl der folgenden Byte ist
                   die Byteanzahl des vorangestellten Datentyps in diesem Fall
   char = 1 Byte Beispiel:

    char-Array (String) => H a l l o \0
    'H' = adr 1000, 'a' = adr 1001, 'l' = adr 1002, 'l' = adr 1003, 'o' = adr
   1004,
    '\0' = adr 1005  oder auch  0 = adr 1005

    *s entspricht dem Inhalt an der Adresse wenn kein Datentyp davor steht
    &s gibt die Adresse der Speicherzelle aus: in diesem Fall für 'a' entspricht 
   001 s++ bedeutet das der Adresspointer um eins erhöht wird: in diesem Fall 
   on 1000 auf 1001
*/
void usart_puts(char *s) // Pointer vom Typ char => jede Adresse hat 1 Datenbyte
{                        //  loop until *s != NULL
  while (*s)             //
  {
    usart_putc(*s);
    s++; // spring auf die nächste Adresse in diesem Fall um 1 Byte weiter, da 
         // er Datentyp
         // char ist und aus einem Byte besteht
  }
}

void usart_itoa(int a) {
  char i[7];
  sprintf(i, "%i", a);
  usart_puts(i);
}

void usart_uint16toa(uint16_t a) {
  char i[6];
  sprintf(i, "%u", a);
  usart_puts(i);
}

void usart_ftoa(float a) {
  char i[20];
  sprintf(i, "%.2f", a);
  usart_puts(i);
}

void usart_dtoa(double a) {
  char i[20];
  sprintf(i, "%.2f", a);
  usart_puts(i);
}
