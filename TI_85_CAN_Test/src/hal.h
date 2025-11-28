#ifndef HAL_H
#define HAL_H

#include <avr/io.h>

#define DDR_SPI DDRB
#define DD_MOSI DDB3
#define DD_SCK DDB5

#define hal_io_set(port, pin, state) port = port & ~(1 << pin) | (state << pin)
#define hal_io_get(port, pin) port = port >> pin & 1

static const unsigned char OUTPUT = 1;
static const unsigned char INPUT = 0;

static inline void hal_init(void) {
  hal_io_set(DDR_SPI, DD_MOSI, OUTPUT);
  hal_io_set(DDR_SPI, DD_SCK, OUTPUT);
}

#endif
