/*--------------------------
  @title:   Vocatus: Flow Meter - Making the world a better place
  @author:  Stephen Lago & Taylor White 
  @date:    May 2, 2018
  @purpose: Store and calculate flow meter data for a beer bong.
  --------------------------
*/

/*--------------------------
  Useful resources:
    http://playground.arduino.cc/Code/Time
    https://playground.arduino.cc/Code/Stopwatch
    https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
    https://www.arduino.cc/en/Reference/StyleGuide
    https://www.bc-robotics.com/tutorials/using-a-flow-sensor-with-arduino/
    https://www.norwegiancreations.com/2017/02/using-eeprom-to-store-data-on-the-arduino/
    https://www.makeuseof.com/tag/use-arduino-eeprom-save-data-power-cycles/
    https://www.arduino.cc/en/Reference/EEPROM
  --------------------------
*/

/*--------------------------
  Dev Resources:
  Use for comments, determination of proper conventions in PQAing this code, in this order of correctness:
    1. Ed Parrish's C++ Style Guide - http://www.edparrish.net/common/cppdoc.html
    2. C++ Style Guide - https://google.github.io/styleguide/cppguide.html
  
  Exceptions to PQA resources linked:
    Use tabs, not spaces. Use the arduino web IDE to determine what the tab keystroke translates to.
    
  --------------------------
*/

#include <EEPROM.h>

//***************
// Globals
//***************
// Init & Constants
int bitsPerSecond;
int changeDisplayPin;
int flowPin;
int rebootPin;
int resetPin;
int pulsesPerCup;

// Counts
int currCount;
int prevCount;

int lifetimeBeerCount;
int totalBeersTonight;

float lifetimeVolume;
float totalVolumeTonight;

int fastestBeerTime;
int fastestBeerTonightTime;
float multiplier;

double flowRate;
volatile int count;

// Display strings
const String STR_BEER_TIMING          = "Time: ";
const String STR_BEER_TIMING_UNIT     = " ms!";
const String STR_PREV_COUNT           = "Prev: ";
const String STR_CURR_COUNT           = "Curr: ";
const String STR_FASTEST_TIME         = "Fastest time: ";
const String STR_LIFETIME_COUNT       = "Total beers: ";
const String STR_LIFETIME_VOLUME      = "Total volume: ";
const String STR_LIFETIME_VOLUME_UNIT = " ml";
const String STR_TONIGHT              = " tonight";

// Addresses
const int ADDR_FASTEST_BEER           = 0;
const int ADDR_BEER_COUNT             = 1*sizeof(int);
const int ADDR_LIFETIME_VOLUME        = 2*sizeof(int);

const int ADDR_TONIGHT_FASTEST_BEER   = 10*sizeof(int);
const int ADDR_TONIGHT_BEER_COUNT     = 11*sizeof(int);
const int ADDR_TONIGHT_VOLUME         = 12*sizeof(int);


// Timing
unsigned long endTime;
unsigned long startTime;
int lastBeerDay;
int lastBeerHour;
int currentBeerDay;
int currentBeerHour;

const unsigned long SECONDS_IN_DAY = 86400;
unsigned long lastBeerCompletionInstant;
unsigned long currentBeerCompletionInstant;

// States
boolean booDirtyPrint;


//***************
// Core
//***************
void setup() {
  initGlobals();
  Serial.begin(bitsPerSecond);
  
  pinMode(flowPin, INPUT);           //Sets the pin as an input
  attachInterrupt(0, Flow, RISING);  //Configures interrupt 0 (pin 2 on the Arduino Uno) to run the function "Flow" 

}

void loop() {
  prevCount=currCount;
  currCount=count;
  interrupts();   //Enables interrupts on the Arduino
  delay (1000);   //Wait 1 second 
  noInterrupts(); //Disable the interrupts on the Arduino
  if (shouldPrintBeerTime()) {
    recordBeerEnd();
    printStatusReport();
    resetBeerSession();
  } 
  else if (prevCount!=currCount) {
    //Serial.println(STR_PREV_COUNT + prevCount);
    //Serial.println(STR_CURR_COUNT + currCount);
  }
}

