
/*
   Test class for ESP8266MOD
*/

/*
 * Test for Door sensor
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



/* Sweep
  by BARRAGAN <http://barraganstudio.com>
  This example code is in the public domain.
  modified 28 May 2015
  by Michael C. Miller
  modified 8 Nov 2013
  by Scott Fitzgerald
  http://arduino.cc/en/Tutorial/Sweep
*/

//#include <Servo.h>
//
//Servo myservo;  // create servo object to control a servo
//// twelve servo objects can be created on most boards
//
//
//void setup()
//{
//  myservo.attach(2);  // attaches the servo on GIO2 to the servo object
//}
//
//void loop()
//{
//  int pos;
//
//  for (pos = 0; pos <= 180; pos += 1) // goes from 0 degrees to 180 degrees
//  { // in steps of 1 degree
//    myservo.write(pos);              // tell servo to go to position in variable 'pos'
//    delay(15);                       // waits 15ms for the servo to reach the position
//  }
//  for (pos = 180; pos >= 0; pos -= 1) // goes from 180 degrees to 0 degrees
//  {
//    myservo.write(pos);              // tell servo to go to position in variable 'pos'
//    delay(15);                       // waits 15ms for the servo to reach the position
//  }
//}

