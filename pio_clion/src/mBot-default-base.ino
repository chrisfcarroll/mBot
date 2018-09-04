#include <Servo.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include "mCore.h"
#include "Me2Buzzer.h"
#include "MeDefaultPrograms.h"
#include "Me2MotorControl.h"

MeBuzzer buzzer;
MeLEDMatrix ledMx;
MeRGBLed rgb;
ProgramMode programMode;

void FlashAndBeepStartupPattern();
void process_ir_command(Me2Motor::MotorState motorState);
void serialHandle();
void parseData();
boolean isStart = false;

Me2Motor::MotorState motorState;
MeIR ir;
MeUltrasonic ultrasonicSensor(PORT_3);
MeLineFollower lineFollower(PORT_2);

void setup()
{
    Me2Motor::Stop();
    FlashAndBeepStartupPattern();

    injectDefaultProgramDependencies(motorState, ultrasonicSensor, ir, lineFollower);
    Serial.begin(115200);
    ir.begin();
}

void loop()
{
    while(1)
    {
        process_ir_command(motorState);
        serialHandle();
        switch (programMode)
        {
            case MODE_A:
                modeA();
                break;
            case MODE_B:
                modeB();
                break;
            case MODE_C:
                modeC();
                break;
        }
    }
}

MeTemperature ts;
Me7SegmentDisplay seg;

MeDCMotor MotorL(M1);
MeDCMotor MotorR(M2);
MePort generalDevice;
Servo servo;



typedef struct MeModule
{
    int device;
    int port;
    int slot;
    int pin;
    int index;
    float values[3];
} MeModule;

union{
    byte byteVal[4];
    float floatVal;
    long longVal;
}val;

union{
  byte byteVal[8];
  double doubleVal;
}valDouble;

union{
  byte byteVal[2];
  short shortVal;
}valShort;

MeModule modules[12];
int analogs[8]={A0,A1,A2,A3,A4,A5,A6,A7};

boolean isAvailable = false;
int len = 52;
char buffer[52];
char bufferBt[52];
byte index = 0;
byte dataLen;
byte modulesLen=0;
char serialRead;
String mVersion = "1.2.103";
float angleServo = 90.0;
unsigned char prevc=0;
boolean buttonPressed = false;
double lastTime = 0.0;
double currentTime = 0.0;

#define VERSION 0
#define ULTRASONIC_SENSOR 1
#define TEMPERATURE_SENSOR 2
#define LIGHT_SENSOR 3
#define POTENTIONMETER 4
#define JOYSTICK 5
#define GYRO 6
#define SOUND_SENSOR 7
#define RGBLED 8
#define SEVSEG 9
#define MOTOR 10
#define SERVO 11
#define ENCODER 12
#define IR 13
#define IRREMOTE 14
#define PIRMOTION 15
#define INFRARED 16
#define LINEFOLLOWER 17
#define IRREMOTECODE 18
#define SHUTTER 20
#define LIMITSWITCH 21
#define BUTTON 22
#define DIGITAL 30
#define ANALOG 31
#define PWM 32
#define SERVO_PIN 33
#define TONE 34
#define BUTTON_INNER 35
#define LEDMATRIX 41
#define TIMER 50

#define GET 1
#define RUN 2
#define RESET 4
#define START 5




unsigned char readBuffer(int index){
 return buffer[index]; 
}
void writeBuffer(int index,unsigned char c){
  buffer[index]=c;
}
void writeHead(){
  writeSerial(0xff);
  writeSerial(0x55);
}
void writeEnd(){
 Serial.println(); 
}
void writeSerial(unsigned char c){
 Serial.write(c);
}
void readSerial(){
  isAvailable = false;
  if(Serial.available()>0){
    isAvailable = true;
    serialRead = Serial.read();
  }
}
void serialHandle(){
  readSerial();
  if(isAvailable){
    unsigned char c = serialRead&0xff;
    if(c==0x55&&isStart==false){
     if(prevc==0xff){
      index=1; 
      isStart = true;
     }
    }else{
      prevc = c;
      if(isStart){
        if(index==2){
         dataLen = c; 
        }else if(index>2){
          dataLen--;
        }
        writeBuffer(index,c);
      }
    }
     index++;
     if(index>51){
      index=0; 
      isStart=false;
     }
     if(isStart&&dataLen==0&&index>3){ 
        isStart = false;
        parseData(); 
        index=0;
     }
  }
}
            int px = 0;

