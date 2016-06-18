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
const int igniter = 44;
const int sysArm = 25;
const int igArm = 23;
const int contSelect = 40;
const int buzzer = 6;
const int tTrig = 7;

//Inputs
const int contTest = 42;
const int prearmMon = 35;
const int igDisableMon = 37;
const int tOut = 8;
const int v9 = A7;
const int v12 = A8;
const int v24 = A9;

//LCD Display
LiquidCrystal lcd = LiquidCrystal(39,41,45,47,49,51);

//Counting
float volt9 = 0.0;
float volt12 = 0.0;
float volt24 = 0.0;

void setup() {
  //LCD screen
  lcd.begin(16,2);
  lcd.clear();

  //Outputs, declare and set low
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
  digitalWrite(tTrig, HIGH);
  digitalWrite(igniter, LOW);
  
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


  //input pins
  pinMode(prearmMon, INPUT);
  pinMode(igDisableMon, INPUT);
  pinMode(contTest, INPUT_PULLUP);
  pinMode(tOut, INPUT);

  //Splash Screen
  lcd.print("Launch Box");
  lcd.setCursor(0,1);
  lcd.print("Relay Test");
  delay(1000);

}

void loop() {
  //Arm box
  inputPrinter(1);
  digitalWrite(sysArm, HIGH);
  delay(200);
  
  // Go through each output: turn on 500 ms, off 500 ms
  digitalWrite(r1, HIGH);
  delay(500);
  digitalWrite(r1, LOW);
  delay(500);
  inputPrinter(0);

  digitalWrite(r2, HIGH);
  delay(500);
  digitalWrite(r2, LOW);
  delay(500);
  inputPrinter(0);
  
  digitalWrite(r3, HIGH);
  delay(500);
  digitalWrite(r3, LOW);
  delay(500);
  inputPrinter(0);

  digitalWrite(r4, HIGH);
  delay(500);
  digitalWrite(r4, LOW);
  delay(500);
  inputPrinter(0);

  digitalWrite(r5, HIGH);
  delay(500);
  digitalWrite(r5, LOW);
  delay(500);
  inputPrinter(0);

  digitalWrite(r6, HIGH);
  delay(500);
  digitalWrite(r6, LOW);
  delay(500);
  inputPrinter(1);

  digitalWrite(r7, HIGH);
  delay(500);
  digitalWrite(r7, LOW);
  delay(500);
  inputPrinter(0);

  digitalWrite(r8, HIGH);
  delay(500);
  digitalWrite(r8, LOW);
  delay(500);
  inputPrinter(0);

  digitalWrite(r9, HIGH);
  delay(500);
  digitalWrite(r9, LOW);
  delay(500);
  inputPrinter(0);

  digitalWrite(r10, HIGH);
  delay(500);
  digitalWrite(r10, LOW);
  delay(500);
  inputPrinter(0);

  digitalWrite(rA, HIGH);
  delay(500);
  digitalWrite(rA, LOW);
  delay(500);
  inputPrinter(0);

  digitalWrite(rB, HIGH);
  delay(500);
  digitalWrite(rB, LOW);
  delay(500);
  inputPrinter(1);

  digitalWrite(igArm, HIGH);
  delay(100);
  digitalWrite(igniter, HIGH);
  delay(500);
  digitalWrite(igniter, LOW);
  digitalWrite(igArm, LOW);
  delay(500);
  inputPrinter(0);

  //Buzzer
  if (digitalRead(igDisableMon)) {
    inputPrinter(0);
    for (int j=0; j <= 255; j+=5) {
      analogWrite(buzzer, j);
      delay(100);
    }
    digitalWrite(buzzer, LOW);
  }
  inputPrinter(0);

  digitalWrite(sysArm, LOW);
  delay(500);
  
}

//Function to check input stats and print
//If mode == 1, a continuity check is also performed
void inputPrinter(int mode) {
  //Print voltages
  volt9 = 0.0097*analogRead(v9) + 0.4742;
  volt12 = 0.0152*analogRead(v12) + 0.5152;
  volt24 = 0.0315*analogRead(v24) + 0.5752;
  //diode effect
  if (volt9 < 0.7) {
    volt9 = 0.0;
  }
  if (volt12 < 0.7) {
    volt12 = 0.0;
  }
  if (volt24 < 0.7) {
    volt24 = 0.0;
  }
  lcd.setCursor(0,0);
  String vLine1 = String(String(volt9) + "V " + String(volt12) + "V     ");
  String vLine2 = String(String(volt24) + "V ");
  lcd.print(vLine1);
  lcd.setCursor(0,1);
  lcd.print(vLine2);
  
  lcd.setCursor(6,1);
  /*
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
*/
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

