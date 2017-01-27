/*
  SendEmail

  Author: Axel Lundberg and Jonas Eiselt

  Setup:
  https://learn.sparkfun.com/tutorials/esp8266-thing-hookup-guide/installing-the-esp8266-arduino-addon
 */

//#include "ESP8266WiFi.h"
#include <ESP8266WiFi.h>

// byte mac[] = { 0x00, 0x40, 0x8C, 0xEC, 0x60, 0xA5 };
char ssid[] = "iot";
int port = 465;
byte mac[6]; // MAC-address
IPAddress ip(192, 168, 0, 55);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

char server[] = "smtp.gmail.com";

WiFiClient client;

/*https://www.lifewire.com/what-are-the-gmail-smtp-settings-1170854 */
/* Modified code from http://www.instructables.com/id/IoT-ESP8266-Series-1-Connect-to-WIFI-Router/ */
void setup()
{
  Serial.begin(115200);
  // Connect to WiFi
  WiFi.begin(ssid);


  WiFi.config(ip, gateway, subnet);
  // while wifi not connected yet, print '.'
  // then after it connected, get out of the loop
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }


  Serial.println("\nWiFi connected");
  Serial.println(WiFi.localIP());  // IP address of ESP8266

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

  // initialize digital pin 13 as an output.
  pinMode(2, OUTPUT);

  sendEmail();
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
void sendEmail()
{
  client.connect(server,port);
  Serial.println("connected");
  client.println("EHLO www.gmail.com");
  client.println("AUTH LOGIN");
  client.println("Ym9iZ3VhcmQ0NUBnbWFpbC5jb20="); // mail
  client.println("bWFyeWphbmU0NQ==");     //pass
  client.println("MAIL FROM:<bobguard45@gmail.com>");
  client.println("RCPT TO:<jonazeiselt@gmail.com>");
  client.println("DATA");
  client.println("from:bobguard45@gmail.com");
  client.println("to:axel0lundberg@gmail.com");
  client.println("SUBJECT: Warning!");
  client.println();
  client.println("Someone is too close to painting x.");
  client.println(".");
  client.println(".");
  client.println("QUIT");
  if (client.available()) {
    char a = client.read();
    Serial.print(a);
  }
}

void smtpResponse()
{
  String res = client.readStringUntil('\n');
  Serial.println(res);
}

