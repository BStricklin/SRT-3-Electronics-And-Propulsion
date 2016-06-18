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
//b - buzzer
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

#include <TimerOne.h>
#include "LiquidCrystal.h"

double countTest;
int count2;

char missionCMD;
String data;
LiquidCrystal lcd = LiquidCrystal(39,41,45,47,49,51);

//int n2OVentTime = 5*1000000;
//int qdAirOpenTime = 2*1000000;

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
int igniter = 33;

int timerTrigger = 7;
int timerCheck = 8;
int prevTimerState = 0;
int timerState = 0;

int prevSolenoid;
int prev2;
int prev3;
int prev4;
double solDelay;

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
  Serial1.begin(9600);
  delay(100);
  Serial.print("+++");
  delay(100);
  Serial.println("ATBD 7");
  delay(100);
  Serial.println("b");
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

  solDelay = 1500000;

  digitalWrite(igniterTest, INPUT_PULLUP);
  pinMode(igniterTest, INPUT);
  pinMode(preArmMonitor, INPUT);
  pinMode(igniterDisable, INPUT);

//  pinMode(loadCellPin, INPUT);
//  pinMode(battery1, INPUT);
//  pinMode(battery2, INPUT);
//  pinMode(battery3, INPUT);

//  Timer1.initialize(solDelay);
//  Timer1.attachInterrupt(Timer1_Interrupt);

  lcd.begin(16,2);
  lcd.clear();
  lcd.print("Launch Box");
  delay(1000);

}

