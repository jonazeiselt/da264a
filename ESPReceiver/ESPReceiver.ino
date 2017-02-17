/*
   ESPReceiver.ino
*/

#include <WiFiClientSecure.h>
#include <ESP8266WiFi.h>

WiFiClientSecure client; // SSL/TLS support

const char* ssid = "AndroidAP";
const char* pass = "wury1981";

const char* server = "smtp.gmail.com";
const int port = 465;

const int rcvPin = 5;
unsigned long currentTime;

void setup()
{
  Serial.begin(9600);
  WiFi.begin(ssid, pass); // Connect to WiFi

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.println(WiFi.localIP());  // IP address of ESP8266

  pinMode(rcvPin, INPUT); // receives 1 or 0 on this pin
  attachInterrupt(rcvPin, eMailInterrupt, RISING);
}

void loop()
{
  int val = digitalRead(rcvPin);
  Serial.println(val);

  //if (val)
  //{
   // eMailInterrupt();
  //}
  ESP.wdtFeed();
}

void eMailInterrupt()
{
  detachInterrupt(rcvPin);
  if (!client.connect(server, port))
  {
    Serial.println("Connection failed");
  }
  smtpResponse(client);
  Serial.println("Connection successful");

  client.println("HELO smtp.gmail.com");
  smtpResponse(client);
  client.println("AUTH LOGIN");
  smtpResponse(client);
  client.println("Ym9iZ3VhcmQ0NUBnbWFpbC5jb20=");
  smtpResponse(client);
  client.println("bWFyeWphbmU0NQ==");
  smtpResponse(client);

  client.println("MAIL FROM:<bobguard45@gmail.com>");
  smtpResponse(client);

  client.println("RCPT TO:<jonazeiselt@gmail.com>");
  smtpResponse(client);

  client.println("DATA");
  smtpResponse(client);

  client.println("From: bobguard45@gmail.com");
  client.println("To: jonazeiselt@gmail.com");
  client.println("Subject: Alert!");
  smtpResponse(client);

  client.println("Someone is stealing a painting!");
  client.println(".");
  client.println("QUIT");
  smtpResponse(client);

  client.stop();

  Serial.println("~~ Mail sent ~~");

  attachInterrupt(rcvPin, eMailInterrupt, RISING);
}

void smtpResponse(WiFiClientSecure client)
{
  currentTime = millis();
  while (!client.available())
  {
    if (millis() > (currentTime + 10000))
    {
      Serial.println("Error: Timeout" + millis());
      return;
    }
  }
  String res = client.readStringUntil('\n');
  Serial.println(res);
}
