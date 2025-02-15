#define PIN_DRIVE_GREEN 2
#define PIN_DRIVE_YELLOW 3
#define PIN_DRIVE_RED 4
#define PIN_BRIGHT 7
#define PIN_WALK_GREEN_LA 10
#define PIN_WALK_RED_LA 11

// Pins für Füßgänger sind Low Active (LA)

void setup()
{
    // Übertragungsrate auf 9600 Baud
    Serial.begin(9600);

    // Senden über Seriell
    Serial.println("Hallo");

    // Setze Pins auf Ausgang oder Eingang 
    pinMode(PIN_WALK_GREEN_LA, OUTPUT);
    pinMode(PIN_WALK_RED_LA, OUTPUT);
    pinMode(PIN_DRIVE_GREEN, OUTPUT);
    pinMode(PIN_DRIVE_YELLOW, OUTPUT);
    pinMode(PIN_DRIVE_RED, OUTPUT);
    pinMode(PIN_BRIGHT, INPUT);

    // Fußgänger LEDs auf HIGH ausgeschalten
    digitalWrite(PIN_WALK_GREEN_LA, HIGH);
    digitalWrite(PIN_WALK_RED_LA, HIGH);
}

// Aktuelle Ampelphase, Geht von 0 bis 5
int ampel_index = 0;
// Gelb blinken
bool blink = false;
int received_byte = 0;

void loop()
{
    // Wurde etwas geschickt?
    if (Serial.available())
    {
        // Byte lesen
        received_byte = Serial.read(); 
    }

    if (received_byte == 'B')
    {
        // Wenn ein 'B' geschickt wurde, dies hat Vorrang vor der Helligkeit
        blink = true;
    }
    else if (digitalRead(PIN_BRIGHT))
    {
        // Wenn es hell ist
        blink = false;
    }
    else
    {
        // Wenn es dunkel ist
        blink = true;
    }

    if (blink)
    {
        // Ampelphase zurücksetzen
        ampel_index = 0;

        // LEDs ausschalten außer Gelb
        digitalWrite(PIN_WALK_GREEN_LA, HIGH);
        digitalWrite(PIN_WALK_RED_LA, HIGH);
        digitalWrite(PIN_DRIVE_GREEN, LOW);
        digitalWrite(PIN_DRIVE_RED, LOW);

        // Gelb blinken
        digitalWrite(PIN_DRIVE_YELLOW, !digitalRead(PIN_DRIVE_YELLOW));
        delay(1000);
    }
    else
    {
        // Ampel
        switch (ampel_index)
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

            // Ampelindex wird auf 0 im nächsten Schritt gemacht
            ampel_index = -1;
        }
    }

    ampel_index++;
}
