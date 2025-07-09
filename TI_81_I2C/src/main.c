#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>

// 10 kHz
#define I2C_F 10000
#define I2C_T_3_US (1000000.0 / (I2C_F * 3.0))

// PB0 = D8
// PB1 = D9
// Both are default high
#define I2C_SCL PB0
#define I2C_SDA PB1
#define INTERNAL_LED PB1

#define I2C_PORT_SCL PORTB
#define I2C_PORT_SDA PORTB

#define I2C_PIN_SCL PINB
#define I2C_PIN_SDA PINB

#define I2C_DDR_SCL DDRB
#define I2C_DDR_SDA DDRB

/*
 * Transmit 8 bits and check for acknowledge.
 * @returns 1 for acknowledge missing
 * */
uint8_t i2c_tx_byte(uint8_t data) {
  // Transmit each bit with the MSB first
  for (int8_t i = 7; i >= 0; i--) {
    // Clear SDA bit and then set
    I2C_PORT_SDA = I2C_PORT_SDA & ~(1 << I2C_SDA) | (data >> i & 1) << I2C_SDA;
    _delay_us(I2C_T_3_US);
    I2C_PORT_SCL |= 1 << I2C_SCL;
    _delay_us(I2C_T_3_US);
    I2C_PORT_SCL &= ~(1 << I2C_SCL);
    _delay_us(I2C_T_3_US);
  }

  // Check acknowledge
  _delay_us(I2C_T_3_US);
  I2C_DDR_SDA &= ~(1 << I2C_SDA);

  //_delay_us(I2C_T_3_US);
  I2C_PORT_SCL |= 1 << I2C_SCL;
  _delay_us(I2C_T_3_US);

  const bool ack = I2C_PIN_SDA >> I2C_SDA & 1;

  I2C_PORT_SCL &= ~(1 << I2C_SCL);
  _delay_us(I2C_T_3_US);

  I2C_PORT_SDA |= 1 << I2C_SDA;
  I2C_DDR_SDA |= 1 << I2C_SDA;

  return ack;
}

/*
 * Initialize as master and transmit start sequence
 * @param addr - 0100 001_ => 0x42
 * @param read - 0x01 => read; 0x00 => write
 * @returns 1 for acknowledge missing
 * */
uint8_t i2c_start(uint8_t addr, bool read) {
  I2C_PORT_SCL |= 1 << I2C_SCL;
  I2C_PORT_SDA |= 1 << I2C_SDA;
  I2C_DDR_SCL |= 1 << I2C_SCL;
  I2C_DDR_SDA |= 1 << I2C_SDA;

  I2C_PORT_SDA &= ~(1 << I2C_SDA);
  _delay_us(I2C_T_3_US);

  I2C_PORT_SCL &= ~(1 << I2C_SCL);
  _delay_us(I2C_T_3_US);

  return i2c_tx_byte(addr | read);
}

/*
 * Send stop sequence
 * */
void i2c_stop(void) {
  I2C_PORT_SCL &= ~(1 << I2C_SCL);
  I2C_PORT_SDA &= ~(1 << I2C_SDA);
  _delay_us(I2C_T_3_US);

  I2C_PORT_SCL |= 1 << I2C_SCL;
  _delay_us(I2C_T_3_US);

  I2C_PORT_SDA |= 1 << I2C_SDA;
  _delay_us(I2C_T_3_US);
}

int main(void) {
  DDRB |= 1 << INTERNAL_LED;

  // Slave address (0x42) + write
  if (i2c_start(0x42, false)) {
    PORTB |= 1 << INTERNAL_LED;
    return 1;
  }

  // Send byte
  if (i2c_tx_byte(0xA6)) {
    PORTB |= 1 << INTERNAL_LED;
    return 1;
  }

  // Send stop sequence
  i2c_stop();
}
