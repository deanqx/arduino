#define F_CPU 16000000UL
#define BAUD 19200UL

#include "i2c.h"
#include "uartAT328p.h"
#include <stdint.h>
#include <util/delay.h>

#define READER_ADDR 0x71

uint8_t read_count_start(void) {
  i2c_start();
  i2c_send(0x74);
  uint8_t count_start = i2c_receive(0);
  i2c_stop();

  return count_start;
}

int main(void) {
  usart_init();
  i2c_init();
  i2c_start();

  while (1) {
    i2cPORT |= (1 << i2cSDApin);  // SDA auf High
    _delay_us(120000);            // Pause
    i2cPORT &= ~(1 << i2cSDApin); // SDA auf Low
    _delay_us(120000);            // Pause
    usart_puts("test\r\n");
  }

  i2c_write(PCF8574_ADR, 0x00, 0x00, 0xAA, PCF8574_TYP);
  return 0;
  i2c_start();
  return 0;

  uint8_t count_start = read_count_start();
  usart_itoa(count_start);
  usart_puts("\r\n");

  i2c_write(PCF8574_ADR, 0x00, 0x00, ~count_start, PCF8574_TYP);
}
