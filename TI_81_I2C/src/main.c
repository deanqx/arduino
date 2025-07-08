#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>

// #define I2C_F 100000
#define I2C_F 100
#define I2C_T_QUARTER_US (1000000.0 / (I2C_F * 4.0))

// PB0 = D8
// PB1 = D9
// Both are default high
#define I2C_SCL PB0
#define I2C_SDA PB1

#define I2C_PORT_SCL PORTB
#define I2C_PORT_SDA PORTB

#define I2C_PIN_SCL PINB
#define I2C_PIN_SDA PINB

#define I2C_DDR_SCL DDRB
#define I2C_DDR_SDA DDRB

#define SET_BIT(port, pin) port |= 1 << pin;
#define CLEAR_BIT(port, pin) port &= ~(1 << pin);

void i2c_init_master(void) {
  I2C_DDR_SCL |= 1 << I2C_SCL;
  I2C_DDR_SDA |= 1 << I2C_SDA;
  I2C_PORT_SCL |= 1 << I2C_SCL;
  I2C_PORT_SDA |= 1 << I2C_SDA;
}

/*
 * Returns >0 when error ocurs
 * */
uint8_t i2c_tx_byte(uint8_t data) {
  for (int8_t i = 7; i >= 0; i--) {
    // Clear SDA bit and then set
    I2C_PORT_SDA = I2C_PORT_SDA & ~(1 << I2C_SDA) | ((data >> i) & 1)
                                                        << I2C_SDA;
    _delay_us(I2C_T_QUARTER_US);
    I2C_PORT_SCL |= 1 << I2C_SCL;
    _delay_us(I2C_T_QUARTER_US);
    I2C_PORT_SCL &= ~(1 << I2C_SCL);
    _delay_us(I2C_T_QUARTER_US);
  }

  // Check acknowledge
  I2C_DDR_SDA &= ~(1 << I2C_SDA);

  _delay_us(I2C_T_QUARTER_US);
  I2C_PORT_SCL |= 1 << I2C_SCL;
  _delay_us(I2C_T_QUARTER_US);
  I2C_PORT_SCL &= ~(1 << I2C_SCL);
  _delay_us(I2C_T_QUARTER_US);

  I2C_DDR_SDA |= 1 << I2C_SDA;

  return I2C_PIN_SDA >> I2C_SDA & 1;
}

/*
 * addr - 0100 001_ => 0x42
 * read - 0x01 => read; 0x00 => write
 * */
void i2c_start(uint8_t addr, bool read) {
  I2C_PORT_SDA &= ~(1 << I2C_SDA);
  _delay_us(I2C_T_QUARTER_US);
  I2C_PORT_SCL &= ~(1 << I2C_SCL);
  _delay_us(I2C_T_QUARTER_US);

  i2c_tx_byte(addr | read);
}
void i2c_stop(void) {
  I2C_PORT_SCL |= 1 << I2C_SCL;
  _delay_us(I2C_T_QUARTER_US);
  I2C_PORT_SDA |= 1 << I2C_SDA;
  _delay_us(I2C_T_QUARTER_US);
}

int main(void) {
  i2c_init_master();

  // Slave address (0x42) + write
  i2c_start(0x42, false);

  // Send byte
  i2c_tx_byte(0xAA);

  i2c_stop();
}
