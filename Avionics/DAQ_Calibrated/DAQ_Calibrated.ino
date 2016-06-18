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
      String data = "";
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
    String data = "";
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

    delay(50);
    
  }
   myFile.close();
  
}

double readThermo(int therm)
{
  int i = 0;
  double internalTemp = -99;
  double rawTemp = -99;
  double thermocoupleVoltage= 0;
  double internalVoltage = 0;
  double correctedTemp = 0;
       
  if (therm == 1)
  {
    internalTemp = thermocouple1.readInternal();
    rawTemp = thermocouple1.readCelsius();
  }

  if (therm == 2)
  {
    internalTemp = thermocouple2.readInternal();
    rawTemp = thermocouple2.readCelsius();
  }

  if (therm == 3)
  {
    internalTemp = thermocouple3.readInternal();
    rawTemp = thermocouple3.readCelsius();;
  }

  if (isnan(rawTemp)) {
        Serial.println(F("Something wrong with thermocouple!"));
      }
       else {
          // Steps 1 & 2. Subtract cold junction temperature from the raw thermocouple temperature.
          thermocoupleVoltage = (rawTemp - internalTemp)*0.041276;  // C * mv/C = mV

          // Step 3. Calculate the cold junction equivalent thermocouple voltage.

          if (internalTemp >= 0) { // For positive temperatures use appropriate NIST coefficients
             // Coefficients and equations available from http://srdata.nist.gov/its90/download/type_k.tab

             double c[] = {-0.176004136860E-01,  0.389212049750E-01,  0.185587700320E-04, -0.994575928740E-07,  0.318409457190E-09, -0.560728448890E-12,  0.560750590590E-15, -0.320207200030E-18,  0.971511471520E-22, -0.121047212750E-25};

             // Count the the number of coefficients. There are 10 coefficients for positive temperatures (plus three exponential coefficients),
             // but there are 11 coefficients for negative temperatures.
             int cLength = sizeof(c) / sizeof(c[0]);

             // Exponential coefficients. Only used for positive temperatures.
             double a0 =  0.118597600000E+00;
             double a1 = -0.118343200000E-03;
             double a2 =  0.126968600000E+03;


             // From NIST: E = sum(i=0 to n) c_i t^i + a0 exp(a1 (t - a2)^2), where E is the thermocouple voltage in mV and t is the temperature in degrees C.
             // In this case, E is the cold junction equivalent thermocouple voltage.
             // Alternative form: C0 + C1*internalTemp + C2*internalTemp^2 + C3*internalTemp^3 + ... + C10*internaltemp^10 + A0*e^(A1*(internalTemp - A2)^2)
             // This loop sums up the c_i t^i components.
             for (i = 0; i < cLength; i++) {
                internalVoltage += c[i] * pow(internalTemp, i);
             }
                // This section adds the a0 exp(a1 (t - a2)^2) components.
                internalVoltage += a0 * exp(a1 * pow((internalTemp - a2), 2));
          }
          else if (internalTemp < 0) { // for negative temperatures
             double c[] = {0.000000000000E+00,  0.394501280250E-01,  0.236223735980E-04 - 0.328589067840E-06, -0.499048287770E-08, -0.675090591730E-10, -0.574103274280E-12, -0.310888728940E-14, -0.104516093650E-16, -0.198892668780E-19, -0.163226974860E-22};

             // Count the number of coefficients.
             int cLength = sizeof(c) / sizeof(c[0]);

             // Below 0 degrees Celsius, the NIST formula is simpler and has no exponential components: E = sum(i=0 to n) c_i t^i
             for (i = 0; i < cLength; i++) {
                internalVoltage += c[i] * pow(internalTemp, i) ;
             }
          }

          // Step 4. Add the cold junction equivalent thermocouple voltage calculated in step 3 to the thermocouple voltage calculated in step 2.
          double totalVoltage = thermocoupleVoltage + internalVoltage;

          // Step 5. Use the result of step 4 and the NIST voltage-to-temperature (inverse) coefficients to calculate the cold junction compensated, linearized temperature value.
          // The equation is in the form correctedTemp = d_0 + d_1*E + d_2*E^2 + ... + d_n*E^n, where E is the totalVoltage in mV and correctedTemp is in degrees C.
          // NIST uses different coefficients for different temperature subranges: (-200 to 0C), (0 to 500C) and (500 to 1372C).
          if (totalVoltage < 0) { // Temperature is between -200 and 0C.
             double d[] = {0.0000000E+00, 2.5173462E+01, -1.1662878E+00, -1.0833638E+00, -8.9773540E-01, -3.7342377E-01, -8.6632643E-02, -1.0450598E-02, -5.1920577E-04, 0.0000000E+00};

             int dLength = sizeof(d) / sizeof(d[0]);
             for (i = 0; i < dLength; i++) {
                correctedTemp += d[i] * pow(totalVoltage, i);
             }
          }
          else if (totalVoltage < 20.644) { // Temperature is between 0C and 500C.
             double d[] = {0.000000E+00, 2.508355E+01, 7.860106E-02, -2.503131E-01, 8.315270E-02, -1.228034E-02, 9.804036E-04, -4.413030E-05, 1.057734E-06, -1.052755E-08};
             int dLength = sizeof(d) / sizeof(d[0]);
             for (i = 0; i < dLength; i++) {
                correctedTemp += d[i] * pow(totalVoltage, i);
             }
          }
          else { // Temperature is between 500C and 1372C.
             double d[] = {-1.318058E+02, 4.830222E+01, -1.646031E+00, 5.464731E-02, -9.650715E-04, 8.802193E-06, -3.110810E-08, 0.000000E+00, 0.000000E+00, 0.000000E+00};
             int dLength = sizeof(d) / sizeof(d[0]);
             for (i = 0; i < dLength; i++) {
                correctedTemp += d[i] * pow(totalVoltage, i);
             }
          } 
          return correctedTemp;

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



