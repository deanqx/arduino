/* TI_71_LCD_8Bit_UART
 * LCD      Adruino     MC 
 * RS         13        PB5
 * R/!W       12        PB4
 * E          11        PB3
 * D0         10        PB2
 * D1          9        PB1
 * D2          8        PB0
 * D3          7        PD7
 * D4          6        PD6
 * D5          5        PD5
 * D6          4        PD4
 * D7          3        PD3
 *
 * Spannungteiler
 *            A0        PC0
 *            A1        PC1
 *            A2        PC2
 *            A3        PC3
 *            A4        PC4
 */

#define info "TI_71_LCD_8Bit_UART"
#include <LiquidCrystal.h>
/* Create object named lcd of the class LiquidCrystal */
LiquidCrystal lcd(13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3); /* For 8-bit mode */
//LiquidCrystal lcd(13, 12, 11, 6, 5, 4, 3);      /* For 4-bit mode */

unsigned char Character1[8] = { 0x04, 0x1F, 0x11, 0x11, 0x1F, 0x1F, 0x1F, 0x1F }; /* Custom Character 1 */
unsigned char Character2[8] = { 0x01, 0x03, 0x07, 0x1F, 0x1F, 0x07, 0x03, 0x01 }; /* Custom Character 2 */

void setup() {
  lcd.begin(16, 2);              /* Initialize 16x2 LCD */
  lcd.clear();                   /* Clear the LCD */
  lcd.createChar(0, Character1); /* Generate custom character */
  lcd.createChar(1, Character2);
  lcd.setCursor(0, 0); /* Set cursor to column 0 row 0 */
  lcd.print(info);     /* Print data on display */

  Serial.begin(115200);  // highspeed UART Übertragung per USB
  while (!Serial);       // Warte bis Serielle Schnittstelle aktiv ist
  delay(2000);
  Serial.println(info);
  Serial.println("Start in 3 sek!"); /* Print data on display */
  lcd.setCursor(0, 1);               /* Set cursor to column 0 row 0 */
  lcd.print("Start in 3 sek!");      /* Print data on display */
  delay(3000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Bitte Analog-Pin");
  lcd.setCursor(0, 1);
  lcd.print("via UART senden");
}

// Liest die 10 fache Spannung eines Analogen eingangs aus. (adc_index=0 entspricht A0) 
uint8_t read_voltage_10x(uint8_t adc_index) {
  // A0 = 14
  const uint8_t adc_port = adc_index + 14;

  // 50V anstatt von 5V da es sich um den 10 fachen Wert handelt
  return (50 * (uint16_t) analogRead(adc_port)) / 1023;
}

void loop() {
  if (Serial.available()) {
    lcd.clear();
    lcd.setCursor(0, 0);

    // '0' - '4'
    const char adc_selection = Serial.read();

    if (adc_selection < '0' || adc_selection > '4') {
      Serial.println("Bitte einen Wert zwischen 0 und 4 eingeben.");
      return;
    }

    // ASCII: '0' = 48
    // char zu int umwandeln
    const uint8_t adc_selection_uint = adc_selection - '0';

    Serial.print("Spannung an A");
    Serial.println(adc_selection_uint);

    lcd.print("Spannung an A");
    lcd.print(adc_selection_uint);

    const uint8_t voltage_10x = read_voltage_10x(adc_selection_uint);

    // Erste Zahl vor dem Komma
    const uint8_t voltage_first_digit = voltage_10x / 10;

    // Erste Zahl nach dem Komma
    const uint8_t voltage_after_comma = voltage_10x % 10;

    Serial.print(voltage_first_digit);
    Serial.print(',');
    Serial.print(voltage_after_comma);
    Serial.println('V');

    lcd.setCursor(0, 1);
    lcd.print(voltage_first_digit);
    lcd.print(',');
    lcd.print(voltage_after_comma);
    lcd.print('V');
  }
}
