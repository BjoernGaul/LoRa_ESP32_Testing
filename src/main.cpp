#include <Arduino.h>
#include <LoRa.h>
#include <SPI.h>

//Joystick
#define xPinA A3  // xPin Joystick A
#define yPinA A2  // yPin Joystick A
#define yPinB A0  // yPin Joystick B
#define xPinB A1  // xPin Joystick B

//LoRa
#define DIO0 D3
#define NSS D7
#define frequency 433E6

void sendJoystick(int joypos);
void onReceive(int packetSize);
void onTxDone();
void LoRa_rxMode();
void LoRa_txMode();
void LoRa_sendMessage(String message);
boolean runEvery(unsigned long interval);
int readJoystick(int xPin,int yPin);
void manageSend();

int joyLeft = 0;
int joyRight = 0;
int taskLeft = 0;
int taskRight = 0;

void setup(){
  Serial.begin(9600);
  delay(1000);
  Serial.println("LoRa Start");
  LoRa.setPins( NSS , -1 , DIO0 );
  if (!LoRa.begin(frequency)) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true);    
  }
  LoRa.onReceive(onReceive);
  LoRa.onTxDone(onTxDone);
  LoRa_rxMode();
  Serial.println("LoRa init succeeded.");
}

void loop(){
  if(runEvery(1000))
  {
    //Serial.println("Read JoystickA");
    joyLeft = readJoystick(xPinA,yPinA);
    joyLeft += 10;
    //Serial.println("Read JoystickB");
    joyRight = readJoystick(xPinB,yPinB);
    joyRight += 20;

    manageSend();

  }
}

void LoRa_rxMode(){
  //Serial.println("LoRa_rxMode");
  LoRa.enableInvertIQ();                // active invert I and Q signals
  LoRa.receive();                       // set receive mode
}

void LoRa_txMode(){
  //Serial.println("LoRa_txMode");
  LoRa.idle();  
  //Serial.println("idle done");          // set standby mode;
  LoRa.disableInvertIQ();              // normal mode
  //Serial.println("disableInvertIQ done");
}

void LoRa_sendMessage(String message) {
  LoRa_txMode();                       // set tx mode
  //Serial.println("Begin message");
  LoRa.beginPacket();                   // start packet
  //Serial.println("Begin packet done");
  LoRa.print(message);                  // add payload
  //Serial.println("Print done");
  LoRa.endPacket(false);                 // finish packet and send it
  //Serial.println("End packet done");
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
    int xValue = analogRead(xPin);
    int yValue = analogRead(yPin);

    Serial.print("xPin: ");
    Serial.print(xPin);
    Serial.print(" yPin: ");
    Serial.println(yPin);

    Serial.print("xValue: ");
    Serial.print(xValue);
    Serial.print(" yValue: ");
    Serial.println(yValue);

    if((xValue < 1400)&&(yValue > 1900))
    {
      Serial.println("Left");
      return 1;
    }
    else if((xValue > 2000)&&(yValue < 1400))
    {
      Serial.println("Right");
      return 2;
    }
    else if((yValue > 2500)&&(xValue < 2800))
    {
      Serial.println("Down");
      return 3;
    }
    else if((xValue > 2500)&&(yValue > 1950))
    {
      Serial.println("Up");
      return 4;
    }else{
      return 0;
    }
}

void sendJoystick(int joypos){

  Serial.println(joypos);

  LoRa_sendMessage(String(joypos));
}

void manageSend()
{
  if(!taskLeft)
  {
    sendJoystick(joyLeft);
    taskLeft = joyLeft;
  }else if(taskLeft != joyLeft)
  {
    taskLeft = 0;
  }
  
  if(!taskRight)
  {
    sendJoystick(joyRight);
    taskRight = joyRight;
  }else if(taskRight != joyRight)
  {
    taskRight = 0;
  }
}