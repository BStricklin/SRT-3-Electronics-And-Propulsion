#include "Timer.h"
//SRT-3 Launch Box Arduino Software V1 - DESIGNED FOR SOLID ENGINE TEST

//Connections
const int genArmPin = 22;
const int ignArmPin = 23;
const int ignRelayPin = 24;
const int ccControlPin = 25;  //continuity check relay control
const int ccSigPin = 26;  //cont. check signal digital input

//Values and objects
Timer t;
byte inBuffer[4];

void setup() {

  //Set I/O pin type and initial state
  pinMode(genArmPin, OUTPUT);
  pinMode(ignArmPin, OUTPUT);
  pinMode(ignRelayPin, OUTPUT);
  pinMode(ccControlPin, OUTPUT);
  pinMode(ccSigPin, INPUT_PULLUP);

  digitalWrite(genArmPin, LOW);
  digitalWrite(ignArmPin, LOW);
  digitalWrite(ignRelayPin, LOW);
  digitalWrite(ccControlPin, LOW);

  //Serial Connection   ***NOTE: Serial(0) was convenient because it worked with my USB cable, this will likely be Serial1***
  Serial.begin(115200);
  delay(300);
  
}

void loop() {

  //Wait for full packet to arrive (4 bytes)
  if (Serial.available() >= 4) {
      Serial.readBytes(inBuffer, 4);
  }
 
  t.update(); //timer routine
}

//Function to decode the most current input in inBuffer
int inputDecode() {
  
  //All inputs start with "D0"
  if (inBuffer[0] != byte(0xD0)) {
    return 0;
  }
  else if (inBuffer[1] == byte(0xAB)) {
    if (inBuffer[2] == byte(0x01) && inBuffer[3] == byte(0)) {
      //
    }
    else if (inBuffer[2] == byte(0x05) && inBuffer[3] == byte(0)) {

    }
      
  }
  else if (inBuffer[1] == byte(0xFF)) {
    if (inBuffer[2] == byte(0x08) && inBuffer[3] == byte(0)) {

    }
    else if (inBuffer[2] == byte(0x1F) && inBuffer[3] == byte(0)) {

    }
  }
  else  {
    return 0;
  }
 
}

