/* ET_49_
 *
 * Pinbelegung
 *
 * Funktion | M74HC595B1 | Arduino | MC
 * Neg CLR  | 10         | A1      | PC0 
 * Data     | 14         | A2      | PC1 
 * SRCLK    | 11         | A3      | PC2 
 * RCLK     | 12         | A4      | PC4 
 * Neg OE   | 13         | A5      | PC5 
 *
 * LA steht für Low Aktiv.
 */

#define PIN_CLR_LA 0
#define PIN_DATA 1
#define PIN_CLK 2
#define PIN_RCLK 3
#define PIN_OE_LA 4

#define THIRD_CLK_MS 10

// 21 entspricht 0100 1000 nach ET_48
//const byte data = 0x48;

// entspricht die Zahl 2
// const byte data = 0b01011011;

const byte data = 0x37; // 0011 0111

void transmit_data() {
  PORTC &= ~(1 << PIN_CLR_LA);
  delay(THIRD_CLK_MS);

  PORTC |= 1 << PIN_CLR_LA;
  delay(THIRD_CLK_MS);

  for (int8_t bit_index = 7; bit_index >= 0; bit_index--) {
    // Serial data bit
    const byte data_bit = (data >> bit_index) & 0x01;

    // Clear PIN_DATA then set the data_bit
    PORTC = (PORTC & ~(1 << PIN_DATA)) | data_bit << PIN_DATA;
    delay(THIRD_CLK_MS);

    // Clock high
    PORTC |= 1 << PIN_CLK;
    Serial.println("Clock high");
    delay(THIRD_CLK_MS);

    // Clock low
    PORTC &= ~(1 << PIN_CLK);
    Serial.println("Clock low");
    delay(THIRD_CLK_MS);
  }

  PORTC |= 1 << PIN_RCLK;
  delay(THIRD_CLK_MS);

  PORTC &= ~(1 << PIN_RCLK);
  delay(THIRD_CLK_MS);

  PORTB |= 1 << PB5;
}

void setup() {
  Serial.begin(9600);

  // Use onboard LED
  DDRB = 1 << PB5;

  DDRC = 0x1F;
  // OE = 0: alwas enable
  PORTC = 1 << PIN_CLR_LA | 0 << PIN_OE_LA;

  transmit_data();
}

void loop() {
}
