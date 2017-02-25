/* HC-SR04 Sensor

   The sketch initates the ultrasonic sensors trigg pulse which releases a burst
   of ultrasounds. Echo-pin is raised to detect any incoming signal. Distance to
   closest object within discernible range is written in Serial Monitor.

   Created by: Danial Mahmoud and Mardokh Kakadost (2017-02-07)
   Modified by: Stefan Angelov (2017-02-14)
*/

int trigPin = 12;
int echoPin = 13;
int redLED = 2;

// Variables for detected objects
long duration, cm;

// Minimum allowed distance to painting
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
  // Trigger sensor by giving a LOW pulse for 2 microsec and then a HIGH pulse for 10 microsec:
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

  // raise echo to detect incoming signals
  duration = pulseIn(echoPin, HIGH);

  //  int adc_filter_values_total = 0;
  //  for(int i = 0; i<5; i++) {// Add up all the values
  //    adc_filter_values_total += adc_filter_values[i];
  //  }
  //  duration = adc_filter_values_total / 5;

  // convert the time into distance in cm
  cm = duration / 58;

  Serial.print("Distance to object: ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();

  // LED indicates if an object comes within a certain distance
  if (cm <= 20) {
    digitalWrite(redLED, HIGH);
  } else {
    digitalWrite(redLED, LOW);
  }

  delay(10);
}

