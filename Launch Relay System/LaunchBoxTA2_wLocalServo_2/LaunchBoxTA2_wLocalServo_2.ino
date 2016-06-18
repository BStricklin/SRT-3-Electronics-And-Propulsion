//All commands need to be accounted for in switch statement
//Should probably be "launching" state where launch box just
//reads data and passes it along (Comm Master checks for abort).

//Commands
//r - readys launch 1 on
//f - final launch 2 on
//u - readys launch 1 off
//j - final launch 2 off
//l - LAUNCH
//a - abort
//s - returns state of variables

//Commands Meant for DAQ
//i - initialization: initializes SD card
//t - sensor test: sends sensor values ten times over 5 seconds
//c - collect data: collects data continuously. outputs over serial and saves. Also checks temp
//to make sure it's not too hot. Command to make pins go high to signal heat event are commented out
//e - exit data collect

//Commands Meant for Launch Box
//p - system arm
//o - igniter arm
//k - igniter continuity test
//? - buzzer
//1 - N2O fill open
//2 - N2O fill close
//3 - N2O vent open
//4 - N2O vent close
//5 - Oxygen fill open
//6 - Oxygen fill close
//7 - QD air open
//8 - QD air close
//9 - aux
//0 - chA
//q - chB

#include "LiquidCrystal.h"
#include <Servo.h>

int countTest;
int count2;

char missionCMD;
String data;
LiquidCrystal lcd = LiquidCrystal(39,41,45,47,49,51);
const int delayTime = 1000;
const int lowBuzzLevel = 2;  //buzzer pwm value for "quiet" mode
const int willBuzz = HIGH; //decides if buzzer sounds at full volume when igniter is armed
Servo localServo;
const int lServoPin = 14;
const int lServoOpen = 850; //position in microseconds
const int lServoClose = 2025; //position in us
// NOTE: open/close values only valid for HITEC HS-M7990TH servo

int systemArmState = LOW;
int igniterArmState = LOW;
int igniterContTestState = HIGH;
int igniterState = LOW;
int buzzerState = LOW;

int igMon = LOW;
int sysMon = LOW;
boolean solsClosed = 0;
int sysMonPrev = 0;
int igMonPrev = 0;
int readyLaunch1 = 0;
int readyLaunch2 = 0;

int systemArm = 25;
int igniterArm = 23;
int igniterContTest = 40;
int buzzer = 6;
int n2OFillOpen = 38;
int n2OFillClose = 36;
int n2OVentOpen = 34;
int n2OVentClose = 32;
int oxyFillOpen = 30;
int oxyFillClose = 28;
int qdAirOpen = 26;
int qdAirClose = 24;
int aux = 22;
int chA = 31;
int chB = 29;
int igniter = 44;

int timerTrigger = 7;
int timerCheck = 8;
int prevTimerState = 0;
int timerState = 0;

int prevSolenoid;
int prev2;
int prev3;
int prev4;
double solDelay;

int prev12V = LOW;
int prev24V = LOW;

int igniterTest = 42;
int preArmMonitor = 35;
int igniterDisable = 37;

int loadCell1 = A10;
int loadCell2 = A11;
int loadCell3 = A12;
int loadCellPin1;
int loadCellPin2;
int loadCellPin3;
String lc;
boolean checkLC = 0;

int battery9 = A7;
int battery12 = A8;
int battery24 = A9;
String batts;
boolean checkBatts = 0;

