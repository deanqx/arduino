/*  TI_63_7Seg_ComK
    7 Segment Display Test mit 5161AS
    LEDs sind Positivaktiv
    Vorwiderstand für LEDs jeweils 220 Ohm

    Pin Belegung
    Bauteilkomponente   BauteilPin  ArduinoPin  MC_Pin
    A                   7           2           PD2
    B                   6           3           PD3
    C                   4           4           PD4
    D                   2           5           PD5
    E                   1           6           PD6
    F                   9           7           PD7
    G                   10          8           PB0
    DP                  5           9           PB1
    GND                 3 oder 8    GND         GND
*/

// Arduino Pins
#define A 2
#define B 3
#define C 4
#define D 5
#define E 6
#define F 7
#define G 8
#define DP 9

// MC Pins
#define A_D PB0

uint8_t current_number = -1;

// An und Auszustände für 7 Segment LEDs: A - DP 
typedef unsigned char DiceMap;

// Zuweisungsliste: Zahl (0 - 9) zu LED Zustände; [10] => aus
const DiceMap segment_assignments[] = {
  0b11111100,
  0b11111100,
  0b01100000,
  0b11110010,
  0b01100110,
  0b10110110,
  0b10111110,
  0b11100000,
  0b11111110,
  0b11110110
  0b00000000
  };

/* Zeigt Zahl auf 7 Segment Display
@returns 0=Erfolgreich; 1=Fehler
*/ 
int display_one_number(uint8_t number)
{
  if (number > 9)
  {
    return 1;
  }

  // LED Zustand Code
  DiceMap segment_assigment = segment_assigments[number];

  //  
  DiceMap pins_0 = segment_assigment & 0b11110000;
  DiceMap pins_1 = segment_assigment & 0b00001111;

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

  display_number(11);
}

void test_lauflicht()
{
  Serial.println("Lauflicht");
  digitalWrite(A, 0);
  digitalWrite(B, 0);
  digitalWrite(C, 0);
  digitalWrite(D, 0);
  digitalWrite(E, 0);
  digitalWrite(F, 0);
  digitalWrite(G, 0);
  digitalWrite(DP, 0);
  delay(500);
  digitalWrite(A, 1);
  delay(500);
  digitalWrite(A, 0);
  digitalWrite(B, 1);
  delay(500);
  digitalWrite(B, 0);
  digitalWrite(C, 1);
  delay(500);
  digitalWrite(C, 0);
  digitalWrite(D, 1);
  delay(500);
  digitalWrite(D, 0);
  digitalWrite(E, 1);
  delay(500);
  digitalWrite(E, 0);
  digitalWrite(F, 1);
  delay(500);
  digitalWrite(F, 0);
  digitalWrite(G, 1);
  delay(500);
  digitalWrite(G, 0);
  digitalWrite(DP, 1);
  delay(500);
  digitalWrite(DP, 0);

  Serial.println("Blinken");
  for (uint8_t i = 0; i < 3 * 2; i++)
  {
    delay(500);
    digitalWrite(A, !digitalRead(A));
    digitalWrite(B, !digitalRead(B));
    digitalWrite(C, !digitalRead(C));
    digitalWrite(D, !digitalRead(D));
    digitalWrite(E, !digitalRead(E));
    digitalWrite(F, !digitalRead(F));
    digitalWrite(G, !digitalRead(G));
    digitalWrite(DP, !digitalRead(DP));
  }
}

void loop()
{
  test_lauflicht();
  return;

  current_number++;

  if (current_number > 9)
  {
    current_number = 0;
  }

  display_number(current_number);
  delay(500);
}

