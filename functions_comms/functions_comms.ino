/*
  Functions for HTTPS request functions
  Written by: Stefan Angelov

  Modified: Axel Lundberg and Jonas Eiselt
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
  WiFiClient client;
  const int httpPort = 80;
  String alert = "http://192.168.1.69/axis-cgi/com/ptz.cgi?pan=-25.0&tilt=-14.2&zoom=1000";

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  client.print(String("GET ") + alert + " HTTP/1.1\r\n"
               + "Host:" + host + "\r\n"
               + "Connection: close \r\n\r\n");

}

/* Function that triggers the event to send email+picture */
void trigger_alert()
{
  WiFiClient client;
  const int httpPort = 80;

  String trigger_on = "http://192.168.1.101/axis-cgi/io/virtualinput.cgi?action=6:/";
  String trigger_off = "http://192.168.1.101/axis-cgi/io/virtualinput.cgi?action=6:\\";

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  client.print(String("GET ") + trigger_on + " HTTP/1.1\r\n"
               + "Host:" + host + "\r\n"
               + "Connection: close \r\n\r\n");

  delay(500);

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  client.print(String("GET ") + trigger_off + " HTTP/1.1\r\n"
               + "Host:" + host + "\r\n"
               + "Connection: close \r\n\r\n");

}

/* Uses virual port 2 */
void sendNotification()
{
  const char* reqOn = "http://192.168.1.59/axis-cgi/io/virtualinput.cgi?action=2:/";
  const char* reqOff = "http://192.168.1.59/axis-cgi/io/virtualinput.cgi?action=2:\\";

  if (!client.connect(host, 80))
  {
    Serial.println("Connection failed..");
  }

  client.println(String("GET ") + reqOn + " HTTP/1.1");
  client.println("Host: " + String(host));
  client.println("Connection: close");
  client.println();

  delay(500); 

  if (!client.connect(host, 80))
  {
    Serial.println("Connection failed..");
  }
  
  client.println(String("GET ") + reqOff + " HTTP/1.1");
  client.println("Host: " + String(host));
  client.println("Connection: close");
  client.println();
}

/* Uses virtual port 3 */
void sendImage()
{
  const char* reqOn = "http://192.168.1.59/axis-cgi/io/virtualinput.cgi?action=3:/";
  const char* reqOff = "http://192.168.1.59/axis-cgi/io/virtualinput.cgi?action=3:\\";

  if (!client.connect(host, 80))
  {
    Serial.println("Connection failed..");
  }

  client.println(String("GET ") + reqOn + " HTTP/1.1");
  client.println("Host: " + String(host));
  client.println("Connection: close");
  client.println();

  delay(500); 

  if (!client.connect(host, 80))
  {
    Serial.println("Connection failed..");
  }
  
  client.println(String("GET ") + reqOff + " HTTP/1.1");
  client.println("Host: " + String(host));
  client.println("Connection: close");
  client.println();
}

void continuousRotate()
{
    String casRotate = "http://192.168.1.69/axis-cgi/com/ptz.cgi?continuouspantiltmove=20,20";
   
    if (!client.connect(host, 80))
  {
    Serial.println("Connection failed..");
  }
  
  client.println(String("GET ") + casRotate + " HTTP/1.1");
  client.println("Host: " + String(host));
  client.println("Connection: close");
  client.println();
}
