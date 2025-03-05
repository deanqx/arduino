// Pin-Belegung (A: analog)
#define A_NTC A0
#define A_LDR A1
#define A_POTI A2

uint16_t iteration = 0;

void setup()
{
  Serial.begin(9600);
}

// Sensor Spannung in mV analog lesen
uint16_t read_voltage(const uint8_t analog_pin)
{
  // (Analog Wert * Gesamt Spannung) / Maximaler Analog Wert (10 bit)

  return analogRead(analog_pin) * 5000 / 1023;
}

// Sensor Widerstand in Ohm lesen
uint16_t read_resistance(const uint8_t analog_pin, const uint16_t sensor_mv)
{
  /*
   Vorwiderstand * Sensor Spannung / (Gesamtspannung - Sensor Spannung)

   Hergeleitet aus:
     Sensor Spannung = Gesamt Spannung * Sensor Widerstand
                       / (Vorwiderstand + Sensor Widerstand)
  */

  return 10000 * sensor_mv / (5000 - sensor_mv);
}

void loop()
{
  const uint16_t ntc_mv = read_voltage(A_NTC);
  const uint16_t ldr_mv = read_voltage(A_LDR);
  const uint16_t poti_mv = read_voltage(A_POTI);

  Serial.print("Messung: ");
  Serial.print(iteration);
  Serial.print("\tNTC: ");
  Serial.print(ntc_mv);
  Serial.print("mv\tLDR: ");
  Serial.print(ldr_mv);
  Serial.print("mv\tPoti: ");
  Serial.print(poti_mv);
  Serial.println("mv");

  const uint16_t ntc_ohm = read_resistance(A_NTC, ntc_mv);
  const uint16_t ldr_ohm = read_resistance(A_LDR, ldr_mv);
  const uint16_t poti_ohm = read_resistance(A_POTI, poti_mv);

  Serial.print("Messung: ");
  Serial.print(iteration);
  Serial.print("\tNTC: ");
  Serial.print(ntc_ohm);
  Serial.print(" Ohm\tLDR: ");
  Serial.print(ldr_ohm);
  Serial.print(" Ohm\tPoti: ");
  Serial.print(poti_ohm);
  Serial.println(" Ohm");

  // TODO: Temperature, Lux, Distance
  const uint16_t ntc_ohm = read_resistance(A_NTC, ntc_mv);
  const uint16_t ldr_ohm = read_resistance(A_LDR, ldr_mv);
  const uint16_t poti_ohm = read_resistance(A_POTI, poti_mv);

  Serial.print("Messung: ");
  Serial.print(iteration);
  Serial.print("\tNTC: ");
  Serial.print(ntc_ohm);
  Serial.print(" Ohm\tLDR: ");
  Serial.print(ldr_ohm);
  Serial.print(" Ohm\tPoti: ");
  Serial.print(poti_ohm);
  Serial.println(" Ohm");
  delay(500);
  iteration++;
}
