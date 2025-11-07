#include <stdint.h>
#include <util/delay.h>
#define F_CPU 16000000UL
#define BAUD 19200UL

#include "i2c.h"

// Zahl zu 7-Segment LED-Zustand
static const uint8_t number_map[] = {0xBF, 0x06, 0x5B, 0x4F, 0x66,
                                     0x6D, 0xFD, 0x07, 0x7F, 0x6F};

// Zahlstelle (von rechts nach links)
static const uint8_t digit_addr_map[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};

void turn_off_all_digits(void) {
  for (uint8_t digit = 0; digit < 6; digit++) {
    i2c_write(PCF8574A_ADR, digit_addr_map[digit], 0, 0x00, PCF8574A_TYP);
  }
}

int main(void) {
  DDRB = 1 << i2cSCLpin | 1 << i2cSDApin;

  i2c_init();

  turn_off_all_digits();

  for (uint8_t digit = 0; digit < 6; digit++) {
    for (uint8_t i = 0; i <= 9; i++) {
      i2c_write(PCF8574A_ADR, digit_addr_map[digit], 0, number_map[i],
                PCF8574A_TYP);
      _delay_ms(500);
    }
  }

  return 0;
}
