#define F_CPU 16000000UL
#define BAUD 19200UL

#include "i2c.h"
#include "uartAT328p.h"
#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>

#define READER_ADDR 0x04
#define DISPLAY_ADDR 0x00

int main(void) {
  usart_init();

  DDRB = 1 << i2cSCLpin | 1 << i2cSDApin;
  i2c_init();

  // inverting pins because it is easier to pull
  uint8_t input = ~i2c_read(PCF8574A_ADR, READER_ADDR, 0, PCF8574A_TYP);

  // inverting displayed value because leds are in negative logic
  i2c_write(PCF8574_ADR, DISPLAY_ADDR, 0, ~input, PCF8574_TYP);

  return 0;

  // i2c_write(PCF8574_ADR, 0x00, 0x00, ~count_start, PCF8574_TYP);
}
