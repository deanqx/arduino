/*  TI_63_7Seg_ComK
    7 Segment Display Test mit 5161AS
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

// Arduino Pins
#define A 4
#define B 5
#define C 6
#define D 7
#define E 8
#define F 9
#define G 10
#define DP 11

#define TURN_OFF_DISPLAY 10

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

  PORTC = ~(1 << decimal_place);

  // LED Zustand Code
  uint8_t segment_assignment = segment_assignments[number];

  // Using PB0 to PB3
  PORTB = PORTB & 0xF0 | segment_assignment & 0x0F;
  // Using PD4 to PD7
  PORTD = PORTD & 0x10001111 | segment_assignment & 0b01110000;

  return 0;
}

void setup()
{
  Serial.begin(9600);
  Serial.println("7 Segment Display");

  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(D, OUTPUT);
  pinMode(E, OUTPUT);
  pinMode(F, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(DP, OUTPUT);
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  digitalWrite(A0, 1);
  digitalWrite(A1, 1);
  digitalWrite(A2, 1);
  digitalWrite(A3, 1);
  digitalWrite(A4, 1);

  // display_one_number(TURN_OFF_DISPLAY, 0);
}

void loop()
{
  if (current_number > 9999)
  {
    current_number = 0;
  }

  uint16_t digit_10 = current_number % 10;
  uint16_t digit_100 = (current_number / 10) % 10;

  display_one_number(0, 0);
  delay(1);
  display_one_number(1, 1);
  delay(200);

  current_number++;
}

