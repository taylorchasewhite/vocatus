/*
  @title:   Vocatus: Flow Meter - Making the world a better place
  @author:  Stephen Lago & Taylor White 
  @date:    May 2, 2018
  @purpose: Store and calculate flow meter data for a beer bong.
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
int fastestTime;
double flowRate;
volatile int count;

// Display strings
String strBeerTiming = "Time: ";
String strBeerTimingUnit = " ms!";
String strPrevCount = "Prev: ";
String strCurrCount = "Curr: ";
String strLifetimeCount = "Total beers: ";

// Timing
unsigned long endTime;
unsigned long startTime;

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

unsigned long getBeerTime() {
  return endTime-startTime;
}

//***************
// Statistics & State Change
//***************
void recordBeerEnd() {
  lifetimeBeerCount++;
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
  Serial.println(strBeerTiming + getBeerTime() + strBeerTimingUnit);
  Serial.println(strLifetimeCount + lifetimeBeerCount);
}
