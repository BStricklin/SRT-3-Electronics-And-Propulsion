char missionCMD;
const int sysArm = 25;
const int igArm = 23;
const int igniter = 44;
const int contSel = 40;
const int cCheck = 42;
const int bat12 = A8;
const int bat24 = A9;

int cont = LOW;
int v12, v24;

void setup() {
  digitalWrite(sysArm, LOW);
  digitalWrite(igArm, LOW);
  digitalWrite(igniter, LOW);
  digitalWrite(contSel, LOW);

  pinMode(sysArm, OUTPUT);
  pinMode(igArm, OUTPUT);
  pinMode(igniter, OUTPUT);
  pinMode(contSel, OUTPUT);
  pinMode(cCheck, INPUT_PULLUP);

  Serial.begin(9600);

}

void loop() {
  delay(100);
  if (Serial.available()>0) {
    missionCMD = Serial.read();

    switch(missionCMD) {
      case 'a' :  //arm
        digitalWrite(sysArm, HIGH);
        digitalWrite(igArm, HIGH);
        Serial.println("Armed");
        break;
      case 'd': //disarm
        digitalWrite(sysArm, LOW);
        digitalWrite(igArm, LOW);
        digitalWrite(igniter, LOW);
        Serial.println("Disarmed");
        break;
      case 'l': //launch
        digitalWrite(igniter, HIGH);
        Serial.println("Igniter ON");
        break;
      case 'h': //release launch
        digitalWrite(igniter, LOW);
        Serial.println("Igniter OFF");
        break; 
      case 'k': //cont. check
        if (!digitalRead(igniter)) {
          digitalWrite(contSel, HIGH);
          delay(100);
          cont = digitalRead(cCheck);
          digitalWrite(contSel, LOW);
          if (!cont) {
            Serial.println("Cont. Passed");
          }
          else {
            Serial.println("Cont. Failed");
          }
        }
        else {
          Serial.println("Error");
          delay(400);
        }
    }
  }

  //Check voltage and print with time
  v12 = analogRead(bat12);
  v24 = analogRead(bat24);
  Serial.print(millis());
  Serial.print(",");
  Serial.print(v12);
  Serial.print(",");
  Serial.println(v24);
  
}
