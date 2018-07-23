/* SerialUtil library
 * written by Alexander Loquet
 *
 * ---COMMUNICATION PROTOCOL---
 * (1) SENDER
 * (2) RECIEVER
 *
 * (1)				                      (2)
 * STEP 0:        INIT
 * TX ----------------------------- RX
 * STEP 1:	   readyToSent [@]
 * TX ----------------------------> RX
 * STEP 2:        OK [$]
 * RX <---------------------------- TX
 * STEP 3:         DATA
 * TX ----------------------------> RX
 * STEP 4:      endCode [#]
 * TX ----------------------------> RX
 * STEP 5:	      OK [$] 
 * RX <---------------------------- TX
 * STEP 6:	   releaseSent [&]
 * TX ----------------------------> RX
 *
 * TIMEOUT
 * If the sender get no respons (OK [$]) within the configured timeout from the reciever, it will ask again for confirmation.
 * - Step 2 => Step 1: send readyToSent code again
 * - Step 5 => Step 4: send end code again
 */

#include "SerialUtil.h"

//---MARK: CONSTRUCTOR---

/*
 * Default Constructor
 */
SerialUtil::SerialUtil() {
  timeOut = 1000;
  timeChangeCom = 5000;
  sendWaitTime = 50;
  stepNr = 0;
  sendBusy = false;
  sendDone = false;
  recieveBusy = false;
  recieveDone = false;
}

//---MARK: CONFIGURATION METHODS---

/*
 * Set the communication baudrate.
 * @param baudrate: the baudrate
 */
void SerialUtil::setBaudrate(int baudrate) {
  Serial.begin(baudrate);
}

/*
 * Changes the current baudrate the a different baudrate.
 * @param newBaudrate: the new baudrate
 */
void SerialUtil::changeBaudrate(int newBaudrate) {
  Serial.end();
  Serial.begin(newBaudrate);
}

/*
 * Define the start communication mode in which the arduino must start when the arduino powers on.
 * @param comMode: communication start mode
 */
void SerialUtil::startMode(int comMode){
  this->comMode = comMode;
}

/*
 * Define the communication type. They're three different options.
 * - SIMPLEX SEND; only sending data
 * - SIMPLEX RECIEVE: only recieven data
 * - HALF DUPLEX: sending and recieving data
 * @param enalbeSend : enable sending data
 * @param enableRecieve: enable recieving data
 */
void SerialUtil::comType(bool enableSend, bool enableRecieve){
  sendEnabled = enableSend;
  recieveEnabled = enableRecieve;
}


/*
 * Change the wait time after the value is send.
 * @param waitTime: the waittime in milliseconds.
 */
void SerialUtil::changeWaitTime(int waitTime) {
  sendWaitTime = waitTime;
}

/*
 * Set the time of the timeout time.
 * @param timeOut: the timeout time in milliseconds.
 */
void SerialUtil::setTimeOut(long timeOut){
  this->timeOut = timeOut;
}

/*
 * Define the time after the communicaition mode will be swithed.
 * @param timeChangeCom: changeover time
 */
void SerialUtil::setTimeChangeCom(long timeChangeCom){
  this->timeChangeCom = timeChangeCom;
}

//---MARK: THE MAIN METHODE---

/*
 * Method must be defined in the loop the get SerialUtil working.
 * This method determines which mode is active.
 * When a mode is finish. It checks which mode will be active in the next run.
 * @param millis: the current millis passed since startup.
 */
void SerialUtil::loop(long millis){
  //Calculate the delta time since last call
  timeElapsed += (millis - timePrevious);
  timePrevious = millis;

  //Determine the communication mode (send or recieve)
  if (comMode == MODE_SEND){
    sendMode();
  }else{
    recieveMode();
  }

  if(sendEnabled && !recieveEnabled){
    //SIMPLEX: SEND
    if (!sendBusy && sendDone && timeElapsed > timeChangeCom){
      comMode = MODE_SEND;
      stepNr = S_INIT;
      Serial.println("MODE: SEND AGAIN");
    }
  }else if(!sendEnabled && recieveEnabled){
    //SIMPLEX: RECIEVE
    if (!recieveBusy && recieveDone){
      comMode = MODE_RECIEVE;
      stepNr = R_INIT;
      Serial.println("MODE: RECIEVE AGAIN");
    }
  }else{
    //HALF DUPLEX: SEND & RECIEVE
    if (!sendBusy && sendDone && timeElapsed > timeChangeCom){
      comMode = MODE_RECIEVE;
      stepNr = R_INIT;
      sendDone = false;
      Serial.println("MODE: RECIEVE");
    }
  
    if (!recieveBusy && recieveDone && timeElapsed > timeChangeCom){
      comMode = MODE_SEND;
      stepNr = S_INIT;
      recieveDone = false;
      Serial.println("MODE: SEND");
    }
  }
}

//---MARK: SEND MODE---

/*
 * This method handles the sequence of the send mode.
 */
void SerialUtil::sendMode(){
  switch(stepNr){
    case S_INIT:
      Serial.println("Send is STARTED");
      sendBusy = true;
      sendDone = false;
      stepNr += 1;
    case S_RTS:
      Serial.write(CODE_RTS);      
      timeElapsed = 0;
      stepNr += 1;
      break;
    case S_RTS_OK:
      if (Serial.available() > 0){
        if(Serial.read() == CODE_OK){
          stepNr += 1;
        }
      }else{
        if(timeElapsed > timeOut){
          stepNr = S_RTS;
        }
      }
      break;
    case S_DATA:
      sendData();
      stepNr += 1;
      break;
    case S_END_CODE:
      Serial.write(CODE_END_DATA);
      timeElapsed = 0;
      stepNr += 1;
      break;
    case S_END_OK:
      if (Serial.available() > 0){
        if(Serial.read() == CODE_OK){
          stepNr += 1;
        }
      }else{
        if(timeElapsed > timeOut){
          stepNr = S_END_CODE;
        }
      }
      break;
    case S_RS:
      Serial.write(CODE_RS);
      stepNr += 1;
      break;
    case S_END:
      Serial.println("Send is DONE");
      sendBusy = false;
      sendDone = true;
      timeElapsed = 0;
      stepNr += 1;
      break;
    default:
      break;
  }
}

