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
 * Poti                         A6         ADC6
*/

// number_to_segments[TURN_OFF_DISPLAY] == Aus 
#define TURN_OFF_DISPLAY 16
#define ADC_CHANNEL_6 6
#define TEST_COUNT_UP 0

// Dezimalpunkt nach Stelle x
#define DECIMAL_POINT_AFTER_PLACE -1
// Zeit zwischen Wechsel der Anzeigen. 3ms * 5 = 15ms entspricht ungefähr 60Hz
#define DISPLAY_TIME_MS 2
// Verzögerung bis Zahl hochgezählt wird
#define COUNTING_DELAY_MS 200

// Hält Dezimalstellen und Vorzeichen
typedef struct digits_t {
  uint8_t d[5];
  bool negative;
};

/* Zuweisungsliste: Hexadezimal (0 - 9, A - F) zu LED Zustände; [TURN_OFF_DISPLAY] => aus
 Format:
 0x[PD7 - PD4][PB4 - PB0]
 0b[DP,G,F,E,D,C,B,A]
*/
const byte number_to_segments[] = {
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

// PORTB, PORTC und PORTD werden zusammen gelesen und gesetzt.
byte port_b;
byte port_c;
byte port_d;

// Hält Zeitpunkt der letzten Ausführung (Für nicht blockierende Verzögerung)
// INT32_MIN = 0b1000... was eine größe Zahl beim subtrahieren wird
uint32_t prev_number_ms = INT32_MIN;
uint32_t prev_display_ms = INT32_MIN;

digits_t digits;

// Werden hochgezählt bis zu den jeweiligen Limit
int32_t curr_number = -10;
uint8_t curr_digit = 0;

/* Zeigt Zahl auf 7 Segment Display an. Unterstützt Hexadezimalzahlen.
 * @param decimal_place Aktiviert GND für einen jeweiligen PCx Pin
 * @returns 0 Erfolgreich; >1 Fehler
 */ 
uint8_t display_single_number(uint8_t number, uint8_t decimal_place, bool decimal_point)
{
  // number muss im Bereich der Indexe liegen
  if (number > sizeof(number_to_segments) / sizeof(uint8_t))
  {
    return 1;
  }

  // Es gibt nur 5 Anzeigen
  if (decimal_place > 4)
  {
    return 2;
  }

  // Alle 5 GND deaktivieren danach ein GND aktivieren (positiv Logik)
  port_c = port_c & 0xE0 | 1 << decimal_place;

  if (decimal_point)
  {
    // Punkt anschalten
    port_d |= 1 << PD7;
  }
  else
  {
    // Punkt ausschalten
    port_d &= ~(1 << PD7);
  }

  // Segmentzustände
  byte segment_assignment = number_to_segments[number];

  // LED Pins setzen
  // PB0 - PB3
  port_b = port_b & 0xF0 | segment_assignment & 0x0F;
  // PD4 - PD6
  port_d = port_d & 0b10001111 | segment_assignment & 0b01110000;

  return 0;
}

// Maximum number is 99999
// digits[n] => 10^n
void int_to_digits(int32_t number)
{
  // number ohne Vorzeichen
  const uint32_t number_abs = abs(number);

  // Dezimalstellen auf die verschiedenen Anzeigen aufteilen
  digits.d[0] = number_abs % 10;
  digits.d[1] = (number_abs / 10) % 10;
  digits.d[2] = (number_abs / 100) % 10;
  digits.d[3] = (number_abs / 1000) % 10;
  digits.d[4] = (number_abs / 10000) % 10;
  
  // Nullen vor der ersten aktiven Anzeige ausschalten
  // Die erste Anzeige bleibt immer aktiv
  int32_t d = 10000;
  for (uint8_t i = 4; i > 0 && d > number_abs; i--, d /= 10)
  {
    digits.d[i] = TURN_OFF_DISPLAY;
  }

  // Negatives Vorzeichen
  digits.negative = number < 0;
}

// Dezimalstellen anzeigen (non blocking delay)
// @param decimal_point_after_place Deaktivieren mit -1
void display_number(uint32_t curr_ms)
{
  if (digits.negative)
  {
    port_c |= 1 << PC5;
  }
  else
  {
    port_c &= ~(1 << PC5);
  }

  display_single_number(digits.d[curr_digit], curr_digit, DECIMAL_POINT_AFTER_PLACE == curr_digit);

  curr_digit++;
  if (curr_digit > 4)
  {
    curr_digit = 0;
  }
}

// Zahl hochzählen (non blocking delay)
// @param decimal_point_after_place Deaktivieren mit -1
void test_count_up(uint32_t curr_ms)
{
  int_to_digits(curr_number);

  curr_number++;
  if (curr_number > 99999)
  {
    curr_number = -9999;
  }
}

void adc_init(void)
{
  // https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf#page=217
  
  // AVcc als referenze Spannung (5V), und ADC6 (MUX6) auswählen
  // REFS = 0b01 => 5V
  // MUX = 0b0110 => ADC6
  ADMUX = (1 << REFS0) | ADC_CHANNEL_6;

  // ADC aktivieren, Umwandelung starten, prescaler = 0b111 für 125kHz ADC clock @16MHz system clock
  ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

void setup(void)
{
  Serial.begin(9600);
  Serial.println("7 Segment Display");

  // Ausgänge setzen
  DDRB = 0x0F;
  DDRC = 0x3F;
  DDRD = 0xF0;

  adc_init();
}

void loop(void)
{
  port_b = PORTB;
  port_c = PORTC;
  port_d = PORTD;

  // current_ms - aktuelle Millisekunden
  const uint32_t curr_ms = millis();

#if TEST_COUNT_UP
  if (curr_ms - prev_number_ms >= COUNTING_DELAY_MS)
  {
    prev_number_ms = curr_ms;
  
    test_count_up(curr_ms);
  }
#else
  // Warten bis ADC gelesen wurde
  if (!(ADCSRA & (1 << ADSC)))
  {
    int_to_digits(ADC);

    // ADC Umwandelung starten
    ADCSRA |= (1 << ADSC);
  }
#endif

  if (curr_ms - prev_display_ms >= DISPLAY_TIME_MS)
  {
    prev_display_ms = curr_ms;
    display_number(curr_ms);
  }

  PORTB = port_b;
  PORTC = port_c;
  PORTD = port_d;
}
