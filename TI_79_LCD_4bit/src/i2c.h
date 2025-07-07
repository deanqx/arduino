/*
 * I2C Bibliothek für ATmega328P ohne interne Funktionen
 *
 * PCF8574 addresses:
 * 0100 A2 A1 A0 RW
 *
 * Output mode:
 *
 * <S> <slave address + write> <ACK> <data out> <ACK> <data out> <ACK> ... <data out> <ACK> <P>
 * ACK is send by slave
 */

#ifndef I2C_H
#define I2C_H

#include <avr/io.h>
#include <stddef.h>
#include <stdint.h>
#include <util/twi.h>

#define I2C_F_SCL 100000

// A5 and A4 on the Arduino
#define I2C_SCL PC5
#define I2C_SDA PC4

#define I2C_PORT_SCL PORTC
#define I2C_PORT_SDA PORTC

#define I2C_DDR_SCL DDRC
#define I2C_DDR_SDA DDRC

/**
 * Initialize I2C Interface
 */
void i2c_init(void) {
  TWCR = _BV(TWEN);
  TWBR = (F_CPU / I2C_F_SCL - 16) / 2;
}

/**
 * Initialize I2C Interface with pullups
 */
void i2c_init_with_pullups(void) {
  I2C_DDR_SCL &= ~(1 << I2C_SCL);
  I2C_DDR_SDA &= ~(1 << I2C_SDA);
  I2C_PORT_SCL |= 1 << I2C_SCL;
  I2C_PORT_SDA |= 1 << I2C_SDA;
  i2c_init();
}

/**
 * Uninitialize I2C Interface
 */
void i2c_deinit(void) { TWCR &= ~_BV(TWEN); }

/**
 * Wait until I2C Interface is ready
 */
static void i2c_wait(void) {
  while (!(TWCR & _BV(TWINT)))
    ;
}

/**
 * Send I2C Start Condition
 */
int8_t i2c_start(void) {
  TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTA);
  i2c_wait();
  return (TWSR & TW_STATUS_MASK) != TW_START;
}

/**
 * Send I2C Repeated-Start Condition
 */
int8_t i2c_restart(void) {
  TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTA);
  i2c_wait();
  return (TWSR & TW_STATUS_MASK) != TW_REP_START;
}

/**
 * Send I2C Stop Condition
 */
int8_t i2c_stop(void) {
  TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTO);
  while (TWCR & _BV(TWSTO))
    ;
  return 0;
}

/**
 * Select I2C Slave Address
 */
int8_t i2c_addr(uint8_t addr) {
  TWDR = addr;
  TWCR = _BV(TWEN) | _BV(TWINT);
  i2c_wait();
  return (TWSR & TW_STATUS_MASK) !=
         ((addr & 1) ? TW_MR_SLA_ACK : TW_MT_SLA_ACK);
}

/**
 * Send data byte to Slave Device
 */
int8_t i2c_tx_byte(uint8_t byte) {
  TWDR = byte;
  TWCR = _BV(TWEN) | _BV(TWINT);
  i2c_wait();
  return (TWSR & TW_STATUS_MASK) != TW_MT_DATA_ACK;
}

/**
 * Send data bytes to Slave Device
 */
int8_t i2c_tx_data(const uint8_t *data, size_t len) {
  for (size_t i = 0; i < len; i++) {
    if (i2c_tx_byte(data[i])) {
      return 1;
    }
  }

  return 0;
}

/**
 * Receive data byte from Slave Device
 */
int8_t i2c_rx_byte(uint8_t *byte) {
  TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWEA);
  i2c_wait();
  *byte = TWDR;
  return (TWSR & TW_STATUS_MASK) != TW_MR_DATA_ACK;
}

/**
 * Receive last data byte from Slave Device
 */
int8_t i2c_rx_last(uint8_t *byte) {
  TWCR = _BV(TWEN) | _BV(TWINT);
  i2c_wait();
  *byte = TWDR;
  return (TWSR & TW_STATUS_MASK) != TW_MR_DATA_NACK;
}

/**
 * Receive data bytes from Slave Device
 */
int8_t i2c_rx_data(uint8_t *data, size_t len) {
  size_t i;

  for (i = 0; i + 1 < len; i++) {
    if (i2c_rx_byte(data + i)) {
      return 1;
    }
  }

  if (len) {
    if (i2c_rx_last(data + i)) {
      return 1;
    }
  }

  return 0;
}

#endif
