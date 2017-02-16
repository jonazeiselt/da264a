/*
   ImageRequest.ino

   @author Axel Lundberg and Jonas Eiselt
   @date 2017-02-16
*/

#include <ESP8266WiFi.h>

#include <SPI.h>
#include <SD.h>

const char* camera = "192.168.1.60"; // ..1.69
const char* imgReq = "http://192.168.1.60/axis-cgi/jpg/image.cgi?resolution=160x120&camera=1&compression=100&colorlevel=0&clock=1&date=1";

const char* ssid = "WhyFi";
const char* pass = "puninabun";

WiFiClient client;
File image;

char buf[32];

void setup()
{
  Serial.begin(115200);
  WiFi.begin(ssid, pass);

  if (!SD.begin(4))
  {
    Serial.println(F("SD card initialization failed.."));
  }

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(F("\nWiFi connected"));
}

void loop()
{
  if (!client.connect(camera, 80))
  {
    Serial.println("Connection failed..");
    return;
  }

  Serial.println(F("Sending request.."));
  if (client.connect(camera, 80))
  {
    client.println(String("GET ") + imgReq + " HTTP/1.1");
    client.println("Host: " + String(camera));
    client.println("Connection: close");
    client.println();
  }

  delay(500);
  image = SD.open("image.jpg", FILE_WRITE);

  int lines = 0;

  int k = 0;
  while (client.available())
  {
    char c = client.read();
    if (c == '\n')
    {
      lines++;
    }

    /*
      HTTP/1.0 200 OK
      Cache-Control: no-cache
      Pragma: no-cache
      Expires: Thu, 01 Dec 1994 16:00:00 GMT
      Connection: close
      Content-Type: image/jpeg
      Content-Length: 1361

    */
    if (lines > 7)
    {
      Serial.print(c);
      buf[k] = c;
      k++;

      if (k == 32)
      {
        for (int i = 0; i < 32; i++)
        {
          image.print(buf[i]);
        }
        k = 0;
        image.println();
      }
      // byte val = (byte) c;
      // Serial.print(val);
      // Serial.print(" ");
      // delay(100);

      // byte val = (byte) c;
      // image.print(c);

      delay(10);
    }
    for (int i = 0; i < sizeof(buf); i++)
    {
      image.print(buf[i]);
    }
    image.println();
    image.close();
  }

  while (1)
  {
    ESP.wdtFeed();
  }
}
