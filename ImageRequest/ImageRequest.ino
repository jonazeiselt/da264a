/*
   ImageRequest.ino

   @author Axel Lundberg and Jonas Eiselt
   @date 2017-02-16
*/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <SPI.h>
#include <SD.h>

const char* imgReq = "http://192.168.0.50/axis-cgi/jpg/image.cgi?resolution=160x120&camera=1&compression=100&colorlevel=0&clock=1&date=1";

const char* ssid = "Tele2Gateway6P43";
const char* pass = "y3wjamMM";

const char* host = "192.168.0.50";

IPAddress ip(192, 168, 0, 32);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

File image;

char buf[32];


void setup()
{
  Serial.begin(115200);
  delay(10);

  WiFi.begin(ssid, pass); // Connect to WiFi
  WiFi.config(ip, gateway, subnet);

  // while wifi not connected yet, print '.'
  // then after it connected, get out of the loop
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.println(WiFi.localIP());  // IP address of ESP8266
  if (!SD.begin(4))
  {
    Serial.println(F("SD card initialization failed.."));
  }
}

void loop()
{
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort))
  {
    Serial.println("Connection failed..");
    return;
  }

  Serial.println(F("Sending request.."));
  if (client.connect(host, httpPort))
  {
    client.println(String("GET ") + imgReq + " HTTP/1.1");
    client.println("Host: " + String(host));
    client.println("Connection: close");
    client.println();
  }

  delay(500);
  image = SD.open("imagedata.txt", FILE_WRITE);

  int lines = 0;

  int k = 0;
  while (client.available())
  {
    char c = client.read();
    Serial.print(c);
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
      unsigned char val = (unsigned char) c;
      image.print(val);
      image.print(" ");
      // Serial.print(c);
      // byte byteVal = (byte) c;
      // buf[k] = c; // byteVal;
      // k++;

      /*if (k == 32)
        {
        for (int i = 0; i < 32; i++)
        {
          // image.print(buf[i]);
          byte val = (byte) c;
          image.print(c);
          image.print(' ');
        }
        k = 0;
        image.println();
        /*if (k == 32)
          {
          image.write(buf, 32);
          }*/
      // byte val = (byte) c;
      //image.print(c);
      //image.print(" ");
    }
    // byte val = (byte) c;
    // Serial.print(val);
    // Serial.print(" ");
    // delay(100);

    // byte val = (byte) c;
    // image.print(c);
    image.close();
    delay(10);

    while (1)
    {
      ESP.wdtFeed();
    }
  }
}
/*for (int i = 0; i < sizeof(buf); i++)
  {
  image.print(buf[i]);
  image.print(c);
  image.print(' ');
  }
  image.println();*/

// image.write(buf, k);

