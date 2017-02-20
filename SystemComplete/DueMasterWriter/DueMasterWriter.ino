
// Include Scheduler since we want to manage multiple tasks.
#include <Scheduler.h>
#include <Arduino.h>


int trigPin = 12;
int echoPin = 13;
int redLED = 2;

int INPUT_PIN = 9;
int greenLED = 3;

boolean pinState = false ;
Tc *timerChannel = TC1;
int channelNumber = 0;

int alarmPin = 8;

void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(redLED, OUTPUT);

  // Setup the magnetsensors pins
  pinMode(greenLED, OUTPUT);
  pinMode(INPUT_PIN, INPUT_PULLUP);

  // Setup pin for alarm
  pinMode(alarmPin, OUTPUT);

  pinMode(5, OUTPUT);
  pinMode(4, INPUT);

  // Add "loop2" and "loop3" to scheduling.
  // "loop" is always started by default.
  Scheduler.startLoop(loop2);
  Scheduler.startLoop(loop3);
  Scheduler.startLoop(loop4);
}

static void enableTimer() {
  //Enable or disable write protect of PMC registers.
  pmc_set_writeprotect(false);
  //Enable the specified peripheral clock.
  pmc_enable_periph_clk(TC3_IRQn);
  // Configure timer: TC_configure just sets bits in the TC_CMR register that belongs to TC3 (= Timer 1 channel 0 )
  TC_Configure(timerChannel, channelNumber, TC_CMR_TCCLKS_TIMER_CLOCK4 | TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC ); // Waveform, Counter running up and reset when equals to RC,                                                                     
  timerChannel->TC_CHANNEL[channelNumber].TC_IER = TC_IER_CPCS; // RC compare interrupt
  timerChannel->TC_CHANNEL[channelNumber].TC_IDR = ~TC_IER_CPCS;
  NVIC_EnableIRQ(TC3_IRQn); // enable the TC3_IRQn interrupt, so handler can be called
}

void tone(uint32_t frequency)
{
  const uint32_t rc = VARIANT_MCK / 256 / frequency; // VARIANT_MCK the frequency of arduino DUE(84MHz), rc is the value the timer counts towards
  enableTimer();
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

void loop() {
  int inputState = digitalRead(INPUT_PIN);
  if (inputState == LOW) {
    Serial.println("Locked");
    digitalWrite(greenLED, LOW);
  } else {
    Serial.println("Open");
    digitalWrite(greenLED, HIGH);
  }

  yield();
}

void loop2() {
  // variables for duration of the ping and the distance result in centimeters:
  long duration, cm;

  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse a priore to ensure a clean HIGH pulse:

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.

  duration = pulseIn(echoPin, HIGH);

  // convert the time into a distance
  cm = duration / 29 / 2;

  Serial.print("Distance to object: ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();

  // LED indicate if an object comes within a certain distance
  if (cm <= 20) {
    digitalWrite(redLED, HIGH);
  } else {
    digitalWrite(redLED, LOW);
  }
  delay(300);

  yield();
}

void loop3() {
  if (digitalRead(INPUT_PIN) == HIGH) {
    // Keep tone at frequency 300 Hz
    tone(300);
    delay(1000);
    // Stop playing tone
    noTone(alarmPin);
    delay(1000);
  }
  yield();
}

/*
   This loop checks if the ESP wants to stop receiving a high signal that
   alerts it about the event when the magnet contact switch breaks.
*/
void loop4() {
  if (digitalRead(INPUT_PIN) == HIGH) {
    if (digitalRead(4) == HIGH) {
      digitalWrite(5, LOW);
    } else {
      digitalWrite(5, HIGH);
    }
  }
  yield();
}

