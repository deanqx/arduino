#include "can.h"
#include "lcd.h"
#include "usart.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include <util/delay.h>

// -----------------------------------------------------------------------------
/** Set filters and masks.
 *
 * The filters are divided in two groups:
 *
 * Group 0: Filter 0 and 1 with corresponding mask 0.
 * Group 1: Filter 2, 3, 4 and 5 with corresponding mask 1.
 *
 * If a group mask is set to 0, the group will receive all messages.
 *
 * If you want to receive ONLY 11 bit identifiers, set your filters
 * and masks as follows:
 *
 *	uint8_t can_filter[] PROGMEM = {
 *		// Group 0
 *		MCP2515_FILTER(0),				// Filter 0
 *		MCP2515_FILTER(0),				// Filter 1
 *
 *		// Group 1
 *		MCP2515_FILTER(0),				// Filter 2
 *		MCP2515_FILTER(0),				// Filter 3
 *		MCP2515_FILTER(0),				// Filter 4
 *		MCP2515_FILTER(0),				// Filter 5
 *
 *		MCP2515_FILTER(0),				// Mask 0 (for
 *group 0) MCP2515_FILTER(0),				// Mask 1 (for group 1)
 *	};
 *
 *
 * If you want to receive ONLY 29 bit identifiers, set your filters
 * and masks as follows:
 *
 * \code
 *	uint8_t can_filter[] PROGMEM = {
 *		// Group 0
 *		MCP2515_FILTER_EXTENDED(0),		// Filter 0
 *		MCP2515_FILTER_EXTENDED(0),		// Filter 1
 *
 *		// Group 1
 *		MCP2515_FILTER_EXTENDED(0),		// Filter 2
 *		MCP2515_FILTER_EXTENDED(0),		// Filter 3
 *		MCP2515_FILTER_EXTENDED(0),		// Filter 4
 *		MCP2515_FILTER_EXTENDED(0),		// Filter 5
 *
 *		MCP2515_FILTER_EXTENDED(0),		// Mask 0 (for group 0)
 *		MCP2515_FILTER_EXTENDED(0),		// Mask 1 (for group 1)
 *	};
 * \endcode
 *
 * If you want to receive both 11 and 29 bit identifiers, set your filters
 * and masks as follows:
 */
const uint8_t can_filter[] PROGMEM = {
    // Group 0
    MCP2515_FILTER(0), // Filter 0
    MCP2515_FILTER(0), // Filter 1

    // Group 1
    MCP2515_FILTER(0), // Filter 2
    MCP2515_FILTER(0), // Filter 3
    MCP2515_FILTER(0), // Filter 4
    MCP2515_FILTER(0), // Filter 5

    MCP2515_FILTER(0), // Mask 0 (for group 0)
    MCP2515_FILTER(0), // Mask 1 (for group 1)
};
// You can receive 11 bit identifiers with either group 0 or 1.

// -----------------------------------------------------------------------------
// Main loop for receiving and sending messages.

// analog to digital converter init
void adc_init(const uint8_t adc_pin) {
  // use Vcc as reference voltage, use ADC0
  ADMUX = 1 << REFS0 | adc_pin;
  // enable ADC, F_CPU/128 prescaling so average can be measured
  ADCSRA = 1 << ADEN | 1 << ADPS2 | 1 << ADPS1 | 1 << ADPS0;
  // disable digital input at pin ADC0
  DIDR0 = ADC0D;
}

// read synchronus analog to digital converter pin
uint16_t adc_read_sync(void) {
  // start conversion
  ADCSRA |= 1 << ADSC;

  // wait for ADC
  while (ADCSRA >> ADSC & 1)
    ;

  return ADCL | ADCH << 8;
}

// init pwm
void pwm_init(void) {
  // use timer0, Clear OC0A (PD6) on compare match, set OC0A at BOTTOM,
  // (non-inverting mode), fast PWM
  TCCR0A = 1 << COM0A1 | 1 << WGM01 | 1 << WGM00;
  // FCPU without prescaling
  TCCR0B = 1 << CS00;
}

void pwm_set(uint8_t numerator) { OCR0A = numerator; }

void test_lcd(void) {
  lcd_puts("Hello World");
  lcd_gotoxy(0, 1);
  lcd_puts("iiiiiiiiiiii");
  lcd_gotoxy(0, 2);
  lcd_puts("MMMMMMMMMMMM");
  lcd_gotoxy(0, 3);
  lcd_puts_p(PSTR("String from flash"));

  while (1) {
  }
}

int main(void) {
  uart0_init(BAUD_CALC(9600UL));
  lcd_init(LCD_DISP_ON);
  adc_init(ADC0D);
  pwm_init();

  DDRD &= ~(1 << ADC0D);
  DDRD |= 1 << PD6;

  uart0_puts("TI_86_OLED_Statusanzeige\r\n");

  while (1) {
    const uint8_t brightness = adc_read_sync() >> 2;

    uart0_puts("Brightness: ");
    uart0_putuint(brightness);
    uart0_puts("\r\n");

    pwm_set(brightness);
  }

  // Initialize MCP2515
  can_init(BITRATE_250_KBPS);

  // Load filters and masks
  can_static_filter(can_filter);

  // Create a test messsage
  can_t msg;

  msg.id = 0x400;
  msg.flags.rtr = 0;
  // msg.flags.extended = 1;

  msg.length = 4;
  msg.data[0] = 0xde;
  msg.data[1] = 0xad;
  msg.data[2] = 0xbe;
  msg.data[3] = 0xef;

  // Send the message
  can_send_message(&msg);

  while (1) {
    // Check if a new messag was received
    if (can_check_message()) {
      can_t msg;

      // Try to read the message
      if (can_get_message(&msg)) {
        uart0_puts("received something\r\n");
      }
    }
  }

  return 0;
}
