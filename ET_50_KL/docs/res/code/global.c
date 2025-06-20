/*
 * Warnung: Boolische True-Werte müssen den Wert 1 haben
 *
 * Zuweisungsliste:
 *
 * Arduino | MCU Pin | Bauteil Pin | Bezeichnung
 * 5V        Vcc
 * GND       Vss
 * 13        PB5       RS            LCD
 * 12        PB4       R/~W          LCD
 * 11        PB3       E             LCD
 * 10        PB2       D0            8 bit databus
 * 9         PB1       D1            8 bit databus
 * 8         PB0       D2            8 bit databus
 * 7         PD7       D3            8 bit databus
 * 6         PD6       D4            8 bit databus
 * 5         PD5       D5            8 bit databus
 * 4         PD4       D6            8 bit databus
 * 3         PD3       D7            8 bit databus
 * 2         PD2       CLK           Datenübernehmen für 7 segment display
 * A0        PC0       S0            Taster in Negativlogik
 * A1        PC1       S1            Taster in Negativlogik
 * A2        PC2       S2            Taster in Negativlogik
 * A3        PC3       Q1            Relais Rechts
 * A4        PC4       Q2            Relais Links
 * A5        PC5
 * */

#define PIN_S0_AUS PC0
#define PIN_S1_RECHTS PC1
#define PIN_S2_LINKS PC2
#define PIN_Q1_RECHTS PC3
#define PIN_Q2_LINKS PC4

#define F_CPU 16000000
