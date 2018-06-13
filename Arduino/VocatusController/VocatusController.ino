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

#include "Record.h"
#include "Drink.h"
#include "DisplayManager.h"
#include <EEPROM.h>
#include "StorageManager.h"

/****************************************************************/
/********************        Globals        *********************/
/****************************************************************/
// Init & Constants
int bitsPerSecond;
int changeDisplayPin;
int flowPin;
int resetButtonInPin;
int modeCycleButtonInPin;

// Counts
int currCount;
int prevCount;

int mostRecentDrinkTime;  // The time it took in ms to finish the last drink

float mostRecentVolume;
float multiplier;

double flowRate;
volatile int count;

// Timing
//@NOTE:: we aren't using these anywhere
Drink lifetimeBestDrink;
Drink tonightBestDrink; 
Drink lastDrink;

Record lifetime;
Record tonight;

unsigned long endTime;
unsigned long startTime;
int lastDrinkDay;
int lastDrinkHour;
int currentDrinkDay;
int currentDrinkHour;

const unsigned long SECONDS_IN_DAY = 86400;
unsigned long lastDrinkCompletionInstant; //@NOTE:: why are all of these globals?
unsigned long currentDrinkCompletionInstant;

// States
bool firstDropOfDrink; //@NOTE:: this does nothing; remove all references
bool startingUp;

// Display strings
DisplayManager display;

//@Note:: Do these need to be constants? we should either make our debug/LCD strings consistent or get rid of these
const String STR_BEER_TIMING          = "Time: ";
const String STR_BEER_TIMING_UNIT     = "ms";
const String STR_PREV_COUNT           = "Prev: ";
const String STR_CURR_COUNT           = "Curr: ";
const String STR_FASTEST_TIME         = "Fastest time: ";
const String STR_LIFETIME_COUNT       = "Total drinks: ";
const String STR_LIFETIME_VOLUME      = "Total volume: ";
const String STR_LIFETIME_VOLUME_UNIT = " ml";
const String STR_TONIGHT              = " tonight";

// Addresses
StorageManager storage;

const int ADDR_BEER_COUNT             = 0;
const int ADDR_LIFETIME_VOLUME        = 1*sizeof(float);
const int ADDR_FASTEST_BEER           = 2*sizeof(float);

const int ADDR_TONIGHT_FASTEST_BEER   = 10*sizeof(float);
const int ADDR_TONIGHT_BEER_COUNT     = 11*sizeof(float);
const int ADDR_TONIGHT_VOLUME         = 12*sizeof(float);



/****************************************************************/
/********************         Core          *********************/
/****************************************************************/
void setup() {
  initGlobals();
  Serial.begin(bitsPerSecond);
  initializeDisplay(); // @TODO: Would like to get this to run here...not in loop.

  //Setup pins
  pinMode(flowPin, INPUT);           
  pinMode(resetButtonInPin, INPUT_PULLUP);
  pinMode(modeCycleButtonInPin, INPUT_PULLUP);
  
  attachInterrupt(0, Flow, RISING);  //Configures interrupt 0 (pin 2 on the Arduino Uno) to run the function "Flow" 

}

void loop() {
  //read the pushbutton value into a variable
  int resetButtonVal = digitalRead(resetButtonInPin);
  int modeCycleButtonVal = digitalRead(modeCycleButtonInPin);
  
  prevCount=currCount;
  currCount=count;
  
  interrupts();   //Enables interrupts on the Arduino
  delay (1000);   //Wait 1 second 
  noInterrupts(); //Disable the interrupts on the Arduino
  
  if (shouldPrintDrinkTime()) {
    recordDrinkEnd();
    printStatusReport(false);
    resetDrinkSession();
  } 
  else if (prevCount!=currCount) {
    //display.DebugPrintln(STR_PREV_COUNT + prevCount);
    //display.DebugPrintln(STR_CURR_COUNT + currCount);
  }

  //if the mode cycle button is pressed
  if (modeCycleButtonVal == LOW) {
    display.CycleCurrentValueToDisplay();
    printStatusReport(true);
  } 

  //if the reset button is pressed
  if (resetButtonVal == LOW) {
    debugPrintln("Reset button pushed");
    totalReset();
    printStatusReport(true);
  } 
}

/**
 * The method to run each time we recieve input from the device
 */
void Flow() {
  if (count==0) {
      drinkStart();
      firstDropOfDrink=true;
  }
  count++;
  drinkPulse();
  if(shouldPrint()){ display.DebugPrintln(count); }
}

/****************************************************************/
/********************      Initialize       *********************/
/****************************************************************/
/**
 * Set starting values for globals
 */
void initGlobals() {
  flowPin = 2;
  resetButtonInPin = 3;
  modeCycleButtonInPin = 5;
  bitsPerSecond = 9600;   // initialize serial communication at 9600 bits per second:
  multiplier = 3.05; //TCW: 2.647  Chode: 3.05

  currCount = 0;
  prevCount = -1;
  
  currCount=0;
  prevCount=-1;
  
  firstDropOfDrink=false;
  startingUp = false;
  
  //initialize all tracking variables to 0 in case they are not read from storage
  tonight = *new Record();
  lifetime = *new Record();
  
  lastDrink = *new Drink();

  display = new DisplayManager(DEBUG); //set it to whatever mode(s) you want: DEBUG|STATUSBOARD|LCD

  readFromStorage();
}

