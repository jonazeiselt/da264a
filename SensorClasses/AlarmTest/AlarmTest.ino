
/*
  Alarm generator on digital pin 8 of Due
  Caller control frequency
  We use use TC1 channel 0
  Made by: Danial Mahmoud and Naser Kakadost (2017-02-16)
*/


boolean pinState = false ;
Tc *timerChannel = TC1;
int channelNumber = 0;

int alarmPin = 8;

void setup() {
  pinMode(alarmPin, OUTPUT);
}

void loop() {
  // Keep tone at frequency 300 Hz
  tone(300);
  delay(1000);
  // Stop playing tone 
  noTone(alarmPin);
  delay(1000);
}

static void enableTimer() {
  //Enable or disable write protect of PMC registers.
  pmc_set_writeprotect(false);
  //Enable the specified peripheral clock.
  pmc_enable_periph_clk(TC3_IRQn);
  // Configure timer to operate in a given mode; just sets bits in the TC_CMR register that belongs to TC3 (= Timer 1 channel 0 )
  TC_Configure(timerChannel, channelNumber, TC_CMR_TCCLKS_TIMER_CLOCK4 | TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC ); // select Waveform, Counter running up and reset when equals to contents of Register C
  // RC compare interrupt
  timerChannel->TC_CHANNEL[channelNumber].TC_IER = TC_IER_CPCS; 
  timerChannel->TC_CHANNEL[channelNumber].TC_IDR = ~TC_IER_CPCS;
  NVIC_EnableIRQ(TC3_IRQn); // enable the TC3_IRQn interrupt, so handler can be called
}

void tone(uint32_t frequency)
{
  const uint32_t rc = VARIANT_MCK / 256 / frequency; // VARIANT_MCK the frequency of arduino DUE(84MHz), rc is the value the timer counts towards
  enableTimer();
  // enable the Register C (RC) compare interrupt 
  TC_SetRC(timerChannel, channelNumber, rc);    // set frequency, this is where we actually set the rate that we want the timer to interrupt at
  // Timer is stopped after configuration so start the timer running again
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

