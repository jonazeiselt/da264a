/* HC-SR04 Sensor

   This sketch reads a HC-SR04 ultrasonic rangefinder and returns the
   distance to the closest object in range. To do this, it sends a pulse
   to the sensor to initiate a reading, then listens for a pulse
   to return.  The length of the returning pulse is proportional to
   the distance of the object from the sensor.

   The circuit:
    VCC connection of the sensor attached to +5V
    GND connection of the sensor attached to ground
    TRIG connection of the sensor attached to digital pin 2
    ECHO connection of the sensor attached to digital pin 4

   Created by: Danial Mahmoud and Mardokh Kakadost (2017-02-07)
   Modified by: Stefan Angelov (2017-02-14)
*/
/*
int outPin = 10;
int inPin = 11;

void setup()
{
  Serial.begin(115200);
  pinMode(outPin, OUTPUT);
  pinMode(inPin, INPUT);
}

void loop()
{
  digitalWrite(outPin, HIGH);
  delayMicroseconds(15);     
  digitalWrite(outPin, LOW);
  int distance = pulseIn(inPin, HIGH);
  distance  = distance/58;
  Serial.println(distance);
  delay(50);     
}
*/

const int trigPin = 12;
const int echoPin = 13;
const int redLED = 2;

int breachingDistance = 20;

//int adc_filter_values[5] = {1, 1, 1, 1, 1};

void setup() {
  // initialize serial communication:
  Serial.begin(115200);
  // initialize pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(redLED, OUTPUT);
}

void loop()
{
  // variables for duration of the ping and the distance result in centimeters:
  long duration, duration_temp, cm;

  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse a priore to ensure a clean HIGH pulse:

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);


  //A moving average filter made to sooth out the HC04 sensor readings
  //  for(int i = 0; i<5; i++){ // Remove the oldest value
  //    adc_filter_values[i] = adc_filter_values[i+1];
  //  }
  //  adc_filter_values[4] = pulseIn(echoPin, HIGH); // Save the latest ADC value at the back of the array.
  duration = pulseIn(echoPin, HIGH);

  //  int adc_filter_values_total = 0;
  //  for(int i = 0; i<5; i++) {// Add up all the values
  //    adc_filter_values_total += adc_filter_values[i];
  //  }
  //  duration = adc_filter_values_total / 5;

  // convert the time into a distance
  cm = microsecondsToCentimeters(duration);

  Serial.print("Distance to object: ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();

//  // LED indicate if an object comes within a certain distance
//  if (cm <= 20) {
//    digitalWrite(redLED, HIGH);
//  } else {
//    digitalWrite(redLED, LOW);
//  }
  

  delay(10);
}

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}
