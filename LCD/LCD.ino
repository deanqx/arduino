/*
TI_70_LCD_8Bit_Testaufbau

Pinbelegung:

    LCD    |    Arduino    |   MC    
VSS         GND
VDD         5V
V0          5V -> 1 kOhm
RS          13              PB5
RW          12              PB4
E           11              PB3
D0          10              PB2
D1          9               PB1
D2          8               PB0
D3          7               PD7
D4          6               PD6
D5          5               PD5
D6          4               PD4
D7          3               PD3
A           5V -> 330 Ohm
K           GND
*/

#include <LiquidCrystal.h>

// Maximum PROGRAM_NAME length is 16
#define PROGRAM_NAME0 "TI_71_LCD_8Bit_U"
#define PROGRAM_NAME1 "ART"

/* Create object named lcd of the class LiquidCrystal */
LiquidCrystal lcd(13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3); /* For 8-bit mode */
//LiquidCrystal lcd(13, 12, 11, 6, 5, 4, 3);      /* For 4-bit mode */

unsigned char Character1[8] = { 0x04, 0x1F, 0x11, 0x11, 0x1F, 0x1F, 0x1F, 0x1F }; /* Custom Character 1 */
unsigned char Character2[8] = { 0x01, 0x03, 0x07, 0x1F, 0x1F, 0x07, 0x03, 0x01 }; /* Custom Character 2 */

void setup() {
  Serial.begin(9600);
  Serial.print(PROGRAM_NAME0);
  Serial.println(PROGRAM_NAME1);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.createChar(0, Character1);
  lcd.createChar(1, Character2);
  Serial.println("LCD wurde initialisiert");

  lcd.setCursor(0, 0);
  lcd.print(PROGRAM_NAME0);
  lcd.setCursor(0, 1);
  lcd.print(PROGRAM_NAME1);
}

void loop() {
  String content = Serial.readStringUntil('\n');

  // Wurde nichts gesendet?
  if (content.length() == 0) {
    return;
  }

  Serial.println("Setting content");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(content);
}
