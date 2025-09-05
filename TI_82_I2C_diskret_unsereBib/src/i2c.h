/*
 * Diese Bibliothek ist fuer den TWI/I2C Bus
 * man kann zwischen Internes RegisterModul oder
 * diskreter PIN Ansteuerung auswaehlen
 *
 * i2c.h
 *
 * M.Schmidt 20160630
 *
 * Version 		Aenderung		Status
 * 0.1			Aufsplittung	PCF8575TS Write noch nicht geprüft und
 * esen der Eingänge auch noch nicht angepasst!
 *
 */

#define i2c_baud 100000UL // TWI Baudrate 100kHz
#define TWI_Diskret 1     // 0 = TWI_intern 1 = TWI_Diskret

// I2C Bausteine:
#define PCF8574_TYP 0   // Adresse: 0x40
#define PCF8574A_TYP 1  // Adresse: 0x70
#define PCF8575TS_TYP 2 // Adresse: 0x20
#define PCA9555D_TYP 3  // Adresse: 0x40
#define MCP23016_TYP 4  // Adresse: 0x40
#define MCP23017_TYP 5  // Adresse: 0x40
// I2C EEPROM:
#define EEP_24Cxx_TYP 10 // Adresse: unbekannt

// I2C Bausteine-Adressen:
#define PCF8574_ADR 0x40
#define PCF8574A_ADR 0x70
#define PCF8575TS_ADR 0x20
#define PCA9555D_ADR 0x40
#define MCP23016_ADR 0x40
#define MCP23017_ADR 0x40

#include <avr/io.h>
#include <stdint.h>

#if (TWI_Diskret == 1)
#include <util/delay.h>
#endif

void i2c_init(void) {
#if (TWI_Diskret == 1)
#if (i2c_baud == 100000UL)
#define wartein_u 10
#else
#define wartein_u 100
#endif
#else
  TWBR = (F_CPU / i2c_baud - 16) / 2; // TWI Baudrate 100kHz
  TWSR = 0x00;                        // Schalte Prescaler aus
  TWCR |= (1 << TWEN);                // TWI Enable
#endif
}

#if (TWI_Diskret == 1)
#define i2cPORT PORTB
#define i2cPIN PINB
#define i2cDDR DDRB
#define i2cSCLpin PB0
#define i2cSDApin PB1
#define TW_SLA_ACK 1  // Antwort ADDR vom Slave
#define TW_DATA_ACK 1 // Antwort Data vom Slave
#define TW_WRITE 0
#define TW_READ 1

void takt(void) // Takt generieren
{
  _delay_us(wartein_u);
  i2cPORT &= ~(1 << i2cSCLpin); // SCL auf Low
  _delay_us(wartein_u);
  i2cPORT |= (1 << i2cSCLpin); // SCL auf High
  _delay_us(wartein_u);
  i2cPORT &= ~(1 << i2cSCLpin); // SCL auf Low
  _delay_us(wartein_u);
}

uint8_t readBit(void) // ACK abfragen
{
  uint8_t bit = 0;
  i2cDDR &= ~(1 << i2cSDApin); // Port Pin auf Lesen stellen
  //	i2cPORT |= (1<<i2cSDApin); // SDA auf High
  _delay_us(wartein_u);
  i2cPORT |= (1 << i2cSCLpin); // SCL auf High
  _delay_us(wartein_u);
  if (i2cPIN &
      (1 << i2cSDApin)) // Dauerabfrage des ACK ob die Nachricht komplett ist
    bit = 1;
  _delay_us(wartein_u);
  i2cPORT &= ~(1 << i2cSCLpin); // SCL auf Low
  _delay_us(wartein_u);
  i2cDDR |=
      (1 << i2cSDApin) | (1 << i2cSCLpin); // Port Pin auf schreiben stellen
  return bit;
}

