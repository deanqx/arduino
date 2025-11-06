#include <util/delay.h>
#define F_CPU 16000000UL
#define BAUD 19200UL

#include "i2c.h"

#define ADDR0 0x04

int main(void) {
  DDRB = 1 << i2cSCLpin | 1 << i2cSDApin;

  i2c_init();

  i2c_write(PCF8574A_ADR, ADDR0, 0, 0x55, PCF8574A_TYP);

  return 0;
}
