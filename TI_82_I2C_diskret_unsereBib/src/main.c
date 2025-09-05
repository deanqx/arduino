#include <util/delay.h>
#define F_CPU 16000000UL
#define BAUD 19200UL

#include "i2c.h"
#include <stdint.h>

#define READER_ADDR 0x04
#define DISPLAY_ADDR 0x00

int main(void) {
  DDRB = 1 << i2cSCLpin | 1 << i2cSDApin;

  i2c_init();

  while (1) {
    // inverting pins because it is easier to pull low
    uint8_t start_value = ~i2c_read(PCF8574A_ADR, READER_ADDR, 0, PCF8574A_TYP);

    for (int16_t i = start_value; i >= 0; i--) {
      // inverting displayed value because leds are in negative logic
      i2c_write(PCF8574_ADR, DISPLAY_ADDR, 0, ~i, PCF8574_TYP);
      _delay_ms(1000);
    }
  }

  return 0;
}
