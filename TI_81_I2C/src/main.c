#define INTERNAL_LED PB5

#include "i2c_soft_delay.h"
// #include "i2c_soft.h"
#include <avr/io.h>

int main(void) {
  DDRB |= 1 << INTERNAL_LED;

  i2c_init();

  // Slave address (0x42) + write
  if (i2c_start(0x42, 0)) {
    PORTB |= 1 << INTERNAL_LED;
    return 1;
  }

  // Send byte
  if (i2c_tx_byte(0xA6)) {
    PORTB |= 1 << INTERNAL_LED;
    return 1;
  }

  // Send stop sequence
  i2c_stop();

  // Slave address (0x42) + read
  if (i2c_start(0x42, 1)) {
    PORTB |= 1 << INTERNAL_LED;
    return 1;
  }

  // Read byte + read only once
  uint8_t received = i2c_rx_byte();

  // Send stop sequence
  i2c_stop();

  if (received != 0xA6) {
    PORTB |= 1 << INTERNAL_LED;
    return 1;
  }
}