//***************
// Initialize
//***************
void initGlobals() {
  resetPin = 3;
  flowPin = 2;
  bitsPerSecond = 9600;   // initialize serial communication at 9600 bits per second:
  pulsesPerCup = 89;
  multiplier=2.647;

  currCount=0;
  prevCount=-1;
  booDirtyPrint=false;
}

//***************
// Interrupts
//***************
void Flow() {
  if (count==0) {
      beerStart();
      booDirtyPrint=true;
  }
  count++;
  beerPulse();
  Serial.println(count);
}

void DisplayChange() {
  
}

//***************
// Timing
//***************
void beerStart() {
  startTime=millis();
}

void beerPulse() {
  endTime=millis();
}

void resetTiming() {
  startTime=0;
  endTime=0;
}

unsigned long getBeerCompletionDuration() {
  return endTime-startTime;
}

void setBeerCompletionDateTime() {
  //currentBeerCompletionInstant=now(); @TODO: Now isn't working/validating
}

boolean isNewDay() {
  return (lastBeerCompletionInstant < (currentBeerCompletionInstant-SECONDS_IN_DAY));
}

//***************
// Statistics & State Change
//***************
void recordBeerEnd() {
  lifetimeBeerCount++;
  lifetimeVolume+=count*multiplier;
  
  setBeerCompletionDateTime();
  
  if (isNewDay()) {
    resetTonightCounts();
  }
}

void resetTonightCounts() {
  totalBeersTonight=0;
  totalVolumeTonight=0;
  fastestBeerTonightTime=0;
}

void resetBeerSession() {
  count=0;
  currCount=0;
  prevCount=0;
  resetTiming();
  booDirtyPrint=true; // print it out
}


//***************
// Printing
//***************
boolean shouldPrintBeerTime() {
  return ((currCount>0) && (booDirtyPrint) && (currCount==prevCount));
}

void printStatusReport() {
  Serial.println(STR_BEER_TIMING + getBeerCompletionDuration() + STR_BEER_TIMING_UNIT);
  Serial.println(STR_LIFETIME_COUNT + lifetimeBeerCount);
  Serial.println(STR_LIFETIME_VOLUME + lifetimeVolume + STR_LIFETIME_VOLUME_UNIT);
}

//***************
// Storage
//***************
int getIntegerData(int address) {
  int value;
  EEPROM.get(address, value);
  return value;
}

float getFloatData(int address) {
  float value;
  EEPROM.get(address,value);
  return value;
}
 
void storeData(int address, int value) {
  EEPROM.put(address,value);
  Serial.print(value);
  Serial.print(" stored at address ");
  Serial.println(address);
}

void storeFloatData(int address, float value) {
  EEPROM.put(address,value);
  Serial.print(value);
  Serial.print(" stored at address ");
  Serial.println(address);
}

int getFastestBeer() {
  return getIntegerData(ADDR_FASTEST_BEER);
}

void storeFastestBeer() {
  storeData(ADDR_FASTEST_BEER,fastestBeerTime);
}

int getLifetimeBeerCount() {
  getIntegerData(ADDR_BEER_COUNT);
}

void storeLifetimeBeerCount() {
  storeData(ADDR_BEER_COUNT,lifetimeBeerCount);

}

float getLifetimeVolume() {
  return getFloatData(ADDR_LIFETIME_VOLUME);
}

void storeLifetimeVolume() {
  storeFloatData(ADDR_LIFETIME_VOLUME,lifetimeVolume);
}


void clearEEPROM() {
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    if(EEPROM.read(i) != 0) {                   //skip already "empty" addresses
    
      EEPROM.write(i, 0);                       //write 0 to address i
    }
  }
  Serial.println("EEPROM erased");
}