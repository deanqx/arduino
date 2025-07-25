#include <avr/io.h>
#include <util/delay.h>

int main(void) {
  DDRB = 1 << PB5;

  while (1) {
    PORTB |= 1 << PB5;

    _delay_ms(1000.0);

    PORTB &= ~(1 << PB5);

    _delay_ms(1000.0);
  }
}