void writeBit(uint8_t bit) {
  if (bit == 1)
    i2cPORT |= (1 << i2cSDApin);
  else
    i2cPORT &= ~(1 << i2cSDApin);
  takt();
}

#endif

uint8_t ACK(void) // ACK abfragen
{
#if (TWI_Diskret == 1)
  uint8_t nack = readBit();
  if (nack)
    return 0;
  else
    return 1;
#else

#endif
}

void i2c_start(void) {
#if (TWI_Diskret == 1)
  i2cPORT |= (1 << i2cSDApin);  // SDA auf High
  i2cPORT |= (1 << i2cSCLpin);  // SCL auf High
  _delay_us(wartein_u);         // Pause
  i2cPORT &= ~(1 << i2cSDApin); // SDA auf Low
  _delay_us(wartein_u);         // Pause
  i2cPORT &= ~(1 << i2cSCLpin); // SCL auf Low
#else
  TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); // send start condition

  while ((TWCR & _BV(TWINT)) == 0)
    ; // wait for transmission
#endif
}

void i2c_stop(void) {
#if (TWI_Diskret == 1)
  i2cPORT &= ~(1 << i2cSDApin); // SDA auf Low
  i2cPORT &= ~(1 << i2cSCLpin); // SCL auf Low
  _delay_us(wartein_u);         // Pause
  i2cPORT |= (1 << i2cSCLpin);  // SCL auf High
  _delay_us(wartein_u);
  i2cPORT |= (1 << i2cSDApin); // SDA auf High
#else
  TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN); // send stop condition
#endif
}

uint8_t i2c_send(uint8_t DataByte) {
#if (TWI_Diskret == 1)
  for (uint8_t i = 8; i > 0; i--) {
    if (DataByte & (1 << (i - 1)))
      writeBit(1);
    else
      writeBit(0);
  }
  return ACK();
#else
  TWDR = DataByte;               // write Databyte
  TWCR = _BV(TWINT) | _BV(TWEN); // clear interrupt to start transmission
  while ((TWCR & _BV(TWINT)) == 0)
    ;       // wait for transmission
  return 1; // ACK erfolgreich
#endif
}

uint8_t i2c_receive(uint8_t ack) {
#if (TWI_Diskret == 1)
  uint8_t daten = 0;
  for (uint8_t i = 8; i > 0; i--) {
    if (readBit())
      daten |= (1 << (i - 1));
  }

  // pull low to send ack
  i2cPORT &= ~(1 << i2cSDApin);
  takt();
  return daten;
#else
  if (ack == 1)
    TWCR = _BV(TWINT) | _BV(TWEA) | _BV(TWEN); // send start condition
  else
    TWCR = _BV(TWINT) | _BV(TWEN); // clear interrupt to start transmission
  while ((TWCR & _BV(TWINT)) == 0)
    ; // wait for transmission
  return TWDR;
#endif
}

void i2c_write(uint8_t basis, uint8_t chipID, uint16_t addr, uint16_t wert,
               uint8_t typ) {
  uint8_t basisAddr = (basis | (chipID << 1));
#if (TWI_Diskret == 1)
  while (1) {
    i2c_start();
    if (i2c_send(basisAddr | TW_WRITE) != TW_SLA_ACK)
      i2c_stop();
    else
      break;
  }
#else
  while (1) {
    //		PORTA=basisAddr;		// Dieser Port wird zur
    // Visulaisierung verwendet LEDs
    i2c_start();
    i2c_send(basisAddr | TW_WRITE); // Pruefen, ob Adressbyte angenommen wurde
    if ((TWSR & 0xF8) != TW_MT_SLA_ACK)
      i2c_stop();
    else
      break;
  }
#endif
  switch (typ) {
  case EEP_24Cxx_TYP:
    i2c_send(addr >> 8);     // bei 16-bit Adresssierung Higher Byte zuerst
    i2c_send(addr & 0x00FF); // oder auch 0xFF	übertragen das Lower Byte
    i2c_send(wert & 0x00FF);
    break;

  case PCF8574_TYP:
  case PCF8574A_TYP:
    i2c_send(wert & 0x00FF); // 16 Bit Wert
    break;

  case PCF8575TS_TYP:
    i2c_send(wert & 0x00FF);          // Low-Byte 16Bit
    i2c_send(((wert & 0xFF00) >> 8)); // High-Byte 16 Bit
    break;

  case PCA9555D_TYP:
  case MCP23016_TYP:
    i2c_send(addr & 0x00FF);          // Low-Byte 16Bit CMD-Register
    i2c_send(wert & 0x00FF);          // Low-Byte 16Bit
    i2c_send(((wert & 0xFF00) >> 8)); // High-Byte 16 Bit
    break;
  case MCP23017_TYP:
    break;
  }
  i2c_stop();
}

