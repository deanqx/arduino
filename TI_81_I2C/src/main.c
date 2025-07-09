#define INTERNAL_LED PB5
#define I2C_ PB5

#include "i2c_soft_delay.h"
#include <avr/interrupt.h>
#include <avr/io.h>

volatile bool data_changed = 0;

void error_handler(void) {
  cli();

  PORTB |= 1 << INTERNAL_LED;

  while (1)
    ;
}

void test_write(void) {
  // Slave address (0x42) + write
  if (i2c_start(0x42, 0)) {
    error_handler();
  }

  // Send byte
  if (i2c_tx_byte(0xA6)) {
    error_handler();
  }

  // Send stop sequence
  i2c_stop();
}

void test_read(void) {
  // Slave address (0x42) + read
  if (i2c_start(0x42, 1)) {
    error_handler();
  }

  // Read byte + read only once
  uint8_t received = i2c_rx_byte();

  // Send stop sequence
  i2c_stop();

  if (received != 0xA6) {
    error_handler();
  }
}

// Pin Change Interrupt for PCINT7..0
ISR(PCINT0_vect) {
  // Only execute on falling edge
  if (!(PINB >> PB2 & 1)) {
    data_changed = 1;
    // PORTB |= 1 << INTERNAL_LED;
  }
}

int main(void) {
  DDRB |= 1 << INTERNAL_LED;

  i2c_init_interrupt();

  sei();

  i2c_init();

  test_write();
  test_read();

  while (1) {
    cli();
    if (data_changed) {
      data_changed = 0;
      test_read();
    }
    sei();
  }
}
