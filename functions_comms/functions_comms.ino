/*
 *  Simple HTTP get webclient test
 */

#include <ESP8266WiFi.h>

const char* ssid     = "KoljemUtorkom";
const char* password = "a28071989";

const char* host = "192.168.1.69";

void setup() {
  Serial.begin(115200);
  delay(100);

  // We start by connecting to a WiFi network
  wifi_connect();
}

void loop() {
  delay(100);
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  // We now create a URL for the request
  String url = "http://192.168.1.69/axis-cgi/com/ptz.cgi?center=1,1";
  String point1 = "http://192.168.1.69/axis-cgi/com/ptz.cgi?pan=-25.0&tilt=-14.2";
  String point2 = "http://192.168.1.69/axis-cgi/com/ptz.cgi?pan=-39.3&tilt=-14.2";
  String point3 = "http://192.168.1.69/axis-cgi/com/ptz.cgi?center=1,1";
  String point4 = "http://192.168.1.69/axis-cgi/com/ptz.cgi?center=1,1";

  // This will send the request to the server
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


