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

  uint8_t blinking_led = 0;

  while (1) {
    // inverting displayed value because leds are in negative logic
    i2c_write(PCF8574_ADR, DISPLAY_ADDR, 0, ~blinking_led, PCF8574_TYP);
    _delay_ms(1000);

    blinking_led = !blinking_led;
  }

  return 0;
}
