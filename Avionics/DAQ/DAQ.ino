//Notes:
//add in thermocouple calibrater
//Sensor values separated by comma in order: Th1, Th2, Th3, Pr1, Pr2
//With no thermcouples in terminals, get NAN

//Commands
//i - initialization: initializes SD card
//t - sensor test: sends sensor values ten times over 5 seconds
//c - collect data: collects data continuously. outputs over serial and saves. Also checks temp
//to make sure it's not too hot. Command to make pins go high to signal heat event are commented out
//e - exit data collect

#include <SD.h>
#include <SPI.h>
#include "Adafruit_MAX31855.h"
#include <TimerOne.h>

int baud = 9600;

Sd2Card card;
SdFile root;
SdVolume volume;
uint32_t volumesize;

const int SD_DO = 12;
const int CLK = 13;
const int SD_DI = 11;
const int SD_CS = 10;

int thermocouple_1_CS = 7;
int thermocouple_1_DO = 12;
int thermocouple_2_CS = 8;
int thermocouple_2_DO = 12;
int thermocouple_3_CS = 9;
int thermocouple_3_DO = 12;
Adafruit_MAX31855 thermocouple1(CLK, thermocouple_1_CS, thermocouple_1_DO);
Adafruit_MAX31855 thermocouple2(CLK, thermocouple_2_CS, thermocouple_2_DO);
Adafruit_MAX31855 thermocouple3(CLK, thermocouple_3_CS, thermocouple_3_DO);
int maxTemp = 1200;

int pressurePin1 = A0;
int pressurePin2 = A3;

double therm1;
double therm2;
double therm3;
double press1;
double press2;

char inputCMD;

int engineTempPin1 = 3;
int engineTempPin2 = 4;
int engineTempPin3 = 5;
 
File myFile;
boolean timeToClose;
boolean dontExit;


void setup() {
  Serial.begin(baud);
  pinMode(pressurePin1,INPUT);
  pinMode(pressurePin2,INPUT);
  
  digitalWrite(engineTempPin1, LOW);
  pinMode(engineTempPin1, OUTPUT);
  digitalWrite(engineTempPin2, LOW);
  pinMode(engineTempPin2, OUTPUT);
  digitalWrite(engineTempPin3, LOW);
  pinMode(engineTempPin3, OUTPUT);

  
  Timer1.initialize(10000000);
  Timer1.attachInterrupt(Timer1_Interrupt);

}

void loop()
{
//  while(Serial.available()==0) {}
  inputCMD = Serial.read();

  switch(inputCMD){
  case 'i':
    initialization();
    break;
  case 't':
    sensorTest();
    break;
  case 'c':
    Serial.println("col");
    collectData();
    break;
  default:
    break;
  }
   
}

void initialization()
{
  if (!card.init(SPI_HALF_SPEED, SD_CS)) {
    Serial.println(F("Initialization failed."));
    return;
  }
  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  volume.init(card);
  
  Serial.println(F("\nFiles found on the card (name, date and size in bytes): "));
  root.openRoot(volume);

  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);
  
  if (!SD.begin(SD_CS)) {
    Serial.println(F("Initialization failed!"));
    return;
  }
  Serial.println(F("Initialization done."));
  
}

void sensorTest()
{
  myFile = SD.open("data.txt", FILE_WRITE);
  if(!myFile)
  {
    Serial.println(F("SD file won't open."));
    return;
  }

  Serial.println(F("Test Start"));
  myFile.println(F("Test Start"));
  for (int n =1; n<=10; n++)
  {
//    String data = "Sensor Test - Th1: ";
      String data = "z";
    data = data + String(readThermo(1),DEC);
    data = data + ",";
//    data = data + " Th2: ";
    data = data + String(readThermo(2),DEC);
    data = data + ",";
//    data = data + " Th3: ";
    data = data + String(readThermo(3),DEC);
    data = data + ",";
//    data = data + " Pr1: ";
    data = data + String(readPressure(1),DEC);
    data = data + ",";
//    data = data + " Pr2: ";
    data = data + String(readPressure(2),DEC);
//    data = data + ".";
    myFile.println(data);
    Serial.println(data);
    delay(500);
  }
  Serial.println(F("Test End"));
  myFile.println(F("Test End"));
  myFile.close();
  
}

void collectData()
{
  myFile = SD.open("data.txt", FILE_WRITE);
  if(!myFile)
  {
    Serial.println(F("SD file won't open."));
    return;
  }
  Serial.println(F("Time for Data."));
  myFile.println(F("Time for Data."));
  dontExit = 1;

  while (dontExit)
  {
    therm1 = readThermo(1);
    therm2 = readThermo(2);
    therm3 = readThermo(3);
    press1 = readPressure(1);
    press2 = readPressure(2);
    
//    String data = "Th1: ";
    String data = "z";
    data = data + String(therm1,DEC);
    data = data + ",";
//    data = data + " Th2: ";
    data = data + String(therm2,DEC);
    data = data + ",";
//    data = data + " Th3: ";
    data = data + String(therm3,DEC);
    data = data + ",";
//    data = data + " Pr1: ";
    data = data + String(press1,DEC);
    data = data + ",";
//    data = data + " Pr2: ";
    data = data + String(press2,DEC);
//    data = data + ".";
    myFile.println(data);
    Serial.println(data);

    checkParams();

    if (Serial.read() == 'e')
    {
      dontExit = 0;
      myFile.println(F("Data Collection Ended."));
      Serial.println(F("Data Collection Ended."));
    }

    if (timeToClose)
    {
      timeToClose = false;
      myFile.close();
      myFile = SD.open("data.txt", FILE_WRITE);
      if(!myFile)
      {
        Serial.println(F("SD file won't open."));
       
      }
    }

    delay(5);
    
  }
   myFile.close();
  
}

double readThermo(int therm)
{
       
  if (therm == 1)
  {
    return thermocouple1.readCelsius();
  }

  if (therm == 2)
  {
    return thermocouple2.readCelsius();
  }

  if (therm == 3)
  {
    return thermocouple3.readCelsius();
  }

  
  return -99;
}

double readPressure(int pres)
{
  if (pres == 1)
  {
    return analogRead(pressurePin1);
      return 4;
  }

  if (pres == 2)
  {
    return analogRead(pressurePin2);
            return 5;
  }

  return -99;
}

void  checkParams()
{
  int count = 0;
  if (therm1 > maxTemp)
  {
    count++;
  }
  if (therm2 > maxTemp)
  {
    count++;
  }
  if (therm3 > maxTemp)
  {
    count++;
  }

  if (count >=2)
  {
    myFile.println(F("Engine too hot."));
//    digitalWrite(engineTempPin1, HIGH;
//    digitalWrite(engineTempPin2, HIGH;
//    digitalWrite(engineTempPin3, HIGH;
    
  }
  
}

void Timer1_Interrupt()
{
  timeToClose = true;
}



