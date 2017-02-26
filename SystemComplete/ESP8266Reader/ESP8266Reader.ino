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
const int httpPort = 80;

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

int doagain = 0;

unsigned long currentTime;

/* Receive and transmit pins from and to Arduino Due */
int RMPIN = 2; // Receive on pin 2 (1/0)
int TMPIN = 4; // Transmit on pin 4 (1/0)
int RUPIN = 13;
int TUPIN = 16;

int doorSensorValue = 0;
int ultraSensorValue = 0;

int times = 0;

/* Camera rotate config: change if needed */
String pan = "137.2";
String tilt = "-0.5";

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

  continuousRotate();
}

/*
   This function reads value from Arduino Due and decideds whether an email
   has to be sent.
*/
void loop()
{
  WiFiClient client;

  String subject;
  String message;

  if (!client.connect(cameraServer, httpPort))
  {
    Serial.println("Connection to camera failed");
    return;
  }

  doorSensorValue = digitalRead(RMPIN);
  ultraSensorValue = digitalRead(RUPIN);

  if (doorSensorValue == 1 || ultraSensorValue == 1)
  {
    times++;
    if (times == 3)
    {
      times = 0;
    }
  }

  Serial.print("\ndoor sensor = ");
  Serial.println(doorSensorValue);
  Serial.print("ultrasonic sensor = ");
  Serial.println(ultraSensorValue);

  if (doorSensorValue == 0)
  {
    doagain = 0;
  }

  if ((doorSensorValue == 1 || ultraSensorValue == 1) && (times == 1))
  {
    delay(50);
    times++;
    if (doorSensorValue == 1)
    {
      doagain++;
      digitalWrite(TMPIN, HIGH);
      delay(200);
      digitalWrite(TMPIN, LOW);
      subject = alertSbj;
      message = alertMsg;
    }
    else
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
    rotatePoint(pan, tilt);
    delay(500);

    // Disconnect from WiFi to connect to another one (email)
    WiFi.disconnect();

    // Send email
    if (doagain == 1 || doagain == 0)
    {
      sendEmail(subject, message);
    }

    // Disconnect from WiFi to connect to another one (camera)
    WiFi.disconnect();
    WiFi.begin(ssid, pass); // Connect to WiFi
    WiFi.config(ip, gateway, subnet);

    /* while wifi not connected yet, print '.'
      then after it connected, get out of the loop */
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }

    continuousRotate();
  }

  doorSensorValue = 0;
  ultraSensorValue = 0;

  delay(1000);
  ESP.wdtFeed(); // Prevents crash..
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

  client.println("MAIL FROM:<" + String(emailSender) + ">");
  smtpResponse(client);

  client.println("RCPT TO:<" + String(emailRecipient) + ">");
  smtpResponse(client);

  client.println("DATA");
  smtpResponse(client);

  client.println("From:" + String(emailSender));
  client.println("To:" + String(emailRecipient));
  client.println("SUBJECT: " + subject);
  client.println(message);
  client.println(".");
  client.println("QUIT");
  smtpResponse(client);

  client.stop();
}

/*
  This function awaits response from the smtp server. It is important a response
  is received before a new command is sent from the client.
*/
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

/*
   This function makes the Axis camera to rotate to a specific point.
*/
void rotatePoint(String pan, String tilt)
{
  WiFiClient client;
  String paintingPoint = String(cameraServer) + "/axis-cgi/com/ptz.cgi?pan=" + pan + "&tilt=" + tilt + "&speed=110";

  if (!client.connect(cameraServer, httpPort))
  {
    Serial.println("Connection failed..");
  }

  Serial.print("Requesting URL: ");
  Serial.println(paintingPoint);

  client.println(String("GET ") + paintingPoint + " HTTP/1.1");
  client.println("Host: " + String(cameraServer));
  client.println("Connection: close");
  client.println();
}

/*
   This function makes the Axis camera rotate continuously.
*/
void continuousRotate()
{
  WiFiClient client;
  String casRotate = String(cameraServer) + "/axis-cgi/com/ptz.cgi?continuouspantiltmove=" + pan + "," + tilt;

  if (!client.connect(cameraServer, httpPort))
  {
    Serial.println("Connection failed..");
  }

  // This will rotate the camera
  client.print(String("GET ") + casRotate + " HTTP/1.1");
  client.println("Host: " + String(cameraServer));
  client.println("Connection: close");
  client.println();
}
