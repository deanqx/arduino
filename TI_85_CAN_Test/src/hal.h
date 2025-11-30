#ifndef HAL_H
#define HAL_H

#include <avr/io.h>

#define PORT_SPI PORTB

#define DDR_SPI DDRB

#define SS 2   // arduino: D10
#define MOSI 3 // arduino: D11
#define MISO 4 // arduino: D12
#define SCK 5  // arduino: D13

#define hal_io_set(port, pin, state) port = port & ~(1 << pin) | (state << pin)
#define hal_io_get(port, pin) port = port >> pin & 1

static const unsigned char OUTPUT = 1;
static const unsigned char INPUT = 0;

static inline void hal_init(void) {
  hal_io_set(DDR_SPI, MOSI, OUTPUT);
  hal_io_set(DDR_SPI, SCK, OUTPUT);
  hal_io_set(DDR_SPI, SS, OUTPUT);
  hal_io_set(DDR_SPI, MISO, INPUT);
}

#endif
