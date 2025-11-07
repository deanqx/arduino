#include <stdint.h>
#include <util/delay.h>
#define F_CPU 16000000UL
#define BAUD 19200UL

#include "i2c.h"

#define ADDR0 0x06

// Zahl zu 7-Segment LED-Zustand
uint8_t number_map[] = {0xBF, 0x06, 0x5B, 0x4F, 0x66,
                        0x6D, 0xFD, 0x07, 0x7F, 0x6F};

int main(void) {
  DDRB = 1 << i2cSCLpin | 1 << i2cSDApin;

  i2c_init();

  for (uint8_t i = 0; i <= 9; i++) {
    i2c_write(PCF8574A_ADR, ADDR0, 0, number_map[i], PCF8574A_TYP);
    _delay_ms(500);
  }
  // i2c_write(PCF8574A_ADR, ADDR0, 0, 0x56, PCF8574A_TYP);

  return 0;
}
