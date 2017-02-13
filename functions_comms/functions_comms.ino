/*
* Functions for HTTPS request functions
* Written by: Stefan Angelov
*/

#include <ESP8266WiFi.h>

const char* ssid     = "KoljemUtorkom";
const char* password = "a28071989";

const char* host = "192.168.1.69";

void setup() {
  Serial.begin(115200);
  delay(100);

  /* Connect to the WiFi network */
  wifi_connect();
  
}

void loop() {

  big_brother();

}

void wifi_connect()
{
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Connection Established");
}

/* Big brother function that watches all of the paintings one after the other in regular intervals */
void big_brother()
{
    /* Use WiFiClient class to create TCP connections */
  WiFiClient client;
  const int httpPort = 80;


  /* URLs for painting coordinates, including a test URL */
  String test_url = "http://192.168.1.69/axis-cgi/com/ptz.cgi?center=1,1";
  String point1 = "http://192.168.1.69/axis-cgi/com/ptz.cgi?pan=-25.0&tilt=-14.2";
  String point2 = "http://192.168.1.69/axis-cgi/com/ptz.cgi?pan=-39.3&tilt=-14.2";
  String point3 = "http://192.168.1.69/axis-cgi/com/ptz.cgi?pan=-50.2&tilt=-14.2";
  String point4 = "http://192.168.1.69/axis-cgi/com/ptz.cgi?pan=-62.9&tilt=-14.2";

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  client.print(String("GET ") + point1 + " HTTP/1.1\r\n"
               + "Host:" + host + "\r\n"
               + "Connection: close \r\n\r\n");

  delay(500);

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  client.print(String("GET ") + point2 + " HTTP/1.1\r\n"
               + "Host:" + host + "\r\n"
               + "Connection: close \r\n\r\n");
               
  delay(500);
  
   if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  client.print(String("GET ") + point3 + " HTTP/1.1\r\n"
               + "Host:" + host + "\r\n"
               + "Connection: close \r\n\r\n");

  delay(500);

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  client.print(String("GET ") + point4 + " HTTP/1.1\r\n"
               + "Host:" + host + "\r\n"
               + "Connection: close \r\n\r\n");
               
  delay(500);
}

/* Alert function that focuses the painting where the alarm was tripped */
void big_brother_alert()
{
    String alert = "http://192.168.1.69/axis-cgi/com/ptz.cgi?pan=-25.0&tilt=-14.2&zoom=1000";
    
    if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  client.print(String("GET ") + alert + " HTTP/1.1\r\n"
               + "Host:" + host + "\r\n"
               + "Connection: close \r\n\r\n");

}


/* Function that requests an image that is sent attached to the email when a alarm is tripped */
void request_image()
{
  String image_request = "http://192.168.1.69/axis-cgi/jpg/image.cgi?resolution=320x240";
  
    if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  client.print(String("GET ") + image_request + " HTTP/1.1\r\n"
               + "Host:" + host + "\r\n"
               + "Connection: close \r\n\r\n");          
               
}




