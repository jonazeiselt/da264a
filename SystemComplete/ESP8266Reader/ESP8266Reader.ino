/*
  ESP8266Reader.ino
  This program connects to a network and sends an email to a desired recipient
  through Gmail's smtp server. A simple communication between Arduino Due and
  ESP8266 is handled.

  @author Axel Lundberg and Jonas Eiselt
  @date 2017-02-21
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
const char* cameraServer = "192.168.1.50";

IPAddress ip(192, 168, 1, 32);
IPAddress gateway(192, 168, 1, 1);
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

/* Receive and transmit pins from and to Arduino Due */
int RMPIN = 2; // Receive on pin 2 (1/0)
int TMPIN = 4; // Transmit on pin 4 (1/0)
int RUPIN = 13;
int TUPIN = 16;

int doorSensorValue = 0;
int ultraSensorValue = 0;

/*
   Connects to network and calls the method sendEmail
*/
void setup()
{
  pinMode(RMPIN, INPUT);
  pinMode(TMPIN, OUTPUT);
  pinMode(RUPIN, INPUT);
  pinMode(TUPIN, OUTPUT);

  Serial.begin(115200);
  delay(10);

  WiFi.begin(ssid, pass); // Connect to WiFi (camera)
  WiFi.config(ip, gateway, subnet);

  /* while wifi not connected yet, print '.'
    then after it connected, get out of the loop */
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
  String rotate = "http://192.168.1.50/axis-cgi/com/ptz.cgi?rpan=-180&speed=30";
  String paintingPoint = "http://192.168.1.50/axis-cgi/com/ptz.cgi?pan=137.2&tilt=-0.5&speed=60";

  String subject;
  String message;

  if (!client.connect(cameraServer, httpPort))
  {
    Serial.println("Connection to camera failed");
    return;
  }

  /*
     M = value from door sensor
     U = value from ultrasonic sensor

     Read from Due
     IF M=1
      Send 1 back to Due
      Rotate to painting
      Send Images to FTP server
      Send Email
     IF U=1
      Send 1 back to Due
      Rotate to painting
      Send Images to FTP server
      Send email
     ELSE
      Rotate
  */
  doorSensorValue = digitalRead(RMPIN);
  ultraSensorValue = digitalRead(RUPIN);

  Serial.print("\ndoor sensor = ");
  Serial.println(doorSensorValue);
  Serial.print("ultrasonic sensor = ");
  Serial.println(ultraSensorValue);
  
  if ((doorSensorValue == 1 || ultraSensorValue == 1))
  {
    if (doorSensorValue == 1)
    {
      digitalWrite(TMPIN, HIGH);
      delay(200);
      digitalWrite(TMPIN, LOW);

      subject = alertSbj;
      message = alertMsg;
    }
    else if(ultraSensorValue == 1)
    {
      digitalWrite(TUPIN, HIGH);
      delay(200);
      digitalWrite(TUPIN, LOW);

      subject = warningSbj;
      message = warningMsg;
    }

    if (!client.connect(cameraServer, httpPort))
    {
      Serial.println("Connection failed");
      return;
    }
    
    if (client.connect(cameraServer, httpPort))
    {
      Serial.println("Connected");

      Serial.print("Requesting URL: ");
      Serial.println(paintingPoint);

      // This will rotate the camera
      client.print(String("GET ") + paintingPoint + " HTTP/1.1\r\n" +
                   "Host: " + cameraServer + "\r\n" +
                   "Connection: close\r\n\r\n");

      delay(500);
    }
    // Send images to FTP-server
    trigger_alert();

    // Disconnect from WiFi to connect to another one
    WiFi.disconnect();

    // Send email
    sendEmail(subject, message);

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
  }
  else
  {
    if (client.connect(cameraServer, httpPort))
    {
      Serial.println("Connected");

      Serial.print("Requesting URL: ");
      Serial.println(rotate);

      // This will send the request to the camera
      client.print(String("GET ") + rotate + " HTTP/1.1\r\n" +
                   "Host: " + cameraServer + "\r\n" +
                   "Connection: close\r\n\r\n");
    }
  }
  doorSensorValue = 0;
  ultraSensorValue = 0;
  
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
  client.println(base64Login); // eMail (Base64 encoded)
  smtpResponse(client);
  client.println(base64Pass); // password (Base64 encoded)
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
    if (millis() > (currentTime + 10000))
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

  String trigger_on = "http://192.168.1.50/axis-cgi/io/virtualinput.cgi?action=4:/";
  String trigger_off = "http://192.168.1.50/axis-cgi/io/virtualinput.cgi?action=4:\\";

  if (!client.connect(cameraServer, httpPort))
  {
    Serial.println("connection failed");
    return;
  }

  client.print(String("GET ") + trigger_on + " HTTP/1.1\r\n"
               + "Host:" + cameraServer + "\r\n"
               + "Connection: close \r\n\r\n");

  delay(100);

  if (!client.connect(cameraServer, httpPort))
  {
    Serial.println("connection failed");
    return;
  }

  client.print(String("GET ") + trigger_off + " HTTP/1.1\r\n"
               + "Host:" + cameraServer + "\r\n"
               + "Connection: close \r\n\r\n");
}