void process_ir_command(Me2Motor::MotorState motorState)
{
  static long time = millis();
  if (ir.decode())
  {
    uint32_t value = ir.value;
    time = millis();
    switch (value >> 16 & 0xff)
    {
      case IR_BUTTON_A:
        motorState.moveSpeed = 220;
        programMode = MODE_A;
        Me2Motor::Stop();
        buzzer.tone(NTD1, 300); 
        rgb.clear();
        rgb.setColor(10, 10, 10);
        break;
      case IR_BUTTON_B:
        motorState.moveSpeed = 200;
        programMode = MODE_B;
        Me2Motor::Stop();
        buzzer.tone(NTD2, 300); 
        rgb.clear();
        rgb.setColor(0, 10, 0);
        break;
      case IR_BUTTON_C:
        programMode = MODE_C;
        motorState.moveSpeed = 120;
        Me2Motor::Stop();
        buzzer.tone(NTD3, 300); 
        rgb.clear();
        rgb.setColor(0, 0, 10);
        break;
      case IR_BUTTON_PLUS:
        motorState.current = RUN_F;
        //buzzer.tone(NTD4, 300); 
        rgb.clear();
        rgb.setColor(10, 10, 0);
        Me2Motor::Forward(motorState);
        break;
      case IR_BUTTON_MINUS:
        motorState.current = RUN_B;
        rgb.clear();
        rgb.setColor(10, 0, 0);
        //buzzer.tone(NTD4, 300);
        Me2Motor::Backward(motorState);
        break;
      case IR_BUTTON_NEXT:
        motorState.current = RUN_R;
        //buzzer.tone(NTD4, 300); 
        rgb.clear();
        rgb.setColor(1,10, 10, 0);
        Me2Motor::TurnRight(motorState);
        break;
      case IR_BUTTON_PREVIOUS:
        motorState.current = RUN_L;
        //buzzer.tone(NTD4, 300); 
        rgb.clear();
        rgb.setColor(2,10, 10, 0);
        Me2Motor::TurnLeft(motorState);
        break;
      case IR_BUTTON_9:
        ChangeSpeed(motorState, 9);
        break;
      case IR_BUTTON_8:
        ChangeSpeed(motorState, 8);
        break;
      case IR_BUTTON_7:
        ChangeSpeed(motorState, 7);
        break;
      case IR_BUTTON_6:
        ChangeSpeed(motorState, 6);
        break;
      case IR_BUTTON_5:
        ChangeSpeed(motorState, 5);
        break;
      case IR_BUTTON_4:
        ChangeSpeed(motorState, 4);
        break;
      case IR_BUTTON_3:
        ChangeSpeed(motorState, 3);
        break;
      case IR_BUTTON_2:
        ChangeSpeed(motorState,2);
        break;
      case IR_BUTTON_1:
        ChangeSpeed(motorState,1);
        break;
    }
  }
  else if (millis() - time > 120)
  {
    motorState.current = STOP;
    time = millis();
  }
}