void loop()
{
  if (count2 == 10)
  {
  count2 = 0;
  Serial.print("z");
  Serial.println(countTest);
//  Serial.print(",");
//  Serial.print(countTest);
//  Serial.print(",");
//  Serial.print(countTest);
//  Serial.print(",");
//  Serial.print(random(0,100));
//  Serial.print(",");
//  Serial.println("1");
  lcd.clear();
  lcd.print(countTest);
  countTest = countTest +1;

  if (checkBatts)
  {
    batts = "v";
    batts = batts + String(analogRead(battery9),DEC) + ",";
    batts = batts + String(analogRead(battery12),DEC) + ","; 
    batts = batts + String(analogRead(battery24),DEC) + ","; 
    Serial.println(batts);
  }
  checkBatts = !checkBatts;
  }
  count2 = count2+1;

//  Serial.print("y");
//  Serial.print(random(0,100));
//  Serial.print(",");
//  Serial.print(random(0,100));
//  Serial.print(",");
//  Serial.println(random(0,100));
//
//  Serial.print("w");
//  Serial.println(random(0,100));

  delay(100);
  if (Serial.available()>0)
  {
    missionCMD = Serial.read();
    Serial1.print(missionCMD);
  
    switch(missionCMD){
    case 'a':
      Serial1.println('a');
      readyLaunch1 = 0;
      readyLaunch2 = 0;
      Serial.println("xLBabo");
      lcd.clear();
      lcd.print("xLBabo");
      break;
    case 'i':
      Serial1.println('i');
      Serial.println("LB: i passed");
      lcd.clear();
      lcd.print("LB: i passed");
      break;
    case 't':
      Serial1.println('t');
      Serial.println("LB: t passed");
      lcd.clear();
      lcd.print("LB: t passed");
      break;
    case 'c':
      Serial1.println('c');
      Serial.print(random(0,100));
      Serial.print("    ");
      Serial.println("LB: c passed");
      lcd.clear();
      lcd.print("LB: c passed");
      
      break;
    case 'e':
      Serial1.println('e');
      Serial.println("LB: e passed");
      lcd.clear();
      lcd.print("LB: e passed");
      break;
    case 'r':
      Serial1.println('r');
      readyLaunch1 = 1;
//      Serial.println("LB: r passed");
      Serial.println("xLBrl11");
      lcd.clear();
      lcd.print("xLBrl11");
      break;
    case 'f':
      Serial1.println('f');
      readyLaunch2 = 1;
//      Serial.println("LB: f passed");
      Serial.println("xLBrl21");
      lcd.clear();
      lcd.print("xLBrl21");
      break;
    case 'u':
      Serial1.println('u');
      readyLaunch1 = 0;
//      Serial.println("LB: u passed");
      Serial.println("xLBrl10");
      lcd.clear();
      lcd.print("xLBrl10");
      break;
    case 'j':
      Serial1.println('j');
      readyLaunch2 = 0;
//      Serial.println("LB: j passed");
      Serial.println("xLBrl20");
      lcd.clear();
      lcd.print("xLBrl20");
      break;
    case 'l':
      if (readyLaunch1 && readyLaunch2)
      {
        Serial1.println('l');
        igniterState = 1;
        digitalWrite(igniter, igniterState);
        Serial.println("xLBlau");
        lcd.clear();
        lcd.print("xLBlau");
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
      lcd.clear();
      lcd.print("xLBnolau");
      break;
    case 's':
      Serial1.println('s');
      Serial.println("LB: s passed");
      lcd.clear();
      lcd.print("LB: s passed");
      break;
    case 'p':
      systemArmState = 1;
      digitalWrite(systemArm, systemArmState);
//      Serial.print("System Armed");
      Serial.println("xsys1");
      lcd.clear();
      lcd.print("xsys1");
      break;
    case 'm':
      systemArmState = 0;
      digitalWrite(systemArm, systemArmState);
//      Serial.print("System Disarmed");
      Serial.println("xsys0");
      lcd.clear();
      lcd.print("xsys0");
      break;
    case 'n':
      igniterArmState = 1;
      digitalWrite(igniterArm, igniterArmState);
//      Serial.print("Igniter Armed");
      Serial.println("xign1");
      lcd.clear();
      lcd.print("xign1");
      
      break;
    case 'o':
      igniterArmState = 0;
      digitalWrite(igniterArm, igniterArmState);
//      Serial.print("Igniter Disarmed");
      Serial.println("xign0");
      lcd.clear();
      lcd.print("xign0");
      break;
    case 'k':
      if (igniterArmState != LOW)
      {
        Serial.println("Igniter is not LOW");
        break;
      }
      digitalWrite(igniterContTest, HIGH);
      delay(200);
      igniterContTestState = digitalRead(igniterTest);
      if (igniterContTestState)
      {
        Serial.println("xictF");
        lcd.clear();
        lcd.print("xictF");
      }
      else
      {
        Serial.println("xictP");
        lcd.clear();
        lcd.print("xictP");
      }
      digitalWrite(igniterContTest, LOW);
      break;
    case '1':
      actuateSolenoid(n2OFillOpen);
//      Serial.println("N2O Fill Open");
//      digitalWrite(n2OFillOpen, HIGH);
      Serial.println("xn2fo");
      lcd.clear();
      lcd.print("xn2fo");
      break;
    case '2':
      actuateSolenoid(n2OFillClose);
//      Serial.println("N2O Fill Close");
      Serial.println("xn2fc");
      lcd.clear();
      lcd.print("xn2fc");
      break;
    case '3':
      actuateSolenoid(n2OVentOpen);
//      Serial.println("N2O Vent Open");
      Serial.println("xn2vo");
      lcd.clear();
      lcd.print("xn2vo");
      break;
    case '4':
      actuateSolenoid(n2OVentClose);
//      Serial.println("N2O Vent Close");
      Serial.println("xn2vc");
      lcd.clear();
      lcd.print("xn2vc");
      break;
    case '5':
      actuateSolenoid(oxyFillOpen);
//      Serial.println("Oxygen Fill Open");
      Serial.println("xoxyo");
      lcd.clear();
      lcd.print("xoxyo");
      break;
    case '6':
      actuateSolenoid(oxyFillClose);
//      Serial.println("Oxygen Fill Close");
      Serial.println("xoxyc");
      lcd.clear();
      lcd.print("xoxyc");
      break;
    case '7':
      actuateSolenoid(qdAirOpen);
//      Serial.println("QD Air Open");
      Serial.println("xqdao");
      lcd.clear();
      lcd.print("xqdao");
      break;
    case '8':
//      digitalWrite(qdAirClose, HIGH);
//      delay(1500);
//      digitalWrite(qdAirClose, LOW);
//      delay(1500);
      actuateSolenoid(qdAirClose);
//      prevSolenoid = qdAirClose;
//      Serial.println("QD Air Close");
      Serial.println("xqdac");
      lcd.clear();
      lcd.print("xqdac");
      break;
    case '9':
      digitalWrite(aux, HIGH);
      Serial.println("xaux1");
      lcd.clear();
      lcd.print("xaux1");
      break;
    case '0':
      digitalWrite(chA, HIGH);
      Serial.println("xchA1");
      lcd.clear();
      lcd.print("xchA1");
      break;
    case 'q':
      digitalWrite(chB, HIGH);
      Serial.println("xchB1");
      lcd.clear();
      lcd.print("xchB1");
      break;
    case 'b':
      digitalWrite(buzzer, HIGH);
      Serial.println("xbuz1");
      lcd.clear();
      lcd.print("xbuz1");
      break;
    case 'z':
      digitalWrite(aux, LOW);
      Serial.println("xaux0");
      lcd.clear();
      lcd.print("xaux0");
      break;
    case 'y':
      digitalWrite(chA, LOW);
      Serial.println("xchA0");
      lcd.clear();
      lcd.print("xchA0");
      break;
    case 'w':
      digitalWrite(chB, LOW);
      Serial.println("xchB0");
      lcd.clear();
      lcd.print("xchB0");
      break;
    case 'g':
      digitalWrite(buzzer, LOW);
      Serial.println("xbuz0");
      lcd.clear();
      lcd.print("xbuz0");
      break;
    case 'd':
      batts = "v";
      batts = batts + String(analogRead(battery9),DEC) + ",";
      batts = batts + String(analogRead(battery12),DEC) + ","; 
      batts = batts + String(analogRead(battery24),DEC); 
      Serial.println(batts);
      break;
    default:
      break;
    }
  }

  if (Serial1.available())
  {
    data = Serial1.readStringUntil('\n');
    Serial.println(data);
  }

  sysMonPrev = sysMon;
  sysMon = digitalRead(preArmMonitor);
  igMonPrev = igMon;
  igMon = digitalRead(igniterDisable);
  
  if (igMon && !igMonPrev)
  {
    digitalWrite(buzzer, HIGH);
    Serial.println("xigMon1");
  }

  if (!igMon && igMonPrev)
  {
    digitalWrite(buzzer, LOW);
    Serial.println("xigMon0");
  }
  
  if (!sysMonPrev && sysMon)
  {
    digitalWrite(igniter, LOW);
    analogWrite(buzzer, 125);
    closeSolenoids();
    digitalWrite(buzzer, LOW);
    Serial.println("xsMon1");
  }

  if (sysMonPrev && !sysMon)
  {
    digitalWrite(igniter, LOW);
    Serial.println("xsMon0");
  }

  prevTimerState = timerState;
  timerState = digitalRead(timerCheck);
  
  if (prevTimerState && !timerState)
  {
    digitalWrite(prevSolenoid, LOW);
    digitalWrite(prev2, LOW);
    digitalWrite(prev3, LOW);
    digitalWrite(prev4, LOW);
  }

  if (checkLC)
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
  digitalWrite(n2OFillClose, HIGH);
  Serial.println("xLBnolau");

  readyLaunch1 = 0;
  Serial.println("xLBrl10");
  
//  actuateSolenoid(n2OFillClose);
  digitalWrite(n2OFillClose, HIGH);
  delay(1500);
  Serial.println("xn2fc");
  digitalWrite(n2OFillClose, LOW);
  
//  actuateSolenoid(n2OVentClose);
  digitalWrite(n2OVentClose, HIGH);
  delay(1500);
  Serial.println("xn2vc");
  digitalWrite(n2OVentClose, LOW);
  
//  actuateSolenoid(oxyFillClose);
  digitalWrite(oxyFillClose, HIGH);
  delay(1500);
  Serial.println("xoxyc");
  digitalWrite(oxyFillClose, LOW);
  
//  actuateSolenoid(qdAirClose);
  digitalWrite(qdAirClose, HIGH);
  delay(1500);
  Serial.println("xqdac");
  digitalWrite(qdAirClose, LOW);

//  delay(1500);
  
//  digitalWrite(n2OFillClose, LOW);
//  digitalWrite(n2OVentClose, LOW);
//  digitalWrite(oxyFillClose, LOW);
//  digitalWrite(qdAirClose, LOW);
  solsClosed = 1;

  //display batter voltage
}

void actuateSolenoid(int relay)
{
    prev4 = prev3;
    prev3 = prev2;
    prev2 = prevSolenoid;
    prevSolenoid = relay;

    digitalWrite(relay, HIGH);
//    Timer1.initialize(solDelay);
//    Timer1.attachInterrupt(Timer1_Interrupt);
    digitalWrite(timerTrigger, LOW);
    delay(50);
    digitalWrite(timerTrigger, HIGH);

//  if (relay == n2OVentOpen)
//  {
//    Timer1.initialize(n2OVentTime);
//    Timer1.attachInterrupt(Timer1_Interrupt); 
//  }
//
//  if (relay == qdAirOpen)
//  {
//    Timer1.initialize(qdAirOpenTime);
//    Timer1.attachInterrupt(Timer1_Interrupt); 
//  }
}

//void Timer1_Interrupt()
//{
//  digitalWrite(prevSolenoid, LOW);
//  digitalWrite(prev2, LOW);
//  digitalWrite(prev3, LOW);
//  digitalWrite(prev4, LOW);
//  Timer1.detachInterrupt();
//
////  TIFR1 = 0x00;
////  TIFR2 = 0x00;
////  TIFR3 = 0x00;
////  TIFR4 = 0x00;
////  TIFR5 = 0x00;
//}

