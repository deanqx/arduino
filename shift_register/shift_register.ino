/* ET_48_74HC164_Test
 *
 * Pinbelegung
 *
 * Funktion | 74164  | Arduino | MC
 * Neg CLR  | 9        A1        PC0 
 * Data     | 1, 2     A2        PC1 
 * CLK      | 8        A3        PC2 
 */

#define PIN_NEG_CLR 0
#define PIN_DATA 1
#define PIN_CLK 2
#define HALF_CLK_MS 1000

// 21 entspricht 0b01001000
const byte content = 0x48;

void setup() {
  Serial.begin(9600);

  // Enable on board LED
  DDRB = 1 << PB5;

  DDRC = 0x07;
  DDRC = 0b00000111;
  PORTC = 1 << PIN_NEG_CLR;
}

void loop() {
  PORTC &= ~(1 << PIN_NEG_CLR);
  delay(HALF_CLK_MS);

  PORTC |= 1 << PIN_NEG_CLR;
  delay(HALF_CLK_MS);

  for (uint8_t bit_index = 0; bit_index < 8; bit_index++) {
    // Serial data bit
    const byte data_bit = (content >> bit_index) & 0x01;

    // Clear, then set PIN_DATA
    PORTC = (PORTC & ~(1 << PIN_DATA)) | data_bit << PIN_DATA;

    // Clock high
    PORTC |= 1 << PIN_CLK;
    Serial.println("Clock high");
    delay(HALF_CLK_MS);

    // Clock low
    PORTC &= ~(1 << PIN_CLK);
    Serial.println("Clock low");
    delay(HALF_CLK_MS);
  }

  PORTB |= 1 << PB5;
  // Stop after first iteration
  while (1);
  delay(10000);
  PORTB &= ~(1 << PB5);
}
