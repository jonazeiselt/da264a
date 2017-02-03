#include <Arduino.h>
#include <Scheduler.h>

class AlarmTask : public Task {
  protected:
    void setup() {
      pinMode(alarmPin, OUTPUT);
    }

    void loop()  { 
      if(digitalRead(5) == HIGH) {
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
      pinMode(alarmPin, OUTPUT);
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
    int alarmPin = 16;
} doorSensor_task;

class BlinkTask : public Task {
  protected:
    void setup() {
      Serial.begin(115200);

      pinMode(redLED, OUTPUT);
    }

    void loop() {
      digitalWrite(redLED, HIGH);
      delay(500);
      digitalWrite(redLED, LOW);
      delay(500);
    }

  private:
    int redLED = 4;
} blink_task;


void setup() {
  Serial.begin(115200);

  Serial.println("");

  delay(1000);

  Scheduler.start(&doorSensor_task);
  Scheduler.start(&blink_task);
  Scheduler.start(&alarm_task);

  Scheduler.begin();
}

void loop() {}
