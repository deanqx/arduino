/*
    TI_60_Taster_Check
    Dies ist eine Ampelschaltung. Wenn es dunkel wird
    wechselt es in einen Blink-Modus. Mit den Tastern
    ...

    Pins mit LA endung steht für Low Active,
    sprich wenn es Aktiviert ist, ist das Signal LOW
    bzw. wenn es Aktiviert werden soll, muss das Signal LOW

    Name                AVR-Pin
    PIN_DRIVE_GREEN     PD2
    PIN_DRIVE_YELLOW    PD3
    PIN_DRIVE_RED       PD4
    PIN_SWITCH          PD5
    PIN_SWITCH_LA       PD6
    PIN_BRIGHT          PD7
    PIN_WALK_GREEN_LA   PB2
    PIN_WALK_RED_LA     PB3
*/

// Arduino Pins:
#define PIN_DRIVE_GREEN 2
#define PIN_DRIVE_YELLOW 3
#define PIN_DRIVE_RED 4
#define PIN_SWITCH 5
#define PIN_SWITCH_LA 6
#define PIN_BRIGHT 7
#define PIN_WALK_GREEN_LA 10
#define PIN_WALK_RED_LA 11


void setup()
{
    // Übertragungsrate auf 9600 Baud
    Serial.begin(9600);

    // Senden über Seriell
    Serial.println("Ampel mit Tag und Nachtmodus");

    // Setze Pins auf Ausgang oder Eingang
    pinMode(PIN_DRIVE_GREEN, OUTPUT);
    pinMode(PIN_DRIVE_YELLOW, OUTPUT);
    pinMode(PIN_DRIVE_RED, OUTPUT);
    pinMode(PIN_SWITCH, INPUT);
    pinMode(PIN_SWITCH_LA, INPUT);
    pinMode(PIN_BRIGHT, INPUT);
    pinMode(PIN_WALK_GREEN_LA, OUTPUT);
    pinMode(PIN_WALK_RED_LA, OUTPUT);

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
    if (digitalRead(PIN_SWITCH))
    {
        Serial.println("Taste S1 in Positiv-Logik wurde gedrückt");
        digitalWrite(PIN_DRIVE_RED, HIGH);
    }
    else
    {
        digitalWrite(PIN_DRIVE_RED, LOW);
    }

    if (!digitalRead(PIN_SWITCH_LA))
    {
        Serial.println("Taste S2 in Negativ-Logik wurde gedrückt");
        digitalWrite(PIN_DRIVE_GREEN, HIGH);
    }
    else
    {
        digitalWrite(PIN_DRIVE_GREEN, LOW);
    }

    return;

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
