/*
  ESP8266Reader.ino
  This program connects to a network and sends an email to a desired recipient
  through Gmail's smtp server.

  Author: Axel Lundberg and Jonas Eiselt

  Setup:
  https://learn.sparkfun.com/tutorials/esp8266-thing-hookup-guide/installing-the-esp8266-arduino-addon
  Send image:
  https://forum.arduino.cc/index.php?PHPSESSID=56s0bhku4016v4dlv30dh1m8k5&topic=67701.30
*/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

/* IP Address for connecting to mobile network */
IPAddress mIp(192, 168, 43, 32);
IPAddress mGateway(192, 168, 43, 1);
IPAddress mSubnet(255, 255, 255, 0);

/* Network config: change variables if neeeded */
const char* ssid = "Tele2Gateway6P43";
const char* pass = "y3wjamMM";

const char* mSsid = "AndroidAP";
const char* mPass = "wury1981";

/* Camera config: change if needed */
const char* cameraServer = "192.168.0.50";

IPAddress ip(192, 168, 0, 32);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

/* Email config: change variables if neeeded */
const char* emailRecipient = "jonazeiselt@gmail.com"; 
const char* emailSender = "bobguard45@gmail.com";

/* Authentication config: change variables if needed */
const char* base64Login = "Ym9iZ3VhcmQ0NUBnbWFpbC5jb20=";
const char* base64Pass = "bWFyeWphbmU0NQ==";

const char* warningSbj = "Warning!";
const char* warningMsg = "The system has detected suspicious behaviour. Someone has been standing too close for too long!";

const char* alertSbj = "Alert!";
const char* alertMsg = "The system has detected a security breach. Someone has stolen a painting!";

const char* server = "smtp.gmail.com";
const int port = 465;

unsigned long currentTime;

int RPIN = 2; // Receive on pin 5 (1/0)
int TPIN = 4; // Transmit on pin 4 (1/0)

boolean flag = false;

/* https://www.lifewire.com/what-are-the-gmail-smtp-settings-1170854
  Modified code from http://www.instructables.com/id/IoT-ESP8266-Series-1-Connect-to-WIFI-Router/ */

/*
   Connects to network and calls the method sendEmail
*/
void setup()
{
  pinMode(RPIN, INPUT);
  pinMode(TPIN, OUTPUT);

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
}

void loop()
{
  WiFiClient client;
  const int httpPort = 80;
  String url = "http://192.168.0.50/axis-cgi/com/ptz.cgi?rpan=-180";
  if (!client.connect(cameraServer, httpPort))
  {
    Serial.println("Connection failed");
    return;
  }

  /* Read from Due
     IF 1
      Send 1 back to Due
      Rotate to painting
      Send Email
      Send Images to FTP server
     IF 0
      Rotate
  */
  int readValue = digitalRead(RPIN);
  Serial.print("readValue = ");
  Serial.println(readValue);
  if (readValue == 1)
  {
    digitalWrite(TPIN, HIGH);
    delay(200);    
    digitalWrite(TPIN, LOW);

    if (client.connect(cameraServer, httpPort))
    {
      Serial.println("Connected");

      Serial.print("Requesting URL: ");
      Serial.println(url);

      // This will send the request to the camera
      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                   "Host: " + cameraServer + "\r\n" +
                   "Connection: close\r\n\r\n");
    }
    // Disconnect from WiFi to connect to another one
    WiFi.disconnect();

    // Send email
    sendEmail(alertSbj, alertMsg);
       
    // Disconnect from WiFi to connect to another one
    WiFi.disconnect();
    WiFi.begin(ssid, pass); // Connect to WiFi
    WiFi.config(ip, gateway, subnet);

    // while wifi not connected yet, print '.'
    // then after it connected, get out of the loop
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
    
    // Send images to FTP-server 
    trigger_alert();
  }
  else
  {
    if (client.connect(cameraServer, httpPort))
    {
      Serial.println("Connected");

      Serial.print("Requesting URL: ");
      Serial.println(url);

      // This will send the request to the camera
      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                   "Host: " + cameraServer + "\r\n" +
                   "Connection: close\r\n\r\n");
    }
  }
  delay(2500);
  ESP.wdtFeed();
}

/* SMTP (Used for sending email)
   -----------------------------
   HELO smtp.gmail.com
   MAIL FROM: <museum@gmail.com>
   RCPT TO: <security@gmail.com>
   DATA
   Someone is too close to the painting!
   QUIT

   Modified code from:
   https://forum.arduino.cc/index.php?topic=126829.0
*/
void sendEmail(String subject, String message)
{
  WiFiClientSecure client;
  WiFi.begin(mSsid, mPass); // Connect to WiFi
  WiFi.config(mIp, mGateway, mSubnet);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.println(WiFi.localIP());  // IP address of ESP8266
  
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
  client.println(base64Login); // mail
  smtpResponse(client);
  client.println(base64Pass);     //pass
  smtpResponse(client);

  client.println("MAIL FROM:<bobguard45@gmail.com>");
  smtpResponse(client);

  client.println("RCPT TO:<jonazeiselt@gmail.com>");
  smtpResponse(client);

  client.println("DATA");
  smtpResponse(client);

  client.println("from:" + String(emailSender));
  client.println("to:" + String(emailRecipient));
  client.println("SUBJECT: " + subject);
  smtpResponse(client);
  client.println(message);
  client.println(".");
  client.println("QUIT");
  smtpResponse(client);

  client.stop();
}

void smtpResponse(WiFiClientSecure &client)
{
  currentTime = millis();
  while (!client.available())
  {
    // Make sure that the smtp server responds before a new command is sent
    if ((currentTime + 10000) < millis())
    {
      Serial.println("Error: Timeout" + millis());
      return;
    }
  }
  String res = client.readStringUntil('\n');
  Serial.println(res);
}

/* Function that triggers the event to send pictures to FTP-server */
void trigger_alert()
{
  WiFiClient client;
  const int httpPort = 80;

  String trigger_on = "http://192.168.0.50/axis-cgi/io/virtualinput.cgi?action=4:/";
  String trigger_off = "http://192.168.0.50/axis-cgi/io/virtualinput.cgi?action=4:\\";

  if (!client.connect(cameraServer, httpPort)) 
  {
    Serial.println("connection failed");
    return;
  }

  client.print(String("GET ") + trigger_on + " HTTP/1.1\r\n"
               + "Host:" + cameraServer + "\r\n"
               + "Connection: close \r\n\r\n");

  delay(500);

  if (!client.connect(cameraServer, httpPort)) 
  {
    Serial.println("connection failed");
    return;
  }

  client.print(String("GET ") + trigger_off + " HTTP/1.1\r\n"
               + "Host:" + cameraServer + "\r\n"
               + "Connection: close \r\n\r\n");
}
