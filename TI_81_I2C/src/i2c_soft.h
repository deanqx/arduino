/*
 * Library to use I2C on a AVR Microcontroller without the hardware module
 * */

// 10 kHz
#define I2C_F 10000
#define I2C_T_4_US (1000000.0 / (I2C_F * 4.0))

/*
 * PB0 = D8
 * PB1 = D9
 * Both are default high
 * */
#define I2C_SCL PB0
#define I2C_SDA PB1
#define INTERNAL_LED PB5

#define I2C_PORT_SCL PORTB
#define I2C_PORT_SDA PORTB

#define I2C_PIN_SCL PINB
#define I2C_PIN_SDA PINB

#define I2C_DDR_SCL DDRB
#define I2C_DDR_SDA DDRB

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>

void i2c_init(void) {
  I2C_PORT_SCL |= 1 << I2C_SCL;
  I2C_PORT_SDA |= 1 << I2C_SDA;
  I2C_DDR_SCL |= 1 << I2C_SCL;
  I2C_DDR_SDA |= 1 << I2C_SDA;
}

/*
 * Transmit 8 bits and check for acknowledge.
 * @returns 1 for acknowledge missing
 * */
uint8_t i2c_tx_byte(uint8_t data) {
  // Transmit each bit with the MSB first
  for (int8_t i = 7; i >= 0; i--) {
    // Clear SDA bit and then set
    I2C_PORT_SDA = I2C_PORT_SDA & ~(1 << I2C_SDA) | (data >> i & 1) << I2C_SDA;
    _delay_us(I2C_T_4_US);
    I2C_PORT_SCL |= 1 << I2C_SCL;
    _delay_us(2 * I2C_T_4_US);
    I2C_PORT_SCL &= ~(1 << I2C_SCL);
    _delay_us(I2C_T_4_US);
  }

  // Check acknowledge
  I2C_DDR_SDA &= ~(1 << I2C_SDA);

  // Delay to equalize frequency
  _delay_us(I2C_T_4_US);

  I2C_PORT_SCL |= 1 << I2C_SCL;
  _delay_us(2 * I2C_T_4_US);

  const bool ack = I2C_PIN_SDA >> I2C_SDA & 1;

  I2C_PORT_SCL &= ~(1 << I2C_SCL);
  _delay_us(I2C_T_4_US);

  I2C_PORT_SDA |= 1 << I2C_SDA;
  I2C_DDR_SDA |= 1 << I2C_SDA;

  return ack;
}

/*
 * Read byte from slave
 * */
uint8_t i2c_rx_byte() {
  I2C_DDR_SDA &= ~(1 << I2C_SDA);

  uint8_t data = 0;

  for (int8_t i = 7; i >= 0; i--) {
    // Delay to equalize frequency between read and write
    _delay_us(I2C_T_4_US);

    I2C_PORT_SCL |= 1 << I2C_SCL;
    _delay_us(2 * I2C_T_4_US);

    data |= (I2C_PIN_SDA >> I2C_SDA & 1) << i;

    I2C_PORT_SCL &= ~(1 << I2C_SCL);
    _delay_us(I2C_T_4_US);
  }

  // Send Acknowledge
  // ACK: Read again
  // NACK: Have to stop

  // Clear bit and then set
  I2C_PORT_SDA = I2C_PORT_SDA & ~(1 << I2C_SDA);
  I2C_DDR_SDA |= 1 << I2C_SDA;
  _delay_us(I2C_T_4_US);

  I2C_PORT_SCL |= 1 << I2C_SCL;
  _delay_us(2 * I2C_T_4_US);

  I2C_PORT_SCL &= ~(1 << I2C_SCL);
  _delay_us(I2C_T_4_US);

  return data;
}

/*
 * Initialize as master and transmit start sequence
 * @param addr - Slaveaddress. 0100 001_ => 0x42
 * @param read - 1: read; 0: write
 * @returns 1 for acknowledge missing
 * */
uint8_t i2c_start(uint8_t addr, bool read) {
  I2C_PORT_SDA &= ~(1 << I2C_SDA);
  _delay_us(2 * I2C_T_4_US);

  I2C_PORT_SCL &= ~(1 << I2C_SCL);
  _delay_us(I2C_T_4_US);

  return i2c_tx_byte(addr | read);
}

/*
 * Send stop sequence
 * */
void i2c_stop(void) {
  I2C_PORT_SCL &= ~(1 << I2C_SCL);
  I2C_PORT_SDA &= ~(1 << I2C_SDA);
  _delay_us(I2C_T_4_US);

  I2C_PORT_SCL |= 1 << I2C_SCL;
  _delay_us(2 * I2C_T_4_US);

  I2C_PORT_SDA |= 1 << I2C_SDA;
  _delay_us(2 * I2C_T_4_US);
}
