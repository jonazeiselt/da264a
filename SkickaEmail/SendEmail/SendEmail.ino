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
  Send image:
  https://forum.arduino.cc/index.php?PHPSESSID=56s0bhku4016v4dlv30dh1m8k5&topic=67701.30
 */

#include "ESP8266WiFi.h"
#include <WiFiClientSecure.h>
#include <SPI.h>
#include <SD.h>

// byte mac[] = { 0x00, 0x40, 0x8C, 0xEC, 0x60, 0xA5 };
// char ssid[] = "iot";

char ssid[] = "AndroidAP";
char pass[] = "wury1981";

//char ssid[] = "Axels iPhone";
//char pass[] = "Nyttlsen";

int port = 465;
byte mac[6]; // MAC-address

IPAddress ip(192, 168, 0, 55);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

char server[] = "smtp.gmail.com";

unsigned long currentTime;
WiFiClientSecure client;
File picture;
static const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
// char tBuf[64];

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
}

// the loop function runs over and over again forever
void loop()
{
  sendEmail("Someone is too close to painting x.");       
  while(1);
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
  // WiFiClientSecure client; // SSL/TLS support
  SD.begin(4);
  picture = SD.open("picture.jpg", FILE_READ);
  
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
  client.print("Content-Type: image/jpeg; name=\"picture.jpg\"\r\nContent-Disposition: attachment; filename=\"picture.jpg\"\r\nContent-Transfer-Encoding: base64\r\n\r\n");
  encode();
  /*int clientCount = 0;
  if (myFile) 
  {
    while (myFile.available()) {
      clientCount = myFile.read(tBuf, 64);
      client.write((byte*)tBuf, clientCount);
    }
    myFile.close();
  }
  else 
  {
    Serial.println(F("File open failed"));
  }*/
  /*client.println("DATA");
  smtpResponse(client);
  client.println("from:bobguard45@gmail.com");
  client.println("to:jonazeiselt@gmail.com");
  client.println("SUBJECT: Warning!");
  smtpResponse(client);
  client.println();
  client.println(message);*/
  client.println(".");
  client.println("QUIT");
  // smtpResponse(client);

  client.stop();
  picture.close();
  Serial.println("Skickat?");
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

void encodeblock(unsigned char in[3], unsigned char out[4], int len)
{
  out[0] = cb64[in[0] >> 2]; out[1] = cb64[((in[0] & 0x03) << 4) | ((in[1] & 0xF0) >> 4)];
  out[2] = (unsigned char) (len > 1 ? cb64[((in[1] & 0x0F) << 2) | ((in[2] & 0xC0) >> 6)] : '=');
  out[3] = (unsigned char) (len > 2 ? cb64[in[2] & 0x3F] : '=');
}

void encode()
{
  unsigned char in[3], out[4]; int i, len, blocksout = 0;
  while (picture.available() != 0) 
  {
    len = 0; 
    for (i = 0; i < 3; i++) 
    {
      in[i] = (unsigned char) picture.read();
      if (picture.available() != 0) len++;
      else in[i] = 0;
    }
    
    if (len) 
    {
      encodeblock(in, out, len);
      for (i = 0; i < 4; i++) client.print(out[i]);
      blocksout++;
    }
    
    if (blocksout >= 19 || picture.available() == 0) 
    {
      if (blocksout) client.print("\r\n");
      blocksout = 0;
    }
    Serial.println("...");
  }
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
