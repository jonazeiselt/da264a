
/*
   Test class for ESP8266MOD
*/

int digitalPin = 16;

void setup() {
  Serial.begin(115200);
  pinMode(digitalPin, OUTPUT);
}

void loop() {

  tone(digitalPin, 1000); // Send 1KHz sound signal...
  delay(1000);        // ...for 1 sec
  noTone(digitalPin);     // Stop sound...
  delay(1000);        // ...for 1sec
}



