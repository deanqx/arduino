#include "can.h"
#include "lcd.h"
#include "usart.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <util/delay.h>

#define LED_MASTER false

static const uint16_t ID_DEAN = 0x200;
static const uint16_t ID_HENRY = 0x400;

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

void update_brightness_lcd(const uint8_t brightness) {
  char brightness_value_str[4]; // 3 digits + \0

  utoa(brightness, brightness_value_str, 10);

  lcd_gotoxy(13, 0); // x offset of "Helligkeit: "
  lcd_puts("   ");   // clear value

  lcd_gotoxy(13, 0); // x offset of "Helligkeit: "
  lcd_puts(brightness_value_str);
}

/// return FALSE falls die Nachricht nicht verschickt werden konnte,
/// ansonsten der Code des Puffes in den die Nachricht gespeichert wurde
uint8_t send_brightness_can(const uint8_t brightness,
                            const uint16_t target_id) {
  can_t msg = {
      .id = target_id,
      .flags.rtr = 0,
      .length = 1,
      .data = {brightness},
  };

  return can_send_message(&msg);
}

void init(void) {
  uart0_init(BAUD_CALC(9600UL));
  uart0_puts("TI_86_OLED_Statusanzeige\r\n");

  adc_init(ADC0D);
  pwm_init();
  lcd_init(LCD_DISP_ON);

  // Initialize MCP2515 with 250 kB/s because of 8 MHz crystal
  // actual bus speed is 125 kB/s
  // CS pin is configured in lib/can/include/config.h
  if (!can_init(BITRATE_250_KBPS)) {
    uart0_puts("error: can_init\r\n");

    while (1) {
    }
  }

  DDRD &= ~(1 << ADC0D);
  DDRD |= 1 << PD6;

  lcd_puts_p(PSTR("Helligkeit: "));

  lcd_gotoxy(16, 0); // "Helligkeit: xxx/255"
  lcd_puts_p(PSTR("/255"));

  lcd_gotoxy(0, 2);
#if LED_MASTER
  lcd_puts_p(PSTR("CAN Ausgang:"));
#else
  lcd_puts_p(PSTR("CAN Eingang:"));
#endif
}

int main(void) {
  init();

  while (1) {
#if LED_MASTER
    const uint8_t brightness = adc_read_sync() >> 2;
#else
    uart0_puts("wait for CAN message...\r\n");

    can_t received_msg;

    while (!can_get_message(&received_msg))
      ;

    uart0_puts("received message from 0x");
    uart0_puthex(received_msg.id >> 8);
    uart0_puthex(received_msg.id);
    uart0_puts("\r\n");

    // TODO print on display

    if (received_msg.id != ID_DEAN) {
      continue;
    }

    if (received_msg.length != 1) {
      uart0_puts("error: received CAN message is larger than 1\r\n");
      continue;
    }

    const uint8_t brightness = received_msg.data[0];
#endif

    uart0_puts("Helligkeit: ");
    uart0_putuint(brightness);
    uart0_puts("\r\n");

    pwm_set(brightness);
    update_brightness_lcd(brightness);

#if LED_MASTER
    uart0_puts("Send brightness over can\r\n");
    send_brightness_can(brightness, ID_HENRY);

    _delay_ms(500); // optional: reduce message rate
#endif
  }
}
