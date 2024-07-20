
#include <SPI.h>
#include <Ethernet.h>


#include <Wiegand.h>
#include <ArduinoRS485.h>  // ArduinoModbus depends on the ArduinoRS485 library
#include <ArduinoModbus.h>
WIEGAND wg;
//pin input output

const int ledPin = 13;
const int slave = 22;
const int relay=52
// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
// The IP address will be dependent on your local network:
//ethernettcp
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 0, 32);

EthernetServer ethServer(502);

ModbusTCPServer modbusTCPServer;
//tombol
const int buttonPin = 52;
int buttonState = 0;
long int key = 14008085;
void setup() {
  delay(1000);
  pinMode(ledPin, OUTPUT);
  pinMode(slave, OUTPUT);

  digitalWrite(ledPin, HIGH);
  Serial1.begin(9600);
  Serial.begin(9600);
  Serial.println("B");
  Ethernet.init(8);
  pinMode(buttonPin, INPUT);
  Ethernet.begin(mac, ip);
  ethServer.begin();
  wg.begin(18, 19);
  modbusTCPServer.begin();
  // configure a single coil at address 0x00
  modbusTCPServer.configureCoils(0x00, 3);
  modbusTCPServer.coilWrite(0, 0);
  modbusTCPServer.coilWrite(1, 0);
  modbusTCPServer.coilWrite(2, 0);
}

void loop() {
  digitalWrite(slave, LOW);
  delay(100);
  Serial1.println("A");
  Serial.println("A");
  digitalWrite(slave, LOW);
  delay(100);

  EthernetClient client = ethServer.available();

  if (client) {
    // a new client connected
    Serial.println("new client");

    // let the Modbus TCP accept the connection
    modbusTCPServer.accept(client);

    while (client.connected()) {
      // poll for Modbus TCP requests, while client connected
      modbusTCPServer.poll();
      if (wg.available()) {


        Serial.print("Wiegand HEX = ");
        Serial.print(wg.getCode(), HEX);
        Serial.print(", DECIMAL = ");
        Serial.print(wg.getCode());
        Serial.print(", Type W");
        Serial.println(wg.getWiegandType());
        if (wg.getCode() == key) {
          open();
          delay(1000);
          digitalWrite(ledPin, LOW);
        }
      }

      //open dengan tombol
      //jika tombol ditekan akan menulis reg 0 ke nilai 1 untuk tanda ke komputer bahwa ada penekanan tombol
      //dan nantinya komputer akan memeperintah printer untuk mencetak karcis
      buttonState = digitalRead(buttonPin);
      Serial.println(buttonState);
      delay(300);
      // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
      if (buttonState == LOW) {
        Serial.println("bhigh");

        modbusTCPServer.coilWrite(0, 1);

        // put your main code here, to run repeatedly:
      }
      int d = modbusTCPServer.coilRead(1);
      if (d == 1) {
        Serial.println("buka");
        modbusTCPServer.coilWrite(0, 0);
        modbusTCPServer.coilWrite(1, 0);

        open();
        delay(1000);
        digitalWrite(ledPin, LOW);
      }


      // update the LED
    }

    Serial.println("client disconnected");
  }
}

//openbariergate
void open() {
  digitalWrite(ledPin, HIGH);


  Serial1.println("halo"); 
  Serial1.write(0xfd);
  Serial1.write(0x00);
  Serial1.write(0x02);
  Serial1.write(0x03);
  Serial1.write(0xfd);
  Serial1.write(0xfa);
}
