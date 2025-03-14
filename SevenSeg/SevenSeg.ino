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

#define A 2
#define B 3
#define C 4
#define D 5
#define E 6
#define F 7
#define G 8
#define DP 9

uint8_t current_number = -1;

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

  display_number(0);
}

void display_number(uint8_t x)
{
  switch (x)
  {
    case 0:
      digitalWrite(A, 1);
      digitalWrite(B, 1);
      digitalWrite(C, 1);
      digitalWrite(D, 1);
      digitalWrite(E, 1);
      digitalWrite(F, 1);
      digitalWrite(G, 0);
      break;
    case 1:
      digitalWrite(A, 0);
      digitalWrite(B, 1);
      digitalWrite(C, 1);
      digitalWrite(D, 0);
      digitalWrite(E, 0);
      digitalWrite(F, 0);
      digitalWrite(G, 0);
      break;
    case 2:
      digitalWrite(A, 1);
      digitalWrite(B, 1);
      digitalWrite(C, 0);
      digitalWrite(D, 1);
      digitalWrite(E, 1);
      digitalWrite(F, 0);
      digitalWrite(G, 1);
      break;
    case 3:
      digitalWrite(A, 1);
      digitalWrite(B, 1);
      digitalWrite(C, 1);
      digitalWrite(D, 1);
      digitalWrite(E, 0);
      digitalWrite(F, 0);
      digitalWrite(G, 1);
      break;
    case 4:
      digitalWrite(A, 0);
      digitalWrite(B, 1);
      digitalWrite(C, 1);
      digitalWrite(D, 0);
      digitalWrite(E, 0);
      digitalWrite(F, 1);
      digitalWrite(G, 1);
      break;
    case 5:
      digitalWrite(A, 1);
      digitalWrite(B, 0);
      digitalWrite(C, 1);
      digitalWrite(D, 1);
      digitalWrite(E, 0);
      digitalWrite(F, 1);
      digitalWrite(G, 1);
      break;
    case 6:
      digitalWrite(A, 1);
      digitalWrite(B, 0);
      digitalWrite(C, 1);
      digitalWrite(D, 1);
      digitalWrite(E, 1);
      digitalWrite(F, 1);
      digitalWrite(G, 1);
      break;
    case 7:
      digitalWrite(A, 1);
      digitalWrite(B, 1);
      digitalWrite(C, 1);
      digitalWrite(D, 0);
      digitalWrite(E, 0);
      digitalWrite(F, 0);
      digitalWrite(G, 0);
      break;
    case 8:
      digitalWrite(A, 1);
      digitalWrite(B, 1);
      digitalWrite(C, 1);
      digitalWrite(D, 1);
      digitalWrite(E, 1);
      digitalWrite(F, 1);
      digitalWrite(G, 1);
      break;
    case 9:
      digitalWrite(A, 1);
      digitalWrite(B, 1);
      digitalWrite(C, 1);
      digitalWrite(D, 1);
      digitalWrite(E, 0);
      digitalWrite(F, 1);
      digitalWrite(G, 1);
      break;
  }
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

