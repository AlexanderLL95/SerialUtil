/* SerialUtil library
  written by Alexander Loquet
*/
#ifndef SerialUtil_h
#define SerialUtil_h

#include "Arduino.h"

/*
 * --- Send codes ---
 *  @ => ReadyToSent
 *  $ => OK
 *  # => End code
 *  & => ReleaseSent
 */
#define CODE_RTS '@'
#define CODE_OK '$'
#define CODE_END_DATA '#'
#define CODE_RS '&'

#define S_INIT 0
#define S_RTS 1
#define S_RTS_OK 2
#define S_DATA 3
#define S_END_CODE 4
#define S_END_OK 5
#define S_RS 6
#define S_END 7

#define R_INIT 0
#define R_RTR 1
#define R_RTR_OK 2
#define R_DATA 3
#define R_END_OK 4
#define R_RR 5
#define R_END 6

/*
 * --- Send Mode ---
 *  0 => Recieve mode is active
 *  1 => Sende mode is active
 *
 */
#define MODE_SEND 1
#define MODE_RECIEVE 0

union {
  byte b[4];
  float fval;
} binaryFloat;

union {
  byte b[2];
  int ival;
} binaryInt;

union {
  byte b[1];
  bool bval;
} binaryBool;

typedef void (*SendCb) ();
typedef void (*RecieveCb) (int sendCode);

class SerialUtil {
  public:
    SerialUtil();
    void startMode(int comMode);
    void comType(bool enableSend,bool enableRecieve);
    void loop(long millis);   
    void setBaudrate(int baudrate);
    void changeBaudrate(int newBaudrate);
    void sendBool(char code, bool value);
    bool readBool();
    void sendInt(char code, int value);
    int readInt();
    void sendFloat(char code, float value);
    float readFloat();
    void sendText(char code, String text, int length);
    String readText(int length);
    void changeWaitTime(int waitTime);
    void setTimeOut(long timeOut);
    void setTimeChangeCom(long timeChangeCom);
    void attachSend(SendCb send);
    void attachRecieve(RecieveCb recieve);
  private:
    void sendMode();
    void recieveMode();
    void sendData();
    void recieveData(int sendCode);
    char sendCode;  
    char buff[100];
    int sendWaitTime;
    int stepNr;
    int comMode;    
    long timeOut;
    long timeChangeCom;
    long timePrevious;
    long timeElapsed;
    bool sendBusy;
    bool sendDone;
    bool recieveBusy;
    bool recieveDone;
    bool sendEnabled;
    bool recieveEnabled;
    SendCb sendCb;
    RecieveCb recieveCb;
};

#endif

