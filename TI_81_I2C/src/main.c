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

  // Address: 0x42 + write
  i2c_send_async(0x41);
  i2c_await();

  if (i2c_nack) {
    error_handler();
  }

  i2c_read_async(0xA6);
  i2c_await();

  if (i2c_nack) {
    error_handler();
  }

  i2c_stop();

  while (1) {
  }
}