uint16_t i2c_read(uint8_t basis, uint8_t chipID, uint16_t cmd, uint8_t typ) {
  uint16_t wert = 0;
  uint8_t dWert[2] = {0};
  uint8_t basisAddr = (basis | (chipID << 1));

#if (TWI_Diskret == 1)
  while (1) {
    i2c_start();
    if (i2c_send(basisAddr | TW_READ) != TW_SLA_ACK)
      i2c_stop();
    else
      break;
  }
#endif

  switch (typ) {
  case EEP_24Cxx_TYP:

    i2c_start();
    i2c_send(basisAddr | TW_READ);
    i2c_send(cmd >> 8); // bei 16-bit Adresssierung HSB zuerst
    i2c_send(cmd);
    i2c_start();
    i2c_send(basisAddr | TW_READ);
    wert = i2c_receive(0);
    break;
  // 8Bit Module mit gleichem Verfahren
  case PCF8574_TYP:
  case PCF8574A_TYP:
    wert = i2c_receive(0);
    break;
  case PCF8575TS_TYP:              // 16Bit Modul
    i2c_start();                   // Start condition
    i2c_send(basisAddr | TW_READ); // Adresse + Read (1)
    dWert[0] = i2c_receive(0);     // Low Byte wird eingelesen
    dWert[1] = i2c_receive(0);     // High Byte wird eingelesen
    i2c_stop();                    // Übertragung wird beendet
    wert = dWert[1];               // Endwert = HighByte
    wert <<= 8;                    // Endwert wird um 1 Byte hochgeshiftet
    wert |= dWert[0];              // LowByte wird zum Endwert hinzugefügt
    break;

  case PCA9555D_TYP: // 16Bit Module mit gleichem Verfahren
  case MCP23016_TYP:
    i2c_start();                    // Start condition
    i2c_send(basisAddr | TW_WRITE); // Adresse + Write (0)
    i2c_send(cmd);                  // Register (command) (z.B. 0x00)
    i2c_start();                    // Start condition
    i2c_send(basisAddr | TW_READ);  // Adresse + Read (1)
    //--> Hier muss eig ein ACK vom Master kommen!
    dWert[0] = i2c_receive(0); // Low Byte wird eingelesen
    i2c_stop();                // Übertragung wird beendet

    i2c_start();                    // Start condition
    i2c_send(basisAddr | TW_WRITE); // Adresse + Write (0)
    i2c_send(cmd | 0x01);           // Register (command) (z.B. 0x01)
    i2c_start();                    // Start condition
    i2c_send(basisAddr | TW_READ);  // Adresse + Read (1)
    //--> Hier muss eig ein ACK vom Master kommen!
    dWert[1] = i2c_receive(0); // High Byte wird eingelesen
    i2c_stop();                // Übertragung wird beendet

    wert = dWert[1];  // Endwert = HighByte
    wert = wert << 8; // Endwert wird um 1 Byte hochgeshiftet
    wert |= dWert[0]; // LowByte wird zum Endwert hinzugefügt
    break;
  case MCP23017_TYP:
    break;
  }
  i2c_stop();
  return wert;
}

