#include "SerialUtil.h"

SerialUtil mySerial;

bool valueBool;
int valueInt;
float valueFloat;
String valueStr;

void setup() {
  // Begin the Serial at 9600 Baud
  mySerial.setBaudrate(19200);
  mySerial.changeWaitTime(20);
  mySerial.startMode(MODE_SEND);
  mySerial.comType(true,true);
  mySerial.attachSend(sendMyData);
  mySerial.attachRecieve(recieveMyData);
  mySerial.setTimeOut(2000);
  mySerial.setTimeChangeCom(5000);
  Serial.println("End setup");
}

void loop() {
  mySerial.loop(millis());  
}

void sendMyData(){
  //Time to send data    
  mySerial.sendFloat('A', 20.6);
  mySerial.sendFloat('B', 10.8);
  mySerial.sendFloat('C', 123.9);
  mySerial.sendFloat('D', 325.0);
  mySerial.sendFloat('E', 129.2);
  mySerial.sendFloat('F', 35.2);
  mySerial.sendFloat('G', 67.9);
  mySerial.sendFloat('H', 23.1);
  mySerial.sendFloat('I', 1.1);
  mySerial.sendFloat('J', 21.7);

  mySerial.sendInt('K', 10);
  mySerial.sendInt('L', 20);
  mySerial.sendInt('M', 30);
  mySerial.sendInt('N', 40);
  mySerial.sendInt('O', 50);

  mySerial.sendBool('P', false);
  mySerial.sendBool('Q', true);
  mySerial.sendBool('R', false);
  mySerial.sendBool('S', true);

  mySerial.sendText('T', "Dit is een test van A naar B", 50);
  mySerial.sendText('T', "Dit is nog een andere test", 50);
}

void recieveMyData(int sendCode){
  if (sendCode == 'a' || sendCode == 'b' || sendCode == 'c') {
    valueFloat = mySerial.readFloat();
    Serial.println(valueFloat);
  }
  else if (sendCode == 'k') {
    valueInt = mySerial.readInt();
    Serial.println(valueInt);
  }
  else if (sendCode == 'p') {
    valueBool = mySerial.readBool();
    Serial.println(valueBool);
  }
  else if (sendCode == 't') {
    valueStr = mySerial.readText(50);
    Serial.println(valueStr);
  }
}

