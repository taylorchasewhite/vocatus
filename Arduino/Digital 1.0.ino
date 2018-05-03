/*
  @title:   Vocatus: Flow Meter - Making the world a better place
  @author:  Stephen Lago & Taylor White 
  @date:    May 2, 2018
  @purpose: Store and calculate flow meter data for a beer bong.
*/

/*
  Useful resources:
    http://playground.arduino.cc/Code/Time
    https://playground.arduino.cc/Code/Stopwatch
    https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
    https://www.arduino.cc/en/Reference/StyleGuide
    https://www.bc-robotics.com/tutorials/using-a-flow-sensor-with-arduino/
*/


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
float lifetimeVolume;
int fastestTime;
float multiplier;
double flowRate;
volatile int count;

// Display strings
String strBeerTiming = "Time: ";
String strBeerTimingUnit = " ms!";
String strPrevCount = "Prev: ";
String strCurrCount = "Curr: ";
String strFastestTime = "Fastest time: ";
String strLifetimeCount = "Total beers: ";
String strLifetimeVolume = "Total volume: ";
String strLifetimeVolumeUnit = " ml";
String strTonight = " tonight";

// Timing
unsigned long endTime;
unsigned long startTime;
int lastBeerDay;
int lastBeerHour;
int currentBeerDay;
int currentBeerHour;
unsigned long secondsInADay = 86400;
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
    //Serial.println(strPrevCount + prevCount);
    //Serial.println(strCurrCount + currCount);
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
  currentBeerCompletionInstant=now();
}

boolean isNewDay() {
  return (lastBeerCompletionInstant < (currentBeerCompletionInstant-secondsInADay))
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
  fastestBeerTonight=0;
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
  Serial.println(strBeerTiming + getBeerCompletionDuration() + strBeerTimingUnit);
  Serial.println(strLifetimeCount + lifetimeBeerCount);
  Serial.println(strLifetimeVolume + lifetimeVolume + strLifetimeVolumeUnit);
}