//---MARK: RECIEVE MODE---

/*
 * This method handles the sequence of the recieve mode.
 */
void SerialUtil::recieveMode(){
  switch(stepNr){
    case R_INIT:
      Serial.println("Recieve is STARTED");
      recieveBusy = true;
      recieveDone = false;
      stepNr += 1;
      break;
    case R_RTR:
      if (Serial.available() > 0){
        if(Serial.read() == CODE_RTS){
          stepNr += 1;
        }
      }
      break;
    case R_RTR_OK:
      Serial.write(CODE_OK);
      stepNr += 1;
      break;
    case R_DATA:
      //Time to read data
      if (Serial.available() > 0) {
        delay(10);
        sendCode = Serial.read();
        Serial.print(String(sendCode) + ": ");

        recieveData(sendCode);
        
        if (sendCode == '@') {
          stepNr = R_RTR;
        }
        if (sendCode == '#') {
          stepNr += 1;
        }
      }
      break;
    case R_END_OK:
      Serial.write(CODE_OK);
      timeElapsed = 0;
      stepNr += 1;
      break;
    case R_RR:
      if (Serial.available() > 0){
          if(Serial.read() == CODE_RS){
            stepNr += 1;
          }
      }else{
        if(timeElapsed > timeOut){
          stepNr = R_END_OK;
        }
      }
      break;
     case R_END:
      recieveBusy = false;
      recieveDone = true;
      timeElapsed = 0;
      stepNr += 1;
      Serial.println("Recieve is DONE");
      break;
     default:
      break;
  }
}

//---MARK: SEND VALUE METHODS---

/*
 * This method send a boolean to the other
 * @param code: the unique code which is used to filter at the other side.
 * @param value: the value which will be send.
 */
void SerialUtil::sendBool(char code, bool value) {
  binaryBool.bval = value;
  Serial.write(code);
  Serial.write(binaryBool.b, 1);
  delay(sendWaitTime);
}

/*
 * This method send a integer to the other
 * @param code: the unique code which is used to filter at the other side.
 * @param value: the value which will be send.
 */
void SerialUtil::sendInt(char code, int value) {
  binaryInt.ival = value;
  Serial.write(code);
  Serial.write(binaryInt.b, 2);
  delay(sendWaitTime);
}

/*
 * This method send a float to the other
 * @param code: the unique code which is used to filter at the other side.
 * @param value: the value which will be send.
 */
void SerialUtil::sendFloat(char code, float value) {
  binaryFloat.fval = value;
  Serial.write(code);
  Serial.write(binaryFloat.b, 4);
  delay(sendWaitTime);
}

/*
 * This method send a text to the other
 * @param code: the unique code which is used to filter at the other side.
 * @param text: the text which will be send.
 * @parm length: the length of the text. The length may be greater than the number of characters.
 */
void SerialUtil::sendText(char code, String text, int length) {
  char temp[length];
  text.toCharArray(temp, length);
  Serial.write(code);
  Serial.write(temp, length);
  delay(sendWaitTime);
}


//---MARK: READ VALUE METHODS---

/*
 * This method read a boolean from the other side.
 * @return: value
 */
bool SerialUtil::readBool() {
  binaryBool.b[0] = Serial.read();
  return binaryBool.bval;
}

/*
 * This method read a integer from the other side.
 * @return: value
 */
int SerialUtil::readInt() {
  binaryInt.b[0] = Serial.read();
  binaryInt.b[1] = Serial.read();
  return binaryInt.ival;
}

/*
 * This method read a float from the other side.
 * @return: value
 */
float SerialUtil::readFloat() {
  binaryFloat.b[0] = Serial.read();
  binaryFloat.b[1] = Serial.read();
  binaryFloat.b[2] = Serial.read();
  binaryFloat.b[3] = Serial.read();
  return binaryFloat.fval;
}

/*
 * This method read a text from the other side.
 * @param: the definied length of the string. The amount of characters send with this text.
 * @return: value
 */
String SerialUtil::readText(int length) {
  Serial.readBytes(buff, length);
  return String(buff);
}

//---MARK: CALLBACK METHODS---

/*
 * Attach the send method which will be definied in the Arduino project.
 * This method will be called when it's time to send data.
 * @param sendCb: the send callback method.
 */
void SerialUtil::attachSend(SendCb sendCb){
  this->sendCb = sendCb;
}

/*
 * Attach the recieve method which will be definied in the Arduino project.
 * This method will be called when it's time to recieve data.
 * @param recieveCb: the recieve callback method.
 */
void SerialUtil::attachRecieve(RecieveCb recieveCb){
  this->recieveCb = recieveCb;
}

/*
 * Method is called when the send callback method must be called in th Arduino project.
 */
void SerialUtil::sendData(){
  if(sendCb){
    sendCb();
  }
}

/*
 * Method is called when the recieve callback method must be called in the Arduino project.
 * @param sendCode: the unique code of the recieving data.
 */
void SerialUtil::recieveData(int sendCode){
  if(recieveCb){
    recieveCb(sendCode);
  }
}

