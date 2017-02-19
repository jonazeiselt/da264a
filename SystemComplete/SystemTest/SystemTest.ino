
#include <Arduino.h>
#include <Scheduler.h>
#include "ESP8266WiFi.h"
#include <WiFiClientSecure.h>

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
      pinMode(greenLED, OUTPUT);
      pinMode(INPUT_PIN, INPUT_PULLUP);
      delay(10);
    }

    void smtpResponse(WiFiClientSecure client)
    {
      currentTime = millis(); // https://www.arduino.cc/en/reference/millis
      while (!client.available())
      {
        // Make sure that the program doesn't lag..
        if ((currentTime + 10000) < millis())
        {
          Serial.println("Error: Timeout" + millis());
          return;
        }
      }
      String res = client.readStringUntil('\n');
      Serial.println(res);
    }

    void loop()  {
      int inputState = digitalRead(INPUT_PIN);
      if (inputState == LOW) {
        //Serial.println("Locked");
        digitalWrite(greenLED, LOW);
        delay(250);
      } else {
        //Serial.println("Open");
        digitalWrite(greenLED, HIGH);
        delay(250);
        if (flag == false) {
          if (!client.connect("smtp.gmail.com", port))
          {
            Serial.println("Connection failed");
          }
          smtpResponse(client);
          Serial.println("Connection successful");

          client.println("HELO smtp.gmail.com");
          smtpResponse(client);
          client.println("AUTH LOGIN");
          smtpResponse(client);
          client.println("Ym9iZ3VhcmQ0NUBnbWFpbC5jb20="); // mail
          smtpResponse(client);
          client.println("bWFyeWphbmU0NQ==");     //pass
          smtpResponse(client);

          client.println("MAIL FROM:<bobguard45@gmail.com>");
          smtpResponse(client);

          client.println("RCPT TO:<jonazeiselt@gmail.com>");
          smtpResponse(client);

          client.println("DATA");
          smtpResponse(client);

          client.println("from:bobguard45@gmail.com");
          client.println("to:jonazeiselt@gmail.com");
          client.println("SUBJECT: Warning!");
          smtpResponse(client);
          client.println();
          client.println("Someone is too close to painting!");
          client.println(".");
          client.println("QUIT");
          client.stop();
          flag = true;
        }

      }
    }

  private:
    int greenLED = 2;
    int INPUT_PIN = 5;
    //char ssid[] = "AndroidAP";
    //char pass[] = "wury1981";
    int port = 465;
    //char server[] = "smtp.gmail.com";
    unsigned long currentTime;
    WiFiClientSecure client; // SSL/TLS support
    boolean flag = false;
} doorSensor_task;


void setup() {
  Serial.begin(115200);
  WiFi.begin("Axels iPhone", "Nyttlsen"); // Connect to WiFi

  // WiFi.config(ip, gateway, subnet);
  // while wifi not connected yet, print '.'
  // then after it connected, get out of the loop
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.println(WiFi.localIP());  // IP address of ESP8266

  Serial.println("");

  delay(1000);

  Scheduler.start(&doorSensor_task);
  Scheduler.start(&alarm_task);
  Scheduler.start(&ultrasensor_task);

  Scheduler.begin();
}

void loop() {}



