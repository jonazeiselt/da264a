/*
  SendEmail

  This program connects to a network and sends an email to a desired recipient
  through Gmail's smtp server.

  Author: Axel Lundberg and Jonas Eiselt

  Further reading:
  ...
  ...
  ...

  Setup:
  https://learn.sparkfun.com/tutorials/esp8266-thing-hookup-guide/installing-the-esp8266-arduino-addon
 */

#include "ESP8266WiFi.h"
#include <WiFiClientSecure.h>

// byte mac[] = { 0x00, 0x40, 0x8C, 0xEC, 0x60, 0xA5 };
// char ssid[] = "iot";

// char ssid[] = "AndroidAP";
// char pass[] = "wury1981";

char ssid[] = "Axels iPhone";
char pass[] = "Nyttlsen";

int port = 465;
byte mac[6]; // MAC-address

IPAddress ip(192, 168, 0, 55);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

char server[] = "smtp.gmail.com";

unsigned long currentTime;

/* https://www.lifewire.com/what-are-the-gmail-smtp-settings-1170854
 Modified code from http://www.instructables.com/id/IoT-ESP8266-Series-1-Connect-to-WIFI-Router/ */

/*
 * Connects to network and calls the method sendEmail
 */
void setup()
{
  Serial.begin(115200);
  WiFi.begin(ssid, pass); // Connect to WiFi

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

  // initialize digital pin 13 as an output.
  pinMode(2, OUTPUT);

  sendEmail("Someone is too close to painting x.");
  
}

// the loop function runs over and over again forever
void loop()
{
  digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);              // wait for a second
  digitalWrite(2, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);              // wait for a second
}

/* SMTP (Used for sending email)
 * -----------------------------
 * HELO smtp.gmail.com
 * MAIL FROM: <museum@gmail.com>
 * RCPT TO: <security@gmail.com>
 * DATA
 * Someone is too close to the painting!
 * QUIT
 * https://forum.arduino.cc/index.php?topic=126829.0 modified
 */
void sendEmail(String message)
{
  WiFiClientSecure client; // SSL/TLS support

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
  client.println(message);
  client.println(".");
  client.println(".");
  client.println("QUIT");
  smtpResponse(client);
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

void printMacAddress()
{
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);
}

