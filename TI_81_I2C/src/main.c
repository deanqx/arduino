#define INTERNAL_LED PB5
#define INT_I2C_LA PB5

// #include "i2c_soft_delay.h"
#include "i2c_soft.h"
#include <avr/io.h>

void error_handler(void) {
  cli();

  PORTB |= 1 << INTERNAL_LED;

  while (1)
    ;
}

int main(void) {
  DDRB |= 1 << INTERNAL_LED;

  i2c_init();
  sei();

  i2c_start();
  i2c_begin_tx(0xA6);

  i2c_wait();

  if (i2c_nack) {
    error_handler();
  }

  while (1) {
  }
}
