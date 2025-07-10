/*
 * Library to use I2C as master on a AVR Microcontroller
 * without the hardware module. TIMER0 COMPA (interrupt)
 * is used for the timing.
 *
 * 10kHz configured in `i2c_init()`.
 * */

#ifndef F_CPU
#warning F_CPU has to be defined with the CPU frequency in Hz!
#endif

/*
 * PB0 = D8
 * PB1 = D9
 * PB2 = D10
 * Both are default high
 * */
#define I2C_SCL PB0
#define I2C_SDA PB1

#define I2C_PORT_SCL PORTB
#define I2C_PORT_SDA PORTB

#define I2C_PIN_SCL PINB
#define I2C_PIN_SDA PINB

#define I2C_DDR_SCL DDRB
#define I2C_DDR_SDA DDRB

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>

/*
 * 0: Data set
 * 1: Clock high
 * 2: (Read)
 * 3: Clock low
 * */
uint8_t i2c_phase = 0;

// Bit 7 to 0
int8_t i2c_current_bit = 7;

uint8_t i2c_data = 0;

volatile bool i2c_busy = 0;
// 1: NACK, 0: ACK
volatile bool i2c_nack = 0;

ISR(TIMER0_COMPA_vect) {
  switch (i2c_phase) {
  case 0:
    I2C_PORT_SDA = I2C_PORT_SDA & ~(1 << I2C_SDA) |
                   (i2c_data >> i2c_current_bit & 1) << I2C_SDA;
    i2c_current_bit--;
    break;
  case 1:
    I2C_PORT_SCL |= 1 << I2C_SCL;
    break;
  case 2:
    if (i2c_current_bit >= 0) {
      break;
    }

    // -- Check acknowledge

    I2C_DDR_SDA &= ~(1 << I2C_SDA);

    i2c_nack = I2C_PIN_SDA >> I2C_SDA & 1;

    I2C_PORT_SDA |= 1 << I2C_SDA;
    I2C_DDR_SDA |= 1 << I2C_SDA;
    break;
  case 3:
    I2C_PORT_SCL &= ~(1 << I2C_SCL);

    // Exit when byte has been transmitted
    if (i2c_current_bit < 0) {
      i2c_busy = 0;

      // Disable Timer0 Comp A
      TIMSK0 &= ~(1 << OCIE0A);
    }

    break;
  }

  i2c_phase++;
  if (i2c_phase > 3) {
    i2c_phase = 0;
  }
}

void i2c_init(void) {
  I2C_PORT_SCL |= 1 << I2C_SCL;
  I2C_PORT_SDA |= 1 << I2C_SDA;
  I2C_DDR_SCL |= 1 << I2C_SCL;
  I2C_DDR_SDA |= 1 << I2C_SDA;

  // https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf

  // CTC mode (OCRA is limit)
  TCCR0A |= 1 << WGM01;

  // 10kHz divided into 4 phases

  /*
   * Clock Select (Page 87)
   * 1: 1
   * 2: 1/8
   * 3: 1/64
   * 4: 1/256
   * 5: 1/1024
   * */
  TCCR0B |= 2;

  /* Output Compare Register A
   * the timer counts from 0 to OCR0A (inclusive)
   * 49 + 1 = 50
   * */
  OCR0A = 49;
}

/*
 * Transmit start sequence
 * */
void i2c_start() {
  I2C_PORT_SDA &= ~(1 << I2C_SDA);
  _delay_us(25);

  I2C_PORT_SCL &= ~(1 << I2C_SCL);
  _delay_us(25);
}

/*
 * Transmit 8 bits and check for acknowledge.
 * @return 0: Successful, 1: Bus is busy
 * */
uint8_t i2c_begin_tx(uint8_t data) {
  if (i2c_busy) {
    return 1;
  }

  i2c_phase = 0;
  i2c_current_bit = 7;
  i2c_data = data;
  i2c_busy = 1;
  i2c_nack = 0;

  // Reset Timer
  TCNT0 = 0;

  // Enable Timer0 Comp A
  TIMSK0 |= 1 << OCIE0A;

  return 0;
}

/*
 * Wait for action to finish.
 * Check `i2c_ack` after transmitting.
 * */
void i2c_wait() {
  while (i2c_busy)
    ;
}

/*
 * Send stop sequence
 * */
void i2c_stop(void) {
  I2C_PORT_SCL &= ~(1 << I2C_SCL);
  I2C_PORT_SDA &= ~(1 << I2C_SDA);
  _delay_us(25);

  I2C_PORT_SCL |= 1 << I2C_SCL;
  _delay_us(25);

  I2C_PORT_SDA |= 1 << I2C_SDA;
  _delay_us(25);
}
