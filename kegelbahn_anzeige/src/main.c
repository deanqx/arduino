#include <stdint.h>
#include <util/delay.h>
#define F_CPU 16000000UL
#define BAUD 19200UL

#include "i2c.h"

// Zahl zu 7-Segment LED-Zustand
static const uint8_t number_map[] = {0xBF, 0x06, 0x5B, 0x4F, 0x66,
                                     0x6D, 0xFD, 0x07, 0x7F, 0x6F};

void turn_off_all_digits(void) {
  for (int8_t digit = 6; digit >= 1; digit--) {
    i2c_write(PCF8574A_ADR, digit, 0, 0x00, PCF8574A_TYP);
  }
}

void display_digit(const uint8_t digit, const uint8_t number) {
  i2c_write(PCF8574A_ADR, digit, 0, number_map[number], PCF8574A_TYP);
}

void display_number(uint32_t number) {
  // Einerstelle anzeigen
  static const uint8_t first_digit = 1;
  display_digit(first_digit, number % 10);

  // restliche Stellen anzeigen
  for (uint8_t digit_reverse = 2; digit_reverse <= 6; digit_reverse++) {
    number /= 10;
    display_digit(digit_reverse, number % 10);
  }
}

int main(void) {
  i2c_init();

  turn_off_all_digits();

  while (1) {
    for (uint32_t number = 0; number <= 999999; number++) {
      display_number(number);

      // wenn wechsel der tausender-Stelle
      if (number % 999 == 0) {
        // alle Segmente an
        display_digit(6, 8);
        display_digit(5, 8);
        display_digit(4, 8);
        display_digit(3, 8);
        display_digit(2, 8);
        display_digit(1, 8);
        _delay_ms(500);

        // alle Segmente aus
        turn_off_all_digits();
        _delay_ms(500);
      }
    }
  }

  return 0;
}
