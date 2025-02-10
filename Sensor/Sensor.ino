#define LED_BLINK 2
#define LED_RED 3
#define LED_YELLOW 4
#define LED_GREEN 5

void setup()
{
  // Übertragungsrate auf 9600 Baud
  Serial.begin(9600);

  // Warten bis USB angeschlossen ist
  while (!Serial)
  {
    ;
  }

  // Senden über Seriell
  Serial.println("Hallo");

  pinMode(, OUTPUT);
  pinMode(, OUTPUT);
  pinMode(, OUTPUT);
  pinMode(, OUTPUT);
  pinMode(, OUTPUT);
}

void ampel()
{

}

void loop()
{
  // Warten bis etwas geschickt wurde
  if (Serial.available() > 0)
  {
    // Byte lesen
    int received_byte = Serial.read();

    if (received_byte == 'B' || )
    {

    }
  }
}
