/*
 * ZWL:
 * Arduino  AVR     Beschreibung
 * D8       PB0     /F7 LA
 * D9       PB1     /F11 LA
 * A0       PC0     /S0 LA
 * A1       PC1     S1 LA
 * A2       PC2     S2 LA
 * A3       PC3     S3 LA
 * A4       PC4     E2 LA
 * A5       PC5     E1 LA
 * D2       PD2     Q3
 * D3       PD3     Q2
 * D4       PD4     Q1
 *
 * LA = Low Active
 */

#include "usart.h"
#include <avr/io.h>
#include <stdbool.h>

static const uint8_t FUSE_TISCH = PB0;
static const uint8_t FUSE_STEIN = PB1;

static const uint8_t NOTAUS = PC0;
static const uint8_t TISCH = PC1;
static const uint8_t AUS = PC2;
static const uint8_t STEIN = PC3;
static const uint8_t E2 = PC4;
static const uint8_t E1 = PC5;

static const uint8_t STEIN_MOTOR = PD2;
static const uint8_t TISCH_ZU_E2 = PD3;
static const uint8_t TISCH_ZU_E1 = PD4;

#define READ_PIN(x) (PINC >> x & 1)

int main(void) {
  // Pull-Up aktivieren
  PORTB = 1 << FUSE_TISCH | 1 << FUSE_STEIN;
  // Pull-Up aktivieren
  PORTC = 1 << NOTAUS | 1 << TISCH | 1 << AUS | 1 << STEIN | 1 << E1 | 1 << E2;
  // Ausgänge setzen
  DDRD = 1 << STEIN_MOTOR | 1 << TISCH_ZU_E2 | 1 << TISCH_ZU_E1;

  uart0_init(BAUD_CALC(9600UL));

  // zwischen Speicher der Zustände
  bool notaus, tisch, aus, stein, e1, e2;
  bool stein_motor = false, tisch_zu_e2 = false, tisch_zu_e1 = false;

  while (1) {
    // --- Einlesen ---
    notaus = !READ_PIN(NOTAUS);
    tisch = READ_PIN(TISCH);
    aus = READ_PIN(AUS);
    stein = READ_PIN(STEIN);
    e1 = READ_PIN(E1);
    e2 = READ_PIN(E2);

    // --- Verarbeitung ---
    if (notaus || aus) {
      uart_puts("Notaus oder aus");
      stein_motor = false;
      tisch_zu_e2 = false;
      tisch_zu_e1 = false;
    } else {
      if (~PINB >> FUSE_TISCH & 1) {
        tisch_zu_e2 = false;
        tisch_zu_e1 = false;
      } else if (tisch && !tisch_zu_e1) {
        uart_puts("Tisch wird zum ersten mal gestartet");
        tisch_zu_e2 = true;
      }

      if (stein) { // Stein aktivieren?
        uart_puts("Stein anschalten");
        stein_motor = true;
      }

      // Stein abschalten wenn tisch abgeschaltet
      if (!(tisch_zu_e1 || tisch_zu_e2) || ~PINB >> FUSE_STEIN & 1) {
        uart_puts("Stein abschalten");
        stein_motor = false;
      }

      if (e1 && tisch_zu_e1) {
        uart_puts("Tisch richtung wechseln nach rechts\n");
        tisch_zu_e1 = false;
        tisch_zu_e2 = true;
      }

      if (e2 && tisch_zu_e2) {
        uart_puts("Tisch richtung wechseln nach links\n");
        tisch_zu_e1 = true;
        tisch_zu_e2 = false;
      }
    }

    // --- Ausgabe ---
    PORTD = stein_motor << STEIN_MOTOR | tisch_zu_e2 << TISCH_ZU_E2 |
            tisch_zu_e1 << TISCH_ZU_E1;
  }
}
