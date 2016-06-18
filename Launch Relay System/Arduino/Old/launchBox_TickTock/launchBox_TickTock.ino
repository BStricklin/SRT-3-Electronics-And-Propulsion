//Launch Box Functions Test
#include "LiquidCrystal.h"

//Pinouts
const int r1 = 38;
const int r2 = 36;
const int r3 = 34;
const int r4 = 32;
const int r5 = 30;
const int r6 = 28;
const int r7 = 26;
const int r8 = 24;
const int r9 = 22;
const int r10 = 33;
const int rA = 31;
const int rB = 29;
const int sysArm = 25;
const int igArm = 23;
const int contSelect = 40;
const int buzzer = 27;

//Inputs
const int contTest = 42;
const int prearmMon = 35;
const int igDisableMon = 37;

//LCD Display
LiquidCrystal lcd = LiquidCrystal(39,41,45,47,49,51);

//Counting
int isArmed = 0;

void setup() {
  //LCD screen
  lcd.begin(16,2);
  lcd.clear();

  //Outputs, declare and set low
  pinMode(r1, OUTPUT);
  pinMode(r2, OUTPUT);
  pinMode(r3, OUTPUT);
  pinMode(r4, OUTPUT);
  pinMode(r5, OUTPUT);
  pinMode(r6, OUTPUT);
  pinMode(r7, OUTPUT);
  pinMode(r8, OUTPUT);
  pinMode(r9, OUTPUT);
  pinMode(r10, OUTPUT);
  pinMode(rA, OUTPUT);
  pinMode(rB, OUTPUT);
  pinMode(sysArm, OUTPUT);
  pinMode(igArm, OUTPUT);
  pinMode(contSelect, OUTPUT);
  pinMode(buzzer, OUTPUT);

  digitalWrite(r1, LOW);
  digitalWrite(r2, LOW);
  digitalWrite(r3, LOW);
  digitalWrite(r4, LOW);
  digitalWrite(r5, LOW);
  digitalWrite(r6, LOW);
  digitalWrite(r7, LOW);
  digitalWrite(r8, LOW);
  digitalWrite(r9, LOW);
  digitalWrite(r10, LOW);
  digitalWrite(rA, LOW);
  digitalWrite(rB, LOW);
  digitalWrite(sysArm, LOW);
  digitalWrite(igArm, LOW);
  digitalWrite(contSelect, LOW);
  digitalWrite(buzzer, LOW);

  //input pins
  pinMode(prearmMon, INPUT);
  pinMode(igDisableMon, INPUT);
  pinMode(contTest, INPUT_PULLUP);

  //Splash Screen
  lcd.print("Launch Box");
  lcd.setCursor(0,1);
  lcd.print("Relay Test");
  delay(1000);

}

void loop() {
  //Arm/diasrm based on what the last cycle was.
  lcd.clear();
  if (isArmed == 0) { //arm
    isArmed = 1;
    digitalWrite(sysArm, HIGH);
    lcd.setCursor(0,1);
    lcd.print("Armed");
  }
  else {  //disarm
    isArmed = 0;
    digitalWrite(sysArm, LOW);
    lcd.setCursor(0,1);
    lcd.print("Disarmed");
  }

  //Start checking inputs
  inputPrinter(1);
  
  // Go through each output: turn on 500 ms, off 500 ms
  digitalWrite(r1, HIGH);
  lcd.setCursor(0,0);
  lcd.print("NOS Fill O      ");
  delay(500);
  digitalWrite(r1, LOW);
  delay(500);
  inputPrinter(0);

  digitalWrite(r2, HIGH);
  lcd.setCursor(0,0);
  lcd.print("NOS Fill C      ");
  delay(500);
  digitalWrite(r2, LOW);
  delay(500);
  inputPrinter(0);
  
  digitalWrite(r3, HIGH);
  lcd.setCursor(0,0);
  lcd.print("NOS Vent O      ");
  delay(500);
  digitalWrite(r3, LOW);
  delay(500);
  inputPrinter(0);

  digitalWrite(r4, HIGH);
  lcd.setCursor(0,0);
  lcd.print("NOS Vent C      ");
  delay(500);
  digitalWrite(r4, LOW);
  delay(500);
  inputPrinter(0);

  digitalWrite(r5, HIGH);
  lcd.setCursor(0,0);
  lcd.print("Ox Fill O       ");
  delay(500);
  digitalWrite(r5, LOW);
  delay(500);
  inputPrinter(0);

  digitalWrite(r6, HIGH);
  lcd.setCursor(0,0);
  lcd.print("Ox Fill C       ");
  delay(500);
  digitalWrite(r6, LOW);
  delay(500);
  inputPrinter(1);

  digitalWrite(r7, HIGH);
  lcd.setCursor(0,0);
  lcd.print("QD O            ");
  delay(500);
  digitalWrite(r7, LOW);
  delay(500);
  inputPrinter(0);

  digitalWrite(r8, HIGH);
  lcd.setCursor(0,0);
  lcd.print("QD C            ");
  delay(500);
  digitalWrite(r8, LOW);
  delay(500);
  inputPrinter(0);

  digitalWrite(r9, HIGH);
  lcd.setCursor(0,0);
  lcd.print("AUX             ");
  delay(500);
  digitalWrite(r9, LOW);
  delay(500);
  inputPrinter(0);

  digitalWrite(rA, HIGH);
  lcd.setCursor(0,0);
  lcd.print("24V A           ");
  delay(500);
  digitalWrite(rA, LOW);
  delay(500);
  inputPrinter(0);

  digitalWrite(rB, HIGH);
  lcd.setCursor(0,0);
  lcd.print("24V B           ");
  delay(500);
  digitalWrite(rB, LOW);
  delay(500);
  inputPrinter(1);

  digitalWrite(r10, HIGH);
  lcd.setCursor(0,0);
  lcd.print("Igniter-Disarmed");
  delay(500);
  digitalWrite(r10, LOW);
  delay(500);
  inputPrinter(0);

  digitalWrite(igArm, HIGH);
  lcd.setCursor(0,0);
  lcd.print("Igniter-Armed   ");
  delay(100);
  digitalWrite(r10, HIGH);
  lcd.setCursor(0,0);
  delay(500);
  digitalWrite(r10, LOW);
  digitalWrite(igArm, LOW);
  delay(500);
  inputPrinter(0);

  digitalWrite(contSelect, HIGH);
  lcd.setCursor(0,0);
  lcd.print("CC Selector     ");
  delay(500);
  digitalWrite(contSelect, LOW);
  delay(500);
  inputPrinter(0);

  /*
   * OMIT For quiet test
  digitalWrite(buzzer, HIGH);
  lcd.setCursor(0,0);
  lcd.print("Buzzer          ");
  delay(500);
  digitalWrite(buzzer, LOW);
  delay(500);
  inputPrinter(0);
  */
}

//Function to check input stats and print
//If mode == 1, a continuity check is also performed
void inputPrinter(int mode) {
  
  lcd.setCursor(0,1);

  if (digitalRead(prearmMon) == HIGH) {
    lcd.print("Armed ");
  }
  else {
    lcd.print("      ");
  }

  if (digitalRead(igDisableMon) == HIGH) {
    lcd.print("Ign. ");
  }
  else {
    lcd.print("     ");
  }

  if (mode == 1) {
    digitalWrite(contSelect, HIGH);
    delay(100);
    if (digitalRead(contTest) == LOW) {
      lcd.print("Pass ");
    }
    else {
      lcd.print("Fail ");
    }
    digitalWrite(contSelect, LOW);
    delay(100);
  }
  else {};
}

