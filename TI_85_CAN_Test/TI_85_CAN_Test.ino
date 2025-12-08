#include <SPI.h>
#include <mcp2515.h>

MCP2515 mcp2515(10);

static const uint8_t LED = 5;

static const uint16_t ID_DEAN = 0x200;
static const uint16_t ID_TAMMO = 0x400;
static const uint8_t ID_BLINK_16 = 0x00;
static const uint8_t ID_BLINK_5 = 0x01;
static const uint8_t ID_BLINK_10 = 0x02;

void print_can_frame(struct can_frame* frame)
{
    Serial.print(frame->can_id, HEX); // print ID
    Serial.print(" "); 
    Serial.print(frame->can_dlc, HEX); // print DLC
    Serial.print(" ");
    
    for (int i = 0; i < frame->can_dlc; i++)  {  // print the data
      Serial.print(frame->data[i],HEX);
      Serial.print(" ");
    }

    Serial.println();      

}

void send_commands(void)
{
  Serial.println("sende Befehle");

  struct can_frame msg_blink_5 = {
    .can_id  = ID_TAMMO | ID_BLINK_5,
    .can_dlc = 0,
  };

  struct can_frame msg_blink_10 = {
    .can_id  = ID_TAMMO | ID_BLINK_10,
    .can_dlc = 0,
  };

  struct can_frame msg_blink_16 = {
    .can_id  = ID_TAMMO | ID_BLINK_16,
    .can_dlc = 1,
    .data = { 0x0C }, // blink 16 mal
  };
  
  // sende Befehle
  mcp2515.sendMessage(&msg_blink_5);
  print_can_frame(&msg_blink_5);
  delay(3000);
  mcp2515.sendMessage(&msg_blink_10);
  print_can_frame(&msg_blink_10);
  delay(5000);
  mcp2515.sendMessage(&msg_blink_16);
  print_can_frame(&msg_blink_16);
  delay(6000);
}

void setup() {
  pinMode(LED, OUTPUT);
  
  while (!Serial);
  Serial.begin(9600);
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();
  
  Serial.println("TI_85_Arduino_CAN-BUS_Test");

  send_commands();

  while (1)
  {
    struct can_frame received_command;

    Serial.println("Warte auf Befehl...");
    while (mcp2515.readMessage(&received_command) != MCP2515::ERROR_OK);

    Serial.println("Befehl erhalten: ");
    print_can_frame(&received_command);

    if (((uint16_t) (received_command.can_id) & 0xF00) != ID_DEAN)
    {
      continue;
    }

    switch ((uint8_t) (received_command.can_id & 0xFF))
    {
      case ID_BLINK_5:
        Serial.println("blink 5 mal");
        for (uint8_t i = 0; i < 5; i++)
        {
          digitalWrite(LED, HIGH);
          delay(200);
          digitalWrite(LED, LOW);
          delay(200);
        }
        break;
      case ID_BLINK_10:
        Serial.println("blink 10 mal");
        for (uint8_t i = 0; i < 10; i++)
        {
          digitalWrite(LED, HIGH);
          delay(200);
          digitalWrite(LED, LOW);
          delay(200);
        }
        break;
      case ID_BLINK_16:
        if (received_command.can_dlc == 1 && received_command.data[0] == 0x0C)
        {
          Serial.println("blink 16 mal");
          for (uint8_t i = 0; i < 16; i++)
          {
            digitalWrite(LED, HIGH);
            delay(200);
            digitalWrite(LED, LOW);
            delay(200);
          }
        }
        break;
    }
  }
}

void loop() {
}