void setup()
{
  Serial.begin(9600);
  Serial2.begin(9600);
  delay(100);
  Serial.print("++");
//  delay(100);
//  Serial.println("ATBD 7");
  delay(100);
  Serial.println("bb");
  delay(100);
  Serial.flush();
  delay(2);
  Serial.end();
  Serial.begin(9600);  

  digitalWrite(systemArm, LOW);
  digitalWrite(igniterArm, LOW);
  digitalWrite(igniterContTest, LOW);
  digitalWrite(buzzer, LOW);
  digitalWrite(n2OFillOpen, LOW);
  digitalWrite(n2OFillClose, LOW);
  digitalWrite(n2OVentOpen, LOW);
  digitalWrite(n2OVentClose, LOW);
  digitalWrite(oxyFillOpen, LOW);
  digitalWrite(oxyFillClose, LOW);
  digitalWrite(qdAirOpen, LOW);
  digitalWrite(qdAirClose, LOW);
  digitalWrite(aux, LOW);
  digitalWrite(chA, LOW);
  digitalWrite(chB, LOW);
  digitalWrite(igniter, LOW);

  digitalWrite(timerTrigger, HIGH);
  pinMode(timerTrigger, OUTPUT);
  pinMode(timerCheck, INPUT);

  pinMode(systemArm, OUTPUT);
  pinMode(igniterArm, OUTPUT);
  pinMode(igniterContTest, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(n2OFillOpen, OUTPUT);
  pinMode(n2OFillClose, OUTPUT);
  pinMode(n2OVentOpen, OUTPUT);
  pinMode(n2OVentClose, OUTPUT);
  pinMode(oxyFillOpen, OUTPUT);
  pinMode(oxyFillClose, OUTPUT);
  pinMode(qdAirOpen, OUTPUT);
  pinMode(qdAirClose, OUTPUT);
  pinMode(aux, OUTPUT);
  pinMode(chA, OUTPUT);
  pinMode(chB, OUTPUT);
  pinMode(igniter, OUTPUT);

  //End serial3 and set up the (locally connected) servo on pin 14
  Serial3.end();  //pins 14 & 15
  delay(2);
   
   //In case pin 15 is needed as ground b/c we are out of grounds
   digitalWrite(15, LOW);
   pinMode(15, OUTPUT);
  
  localServo.attach(lServoPin,lServoOpen,lServoClose); 
  localServo.writeMicroseconds(lServoClose);  //hard close


  solDelay = 1500000;

  //digitalWrite(igniterTest, INPUT_PULLUP);
  pinMode(igniterTest, INPUT_PULLUP);
  pinMode(preArmMonitor, INPUT);
  pinMode(igniterDisable, INPUT);

  lcd.begin(16,2);
  lcd.clear();
  lcd.print("Texas A&M SRT");
  lcd.setCursor(0,1);
  lcd.print("Launch Box");
  delay(1500);

  localServo.writeMicroseconds(lServoClose-50); //soft hold close

  dispBattVoltage();

}

void loop()
{
  if (count2 == 10)
  {
  count2 = 0;
//  Serial.print("z");
//  Serial.println(countTest);
//  Serial.print(",");
//  Serial.print(countTest);
//  Serial.print(",");
//  Serial.print(countTest);
//  Serial.print(",");
//  Serial.print(random(0,100));
//  Serial.print(",");
//  Serial.println("1");
  countTest = countTest +1;

  if (checkBatts)
  {
    batts = "v";
    batts = batts + String(analogRead(battery9),DEC) + ",";
    batts = batts + String(analogRead(battery12),DEC) + ","; 
    batts = batts + String(analogRead(battery24),DEC); 
    Serial.println(batts);
  }
  checkBatts = !checkBatts;
  }
  count2 = count2+1;

  delay(100);
  if (Serial.available()>0)
  {
    missionCMD = Serial.read();
    //Serial2.print(missionCMD);
  
    switch(missionCMD){
    case 'a':
      Serial2.println('a');
      readyLaunch1 = 0;
      readyLaunch2 = 0;
      Serial.println("xLBabo");
      break;
    case 'i':
      Serial2.println('i');
      Serial.println("LB: i passed");
      break;
    case 't':
      Serial2.println('t');
      Serial.println("LB: t passed");
      break;
    case 'c':
      Serial2.println('c');
      Serial.print(random(0,100));
      Serial.print("    ");
      Serial.println("LB: c passed");
      
      break;
    case 'e':
      Serial2.println('e');
      Serial.println("LB: e passed");
      break;
    case 'r':
      Serial2.println('r');
      readyLaunch1 = 1;
//      Serial.println("LB: r passed");
      Serial.println("xLBrl11");
      break;
    case 'f':
      Serial2.println('f');
      readyLaunch2 = 1;
//      Serial.println("LB: f passed");
      Serial.println("xLBrl21");
      break;
    case 'u':
      Serial2.println('u');
      readyLaunch1 = 0;
//      Serial.println("LB: u passed");
      Serial.println("xLBrl10");
      break;
    case 'j':
      Serial2.println('j');
      readyLaunch2 = 0;
//      Serial.println("LB: j passed");
      Serial.println("xLBrl20");
      break;
    case 'l':
      if (readyLaunch1 && readyLaunch2)
      {
        Serial2.println('l');
        igniterState = 1;
        digitalWrite(igniter, igniterState);
        Serial.println("xLBlau");
        lcd.clear();
        lcd.print("WARNING!");
        lcd.setCursor(0,1);
        lcd.print("IGNITER IS ON");
      }
      else
      {
        Serial.println("LB: LR1 and LR2 not ready");
      }
      break;
    case 'h':
      igniterState = 0;
      digitalWrite(igniter, igniterState);
      Serial.println("xLBnolau");
      dispBattVoltage();
      break;
    case 's':
      Serial2.println('s');
      Serial.println("LB: s passed");
      break;
    case '!':
      //open servo/ball valve
      Serial2.println('!');
      localServo.writeMicroseconds(lServoOpen);
      Serial.println("xser1");
      break;
    case '#':
      //close servo/ball valve
      Serial2.println('#');
      localServo.writeMicroseconds(lServoClose);  //hard close
      delay(100);
      localServo.writeMicroseconds(lServoClose-50); //soft hold close
      Serial.println("xser0");
      break;
    case 'p':
      systemArmState = 1;
      digitalWrite(systemArm, systemArmState);
//      Serial.print("System Armed");
      Serial.println("xsys1");
      dispBattVoltage();
      break;
    case 'm':
      systemArmState = 0;
      digitalWrite(systemArm, systemArmState);
//      Serial.print("System Disarmed");
      Serial.println("xsys0");
      dispBattVoltage();
      break;
    case 'n':
      igniterArmState = 1;
      digitalWrite(igniterArm, igniterArmState);
//      Serial.print("Igniter Armed");
      digitalWrite(buzzer, willBuzz);
      Serial.println("xign1");
      break;
    case 'o':
      igniterArmState = 0;
      digitalWrite(igniterArm, igniterArmState);
//      Serial.print("Igniter Disarmed");
      digitalWrite(buzzer, igniterArmState);
      Serial.println("xign0");
      break;
    case 'k':
      if (igniterArmState != LOW)
      {
        Serial.println("Igniter is not LOW");
      }
      else {
      digitalWrite(igniterContTest, HIGH);
      delay(200);
      igniterContTestState = digitalRead(igniterTest);
      if (igniterContTestState)
      {
        Serial.println("xictF");
      }
      else
      {
        Serial.println("xictP");
      }
      digitalWrite(igniterContTest, LOW);
      }
      break;
    case '1':
      actuateSolenoid(n2OFillOpen);
//      Serial.println("N2O Fill Open");
//      digitalWrite(n2OFillOpen, HIGH);
      Serial.println("xn2fo");
      lcd.clear();
      lcd.print("N2O Fill");
      lcd.setCursor(0,1);
      lcd.print("Open");
      break;
    case '2':
      actuateSolenoid(n2OFillClose);
//      Serial.println("N2O Fill Close");
      Serial.println("xn2fc");
      lcd.clear();
      lcd.print("N2O Fill");
      lcd.setCursor(0,1);
      lcd.print("Closed");
      break;
    case '3':
      actuateSolenoid(n2OVentOpen);
//      Serial.println("N2O Vent Open");
      Serial.println("xn2vo");
      lcd.clear();
      lcd.print("N2O Vent");
      lcd.setCursor(0,1);
      lcd.print("Open");
      break;
    case '4':
      actuateSolenoid(n2OVentClose);
//      Serial.println("N2O Vent Close");
      Serial.println("xn2vc");
      lcd.clear();
      lcd.print("N2O Vent");
      lcd.setCursor(0,1);
      lcd.print("Closed");
      break;
    case '5':
      actuateSolenoid(oxyFillOpen);
//      Serial.println("Oxygen Fill Open");
      Serial.println("xoxyo");
      lcd.clear();
      lcd.print("Oxygen Flood");
      lcd.setCursor(0,1);
      lcd.print("Open");
      break;
    case '6':
      actuateSolenoid(oxyFillClose);
//      Serial.println("Oxygen Fill Close");
      Serial.println("xoxyc");
      lcd.clear();
      lcd.print("Oxygen Flood");
      lcd.setCursor(0,1);
      lcd.print("Closed");
      break;
    case '7':
      actuateSolenoid(qdAirOpen);
//      Serial.println("QD Air Open");
      Serial.println("xqdao");
      lcd.clear();
      lcd.print("Q.D. Air");
      lcd.setCursor(0,1);
      lcd.print("Open");
      break;
    case '8':
      actuateSolenoid(qdAirClose);
      Serial.println("xqdac");
      lcd.clear();
      lcd.print("Q.D. Air");
      lcd.setCursor(0,1);
      lcd.print("Closed");
      break;
    case '9':
      digitalWrite(aux, HIGH);
      Serial.println("xaux1");
      break;
    case '0':
      digitalWrite(chA, HIGH);
      Serial.println("xchA1");
      break;
    case 'q':
      digitalWrite(chB, HIGH);
      Serial.println("xchB1");
      break;
    case '?':
      digitalWrite(buzzer, HIGH);
      Serial.println("xbuz1");
      break;
    case 'z':
      digitalWrite(aux, LOW);
      Serial.println("xaux0");
      break;
    case 'y':
      digitalWrite(chA, LOW);
      Serial.println("xchA0");
      break;
    case 'w':
      digitalWrite(chB, LOW);
      Serial.println("xchB0");
      break;
    case 'g':
      digitalWrite(buzzer, LOW);
      Serial.println("xbuz0");
      break;
    default:
      break;
    }
  }


  if (Serial2.available())
  {
    data = Serial2.readStringUntil('\n');
    Serial.println(data);   //could be probelm 
  }

  sysMonPrev = sysMon;
  sysMon = digitalRead(preArmMonitor);
  igMonPrev = igMon;
  igMon = digitalRead(igniterDisable);
  
  if (igMon && !igMonPrev)
  {
    Serial.println("xigMon1");
    dispBattVoltage();
  }

  if (!igMon && igMonPrev)
  {
    digitalWrite(igniter, LOW);
    Serial.println("xigMon0");
    dispBattVoltage();
  }
  
  if (!sysMonPrev && sysMon)
  {
    digitalWrite(igniter, LOW);
    analogWrite(buzzer, lowBuzzLevel);
    closeSolenoids();
    digitalWrite(buzzer, LOW);
    Serial.println("xsMon1");
    dispBattVoltage();
  }

  if (sysMonPrev && !sysMon)
  {
    digitalWrite(igniter, LOW);
    Serial.println("xsMon0");
    dispBattVoltage();
  }
  
  //Digital Battery checks, to detect if batt has been plugged in/unplugged
  if (digitalRead(battery12) != prev12V) {
    prev12V = !prev12V;
    dispBattVoltage();
  }
  else if (digitalRead(battery24) != prev24V) {
    prev24V = !prev24V;
    dispBattVoltage();
  }

  prevTimerState = timerState;
  timerState = digitalRead(timerCheck);
  
  if (prevTimerState && !timerState)
  {
    digitalWrite(prevSolenoid, LOW);
    digitalWrite(prev2, LOW);
    digitalWrite(prev3, LOW);
    digitalWrite(prev4, LOW);
    dispBattVoltage();
  }

  if (0)
  {
    loadCell1 = analogRead(loadCellPin1);
    loadCell2 = analogRead(loadCellPin2);
    loadCell3 = analogRead(loadCellPin3);
    lc = "w";
    lc = lc + String(loadCell1,DEC) + ",";
    lc = lc + String(loadCell2,DEC) + ","; 
    lc = lc + String(loadCell3,DEC); 
    Serial.println(lc);
  }
  checkLC = !checkLC;
}

void closeSolenoids()
{
  igniterState = 0;
  digitalWrite(igniter, igniterState);
  Serial.println("xLBnolau");

  readyLaunch1 = 0;
  Serial.println("xLBrl10");
  
  //Temp variable to make screen print "Closing" instead of arm/disarm
  int sysArmStateHolder = systemArmState;
  systemArmState = 3;
  
//  actuateSolenoid(n2OFillClose);
  dispBattVoltage();
  digitalWrite(n2OFillClose, HIGH);
  delay(delayTime/2);
  dispBattVoltage();
  delay(delayTime/2);
  Serial.println("xn2fc");
  digitalWrite(n2OFillClose, LOW);
  
//  actuateSolenoid(n2OVentClose);
  dispBattVoltage();
  digitalWrite(n2OVentClose, HIGH);
  delay(delayTime/2);
  dispBattVoltage();
  delay(delayTime/2);
  Serial.println("xn2vc");
  digitalWrite(n2OVentClose, LOW);
  
//  actuateSolenoid(oxyFillClose);
  dispBattVoltage();
  digitalWrite(oxyFillClose, HIGH);
  delay(delayTime/2);
  dispBattVoltage();
  delay(delayTime/2);
  Serial.println("xoxyc");
  digitalWrite(oxyFillClose, LOW);
  
//  actuateSolenoid(qdAirClose);
  dispBattVoltage();
  digitalWrite(qdAirClose, HIGH);
  delay(delayTime/2);
  dispBattVoltage();
  delay(delayTime/2);
  Serial.println("xqdac");
  digitalWrite(qdAirClose, LOW);
  systemArmState = sysArmStateHolder;
  dispBattVoltage();
  
  solsClosed = 1;

}

void actuateSolenoid(int relay)
{
    prev4 = prev3;
    prev3 = prev2;
    prev2 = prevSolenoid;
    prevSolenoid = relay;

    digitalWrite(relay, HIGH);
    digitalWrite(timerTrigger, LOW);
    digitalWrite(timerTrigger, HIGH);
}

void dispBattVoltage() {
  //Read in battery voltages and store temporarily, convert to voltage, and print
  int bat9, bat12, bat24;
  float volt9, volt12, volt24;
  String line1, line2, volt24Str;
  
  bat9 = analogRead(battery9);
  bat12 = analogRead(battery12);
  bat24 = analogRead(battery24);

  if (bat9 > 65) {  //9V Diode bias and formula
    volt9 = 0.0097*float(bat9) + 0.4742;
  }
  else {
    volt9 = 0.0;
  }
  if (bat12 > 39) { //12V Diode bias and formula
    volt12 = 0.0152*float(bat12) + 0.5152;
  }
  else {
    volt12 = 0.0;
  }
  if (bat24 > 17) { //24V Diode bias and formula, subract 12V from other battery to see individual batt. voltage
    volt24 = (0.0315*float(bat24) + 0.5752) - volt12;
    volt24Str = String(volt24) + "V";
  }
  else {
    volt24Str = "---";
  }

  //Make strings
  line1 = String(volt12) + "V  " + volt24Str;
  line2 = String(volt9) + "V  ";
  if (systemArmState==1) {
    line2 = line2 + "Armed";   
  }
  else if (systemArmState==3) {
    line2 = line2 + "Closing";
  }
  else {
    line2 = line2 + "Disarmed";
  }

  lcd.clear();
  lcd.print(line1);
  lcd.setCursor(0,1);
  lcd.print(line2);
}

