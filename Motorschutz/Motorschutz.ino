/*
Motorsteuern mit Arduino Uno R3. Als anstatt von einem
Motor werden LEDs angesteuert.

IN_FUSE muss auf high liegen.
IN_RIGHT schaltet rechtslauf.
IN_LEFT schaltet linkslauf.
IN_STOP schaltet den Motor aus.
*/

// Pin Belegung
#define IN_FUSE 2 
#define IN_STOP 3
#define IN_RIGHT 4
#define IN_LEFT 5
#define OUT_LED_RIGHT 8
#define OUT_LED_LEFT 9

// Digital pin status
bool fuse, stop, right, left;

void setup()
{
  pinMode(IN_FUSE, INPUT); 
  pinMode(IN_STOP, INPUT);
  pinMode(IN_RIGHT, INPUT);
  pinMode(IN_LEFT, INPUT);
  pinMode(OUT_LED_RIGHT, OUTPUT);
  pinMode(OUT_LED_LEFT, OUTPUT);
}

// Alle Pin Eingänge lesen
void read_gpio()
{
  fuse = digitalRead(IN_FUSE);
  stop = digitalRead(IN_STOP);
  right = digitalRead(IN_RIGHT);
  left = digitalRead(IN_LEFT);
}

void loop()
{
  read_gpio();

  if (!fuse)
  {
    digitalWrite(OUT_LED_RIGHT, 0);
    digitalWrite(OUT_LED_LEFT, 0);
    return;
  }

  if (stop)
  {
    digitalWrite(OUT_LED_RIGHT, 0);
    digitalWrite(OUT_LED_LEFT, 0);
  }
  else if (right)
  {
    digitalWrite(OUT_LED_RIGHT, 1);
    digitalWrite(OUT_LED_LEFT, 0);
  }
  else if (left)
  {
    digitalWrite(OUT_LED_RIGHT, 0);
    digitalWrite(OUT_LED_LEFT, 1);
  }
}
