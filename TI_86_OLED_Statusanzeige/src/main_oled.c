#include "i2c.h"
#include "usart.h"
#include <util/delay.h>

#ifndef OLED_ADDRESS
#define OLED_ADDRESS 0x78
#endif

/* If the Co bit is set as logic “0”,
 * the transmission of the following
 * information will contain data bytes only.
 */
static const uint8_t OLED_CONTINUATION_BIT_L = 7;
/*
 * The D/C# bit determines the next data byte
 * is acted as a command or a data.
 * If the D/C# bit is set to logic “0”,
 * it defines the following data byte as a command.
 */
static const uint8_t OLED_DATA_SELECTION_BIT = 6;

static const uint8_t OLED_SET_CONTRAST = 0x81;
static const uint8_t OLED_ENTIRE_DISPLAY_ON = 0xA4;
static const uint8_t OLED_SET_DISPLAY_OFF = 0xAE;
static const uint8_t OLED_SET_DISPLAY_ON = 0xAF;

static const uint8_t OLED_SET_MUX = 0xA8;
static const uint8_t OLED_SET_DISPLAY_OFFSET = 0xD3;
static const uint8_t OLED_SET_DISPLAY_START_LINE = 0x40;
static const uint8_t OLED_SET_CHARGE_PUMP = 0x8D;
static const uint8_t OLED_ENABLE_CHARGE_PUMP = 0x14;
static const uint8_t OLED_SET_OSC_FREQ = 0xD5;
static const uint8_t OLED_SET_COM_PINS = 0xDA;
static const uint8_t OLED_SCROLL_DEACTIVATE = 0x2E;

int main(void) {
  uart0_init(BAUD_CALC(9600));
  uart0_puts("hello\r\n");

  i2c_init();

  /*
     # Example from the datasheet

     Set MUX Ratio A8h, 3Fh
     Set Display Offset D3h, 00h
     Set Display Start Line 40h
     Set Segment re-map A0h/A1h
     Set COM Output Scan Direction C0h/C8h
     Set COM Pins hardware configuration DAh, 02
     Set Contrast Control 81h, 7Fh
     Disable Entire Display On A4h
     Set Normal Display A6h
     Set Osc Frequency D5h, 80h
     Enable charge pump regulator 8Dh, 14h
     Display On AFh
   */

  i2c_start();
  i2c_send(OLED_ADDRESS);
  i2c_send(0 << OLED_CONTINUATION_BIT_L | 0 << OLED_DATA_SELECTION_BIT);

  i2c_send(OLED_SET_DISPLAY_OFF);

  i2c_send(OLED_SET_MUX);
  i2c_send(63);

  i2c_send(OLED_SET_DISPLAY_OFFSET);
  i2c_send(0x00);

  i2c_send(OLED_SET_DISPLAY_START_LINE);

  i2c_send(OLED_SET_COM_PINS);
  i2c_send(0x12);

  i2c_send(OLED_SET_CONTRAST);
  i2c_send(127);

  i2c_send(OLED_ENTIRE_DISPLAY_ON);

  i2c_send(OLED_SET_OSC_FREQ);
  i2c_send(0xF0);

  i2c_send(OLED_SET_CHARGE_PUMP);
  i2c_send(OLED_ENABLE_CHARGE_PUMP);

  i2c_send(OLED_SCROLL_DEACTIVATE);

  i2c_send(OLED_SET_DISPLAY_ON);
  i2c_stop();

  uart0_puts("reached\r\n");

  i2c_start();
  i2c_send(OLED_ADDRESS);
  i2c_send(0 << OLED_CONTINUATION_BIT_L | 1 << OLED_DATA_SELECTION_BIT);
  while (1) {
    i2c_send(0xFF);
  }
  i2c_stop();

  while (1) {
  }
}
