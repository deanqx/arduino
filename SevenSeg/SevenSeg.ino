/*  TI_63_7Seg_ComK
    7 Segment Display Test mit 5161AS (Einzelsegment) und
    5641AS (Vierersegment).
    LEDs sind Positivaktiv
    Vorwiderstand für LEDs jeweils 330 Ohm

    Pin Belegung
    LED   Einzelsegment Pin  Vierersegment Pin  ArduinoPin  MC_Pin
    A     7                  11                 8           PB0
    B     6                  7                  9           PB1
    C     4                  4                  10          PB2
    D     2                  2                  11          PB3
    E     1                  1                  4           PD4
    F     9                  10                 5           PD5
    G     10                 5                  6           PD6
    DP    5                  3                  7           PD7
    GND0                     6                  A0          PC0
    GND1                     8                  A1          PC1
    GND2                     9                  A2          PC2
    GND3                     12                 A3          PC3
    GND4  8                                     A4          PC4
*/

#define TURN_OFF_SEGMENT 10
// ungefähr 60Hz
#define FRAME_TIME_MS 15

uint16_t current_number = 0;

/* Zuweisungsliste: Zahl (0 - 9) zu LED Zustände; [10] => aus
 0b[PD7 - PD4][PB3 - PB0]
 0b[DP,G,F,E,D,C,B,A]
*/
const uint8_t segment_assignments[] = {
  0b00111111,
  0b00000110,
  0b01011011,
  0b01001111,
  0b01100110,
  0b01101101,
  0b01111101,
  0b00000111,
  0b01111111,
  0b01101111,
};

/* Zeigt Zahl auf 7 Segment Display
@param decimal_place Enables GND for each PCx pin
@returns 0 Erfolgreich; >1 Fehler
*/ 
uint8_t display_one_number(uint8_t number, uint8_t decimal_place)
{
  if (number > 9)
  {
    return 1;
  }

  if (decimal_place > 4)
  {
    return 2;
  }

  // GND aktivieren
  PORTC = ~(1 << decimal_place);

  // LED Zustand Code
  uint8_t segment_assignment = segment_assignments[number];

  // LED pins
  // Using PB0 - PB3
  PORTB = PORTB & 0xF0 | segment_assignment & 0x0F;
  // Using PD4 - PD6
  PORTD = PORTD & 0b10001111 | segment_assignment & 0b01110000;

  return 0;
}

void setup()
{
  Serial.begin(9600);
  Serial.println("7 Segment Display");

  DDRB = 0x0F;
  DDRD = 0xF0;
  DDRC = 0b00011111;

  // Ground Pins sind standartmäßig auf High
  PORTC = 0b00011111;
}

void loop()
{
  if (current_number > 9999)
  {
    current_number = 0;
  }

  uint8_t digit_1 = current_number % 10;
  uint8_t digit_10 = (current_number / 10) % 10;
  uint8_t digit_100 = (current_number / 100) % 10;
  uint8_t digit_1000 = (current_number / 1000) % 10;
  uint8_t digit_10000 = (current_number / 10000) % 10;

  if (current_number < 1)
  {
    digit_10000 = TURN_OFF_SEGMENT;
    digit_1000 = TURN_OFF_SEGMENT;
    digit_100 = TURN_OFF_SEGMENT;
    digit_10 = TURN_OFF_SEGMENT;
    digit_1 = TURN_OFF_SEGMENT;
  }
  else if (current_number < 10)
  {
    digit_10000 = TURN_OFF_SEGMENT;
    digit_1000 = TURN_OFF_SEGMENT;
    digit_100 = TURN_OFF_SEGMENT;
    digit_10 = TURN_OFF_SEGMENT;
  }
  else if (current_number < 100)
  {
    digit_10000 = TURN_OFF_SEGMENT;
    digit_1000 = TURN_OFF_SEGMENT;
    digit_100 = TURN_OFF_SEGMENT;
  }
  else if (current_number < 1000)
  {
    digit_10000 = TURN_OFF_SEGMENT;
    digit_1000 = TURN_OFF_SEGMENT;
  }
  else if (current_number < 10000)
  {
    digit_10000 = TURN_OFF_SEGMENT;
  }

  for (uint16_t waited_for_ms = 0; waited_for_ms < 200; waited_for_ms += FRAME_TIME_MS + 5)
  {
    display_one_number(digit_1, 0);
    delay(FRAME_TIME_MS / 5);
    display_one_number(digit_10, 1);
    delay(FRAME_TIME_MS / 5);
    display_one_number(digit_100, 2);
    delay(FRAME_TIME_MS / 5);
    display_one_number(digit_1000, 3);
    delay(FRAME_TIME_MS / 5);
    display_one_number(digit_10000, 4);
    delay(FRAME_TIME_MS / 5);
  }

  current_number++;
}