void initializeDisplay() {
  if (!startingUp) {
    display.DebugPrintln("Welcome to the Red Solo Cup Saver!");
    display.DebugPrintln("----------------------------------");
    printStatusReport(true);
    startingUp=true;
  }
}

/****************************************************************/
/********************        Timing         *********************/
/****************************************************************/
/**
 * Called the first time the hall effect sensor has been triggered since the last drink session completed.
 */
void drinkStart() {
  startTime=millis();
}

/**
 * Called every time the hall effect sensor fires. Represents another ~2.65 ml have been drank.
 */
void drinkPulse() {
  endTime=millis();
}

/**
 * Reset the start and end time for a drink that will be drank and judged.
 */
void resetTiming() {
  startTime=0;
  endTime=0;
}

/**
 * Get the time it took to complete the most recent drink.
 * 
 * @return How long it took to complete the last drink.
 */
int getDrinkCompletionDuration() {
  return mostRecentDrinkTime;
}

void setDrinkCompletionDuration(int startTime, int endTime) {
  mostRecentDrinkTime = endTime-startTime;
  //@NOTE:: we should be using globals unless there's a reason to read from memory (globals can exist in the storage class, that's fine; but it looks like the plan is to have them read from memory every time)
  //@NOTE:: this method does not exist
  if ((mostRecentDrinkTime < storage.lifetimeFastestDrinkTime()) || (storage.lifetimeFastestDrinkTime()<=0)) {
    lifetimeFastestDrinkTime = mostRecentDrinkTime;
    storage.lifetimeFastestTime(mostRecentDrinkTime);
  }
}

/**
 * Denote the completion of a new drink at the current instant.
 */
void setDrinkCompletionDateTime() {
  //currentDrinkCompletionInstant=now(); @TODO: Now isn't working/validating
}

/**
 * Determines whether or not the completion of the last drink represents a new day.
 * 
 * @return Is the first drink of new day
 */
boolean isNewDay() {
  return (lastDrinkCompletionInstant < (currentDrinkCompletionInstant-SECONDS_IN_DAY));
}

/****************************************************************/
/***************** Statistics and State Change ******************/
/****************************************************************/
/**
 * Record that a drink has been completed, update any current records, storage and display to the LCD
 */
void recordDrinkEnd() {
  mostRecentVolume=count*multiplier;

  lifetime.addDrink(startTime,endTime,mostRecentVolume);
  tonight.addDrink(startTime,endTime,mostRecentVolume);
  
  storeAllValues();
  
  setDrinkCompletionDuration(startTime,endTime);
  
  setDrinkCompletionDateTime(); // @NOTE:: This function does nothing

  //@NOTE:: we'll want to tear this out once we properly define a session
  if (isNewDay()) {
    resetTonightCounts();
  }
}

/**
 * Call if more than 12 hours have passed since the last time a drink was drank. 
 * This function will reset all of the relevant variables scoring drink statistics for a given night.
 */
void resetTonightCounts() {
  tonight = new Record();
}

/**
 * Resets all of the variables tied to a drink session.
 */
void resetDrinkSession() {
  count=0;
  currCount=0;
  prevCount=0;
  resetTiming();
  firstDropOfDrink=true; // print it out
}

/**
 * Completely reset all tracked values, including tonight and lifetime
 */
void totalReset() {
  count=0;
  currCount=0;
  prevCount=0;
  resetTiming();
  
  lifetime = new Record();
  tonight = new Record();
  
  mostRecentDrinkTime = 0;
  mostRecentVolume = 0.0;

  storeAllValues();
}

/****************************************************************/
/********************        Printing       *********************/
/****************************************************************/

/**
 * Determines whether or not to print out the drink completion data. 
 */
boolean shouldPrintDrinkTime() {
  return ((currCount>0) && (firstDropOfDrink) && (currCount==prevCount));
}

/**
 * Print out status of the device given the last drink that was completed.
 * 
 * @param storage boolean indicating where to read the data from
 */
void printStatusReport(bool readFromStorage) {
  if (readFromStorage) {
    display.DebugPrintln(STR_LIFETIME_COUNT + storage.lifetimeCount());
    display.DebugPrintln(STR_LIFETIME_VOLUME + storage.lifetimeVolume() + STR_LIFETIME_VOLUME_UNIT);
    display.DebugPrintln(STR_FASTEST_TIME + storage.lifetimeFastestTime() + STR_BEER_TIMING_UNIT);
  }
  else {
    display.DebugPrintln(STR_BEER_TIMING + getDrinkCompletionDuration() + STR_BEER_TIMING_UNIT);
    display.DebugPrintln(STR_LIFETIME_COUNT + lifetimeTotalDrinkCount);
    display.DebugPrintln(STR_LIFETIME_VOLUME + lifetimeTotalVolume + STR_LIFETIME_VOLUME_UNIT);
  }
  
  display.OutputData(lifetime,tonight,mostRecentDrinkTime,mostRecentVolume);
}

/****************************************************************/
/********************        Storage        *********************/
/****************************************************************/
/**
 * Get all of the values we want at startup held in permanent storage from EEPROM data.
 * 
 * @return the integer value stored in the desired address.
 */
void readFromStorage() {
  lifetime = storage.lifetimeRecord();
}

/**
 * Store all values we want to persist post power cut to EEPROM. 
 * This should happen ANYTIME data that needs to persist is created and/or updated.
 */
void storeAllValues() {
  storage.lifetimeCount(lifetime.count());
  storage.lifetimeFastestTime(lifetime.fastestTime();
  storage.lifetimeVolume(lifetime.volume());
}