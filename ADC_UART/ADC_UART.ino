// Programm zum auslesen von einem NTC, LDR und Poti
// Schaltplan und mehr Infos in Moodle unter GYT26_TIF_12_SM

// Pin-Belegung (A: analog)
#define A_NTC A0
#define A_LDR A1
#define A_POTI A2

#define KELVIN_CONSTANT 273.15f

uint16_t iteration = 0;

void setup()
{
  Serial.begin(9600);
}

// Sensor Spannung in mV analog lesen
float read_voltage(const uint8_t analog_pin)
{
  // (Analog Wert * Gesamt Spannung) / Maximaler Analog Wert (10 bit)

  return analogRead(analog_pin) * 5000.0f / 1023.0f;
}

// Sensor Widerstand in Ohm lesen
float read_resistance(const uint16_t sensor_mv)
{
  /*
   Vorwiderstand * Sensor Spannung / (Gesamtspannung - Sensor Spannung)

   Hergeleitet aus:
     Sensor Spannung = Gesamt Spannung * Sensor Widerstand
                       / (Vorwiderstand + Sensor Widerstand)
  */

  return 10000.0f * sensor_mv / (5000.0f - sensor_mv);
}

/* NTC Temperatur in Gradcelsius lesen
  Standart werte:
  ambient_temperature_kelvin = 298.15f (25 degree celsius)
  ambient_resistance_ohm = 10000.0f (10k Ohm)
  b_constant = 4000.0f
*/
float read_temperature(const float sensor_ohm, const float ambient_temperature_kelvin,
  const float ambient_resistance_ohm, const float b_constant)
{
  // Formel aus https://de.wikipedia.org/wiki/Hei%C3%9Fleiter
  const float temperature_kelvin = (ambient_temperature_kelvin * b_constant)
    / (ambient_temperature_kelvin * log(sensor_ohm / ambient_resistance_ohm) + b_constant);

  return temperature_kelvin - KELVIN_CONSTANT;
}

// LDR Helligkeit in Lux lesen
uint16_t read_brightness(const uint16_t sensor_ohm)
{
  // Annäherung:
  return 500 * sensor_ohm;
}

// Potiwiderstand in mm lesen
uint16_t read_distance(const uint16_t sensor_ohm)
{
  // 10cm = 100mm
  // 1000 Ohm <=> 100mm | : 100 
  // => 10 Ohm <=> 1mm
  return sensor_ohm / 10;
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
  Serial.print(" mv\tLDR: ");
  Serial.print(ldr_mv);
  Serial.print(" mv\tPoti: ");
  Serial.print(poti_mv);
  Serial.println(" mv");

  const uint16_t ntc_ohm = read_resistance(ntc_mv);
  const uint16_t ldr_ohm = read_resistance(ldr_mv);
  const uint16_t poti_ohm = read_resistance(poti_mv);

  Serial.print("Messung: ");
  Serial.print(iteration);
  Serial.print("\tNTC: ");
  Serial.print(ntc_ohm);
  Serial.print(" Ohm\tLDR: ");
  Serial.print(ldr_ohm);
  Serial.print(" Ohm\tPoti: ");
  Serial.print(poti_ohm);
  Serial.println(" Ohm");

  const uint16_t ntc_temperature = read_temperature(ntc_ohm, 298.15f, 10000.0f, 4000.0f);
  const uint16_t ldr_brightness = read_brightness(ldr_ohm);
  const uint16_t poti_distance = read_distance(poti_ohm);

  Serial.print("Messung: ");
  Serial.print(iteration);
  Serial.print("\tNTC: ");
  Serial.print(ntc_temperature);
  Serial.print(" °C\tLDR: ");
  Serial.print(ldr_brightness);
  Serial.print(" Lux\tPoti: ");
  Serial.print(poti_distance);
  Serial.println(" mm");

  Serial.println("");

  delay(500);
  iteration++;
}
