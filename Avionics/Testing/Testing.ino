int light = 0;
int lightPin = 13;
int prev[20] = {0,1,2,3,4,5,6};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(lightPin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0; i<sizeof(prev)-1;i++)
  {
    Serial.print(prev[i]);
  }

  for (int i = 0; i<sizeof(prev)-2;i++)
  {
    prev[i]=prev[i+1];
  }
  prev[sizeof(prev)-1]=random(100);
  Serial.println("aa");
  
  
  delay(500);
}
