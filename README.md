# SerialUtil
Arduino library for sending and recieving data between arduino's

## How Does it Work? ##

**Inlcude**

When you want to use the SerialUtil class, you have to include the SerialUtil header file at the top of youre Arduino sketch.

```
#include "SerialUtil.h"
```

**Declaration**

You have to declare a variable of the SerialUtil class.

```
SerialUtil mySerial;
```

**SETUP**

In the setup method you have to configure you're SerialUtil variable.
You have to define at least the following methods to get the SerialUtil class working:

  - **setBaudrate(int baudrate)**: ex. 9600, 19200, ...
  - **startMode(startMode)**: MODE_SEND or MODE_RECIEVE
  - **comType(enableSend, enalbeRecieve)**: ex. comTyp(true,false) => only send data
  - **attachSend(callback method)**: attach send callback when you want to send data
  - **attachRecieve(callback method)**: attach recieve callback when you want to recieve data

In the next example codes, both sending and recieving are configured.
If you only want to send data from one to another. It's enough to configere the sending part at one sketch and the recieving part in the other sketch.

```
void setup() {
  // Configure the baudrate of the serial communication.
  mySerial.setBaudrate(19200);
  // Configure the wait time between the different data sends (a little pause).
  mySerial.changeWaitTime(20);
  // Configure the start communication mode.
  // You can't define the same start mode in both Arduino sketches.
  mySerial.startMode(MODE_SEND);
  // Configure the communication type: send, recieve or send and recieve.
  // .comType(enableSend, enableRecieve)
  mySerial.comType(true,true);
  // Configure the timeout time for a new request.
  mySerial.setTimeOut(2000);
  // Configure the time when the communication mode change from send to recieve or from recieve to send.
  mySerial.setTimeChangeCom(5000);
  
  // Configure the send callback method.
  mySerial.attachSend(sendMyData);
  // Confige the recieve callback method.
  mySerial.attachRecieve(recieveMyData);
}
```

**LOOP**

In the loop method you have to call the loop method of the SerialUtil class.

```
void loop() {
  ...
  mySerial.loop(millis()); 
  ...
}
```

**Callbacks**

When it's time to send the data to the other Arduino. The SerialUtil class calls the method you attachted in de setup.
You can use the **sendFloat**, **sendInt**, **sendBool** and **sendText** method to send youre data to the other Arduino.
You have to give always a unique code to youre data.

```
void sendMyData(){
  ...   
  mySerial.sendFloat('a', 20.6);
  mySerial.sendFloat('b', 10.8);
  mySerial.sendFloat('c', 123.9);

  mySerial.sendInt('k', 10);

  mySerial.sendBool('p', false);

  mySerial.sendText('t', "Dit is een test van A naar B", 50);
  ...
}
```

The library send a callback to you Arduino sketch when the SerialUtil class recieves the data.
The callback method has as parameter a unique send code. This is the same unique send code you sended with youre data.
Call the properly method **readFloat**,**readInt**,**readBool** or **readText** to get youre value.

```
void recieveMyData(int sendCode){
  ...
  if (sendCode == 'a' || sendCode == 'b' || sendCode == 'c') {
    valueFloat = mySerial.readFloat();
  }
  else if (sendCode == 'k') {
    valueInt = mySerial.readInt();
  }
  else if (sendCode == 'p') {
    valueBool = mySerial.readBool();
  }
  else if (sendCode == 't') {
    valueStr = mySerial.readText(50);
  }
  ...
}
```


