/* 
 * 7 Segment Display Test mit 5161AS (1x Display) und 5641AS (4x Displays).
 * Beide Displays haben eine gemeinsame Kathode.
 * LEDs sind Positivaktiv
 * Vorwiderstand für LEDs ist jeweils 330 Ohm
 * Ein Schaltplan befindet sich im aktuellen Ordner.
 *
 * GND wird mit Transitoren (BC337) geschalten. High entspricht GND.
 *
 * Pin Belegung
 * LED      5161AS    5641AS    Arduino    MC
 * A        7         11        8          PB0
 * B        6         7         9          PB1
 * C        4         4         10         PB2
 * D        2         2         11         PB3
 * E        1         1         4          PD4
 * F        9         10        5          PD5
 * G        10        5         6          PD6
 * DP       5         3         7          PD7
 * GND0               6         A0         PC0
 * GND1               8         A1         PC1
 * GND2               9         A2         PC2
 * GND3               12        A3         PC3
 * GND4     8                   A4         PC4
 * Neg-LED                      A5         PC5
*/

// number_to_segments[TURN_OFF_DISPLAY] == Aus 
#define TURN_OFF_DISPLAY 16
// Zeit zwischen Wechsel der Anzeigen. 3ms * 5 = 15ms entspricht ungefähr 60Hz
#define DISPLAY_TIME_MS 3
// Zeit zwischen Wechsel der LEDs in Test-Funktionen
#define TESTING_DELAY 200

/* Zuweisungsliste: Hexadezimal (0 - 9, A - F) zu LED Zustände; [TURN_OFF_DISPLAY] => aus
 Format:
 0x[PD7 - PD4][PB4 - PB0]
 0b[DP,G,F,E,D,C,B,A]
*/
const uint8_t number_to_segments[] = {
  0x3F, // 0
  0x06, // 1
  0x5B, // 2
  0x4F, // 3
  0x66, // 4
  0x6D, // 5
  0x7D, // 6
  0x07, // 7
  0x7F, // 8
  0x6F, // 9
  0x77, // A
  0x7C, // b
  0x39, // C
  0x5E, // d
  0x79, // E
  0x71, // F
  0x00  // Aus
};

// PORTB .. PORTD werden zusammen gelesen.
uint8_t read_b;
uint8_t read_c;
uint8_t read_d;

// PORTB .. PORTD werden zusammen gesetzt.
uint8_t set_b;
uint8_t set_c;
uint8_t set_d;

struct Digits {
  uint8_t d[5];
  bool negative;
};

/* Zeigt Zahl auf 7 Segment Display an. Unterstützt Hexadezimalzahlen.
 * @param decimal_place Aktiviert GND für einen jeweiligen PCx Pin
 * @returns 0 Erfolgreich; >1 Fehler
 */ 
uint8_t display_one_number(uint8_t number, bool decimal_point, uint8_t decimal_place)
{
  if (number > sizeof(number_to_segments) / sizeof(uint8_t))
  {
    return 1;
  }

  if (decimal_place > 4)
  {
    return 2;
  }

  // GND aktivieren; positiv Logik
  set_c = 1 << decimal_place;

  if (decimal_point)
  {
    // Punkt anschalten
    set_d = read_d | 1 << PD7;
  }
  else
  {
    // Punkt ausschalten
    set_d = read_d & ~(1 << PD7);
  }

  // LED Zustand Code
  uint8_t segment_assignment = number_to_segments[number];

  // LED pins
  // Using PB0 - PB3
  set_b = read_b & 0xF0 | segment_assignment & 0x0F;
  // Using PD4 - PD6
  set_d = read_d & 0b10001111 | segment_assignment & 0b01110000;

  return 0;
}

// Maximum number is 99999
// digits[n] => 10^n
void int_to_digits(int32_t number, Digits* digits)
{
  const uint32_t number_abs = abs(number);

  // Dezimalstellen auf die verschiedenen Anzeigen aufteilen
  digits->d[0] = number_abs % 10;
  digits->d[1] = (number_abs / 10) % 10;
  digits->d[2] = (number_abs / 100) % 10;
  digits->d[3] = (number_abs / 1000) % 10;
  digits->d[4] = (number_abs / 10000) % 10;
  
  // Nullen vor der ersten aktiven Anzeige ausschalten
  int32_t d = 10000;
  for (uint8_t i = 4; d > number_abs; i--, d /= 10)
  {
    digits->d[i] = TURN_OFF_DISPLAY;
  }

  digits->negative = number < 0;
}

void setup()
{
  Serial.begin(9600);
  Serial.println("7 Segment Display");

  DDRB = 0x0F;
  DDRC = 0x3F;
  DDRD = 0xF0;

  set_b = PORTB;
  set_c = PORTC;
  set_d = PORTD;

  const uint8_t decimal_point_after_place = 0;

  uint32_t prev_number_ms = 0;
  uint32_t prev_display_ms = 0;

  Digits digits;

  int32_t cur_number = -10;
  uint8_t cur_digit = 0;

  while (1)
  {
    read_b = PORTB;
    read_c = PORTC;
    read_d = PORTD;
    set_c = read_c | (1 << PC5);

    const uint32_t cur_ms = millis();

    if (cur_ms - prev_number_ms >= TESTING_DELAY)
    {
      prev_number_ms = cur_ms;

      int_to_digits(cur_number, &digits);

      cur_number++;
      if (cur_number > 99999)
      {
        cur_number = -9999;
      }
    }

    if (cur_ms - prev_display_ms >= DISPLAY_TIME_MS)
    {
      prev_display_ms = cur_ms;

      if (digits.negative)
      {
        set_c = read_c | (1 << PC5);
      }
      else
      {
        set_c = read_c & ~(1 << PC5);
      }

      display_one_number(digits.d[cur_digit], decimal_point_after_place == cur_digit, cur_digit);

      cur_digit++;
      if (cur_digit > 4)
      {
        cur_digit = 0;
      }
    }

    PORTB = set_b;
    PORTC = set_c;
    PORTD = set_d;
  }
}

void loop()
{
}