void callOK(){
    writeSerial(0xff);
    writeSerial(0x55);
    writeEnd();
}
void sendByte(char c){
  writeSerial(1);
  writeSerial(c);
}
void sendString(String s){
  int l = s.length();
  writeSerial(4);
  writeSerial(l);
  for(int i=0;i<l;i++){
    writeSerial(s.charAt(i));
  }
}
//1 byte 2 float 3 short 4 len+string 5 double
void sendFloat(float value){ 
     writeSerial(2);
     val.floatVal = value;
     writeSerial(val.byteVal[0]);
     writeSerial(val.byteVal[1]);
     writeSerial(val.byteVal[2]);
     writeSerial(val.byteVal[3]);
}
void sendShort(double value){
     writeSerial(3);
     valShort.shortVal = value;
     writeSerial(valShort.byteVal[0]);
     writeSerial(valShort.byteVal[1]);
     writeSerial(valShort.byteVal[2]);
     writeSerial(valShort.byteVal[3]);
}
void sendDouble(double value){
     writeSerial(5);
     valDouble.doubleVal = value;
     writeSerial(valDouble.byteVal[0]);
     writeSerial(valDouble.byteVal[1]);
     writeSerial(valDouble.byteVal[2]);
     writeSerial(valDouble.byteVal[3]);
     writeSerial(valDouble.byteVal[4]);
     writeSerial(valDouble.byteVal[5]);
     writeSerial(valDouble.byteVal[6]);
     writeSerial(valDouble.byteVal[7]);
}
short readShort(int idx){
  valShort.byteVal[0] = readBuffer(idx);
  valShort.byteVal[1] = readBuffer(idx+1);
  return valShort.shortVal; 
}
float readFloat(int idx){
  val.byteVal[0] = readBuffer(idx);
  val.byteVal[1] = readBuffer(idx+1);
  val.byteVal[2] = readBuffer(idx+2);
  val.byteVal[3] = readBuffer(idx+3);
  return val.floatVal;
}
char _receiveStr[20] = {};
uint8_t _receiveUint8[16] = {};
char* readString(int idx,int len){
  for(int i=0;i<len;i++){
    _receiveStr[i]=readBuffer(idx+i);
  }
  _receiveStr[len] = '\0';
  return _receiveStr;
}
uint8_t* readUint8(int idx,int len){
  for(int i=0;i<len;i++){
    if(i>15){
      break;
    }
    _receiveUint8[i] = readBuffer(idx+i);
  }
  return _receiveUint8;
}
void runModule(int device){
  //0xff 0x55 0x6 0x0 0x2 0x22 0x9 0x0 0x0 0xa
  int port = readBuffer(6);
  int pin = port;
  switch(device){
   case MOTOR:{
     int speed = readShort(7);
     port==M1?MotorL.run(speed):MotorR.run(speed);
   } 
    break;
    case JOYSTICK:{
     int leftSpeed = readShort(6);
     MotorL.run(leftSpeed);
     int rightSpeed = readShort(8);
     MotorR.run(rightSpeed);
    }
    break;
   case RGBLED:{
     int idx = readBuffer(7);
     int r = readBuffer(8);
     int g = readBuffer(9);
     int b = readBuffer(10);
     rgb.setColor(idx,r, g, b);
     rgb.show();
   }
   break;
   case SERVO:{
     int slot = readBuffer(7);
     pin = slot==1?mePort[port].s1:mePort[port].s2;
     int v = readBuffer(8);
     if(v>=0&&v<=180){
       servo.attach(pin);
       servo.write(v);
     }
   }
   break;
   case SEVSEG:{
     if(seg.getPort()!=port){
       seg.reset(port);
     }
     float v = readFloat(7);
     seg.display(v);
   }
   break;
   case LEDMATRIX:{
     if(ledMx.getPort()!=port){
       ledMx.reset(port);
     }
     int action = readBuffer(7);
     if(action==1){
            int brightness = readBuffer(8);
            int color = readBuffer(9);
            int px = readShort(10);
            int py = readShort(12);
            int len = readBuffer(14);
            char *s = readString(15,len);
            ledMx.clearScreen();
            ledMx.setColorIndex(color);
            ledMx.setBrightness(brightness);
            ledMx.drawStr(px,py,s);
         }else if(action==2){
           int brightness = readBuffer(8);
            int dw = readBuffer(9);
            int px = readShort(10);
            int py = readShort(12);
            int len = readBuffer(14);
            uint8_t *ss = readUint8(15,len);
            ledMx.clearScreen();
            ledMx.setColorIndex(1);
            ledMx.setBrightness(brightness);
            ledMx.drawBitmap(px,py,dw,ss);
         }else if(action==3){
            int brightness = readBuffer(8);
            int point = readBuffer(9);
            int hours = readShort(10);
            int minutes = readShort(12);
            ledMx.clearScreen();
            ledMx.setColorIndex(1);
            ledMx.setBrightness(brightness);
            ledMx.showClock(hours,minutes,point);
     }
   }
   break;
   case LIGHT_SENSOR:{
     if(generalDevice.getPort()!=port){
       generalDevice.reset(port);
     }
     int v = readBuffer(7);
     generalDevice.dWrite1(v);
   }
   break;
   case IR:{
     int len = readBuffer(2)-3;
     String s ="";
     for(int i=0;i<len;i++){
       s+=(char)readBuffer(6+i);
     }
     ir.sendString(s);
   }
   break;
   case SHUTTER:{
     if(generalDevice.getPort()!=port){
       generalDevice.reset(port);
     }
     int v = readBuffer(7);
     if(v<2){
       generalDevice.dWrite1(v);
     }else{
       generalDevice.dWrite2(v-2);
     }
   }
   break;
   case DIGITAL:{
     pinMode(pin,OUTPUT);
     int v = readBuffer(7);
     digitalWrite(pin,v);
   }
   break;
   case PWM:{
     pinMode(pin,OUTPUT);
     int v = readBuffer(7);
     analogWrite(pin,v);
   }
   break;
   case TONE:{
     pinMode(pin,OUTPUT);
     int hz = readShort(6);
     if(hz>0){
       buzzer.tone(hz); 
     }else{
       buzzer.noTone(); 
     }
   }
   break;
   case SERVO_PIN:{
     int v = readBuffer(7);
     if(v>=0&&v<=180){
       servo.attach(pin);
       servo.write(v);
     }
   }
   break;
   case TIMER:{
    lastTime = millis()/1000.0; 
   }
   break;
  }
}
void readSensor(int device){
  /**************************************************
      ff    55      len idx action device port slot data a
      0     1       2   3   4      5      6    7    8
      0xff  0x55   0x4 0x3 0x1    0x1    0x1  0xa 
  ***************************************************/
  float value=0.0;
  int port,slot,pin;
  port = readBuffer(6);
  pin = port;
  switch(device){
   case  ULTRASONIC_SENSOR:{
     if(ultrasonicSensor.getPort()!=port){
       ultrasonicSensor.reset(port);
     }
     value = (float)ultrasonicSensor.distanceCm(50000);
     sendFloat(value);
   }
   break;
   case  TEMPERATURE_SENSOR:{
     slot = readBuffer(7);
     if(ts.getPort()!=port||ts.getSlot()!=slot){
       ts.reset(port,slot);
     }
     value = ts.temperature();
     sendFloat(value);
   }
   break;
   case  LIGHT_SENSOR:
   case  SOUND_SENSOR:
   case  POTENTIONMETER:{
     if(generalDevice.getPort()!=port){
       generalDevice.reset(port);
       pinMode(generalDevice.pin2(),INPUT);
     }
     value = generalDevice.aRead2();
     sendFloat(value);
   }
   break;
   case  JOYSTICK:{
     slot = readBuffer(7);
     if(generalDevice.getPort()!=port){
       generalDevice.reset(port);
       pinMode(generalDevice.pin1(),INPUT);
       pinMode(generalDevice.pin2(),INPUT);
     }
     if(slot==1){
       value = generalDevice.aRead1();
       sendFloat(value);
     }else if(slot==2){
       value = generalDevice.aRead2();
       sendFloat(value);
     }
   }
   break;
   case  IR:{
//     if(ir.getPort()!=port){
//       ir.reset(port);
//     }
//      if(irReady){
//         sendString(irBuffer);
//         irReady = false;
//         irBuffer = "";
//      }
   }
   break;
   case IRREMOTE:{
//     unsigned char r = readBuffer(7);
//     if(millis()/1000.0-lastIRTime>0.2){
//       sendByte(0);
//     }else{
//       sendByte(irRead==r);
//     }
//     //irRead = 0;
//     irIndex = 0;
   }
   break;
   case IRREMOTECODE:{
//     sendByte(irRead);
//     irRead = 0;
//     irIndex = 0;
   }
   break;
   case  PIRMOTION:{
     if(generalDevice.getPort()!=port){
       generalDevice.reset(port);
       pinMode(generalDevice.pin2(),INPUT);
     }
     value = generalDevice.dRead2();
     sendFloat(value);
   }
   break;
   case  LINEFOLLOWER:{
     if(generalDevice.getPort()!=port){
       generalDevice.reset(port);
         pinMode(generalDevice.pin1(),INPUT);
         pinMode(generalDevice.pin2(),INPUT);
     }
     value = generalDevice.dRead1()*2+generalDevice.dRead2();
     sendFloat(value);
   }
   break;
   case LIMITSWITCH:{
     slot = readBuffer(7);
     if(generalDevice.getPort()!=port||generalDevice.getSlot()!=slot){
       generalDevice.reset(port,slot);
     }
     if(slot==1){
       pinMode(generalDevice.pin1(),INPUT_PULLUP);
       value = generalDevice.dRead1();
     }else{
       pinMode(generalDevice.pin2(),INPUT_PULLUP);
       value = generalDevice.dRead2();
     }
     sendFloat(value);  
   }
   break;
   case BUTTON_INNER:{
     pin = analogs[pin];
     char s = readBuffer(7);
     pinMode(pin,INPUT);
     boolean currentPressed = !(analogRead(pin)>10);
     sendByte(s^(currentPressed?1:0));
     buttonPressed = currentPressed;
   }
   break;
   case  GYRO:{
//       int axis = readBuffer(7);
//       gyro.update();
//       if(axis==1){
//         value = gyro.getAngleX();
//         sendFloat(value);
//       }else if(axis==2){
//         value = gyro.getAngleY();
//         sendFloat(value);
//       }else if(axis==3){
//         value = gyro.getAngleZ();
//         sendFloat(value);
//       }
   }
   break;
   case  VERSION:{
     sendString(mVersion);
   }
   break;
   case  DIGITAL:{
     pinMode(pin,INPUT);
     sendFloat(digitalRead(pin));
   }
   break;
   case  ANALOG:{
     pin = analogs[pin];
     pinMode(pin,INPUT);
     sendFloat(analogRead(pin));
   }
   break;
   case TIMER:{
     sendFloat(currentTime);
   }
   break;
  }
}

 void parseData() {
     isStart = false;
     int idx = readBuffer(3);
     int action = readBuffer(4);
     int device = readBuffer(5);
     switch (action) {
         case GET: {
             writeHead();
             writeSerial(idx);
             readSensor(device);
             writeEnd();
         }
             break;
         case RUN: {
             runModule(device);
             callOK();
         }
             break;
         case RESET: {
             //reset
             callOK();
         }
             break;
         case START: {
             //start
             callOK();
         }
             break;
     }
 }

 void FlashAndBeepStartupPattern() {
     rgb.setNumber(16);
     rgb.clear();
     rgb.setColor(10, 0, 0);
     buzzer.tone(NTD1, 300);
     delay(100);
     rgb.setColor(0, 20, 0);
     buzzer.tone(NTD2, 300);
     delay(400);
     rgb.setColor(0, 0, 10);
     buzzer.tone(NTD3, 300);
     delay(100);
     rgb.clear();
     buzzer.noTone();
 }
