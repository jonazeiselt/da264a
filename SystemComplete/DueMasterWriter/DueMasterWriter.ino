
// Include Scheduler since we want to manage multiple tasks.
#include <Scheduler.h>
#include <Arduino.h>


int trigPin = 12;
int echoPin = 13;
int redLED = 2;

int INPUT_PIN = 8;
int greenLED = 3;

int alarmPin = A11;

void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(redLED, OUTPUT);

  // Setup the magnetsensors pins
  pinMode(greenLED, OUTPUT);
  pinMode(INPUT_PIN, INPUT_PULLUP);

  // Setup pin for alarm
  pinMode(alarmPin, OUTPUT);

  pinMode(5, OUTPUT);
  pinMode(4, INPUT);

  // Add "loop2" and "loop3" to scheduling.
  // "loop" is always started by default.
  Scheduler.startLoop(loop2);
  Scheduler.startLoop(loop3);
}

void loop() {
  int inputState = digitalRead(INPUT_PIN);
  if (inputState == LOW) {
    Serial.println("Locked");
    digitalWrite(greenLED, LOW);
    digitalWrite(5, LOW);
  } else {
    Serial.println("Open");
    digitalWrite(greenLED, HIGH);
    digitalWrite(5, HIGH);
    delay(20);
    while (digitalRead(4) == LOW) {
      if (digitalRead(INPUT_PIN) == LOW) {
        digitalWrite(greenLED, LOW);
      } else {
        digitalWrite(greenLED, HIGH);

      }
    }
    digitalWrite(5, LOW);
    Serial.print("På pinne 4 skickar ESP en " + digitalRead(4));
  }
}

void loop2() {
  // variables for duration of the ping and the distance result in centimeters:
  long duration, cm;

  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse a priore to ensure a clean HIGH pulse:

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.

  duration = pulseIn(echoPin, HIGH);

  // convert the time into a distance
  cm = duration / 29 / 2;

  //  Serial.print("Distance to object: ");
  //  Serial.print(cm);
  //  Serial.print("cm");
  //  Serial.println();

  // LED indicate if an object comes within a certain distance
  if (cm <= 20) {
    digitalWrite(redLED, HIGH);
  } else {
    digitalWrite(redLED, LOW);
  }
  delay(300);

  yield();
}

void loop3() {
  if (digitalRead(INPUT_PIN) == HIGH) {
    //tone(alarmPin, 500); // Send 1KHz sound signal...
    delay(1000);        // ...for 1 sec
    //noTone(alarmPin);
    delay(1000);        // ...for 1sec
  }

  yield();

}

