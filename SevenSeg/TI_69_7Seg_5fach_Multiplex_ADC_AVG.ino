/* TI_68_7Seg_5fach_Multiplexing_ADC
 * ZWL für die Anschlussbelegung
 * Siebensegment Anzeige 5611AS (5161AS) mit gemeinsamer Kathode
 * Positivlogik RV = 220Ohm pro LED
 * Bauteilkomponente  BauteilPin  ArduinoPin  MC_Pin
 * COM Kathode        3 , 8         GND       GND
 * Seg A                7           5         PD5
 * Seg B                6           6         PD6
 * Seg C                4           7         PD7
 * Seg D                2           8         PB0
 * Seg E                1           9         PB1
 * Seg F                9           10        PB2
 * Seg G                10          11        PB3
 * Seg DP               5           12        PB4
 * 
 * Transistor Multiplexing
 * Einer                            2         PD2
 * Zehner                           3         PD3
 * Hunderter                        4         PD4
 * Tausender                        A0        PC0
 * Zehntausnder                     A1        PC1
 * 
 * Vorzeichen
 * LED inkl. Vorwiderstand          A5        PC5
 *  
 * Analogwert erfassen 
 * PC3                              A3        PC3
 * 
 *  Aufgabe:  Erstellen Sie die Funktion ausgabe mit folgender Funktion:
 *            werte von 0 - 9 sollen jeweils auf der 7 Segmentanzeige den Wert 0 - 9 anzeigen.
 *            Jede Aktion wird per UART an den PC übertragen.
 *            
 * Anwendung von Registern des AVR ATMEGA328p
 *            
 */

#define info "TI_68_7Seg_5fach_Multiplexing_ADC"
#define a   5
#define b   6
#define c   7
#define d   8
#define e   9
#define f   10
#define g   11
#define dp  12

#define maxArray 11
//uint8_t
int zahl=0;
uint8_t schiebe=1; // LED-Segment auswählen

// databyte => bit  7 6 5 4 3 2 1 0
//                  a b c d e f g dp
uint8_t databyte[maxArray]={0x3F,0x06,0x5b,0x4F,0x66,0x6d,0x7d,0x07,0x7F,0x6F,0x00};

int32_t Zahl = 52401;
uint8_t E=0,Z=0,H=0,T=0,ZT=0;
uint8_t d_p=0;


void setup() 
{
  // Festlegen der Portpinfunktion
  //  pinMode(a, OUTPUT);   // P
  //  pinMode(b, OUTPUT);   // P
  //  pinMode(c, OUTPUT);   // P
  DDRD=0b11111100;  // PD5,PD6,PD7 = Ausgang   ; PD0 - PD4 = Eingang
  //  pinMode(d, OUTPUT);   // P
  //  pinMode(e, OUTPUT);   // P
  //  pinMode(f, OUTPUT);   // P
  //  pinMode(g, OUTPUT);   // P
  //  pinMode(dp, OUTPUT);  // P
  DDRB=0b00011111;  // PB0 - PB4 = Ausgang        ; PB5 - PB7 = Eingang
  DDRC=0x23;        // PC0 - PC1, PC5 = Ausgang   ; PC2 - PC4 = Eingang  
  Serial.begin(115200); // highspeed UART Übertragung per USB
  while(!Serial);       // Warte bis Serielle Schnittstelle aktiv ist
  delay(2000);
  Serial.println(info);    
}

void AnzeigeOFF()   // alle Transitoren sind auszuschalten
{
  digitalWrite(2,LOW);  digitalWrite(3,LOW); digitalWrite(4,LOW); digitalWrite(A0,LOW);  digitalWrite(A1,LOW);
}

