#include <Arduino.h>
#include <LoRa.h>
#include <SPI.h>

//Joystick
#define xPinA D3  // xPin Joystick A
#define yPinA D5  // yPin Joystick A
#define yPinB D6  // yPin Joystick B
#define xPinB D7  // xPin Joystick B

//LoRa
#define DIO0 D3
#define NSS D5
#define frequency 433E6

void sendJoystick();

void setup(){
  Serial.begin(9600);

  LoRa.setPins( NSS , -1 , DIO0 );
  if (!LoRa.begin(frequency)) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true);    
  }
  LoRa.onReceive(onReceive);
  LoRa.onTxDone(onTxDone);
  LoRa_rxMode();
  readJoystick(xPinA,yPinA);
}

void loop(){
  if(runEvery(500))
  {
    sendJoystick();
  }
}

void LoRa_rxMode(){
  LoRa.enableInvertIQ();                // active invert I and Q signals
  LoRa.receive();                       // set receive mode
}

void LoRa_txMode(){
  LoRa.idle();                          // set standby mode
  LoRa.disableInvertIQ();               // normal mode
}

void LoRa_sendMessage(String message) {
  LoRa_txMode();                        // set tx mode
  LoRa.beginPacket();                   // start packet
  LoRa.print(message);                  // add payload
  LoRa.endPacket(true);                 // finish packet and send it
}

void onReceive(int packetSize) {
  String message = "";

  while (LoRa.available()) {
    message += (char)LoRa.read();
  }

  Serial.print("Node Receive: ");
  Serial.println(message);
}

void onTxDone() {
  Serial.println("TxDone");
  LoRa_rxMode();
}

boolean runEvery(unsigned long interval)
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}

int readJoystick(int xPin,int yPin)
{
    static uint16_t lastxA = 0;
    static uint16_t lastyA = 0;
    static uint16_t lastxB = 0;
    static uint16_t lastyB = 0;
    int xValue = analogRead(xPin);
    int yValue = analogRead(yPin);

    if(xPin == xPinA)
    {
      if(xValue == lastxA && yValue == lastyA)
      {
        return 0;
      }
      lastxA = xValue;
      lastyA = yValue;
    }
    else
    {
      if(xValue == lastxB && yValue == lastyB)
      {
        return 0;
      }
      lastxB = xValue;
      lastyB = yValue;
    }
    
    if(xValue < 500)
    {
      Serial.println("Left");
      return 1;
    }
    else if(xValue > 3500)
    {
      Serial.println("Right");
      return 2;
    }
    else if(yValue < 500)
    {
      Serial.println("Up");
      return 3;
    }
    else if(yValue > 3500)
    {
      Serial.println("Down");
      return 4;
    }
}

void sendJoystick(){
  if(readJoystick(xPinA,yPinA)){
    switch (readJoystick(xPinA,yPinA))
    {
      case 1:
        LoRa_sendMessage("Left");
        break;
      case 2:
        LoRa_sendMessage("Right");
        break;
      case 3:
        LoRa_sendMessage("Up");
        break;
      case 4: 
        LoRa_sendMessage("Down");
        break;
      default:
        break;
    }
  }

  if(readJoystick(xPinB,yPinB)){
    switch (readJoystick(xPinB,yPinB))
    {
      case 1:
        LoRa_sendMessage("Left");
        break;
      case 2:
        LoRa_sendMessage("Right");
        break;
      case 3:
        LoRa_sendMessage("Up");
        break;
      case 4: 
        LoRa_sendMessage("Down");
        break;
      default:
        break;
    }
  }
}