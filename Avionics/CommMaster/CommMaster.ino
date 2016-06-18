//For engine shutdown, can have multiple pins to check inputs, or just use an
//interrupt on pins 2 & 3, or use combination. Right now based on 2/3 pin high confirmation
//Need to add servo/stepper motor code
//Countdown not real time (runs slow) if it matters

//Commands
//r - readys launch
//f - final launch ready
//l - LAUNCH
//a - abort
//s - returns state of variables

//Commands Meant for DAQ
//i - initialization: initializes SD card
//t - sensor test: sends sensor values ten times over 5 seconds
//c - collect data: collects data continuously. outputs over serial and saves. Also checks temp
//to make sure it's not too hot. Command to make pins go high to signal heat event are commented out
//e - exit data collect

#include <SoftwareSerial.h>

int baud = 9600;

int rxPin = 10;
int txPin = 11;
SoftwareSerial mySerial(rxPin, txPin);

int engineTempPin1 = 3;
int engineTempPin2 = 4;
int engineTempPin3 = 5;
int imuAbortPin1 = A0;
int imuAbortPin2 = A1;
int imuAbortPin3 = A2;

boolean engineTempPin1State = 0;
boolean engineTempPin2State = 0;
boolean engineTempPin3State = 0;
boolean imuAbortPin1State = 0;
boolean imuAbortPin2State = 0;
boolean imuAbortPin3State = 0;

char inputCMD;
String data;
String msg;

boolean launchReady = 0;
boolean finalLaunchReady = 0;
boolean launchTime = 0;
boolean dontAbort = 1;
String abortLaunch;


void setup() {
  mySerial.begin(baud);
  Serial.begin(baud);
  
  pinMode(engineTempPin1, INPUT);
  pinMode(engineTempPin2, INPUT);
  pinMode(engineTempPin3, INPUT);
  pinMode(imuAbortPin1, INPUT);
  pinMode(imuAbortPin2, INPUT);
  pinMode(imuAbortPin3, INPUT);
  
}

void loop() {

  inputCMD = Serial.read();
  if (mySerial.available())
  {
    data = mySerial.readStringUntil('\n');
    Serial.println(data);
  }

  switch(inputCMD){
  case 'a':
    launchTime = 0;
    launchReady = 0;
    finalLaunchReady = 0;
    Serial.println("xCMabo");
    break;
  case 'i':
    mySerial.println('i');
    Serial.println("CM: i passed");
    break;
  case 't':
    mySerial.println('t');
    Serial.println("CM: t passed");
    break;
  case 'c':
    mySerial.println('c');
    Serial.println("CM: c passed");
    break;
  case 'e':
    mySerial.println('e');
    Serial.println("CM: e passed");
    break;
  case 'r':
    launchReady = 1;
//    Serial.println("First Launch Ready is " + launchReady);
    Serial.println("xCMrl11");
    break;
  case 'f':
    finalLaunchReady = 1;
//    Serial.println("Second Launch Ready is " + finalLaunchReady);
    Serial.println("xCMrl21");
    break;
  case 'u':
    launchReady = 0;
//    Serial.println("First Launch Ready is " + launchReady);
    Serial.println("xCMrl10");
    break;
  case 'j':
    finalLaunchReady = 0;
//    Serial.println("Second Launch Ready is " + finalLaunchReady);
    Serial.println("xCMrl20");
    break;
  case 'l':
    if (launchReady && finalLaunchReady)
    {
//      countdown();
      Serial.println("xCMlau");
      launchTime = 1;
    }
    else
    {
      Serial.println("CM: LR1 and LR2 not ready");
    }
    break;
//  case 's':
//    Serial.println("Laucnh Ready is: " + String(launchReady, DEC) + " Final Launch Ready is: " + String(finalLaunchReady, DEC));
//    
//    engineTempPin1State = digitalRead(engineTempPin1);
//    engineTempPin2State = digitalRead(engineTempPin2);
//    engineTempPin3State = digitalRead(engineTempPin3);
//    msg = "Temp Abort Pin1: " + String(engineTempPin1State);
//    msg = msg + " Temp Abort Pin2: " + String(engineTempPin2State);
//    msg = msg + " Temp Abort Pin3: " + String(engineTempPin3State);
//    Serial.println(msg);
//
//    imuAbortPin1State = digitalRead(imuAbortPin1);
//    imuAbortPin2State = digitalRead(imuAbortPin2);
//    imuAbortPin3State = digitalRead(imuAbortPin3);
//    msg = "IMU Abort Pin1: " + String(imuAbortPin1State);
//    msg = msg + " IMU Abort Pin2: " + String(imuAbortPin2State);
//    msg = msg + " IMU Abort Pin3: " + String(imuAbortPin3State);
//    Serial.println(msg);
//
//    break;
  default:
    break;
  }

  if (launchReady && finalLaunchReady && launchTime)
  {
    launch();
  }

}

void launch()
{
  //turn Stepper
  //turn Servo
  Serial.end();
  while (dontAbort)
  {
    int countTemp = 0;
    int countIMU = 0;
    
    engineTempPin1State = digitalRead(engineTempPin1);
    engineTempPin2State = digitalRead(engineTempPin2);
    engineTempPin3State = digitalRead(engineTempPin3);
    imuAbortPin1State = digitalRead(imuAbortPin1);
    imuAbortPin2State = digitalRead(imuAbortPin2);
    imuAbortPin3State = digitalRead(imuAbortPin3);
    
    if (engineTempPin1State)
    {
      countTemp++;
    }
    if (engineTempPin2State)
    {
      countTemp++;
    }
    if (engineTempPin3State)
    {
    countTemp++;
    }

    if (imuAbortPin1State)
    {
      countIMU++;
    }
    if (imuAbortPin2State)
    {
      countIMU++;
    }
    if (imuAbortPin3State)
    {
      countIMU++;
    }

    if (countTemp >= 2 || countIMU >= 2)
    {
      dontAbort = 0;
      abortFlight();
    }
    
  }
}

//void countdown()
//{
//  for (int x = 10; x>=1; x--)
//  {
//    Serial.println(String(x,DEC));
//    delay(1000);
//    abortLaunch = Serial.readStringUntil(-1);
//    int abortLocation = abortLaunch.indexOf('a');
//    if (abortLocation != -1)
//    {
//      launchReady = 0;
//      finalLaunchReady = 0;
//      launchTime = 0;
//      break;
//    }
//    if (x == 1)
//    {
//      Serial.println("Launch");
//    }
//  }
//}

void abortFlight()
{
  //turn Stepper
  //turn Servo
}

