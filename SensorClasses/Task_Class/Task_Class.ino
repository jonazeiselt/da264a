#include <Arduino.h>
#include <Scheduler.h>


class UltrasensorTask : public Task {
  protected:
    void setup() {
      // initialize serial communication:
      Serial.begin(115200);
      // initialize pins
      pinMode(trigPin, OUTPUT);
      pinMode(echoPin, INPUT);
      pinMode(redLED, OUTPUT);
    }
    void loop() {
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

      Serial.print("Distance to object: ");
      Serial.print(cm);
      Serial.print("cm");
      Serial.println();

      // LED indicate if an object comes within a certain distance
      if (cm <= 20) {
        digitalWrite(redLED, HIGH);
      } else {
        digitalWrite(redLED, LOW);
      }

      delay(300);
    }
  private:
    const int trigPin = 12;
    const int echoPin = 14;
    const int redLED = 0;
} ultrasensor_task;

class AlarmTask : public Task {
  protected:
    void setup() {
      pinMode(alarmPin, OUTPUT);
    }

    void loop()  {
      if (digitalRead(5) == HIGH) {
        tone(alarmPin, 1000); // Send 1KHz sound signal...
        delay(1000);        // ...for 1 sec
        noTone(alarmPin);     // Stop sound...
        delay(1000);        // ...for 1sec
      }
    }

  private:
    int alarmPin = 16;
} alarm_task;

class DoorSensorTask : public Task {
  protected:

    void setup() {
      Serial.begin(115200);
      pinMode(greenLED, OUTPUT);
      pinMode(INPUT_PIN, INPUT_PULLUP);
    }

    void loop()  {
      int inputState = digitalRead(INPUT_PIN);
      if (inputState == LOW) {
        Serial.println("Locked");
        digitalWrite(greenLED, LOW);
        delay(250);
      } else {
        Serial.println("Open");
        digitalWrite(greenLED, HIGH);
        delay(250);
      }
    }

  private:
    int greenLED = 2;
    int INPUT_PIN = 5;
} doorSensor_task;


void setup() {
  Serial.begin(115200);

  Serial.println("");

  delay(1000);

  Scheduler.start(&doorSensor_task);
  Scheduler.start(&alarm_task);
  Scheduler.start(&ultrasensor_task);

  Scheduler.begin();
}

void loop() {}

