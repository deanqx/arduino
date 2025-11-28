/*
 * Library for the MCP2515
 * Relates to the datasheet:
 * https://ww1.microchip.com/downloads/en/DeviceDoc/MCP2515-Stand-Alone-CAN-Controller-with-SPI-20001801J.pdf
 *
 * Author: deanqx
 */

#include <avr/io.h>
#include <stdint.h>

static const uint8_t RESET = 0xC0;
static const uint8_t READ = 0x03;
static const uint8_t READ_RX_BUFFER = 0x90;
static const uint8_t WRITE = 0x02;
static const uint8_t LOAD_TX_BUFFER = 0x40;
static const uint8_t RTS = 0x80; // Request-to-send
static const uint8_t READ_STATUS = 0xA0;
static const uint8_t BIT_MODIFY = 0x05;

static void spi_init_master(void) {
  // Enable SPI, Master, set clock rate fck/16
  SPCR = 1 << SPE | 1 << MSTR | 1 << SPR0;
}

static void spi_tx(const char data) {
  // Start transmission
  SPDR = data;

  // Wait for transmission complete
  while (!(SPSR & (1 << SPIF)))
    ;
}

void can_init(void) {
  spi_init_master();
  spi_tx(RESET);
}
