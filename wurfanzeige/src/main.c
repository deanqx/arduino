#include "i2c.h"
#include <stdint.h>
#include <util/delay.h>

// Register Addressen für MCP23016
static const uint8_t GP0 = 0x00;    // Output Register
static const uint8_t IODIR0 = 0x06; // IO Direction Register
static const uint8_t ICON0 = 0x0A;  // Frequency Register

// Adresse des I2C IC + A2 bis A0 sind high + write enable
static const uint8_t ADDR = 0x4E;

// MCP23016 initialisieren als Ausgang
void init_mcp23016(void) {
  // sende i2c manuell da der angesteuerte I2C IC (MCP23016) 16 bit hat
  i2c_start();
  i2c_send(ADDR);
  i2c_send(ICON0);
  i2c_send(0x01); // high speed Reaktionszeit benutzen
  i2c_stop();

  i2c_start();
  i2c_send(ADDR);
  i2c_send(IODIR0);
  i2c_send(0x00); // Alle IO Pins auf Ausgabe
  i2c_send(0x00);
  i2c_stop();
}

// 16 bit LED Status anzeigen
void display(uint16_t led_states) {
  // sende i2c manuell da der angesteuerte I2C IC (MCP23016) 16 bit hat
  i2c_start();
  i2c_send(ADDR);
  i2c_send(GP0); // wähle Eingabe auf IO Pins
  i2c_send(led_states);
  i2c_send(led_states >> 8);
  i2c_stop(); // Stop nach jeden Befehl, falls man keinen reset durchführt
}

// Alle Lampen blinken lassen
void blink(void) {
  for (uint8_t count = 0; count < 5; count++) {
    display(0xFFFF);
    _delay_ms(100);
    display(0x0000);
    _delay_ms(100);
  }
}

int main(void) {
  i2c_init();
  init_mcp23016();

  while (1) {
    // Wurf 1: Kegel 0, 1, 2, 3
    display(0x000F);
    _delay_ms(1000);

    // Wurf 2: Kegel 4 und Schwein
    display(0x0050);
    _delay_ms(1000);

    // Wurf 3: Kranz
    display(0x0100);
    _delay_ms(500);
    blink();
  }
}
