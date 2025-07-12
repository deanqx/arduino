/*
 * Library to use I2C as master on a AVR Microcontroller
 * without the hardware module. Timer0 CompA (interrupt)
 * is used for the timing.
 *
 * Timer0 CompA is configured in `i2c_init()`.
 * */

/*
 * PB0 = D8
 * PB1 = D9
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
 * 0: Send data
 * 1: Clock high
 * 2: Read data
 * 3: Clock low
 * */
uint8_t i2c_phase = 0;

// Bit 7 to 0
int8_t i2c_current_bit = 0;
uint8_t i2c_data = 0;
bool i2c_read_mode = false;

// 1: NACK, 0: ACK
volatile bool i2c_nack = true;
volatile bool i2c_busy = false;

/*
 * This function advances one phase for write mode.
 * A full clock cycle is divided into 4 phases.
 *
 * Usually called by the Interrupt Server Routine.
 * */
void i2c_advance_phase_write() {
  switch (i2c_phase) {
  case 0:
    i2c_current_bit--;

    if (i2c_current_bit < 0) {
      // Begin listening for acknowledge
      I2C_DDR_SDA &= ~(1 << I2C_SDA);
      break;
    }

    I2C_PORT_SDA = I2C_PORT_SDA & ~(1 << I2C_SDA) |
                   (i2c_data >> i2c_current_bit & 1) << I2C_SDA;
    I2C_DDR_SDA |= 1 << I2C_SDA;
    break;
  case 1:
    I2C_PORT_SCL |= 1 << I2C_SCL;
    break;
  case 2:
    if (i2c_current_bit < 0) {
      // Check acknowledge
      i2c_nack = I2C_PIN_SDA >> I2C_SDA & 1;
    }

    break;
  case 3:
    I2C_PORT_SCL &= ~(1 << I2C_SCL);

    if (i2c_current_bit < 0) {
      // End listening for acknowledge
      I2C_PORT_SDA |= 1 << I2C_SDA;
      I2C_DDR_SDA |= 1 << I2C_SDA;

      // -- Exit when byte has been transmitted

      // Disable Timer0 Comp A
      TIMSK0 &= ~(1 << OCIE0A);
      i2c_busy = 0;
    }

    break;
  }

  i2c_phase++;
  if (i2c_phase > 3) {
    i2c_phase = 0;
  }
}

/*
 * This function advances one phase for read mode.
 * A full clock cycle is divided into 4 phases.
 *
 * Usually called by the Interrupt Server Routine.
 * */
void i2c_advance_phase_read() {
  switch (i2c_phase) {
  case 0:
    i2c_current_bit--;

    if (i2c_current_bit < 0) {
      // Setting acknowledge low
      I2C_PORT_SDA &= ~(1 << I2C_SDA);
      I2C_DDR_SDA |= 1 << I2C_SDA;

      i2c_nack = false;
      break;
    }

    I2C_DDR_SDA &= ~(1 << I2C_SDA);

    break;
  case 1:
    I2C_PORT_SCL |= 1 << I2C_SCL;
    break;
  case 2:
    i2c_data |= (I2C_PIN_SDA >> I2C_SDA & 1) << i2c_current_bit;

    break;
  case 3:
    I2C_PORT_SCL &= ~(1 << I2C_SCL);

    if (i2c_current_bit < 0) {
      // -- Exit when byte has been read
      // Disable Timer0 Comp A
      TIMSK0 &= ~(1 << OCIE0A);
      i2c_busy = 0;
      break;
    }

    break;
  }

  i2c_phase++;
  if (i2c_phase > 3) {
    i2c_phase = 0;
  }
}

ISR(TIMER0_COMPA_vect) {
  if (i2c_read_mode) {
    i2c_advance_phase_read();
  } else {
    i2c_advance_phase_write();
  }
}

// Reset index variables
void i2c_reset() {
  i2c_phase = 0;
  i2c_current_bit = 8;
  i2c_nack = true;
}

/*
 * Call `sei()` after all interrupts are configured.
 * */
void i2c_init(void) {
  i2c_reset();

  I2C_PORT_SCL |= 1 << I2C_SCL;
  I2C_PORT_SDA |= 1 << I2C_SDA;
  I2C_DDR_SCL |= 1 << I2C_SCL;
  I2C_DDR_SDA |= 1 << I2C_SDA;

  /* -- Setup interrupt timer
   *
   * https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf
   * */

  // CTC mode: OCRA is limit
  TCCR0A |= 1 << WGM01;

  // 10 kHz are divided into 4 phases, so each phase works with 40 kHz

  /* Clock Select (Page 87)
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
 * Transmit 8 bits and check for acknowledge.
 * Wait with `i2c_await()` for transmission to complete.
 * @return 0: Successful, 1: Bus is busy
 * */
uint8_t i2c_send_async(uint8_t data) {
  i2c_reset();

  i2c_busy = true;
  i2c_data = data;
  i2c_read_mode = false;

  // Reset Timer
  TCNT0 = 0;

  // Enable Timer0 Comp A
  TIMSK0 |= 1 << OCIE0A;

  return 0;
}

/*
 * Read 8 bits and send acknowledge.
 * Wait with `i2c_await()` for transmission to complete.
 * @return 0: Successful, 1: Bus is busy
 * */
uint8_t i2c_read_async(uint8_t data) {
  i2c_reset();

  i2c_busy = true;
  i2c_data = data;
  i2c_read_mode = true;

  // Reset Timer
  TCNT0 = 0;

  // Enable Timer0 Comp A
  TIMSK0 |= 1 << OCIE0A;

  return 0;
}

/*
 * Transmit start sequence and address with read mode bit.
 * Address format: 0100 010 => 0x44
 * Wait with `i2c_await()` for transmission to complete.
 * */
void i2c_start_async(uint8_t addr, bool read_mode) {
  I2C_PORT_SDA &= ~(1 << I2C_SDA);
  _delay_us(50);

  I2C_PORT_SCL &= ~(1 << I2C_SCL);
  _delay_us(25);

  // Clear read mode bit and then set
  i2c_send_async(addr & ~1 | read_mode);
}

/*
 * Wait for action to finish.
 * Check `i2c_ack` after transmitting.
 * */
void i2c_await() {
  while (i2c_busy)
    ;
}

/*
 * Send stop sequence
 * */
void i2c_stop(void) {
  I2C_PORT_SCL &= ~(1 << I2C_SCL);
  I2C_PORT_SDA &= ~(1 << I2C_SDA);
  _delay_us(50);

  I2C_PORT_SCL |= 1 << I2C_SCL;
  _delay_us(50);

  I2C_PORT_SDA |= 1 << I2C_SDA;
  _delay_us(50); // Cooldown
}
