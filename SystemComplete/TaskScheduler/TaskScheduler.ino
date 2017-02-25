/*
 *  A multitasking sensor-system for security-yearning nuts. 
 *  Task are scheduled by TaskScheduler.h. 
 *  
 *  Created by: Danial Mahmoud and Naser Kakadost (2017-02-13)
 *  Updated: several times, most recent 2017-02-25
 */

#include <TaskScheduler.h>
#include <Arduino.h>

// important pins and variables related to ultrasonicsensor's acitivites
int trigPin = 12;
int echoPin = 13;
int redLED = 2;
int breachingDistance = 20;
int serialPin = 23;
int checkPin = 22;
long currentMillis = 0;

// door sensor pins
int INPUT_PIN = 9;
int greenLED = 3;

// pins and variables pertaining to alarm
boolean pinState = false ;
Tc *timerChannel = TC1;
int channelNumber = 0;
int alarmPin = 8;

// declaration of tasks
void doorSensorTask();
void ultraSensorTask();
void alarmTask();
void serialComDoorSensor();
void serialComUltraSensor();

// create tasks, define their periods and endurance
Task t1(10, TASK_FOREVER, &doorSensorTask);
Task t2(50, TASK_FOREVER, &ultraSensorTask); //300
Task t3(10, TASK_FOREVER, &alarmTask);
Task t4(30, TASK_FOREVER, &serialComDoorSensor);
Task t5(30, TASK_FOREVER, &serialComUltraSensor);

// Declare scheduler
Scheduler runner;

/*
 * Door sensor has a nifty way of squeeling if the magnetic contact-switch 
 * that holds the sensor togheter is broken. This task performs just that.
 * If the contact is shattered then a high signal read by the input-pin and
 * a comfy little LED is lit.
*/
void doorSensorTask() {
  int inputState = digitalRead(INPUT_PIN);
  if (inputState == LOW) {
    Serial.println("Locked");
    digitalWrite(greenLED, LOW);
  } else {
    Serial.println("Open");
    digitalWrite(greenLED, HIGH);
  }
}

/*
 * Ultrasonic sensors come in handy when you want to detect object from a certain
 * distance. The task below enables the user to measure distance to object far away
 * or adjacent. The purpose this task fulfills for the safety-system is that it 
 * divulges if someone is too close to the painting.
*/
void ultraSensorTask() {
  long duration, cm;

  // Trigger sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the signal from the sensor
  duration = pulseIn(echoPin, HIGH);

  // convert the time into a distance
  cm = duration / 58;

  Serial.print("Distance to object: ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();

  // LED indicate if an object comes within a certain distance
  if (cm <= breachingDistance) {
    digitalWrite(redLED, HIGH);
    digitalWrite(checkPin, HIGH);
  } else {
    digitalWrite(redLED, LOW);
    digitalWrite(checkPin, LOW);
  }
  delay(10);

}

/*
 * You might be under the impression that a buzzer-alarm with only two pins 
 * (with one of them being GND) shouldn't be so intricate to configure. Well you're
 * wrong and ignorant. It was an arduous, painstaking job to make the buzzer go buzz. 
 * This task will sound the alarm if somone swipes the painting and busts the magnetic
 * contact switch of the door sensor.
*/
void alarmTask() {
  if (digitalRead(INPUT_PIN) == HIGH) {
    // Keep tone at frequency 300 Hz
    tone(300);
  } else {
    noTone(alarmPin);
  }
}

/*
  This task alerts the ESP that the door sensor magnetic contact has been broken.
*/

void serialComDoorSensor() {
  if (digitalRead(INPUT_PIN) == HIGH) {
    while (digitalRead(4) == LOW) {
      digitalWrite(5, HIGH);
    }
    digitalWrite(5, LOW);
  }

}

/*
   If someone stands to close to painting for more than ten seconds this task will let the
   ESP know. Every time block of if-statement is entered, currentMillis is incremented.
   Loop executes every 30ms ---> 10 seconds equals 333,333...
*/
void serialComUltraSensor() {
  //ultrasonic sensor detects that someone is too close to painting
  if (digitalRead(redLED) == HIGH) {
    currentMillis++;
    //if 10 seconds has passed, signal to ESP that someone has stood at the painting too long
    if (currentMillis == 333) {
      while (digitalRead(checkPin) == LOW) {
        // send signal until ESP signals back 'roger that'
        digitalWrite(serialPin, HIGH);
        //delay(2000);
      }
      digitalWrite(serialPin, LOW);
      currentMillis = 0;
    }
  } else {
    currentMillis = 0;
  }
}

void setup() {
  Serial.begin(115200);

  //Initiate pins related to ultrasonic sensor's activity
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(redLED, OUTPUT);
  pinMode(checkPin, INPUT);
  pinMode(serialPin, OUTPUT);

  // Setup the door sensors pins
  pinMode(greenLED, OUTPUT);
  pinMode(INPUT_PIN, INPUT_PULLUP);

  // Setup pin for alarm
  pinMode(alarmPin, OUTPUT);

  // Communication pins for door sensor
  pinMode(5, OUTPUT);
  pinMode(4, INPUT);

  // Initiate scheduler
  runner.init();

  // Scheduler adds task
  runner.addTask(t1);
  runner.addTask(t2);
  runner.addTask(t3);
  runner.addTask(t4);
  runner.addTask(t5);

  delay(1000);

  // Enable tasks
  t1.enable();
  t2.enable();
  t3.enable();
  t4.enable();
  t5.enable();
}

void loop () {
  runner.execute();
}


static void enableTimer() {
  //Enable or disable write protect of PMC registers.
  pmc_set_writeprotect(false);
  //Enable the specified peripheral clock.
  pmc_enable_periph_clk(TC3_IRQn);
  // Configure timer to operate in a given mode: TC_configure just sets bits in the TC_CMR register that belongs to TC3 (= Timer 1 channel 0 )
  TC_Configure(timerChannel, channelNumber, TC_CMR_TCCLKS_TIMER_CLOCK4 | TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC ); // select Waveform, Counter running up and reset when equal to contents of Register C
  // RC compare interrupt
  timerChannel->TC_CHANNEL[channelNumber].TC_IER = TC_IER_CPCS; 
  timerChannel->TC_CHANNEL[channelNumber].TC_IDR = ~TC_IER_CPCS;
  NVIC_EnableIRQ(TC3_IRQn); // enable the TC3_IRQn interrupt, so handler can be called
}

/*
 * Tone uses a timer interrupt to make a sound, every time the interrupt fires off.
 */
void tone(uint32_t frequency)
{
  const uint32_t rc = VARIANT_MCK / 256 / frequency; // VARIANT_MCK the frequency of arduino DUE(84MHz), rc is the value the timer counts towards
  enableTimer();
  // enable the Register C (RC) compare interrupt 
  TC_SetRC(timerChannel, channelNumber, rc);    // set frequency, this is where we actually set the rate that we want the timer to interrupt at
  TC_Start(timerChannel, channelNumber);  // start TC 1, channel 0
}

void noTone(int pin)
{
  TC_Stop(timerChannel, channelNumber);  // stop timer
  digitalWrite(pin, LOW); // no signal on pin
}

// Interrupt Service Routine for the timer (TC1 ch 0), in this ISR we toggle digital pin
void TC3_Handler () {
  TC_GetStatus(TC1, 0);
  digitalWrite(alarmPin, pinState = !pinState);
}

