/*
 * Library to use I2C as master on a AVR Microcontroller
 * without the hardware module. Delays are used so this library
 * is blocking.
 * */

// 10 kHz
#define I2C_F 10000
#define I2C_T_4_US (1000000.0 / (I2C_F * 4.0))

/*
 * PB0 = D8
 * PB1 = D9
 * PB2 = D10
 * Both are default high
 * */
#define I2C_SCL PB0
#define I2C_SDA PB1
// PB2 can't be changed easily (low active)
#define I2C_INT_LA PB2

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
 * Pin change to PCINT2 (PB2) will cause interrupt
 * This can be used for example with the PCF8574 and its negative INT pin.
 * When some bit D7..0 changes, it pulls the ~INT pin low.
 * */
void i2c_init_interrupt(void) {
  /* https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf
   * Page 56
   * */

  // PB2 = PCINT2 (Page 65)
  PORTB &= ~(1 << PB2);

  /* Pin Change Interrupt Control Register
   * Enable interrupt on pin change for PCIE0 (PCINT7..0)
   * */
  PCICR |= 1 << PCIE0;

  /* Pin Change Mask Register 0
   * Only activate interrupt if PCINT2 changes
   * */
  PCMSK0 |= 1 << PCINT2;
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
uint8_t i2c_rx_byte(void) {
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
