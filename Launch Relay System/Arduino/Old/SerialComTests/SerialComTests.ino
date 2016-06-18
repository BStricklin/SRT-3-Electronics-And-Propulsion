//Serial Interface Test

//Variables
const int l = 13; //LED control
String tempS;

void setup() {
  //Start com
  Serial.begin(9600);

  //Set up indicator LED
  pinMode(l, OUTPUT);
  digitalWrite(l, LOW);

  delay(300);


}

void loop() {

  //Wait until there is something in lnput buffer
  while (Serial.available() > 0) {
    //Test for handshake 
    tempS = Serial.readStringUntil('\n');
    //should recieve the message "HELLO\n" (w/o extra delimiters)
    if (tempS.equalsIgnoreCase("HELLO")) {
        digitalWrite(l, HIGH);
        //Send Acknowledgement
        Serial.flush();
        Serial.println("HS");
    }

  }
  

}
