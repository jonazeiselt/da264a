
/*
 * Test for Door sensor. If the magnetic contact switch is broken a high signal
 * is detected by input-pin due to pull-up resistence.
 * 
 * Created by: Danial Mahmoud and Naser Kakadost (2017-02-02)
 */


#define INPUT_PIN      5

int greenLED = 2;

void setup() {
  Serial.begin(115200);

  pinMode(greenLED, OUTPUT);
  pinMode(INPUT_PIN, INPUT_PULLUP);
}
 
void loop(){

  int inputState = digitalRead(INPUT_PIN);
  if(inputState == LOW){
    Serial.println("Locked");
    digitalWrite(greenLED, LOW);
    delay(500)
  }else{
    Serial.println("Open");
    digitalWrite(greenLED, HIGH);
    delay(500);
  }

}

