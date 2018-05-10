@@ -43,6 +43,7 @@ int flowPin;
int rebootPin;
int resetPin;
int pulsesPerCup;
bool startingUp;

// Counts
int currCount;
@ -104,6 +105,7 @@ boolean booDirtyPrint;
void setup() {
  initGlobals();
  Serial.begin(bitsPerSecond);
  initializeDisplay();
  
  pinMode(flowPin, INPUT);           //Sets the pin as an input
  attachInterrupt(0, Flow, RISING);  //Configures interrupt 0 (pin 2 on the Arduino Uno) to run the function "Flow" 
@ -118,7 +120,7 @@ void loop() {
  noInterrupts(); //Disable the interrupts on the Arduino
  if (shouldPrintBeerTime()) {
    recordBeerEnd();
    printStatusReport();
    printStatusReport(false);
    resetBeerSession();
  } 
  else if (prevCount!=currCount) {
@ -135,11 +137,26 @@ void initGlobals() {
  flowPin = 2;
  bitsPerSecond = 9600;   // initialize serial communication at 9600 bits per second:
  pulsesPerCup = 89;
  multiplier=2.647;
  multiplier = 2.647;

  currCount=0;
  prevCount=-1;
  booDirtyPrint=false;
  currCount = 0;
  prevCount = -1;
  booDirtyPrint = false;
  startingUp = false;
}

void initializeDisplay() {
  printStatusReport(true);
  //printOutStoredData();
}

void printOutStoredData() {
  Serial.print(STR_LIFETIME_VOLUME);
  Serial.print(getLifetimeVolume());
  Serial.println(STR_LIFETIME_VOLUME_UNIT);
  
  Serial.print(STR_LIFETIME_COUNT);
  Serial.println(getLifetimeBeerCount());
}

//***************
@ -215,9 +232,14 @@ boolean isNewDay() {
//***************
void recordBeerEnd() {
  lifetimeBeerCount++;
  storeLifetimeBeerCount(); // @TODO: Abstract these two calls to a function
  
  lifetimeVolume+=count*multiplier;
  storeLifetimeVolume();
  
  
  
  setBeerCompletionDateTime();
  setBeerCompletionDateTime(); // @TODO: This function does nothing
  
  if (isNewDay()) {
    resetTonightCounts();
@ -259,11 +281,20 @@ boolean shouldPrintBeerTime() {

/*
  Print out status of the device given the last beer that was completed.
  
  @param storage boolean indicating where to read the data from
*/
void printStatusReport() {
  Serial.println(STR_BEER_TIMING + getBeerCompletionDuration() + STR_BEER_TIMING_UNIT);
  Serial.println(STR_LIFETIME_COUNT + lifetimeBeerCount);
  Serial.println(STR_LIFETIME_VOLUME + lifetimeVolume + STR_LIFETIME_VOLUME_UNIT);
void printStatusReport(bool storage) {
  if (storage) {
    //Serial.println(STR_BEER_TIMING + getBeerCompletionDuration() + STR_BEER_TIMING_UNIT);
    Serial.println(STR_LIFETIME_COUNT + getLifetimeBeerCount());
    Serial.println(STR_LIFETIME_VOLUME + getLifetimeVolume() + STR_LIFETIME_VOLUME_UNIT);
  }
  else {
    Serial.println(STR_BEER_TIMING + getBeerCompletionDuration() + STR_BEER_TIMING_UNIT);
    Serial.println(STR_LIFETIME_COUNT + lifetimeBeerCount);
    Serial.println(STR_LIFETIME_VOLUME + lifetimeVolume + STR_LIFETIME_VOLUME_UNIT);
  }
}

//***************