void umsetzungsTabelle(uint8_t input,uint8_t DP)
{   
    switch(input)
    {  
      case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9:  
                // Zuweisen der LED Segmente der Portpins
                PORTB  = (databyte[input] >> 3); // Schiebe den Inhalt vom Array
                PORTD  = (databyte[input] << 5); // Schiebe den Inhalt vom Array     
        break;
      default:  // Zahlen die nicht 0 - 9 sind werden hier ausgewertet und 
                // es wird die Dekade um ein nach links verschoben
                PORTB = 0; // Alles aus
                PORTD = 0; // Alles aus
                zahl = -1; // Wieder von vorn starten                
                if(schiebe > 8 || schiebe==0) 
                  schiebe=1;
                else
                  schiebe <<= 1;
    }
    if(DP) 
    {
      PORTB = PORTB | (1<<PB4); // Schalte DP ein und lasse den Rest so wie er ist
    }
    else
    {
      PORTB = PORTB & ~(1<<PB4); // Schalte DP aus und lasse den Rest so wie er ist
    }    
    switch(schiebe) // Gebe sie einzelne Stell auf der ausgefählten Anzeige aus 
    {
      case 1: // Einerstelle 
              digitalWrite(2,HIGH);  digitalWrite(3,LOW); digitalWrite(4,LOW); 
              digitalWrite(A0,LOW);  digitalWrite(A1,LOW);
        break;
      case 2: // Zehnerstelle 
              digitalWrite(2,LOW);  digitalWrite(3,HIGH); digitalWrite(4,LOW);
              digitalWrite(A0,LOW);  digitalWrite(A1,LOW);
        break;
      case 4: // Hunderterstelle 
              digitalWrite(2,LOW);  digitalWrite(3,LOW); digitalWrite(4,HIGH);
              digitalWrite(A0,LOW);  digitalWrite(A1,LOW);
        break;
      case 8: // Tausenderstelle 
              digitalWrite(2,LOW);  digitalWrite(3,LOW); digitalWrite(4,LOW);
              digitalWrite(A0,HIGH);  digitalWrite(A1,LOW);
        break;
      case 16: // Zehntausenderstelle 
               digitalWrite(2,LOW);  digitalWrite(3,LOW); digitalWrite(4,LOW);
               digitalWrite(A0,LOW);  digitalWrite(A1,HIGH);
        break;
    }
}

/*
 * @param channel Der PortPin des Analogpins.
 * @param samples Die Anzahl der Messung für die Mittelwertsbildung.
 * */
uint16_t analogReadAVG(uint8_t channel, uint8_t samples)
{
  uint16_t mittelwert = 0;
  uint32_t sum = 0;

  for (uint8_t i = 0; i < samples; i++)
  {
    sum += analogRead(channel);
  }
  
  mittelwert = sum / samples;

  return mittelwert;
}

void messwertAusgabe(int16_t messwert)
{
  if(messwert < 0)  // Prüfen ob es eine negative Zahl ist
  {   // Schalte die LED bei negativer Zahl ein
      PORTC |= (1<<PC5);   // PORTC = PORTC | (1<<PC5);     // PC5 = 5
      messwert *= (-1);
  }
  else
  {   // Schalte die LED bei einer positiven Zahl aus
      PORTC &= ~(1<<PC5);   // PORTC = PORTC & ~(1<<PC5);   // PC5 = 5
  }
  // für die Anzeige müssen die Dezimalstellen zuvor auf 0 gesetzt werden.
  ZT=0; T=0; H=0; Z=0; E=0;
  if(messwert > 9999)
  { 
    ZT = messwert/10000;  
    messwert -= uint32_t(ZT) * 10000;      
  }  
  if(messwert > 999) { T  = messwert/1000;   messwert = messwert -(T * 1000);}
  if(messwert > 99)  { H  = messwert/100;    messwert = messwert -(H * 100);}
  if(messwert > 9)   { Z  = messwert/10;     messwert = messwert -(Z * 10); }
  E = messwert;

  // Nullen vor der ersten Stelle Ausschalten

  schiebe = 1;
  umsetzungsTabelle(E,0);   delay(1);  AnzeigeOFF();
  schiebe <<= 1;
  umsetzungsTabelle(Z,0);   delay(1);  AnzeigeOFF();
  schiebe <<= 1;
  umsetzungsTabelle(H,0);   delay(1);  AnzeigeOFF();
  schiebe <<= 1;
  umsetzungsTabelle(T,0);   delay(1);  AnzeigeOFF();
  schiebe <<= 1;
  umsetzungsTabelle(ZT,0);  delay(1);  AnzeigeOFF();
}

void loop()  // Zerlegung des eingelesenen Analogwertes in die einzelnen Potenzen (Dekaden)
{
  // A3 entspricht dem Ditigalen PIN 17  des Arduino und hat nichts mit dem MC direkt zu tun
  //  Serial.print(A3); Serial.print("  <= A3    PC3 => ");  Serial.println(PC3);
  Zahl = analogReadAVG(A3, 10);   
  Serial.println(Zahl);

  messwertAusgabe(Zahl);
} 
