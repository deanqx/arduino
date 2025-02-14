#define PIN_WALK_GREEN_LA 2
#define PIN_WALK_RED_LA 3
#define PIN_DRIVE_GREEN 4
#define PIN_DRIVE_YELLOW 5
#define PIN_DRIVE_RED 6

// Pins für Füßgänger sind Low Active (LA)

void setup()
{
  // Übertragungsrate auf 9600 Baud
  Serial.begin(9600);

  // Senden über Seriell
  Serial.println("Hallo");

  // Setze Pins auf Ausgang oder Eingang 
  pinMode(PIN_WALK_GREEN, OUTPUT);
  pinMode(PIN_WALK_RED, OUTPUT);
  pinMode(PIN_DRIVE_GREEN, OUTPUT);
  pinMode(PIN_DRIVE_YELLOW, OUTPUT);
  pinMode(PIN_DRIVE_RED, OUTPUT);
  pinMode(PIN_BRIGHTNESS, INPUT);

  // Fußgänger LEDs auf HIGH ausgeschalten
  digitalWrite(PIN_WALK_GREEN_LA, HIGH);
  digitalWrite(PIN_WALK_RED_LA, HIGH);
}

// Geht von 0 bis 5
int ampelIndex = 0;

void loop()
{
    // Byte lesen
    int received_byte = Serial.read();

    if (received_byte == 'B' || digitalRead(PIN_BRIGHTNESS))
    {
        ampelIndex = 0;

        // Gelb blinken
        digitalWrite(PIN_DRIVE_YELLOW, !digitalRead(PIN_DRIVE_YELLOW));
        delay(1000);
    }
    else
    {
        // Ampel
        switch (ampelIndex)
        {
        case 0:
            // LEDs anschalten
            digitalWrite(PIN_DRIVE_GREEN, HIGH);
            digitalWrite(PIN_WALK_RED_LA, LOW);
        
            // LEDs ausschalten
            digitalWrite(PIN_DRIVE_RED, LOW);
            digitalWrite(PIN_DRIVE_YELLOW, LOW);
        
            delay(2000);
            break;
        case 1:
            // LEDs anschalten
            digitalWrite(PIN_DRIVE_YELLOW, HIGH);
        
            // LEDs ausschalten
            digitalWrite(PIN_DRIVE_GREEN, LOW);
        
            delay(800);
            break;
        case 2:
            // LEDs anschalten
            digitalWrite(PIN_DRIVE_RED, HIGH);
        
            // LEDs ausschalten
            digitalWrite(PIN_DRIVE_YELLOW, LOW);
        
            delay(2000);
            break;
        case 3:
            // LEDs anschalten
            digitalWrite(PIN_WALK_GREEN_LA, LOW);
        
            // LEDs ausschalten
            digitalWrite(PIN_WALK_RED_LA, HIGH);
        
            delay(2000);
            break;
        case 4:
            // LEDs anschalten
            digitalWrite(PIN_WALK_RED_LA, LOW);
        
            // LEDs ausschalten
            digitalWrite(PIN_WALK_GREEN_LA, HIGH);
        
            delay(2000);
            break;
        case 5:
            // LEDs anschalten
            digitalWrite(PIN_DRIVE_YELLOW, HIGH);
        
            delay(800);
            ampelIndex = -1;
    }

    ampelIndex++;
}
