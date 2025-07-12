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

void test_send(void) {
  // Start writing to 0x40
  i2c_start_async(0x40, false);
  i2c_await();

  if (i2c_nack) {
    error_handler();
  }

  i2c_send_async(0xA6);
  i2c_await();

  if (i2c_nack) {
    error_handler();
  }

  i2c_stop();
}

void test_read(void) {
  // Start reading to 0x40
  i2c_start_async(0x40, true);
  i2c_await();

  if (i2c_nack) {
    error_handler();
  }

  i2c_read_async();
  i2c_await();

  if (i2c_nack) {
    error_handler();
  }

  i2c_stop();

  if (i2c_data != 0xA6) {
    error_handler();
  }
}

int main(void) {
  DDRB |= 1 << INTERNAL_LED;

  i2c_init();
  sei();

  test_send();
  test_read();

  while (1) {
  }
}