uint8_t i2c_sendeByte(uint8_t ADDR, uint8_t DATA) {
#if (TWI_Diskret == 1)
  i2cDDR |= (1 << i2cSDApin) | (1 << i2cSCLpin);  // SDA und SDL schreiben
  i2cPORT |= (1 << i2cSDApin) | (1 << i2cSCLpin); // Pullups aktivieren
  i2c_start();
  // Adresse mit Schreibbit (XXXXXXX0) in Datenregister laden
  if (i2c_send(ADDR & 0xFE) !=
      TW_SLA_ACK) // Wenn kein Slave reagiert, abbrechen
  {
    i2c_stop();
    return 2;
  }
  // Daten in das Datenregister laden
  if (i2c_send(DATA) !=
      TW_DATA_ACK) // Wenn nicht vom Slave akzeptiert abbrechen
  {
    i2c_stop();
    return 3;
  }
#else
  DDRC &= !((1 << PC0) | (1 << PC1)); // SDA und SDL lesen
  PORTC |= (1 << PC0) | (1 << PC1);   // Pullups aktivieren
  i2c_start();
  if ((TWSR & 0xF8) != TW_START)
    return 0; // Wenn keine StartCondition gesendet wurde abbrechen
  i2c_send(ADDR &
           0xFE); // Adresse mit Schreibbit (XXXXXXX0) in Datenregister laden
  if ((TWSR & 0xF8) != TW_MT_SLA_ACK) // Wenn kein Slave reagiert, abbrechen
  {
    i2c_stop();
    return 2;
  }
  i2c_send(DATA); // Daten ind Datenregister laden
  if ((TWSR & 0xF8) !=
      TW_MT_DATA_ACK) // Wenn nicht vom Slave akzeptiert abbrechen
  {
    i2c_stop();
    return 3;
  }
#endif
  i2c_stop();
  return 1;
}

void i2c_init_chip(uint8_t basis, uint8_t chipID, uint8_t cmd, uint8_t wert1,
                   uint8_t wert2, uint8_t typ) {
  uint8_t basisAddr = (basis | (chipID << 1));
  switch (typ) {
  case PCF8575TS_TYP:
    // i2c_write(basisAddr, cmd, wert, typ); // Noch nicht fertig
    break;

  case PCA9555D_TYP:
  case MCP23016_TYP:
    while (1) {
      i2c_start();

#if (TWI_Diskret == 1)
      if (i2c_send(basisAddr | TW_WRITE) ==
          TW_SLA_ACK) // Pruefen, ob Adressbyte angenommen wurde
#else
      i2c_send(basisAddr | TW_WRITE);
      if ((TWSR & 0xF8) ==
          TW_MT_SLA_ACK) // Pruefen, ob Adressbyte angenommen wurde
#endif
        break; // Wenn ja, dann aus der while-Schleife rausgehen
      else
        i2c_stop(); // Wenn nicht, dann i2c stoppen und die Schleife erneut
                    // ausführen
    }
    i2c_send(cmd);
    i2c_send(wert1);
    i2c_send(wert2);
    break;

  case MCP23017_TYP:
    break;
  }
  i2c_stop();
}

uint8_t i2c_slaveRequest(uint8_t basis, uint8_t chipID) {
  uint8_t basisAddr = (basis | chipID);
  i2c_start();

#if (TWI_Diskret == 1)
  if ((i2c_send(basisAddr | TW_WRITE)) ==
      TW_SLA_ACK) // pruefen ob Addressbyte angenommen wurde
#else
  i2c_send(basisAddr | TW_WRITE);
  if ((TWSR & 0xF8) == TW_MT_SLA_ACK) // pruefen ob Addressbyte angenommen wurde
#endif
  {
    i2c_stop();
    return 1;
  } else {
    i2c_stop();
    return 0;
  }
}
