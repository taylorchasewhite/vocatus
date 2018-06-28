/*--------------------------
  @title:   Vocatus: Flow Meter - Making the world a better place
  @author:  Stephen Lago & Taylor White  
  @date:    May 2, 2018
  @purpose: Store and calculate flow meter data for a drink bong.
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
      --I'm using the Arduino application, not the web IDE, which turns tabs into spaces -SEL
    
  --------------------------
*/

#include "Record.h"
#include "DisplayManager.h"
#include "StorageManager.h"

/****************************************************************/
/********************        Globals        *********************/
/****************************************************************/
// Arduino constants 
const int BITS_PER_SECOND = 9600; // initialize serial communication at 9600 bits per second:

const int PIN_FLOW = 2;
const int PIN_RESET = 3;
const int PIN_CYCLE_DISPLAY = 5;

// Counts
volatile int flowCount; //@NOTE:: I don't think this needs to be volatile. It's only needed if the variable could be changed from outside of the code (e.g another class)
int prevFlowCount;

float multiplier;
float mostRecentVolume;

// Timing
Record lifetime;
Record tonight;

unsigned long endTime;
unsigned long startTime;
int mostRecentDrinkTime;  // The time it took in ms to finish the last drink

const unsigned long SECONDS_IN_DAY = 86400;
unsigned long lastDrinkCompletionInstant; //@NOTE:: why are all of these globals?
unsigned long currentDrinkCompletionInstant;

// I/O Controls
DisplayManager display;
StorageManager storage;


/****************************************************************/
/********************      Initialize       *********************/
/****************************************************************/
/**
 * Set starting values for globals
 */
void initGlobals() {

  multiplier = 3.05; //TCW: 2.647  Chode: 3.05

  flowCount = 0;
  prevFlowCount = -1;
  
  //initialize all tracking variables to 0 in case they are not read from storage
  tonight = *new Record();
  lifetime = *new Record();

  display = *new DisplayManager(DEBUG|LCD); //set it to whatever mode(s) you want: DEBUG|STATUSBOARD|LCD
  storage = *new StorageManager();
}

/**
 * Standard Arduino setup code, run before starting the loop when code is first initialized
 */
void setup() {
  Serial.begin(BITS_PER_SECOND);
  
  initGlobals();
  readFromStorage();

  
  pinMode(PIN_FLOW, INPUT);           
  pinMode(PIN_RESET, INPUT_PULLUP);
  pinMode(PIN_CYCLE_DISPLAY, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(PIN_FLOW), Flow, RISING);
  attachInterrupt(digitalPinToInterrupt(PIN_RESET), resetListener, RISING);
  attachInterrupt(digitalPinToInterrupt(PIN_CYCLE_DISPLAY), displayButtonListener, RISING);

  printStatusReport();
}

/****************************************************************/
/********************         Core          *********************/
/****************************************************************/
void loop() {
  prevFlowCount=flowCount;
  
  interrupts();   //Enables interrupts on the Arduino
  delay (1000);   //Wait 1 second 
  noInterrupts(); //Disable the interrupts on the Arduino

  if(isDrinkOver()) {
    recordDrinkEnd();
    printStatusReport();
    resetCurrentDrink();
  }
}

/**
 * Called when liquid flows through the flow meter.
 */
void Flow() {
  if (flowCount==0) {
    startDrink();
  }
  flowCount++;
  drinkPulse();
  display.DebugPrintln(flowCount);
}

/**
 * Called when the display cycle button is pressed.
 */
void displayButtonListener() {
  Serial.println("====Executed LCDListener====");
    display.DebugPrintln("  ==LCD Cycle Button pressed==");
    display.CycleCurrentValueToDisplay();
    printStatusReport();
}

/****************************************************************/
/********************        Timing         *********************/
/****************************************************************/
/**
 * Called the first time the hall effect sensor has been triggered since the last drink session completed.
 */
void startDrink() {
  startTime=millis();
}

/**
 * Called every time the hall effect sensor fires. Represents another ~2.65 ml have been drank.
 */
void drinkPulse() {
  endTime=millis();
}

/**
 * Use the timing globals to record the duration for the latest drink
 */
void setDrinkCompletionDuration() {
  mostRecentDrinkTime = endTime-startTime;
  //@NOTE:: we should be using globals unless there's a reason to read from memory (globals can exist in the storage class, that's fine; but it looks like the plan is to have them read from memory every time)
  if ((mostRecentDrinkTime < storage.lifetimeFastestTime()) || (storage.lifetimeFastestTime()<=0)) {
    storage.lifetimeFastestTime(mostRecentDrinkTime);
  }
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
  mostRecentVolume=flowCount*multiplier;

  lifetime.addDrink(startTime,endTime,mostRecentVolume);
  tonight.addDrink(startTime,endTime,mostRecentVolume);
  
  setDrinkCompletionDuration();
  
  //@NOTE:: we'll want to tear this out once we properly define a session
  /*if (isNewDay()) {
    resetTonightRecord();
  }*/

  storeAllValues();
}

void resetListener() {
    display.DebugPrintln("==Reset button pushed==");
    reset();
    printStatusReport();
}

/**
 * Completely reset all tracked values, including tonight and lifetime
 */
void reset() {
  resetCurrentDrink();
  resetTonightRecord();
  resetLifetimeRecord();

  storeAllValues();
}

/**
 * Resets all of the variables tied to the most recent drink
 */
void resetCurrentDrink() {
  //reset volume variables
  flowCount=0;
  prevFlowCount=-1;
  mostRecentVolume = 0.0;

  //reset timing variables
  startTime=0;
  endTime=0;
  mostRecentDrinkTime = 0;
}

/**
 * Call if more than 12 hours have passed since the last time a drink was drank. 
 * This function will reset all of the relevant variables scoring drink statistics for a given night.
 */
void resetTonightRecord() {
  tonight.Reset();
}

/**
 * Call if a total reset is needed. 
 * This function will reset all of the relevant variables scoring drink statistics for the device lifetime.
 */
void resetLifetimeRecord() {
  lifetime.Reset();
}


/****************************************************************/
/********************        Printing       *********************/
/****************************************************************/

/**
 * Determines whether the "current" drink has completed or not
 * First check makes sure it only happens after some liquid has passed through
 * Second check makes sure that we don't cut off a drink mid-stream
 */
boolean isDrinkOver() {
  return ((flowCount>0) && (flowCount==prevFlowCount));
}

/**
 * Print out status of the device given the last drink that was completed.
 * 
 * @param storage boolean indicating where to read the data from
 */
void printStatusReport() {
//  Serial.println("Lifetime");
//  Serial.println("---------");
//  Serial.print(lifetime.count());
//  Serial.println(" drinks");
//
//  Serial.print(lifetime.volume());
//  Serial.println(" ml");
//
//  Serial.println("Tonight");
//  Serial.println("---------");
//  Serial.print(tonight.count());
//  Serial.println(" drinks");
//
//  Serial.print(tonight.volume());
//  Serial.println(" ml");
//
//  Serial.print("Most Recent Drink Time: ");
//  Serial.print(mostRecentDrinkTime);
//  Serial.print(" ms");
  
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
  tonight = storage.tonightRecord();
}

/**
 * Store all values we want to persist post power cut to EEPROM. 
 * This should happen ANYTIME data that needs to persist is created and/or updated.
 */
void storeAllValues() {
  storage.storeAllValues(lifetime,tonight);
}
