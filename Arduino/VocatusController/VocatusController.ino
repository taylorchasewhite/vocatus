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
//#include "TimeManager.h"
#include <TimeLib.h>
#include <TimeAlarms.h>

/****************************************************************/
/********************        Globals        *********************/
/****************************************************************/
// Arduino constants 
const int BITS_PER_SECOND = 9600; // initialize serial communication at 9600 bits per second:

const int PIN_FLOW = 2;
const int PIN_RESET = 3;
const int PIN_CYCLE_DISPLAY = 4;

const int SECONDS_BETWEEN_DISPLAY_CHANGE = 2; // Seconds to wait between cycling displays
const int NUMBER_OF_DISPLAY_MODES_TO_CYCLE = 5; // The number of displays to cycle through when completing a drink

bool currentlyInDisplayCycle;
int currentCycleStep;
bool userCancelledCycleMode;

unsigned int MS_BETWEEN_DRINKS = 2000; //2 seconds after the flow stops is the end of the drink
const float MIN_ML_FOR_DRINK = 44.0; //minimum drink size is 44mL (one shot)
const float MULTIPLIER = 3.05; ///TCW: 2.647 Chode: 3.05

// Counts
volatile int flowCount; //@NOTE:: I don't think this needs to be volatile. It's only needed if the variable could be changed from outside of the code (e.g another class)
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

const unsigned long DEBOUNCE_BUFFER = 50; //time to wait for a "bounce", e.g. machine error reading a single press as multiple
unsigned long resetLastDebounce;
unsigned long displayLastDebounce;

//states
int resetLastState;
int resetCurState;
int displayLastState;
int displayCurState;

// I/O Controls
DisplayManager display;
StorageManager storage;
//TimeManager timeManager;


/****************************************************************/
/********************      Initialize       *********************/
/****************************************************************/
/**
 * Set starting values for globals
 */
void initGlobals() {
  flowCount = 0;

  display = *new DisplayManager(DEBUG|LCD); //set it to whatever mode(s) you want: DEBUG|STATUSBOARD|LCD
  storage = *new StorageManager();
  //timeManager = *new TimeManager(10); // TODO Allow different initialization modes like DisplayManager

  //initialize all tracking variables to 0 in case they are not read from storage
  tonight = *new Record();
  lifetime = *new Record();

  //start the button states at their expected starting values
  resetLastState = HIGH;
  displayLastState = HIGH;
  resetCurState = HIGH;
  displayCurState = HIGH;
  resetLastDebounce = 0;
  displayLastDebounce = 0;
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

  printStatusReport();
}

/****************************************************************/
/********************         Core          *********************/
/****************************************************************/
void loop() {
  //handle button presses
  int resetReading = digitalRead(PIN_RESET);
  int displayReading = digitalRead(PIN_CYCLE_DISPLAY);
  Alarm.delay(0);

  if(checkDebounce(resetReading,resetCurState,resetLastState,resetLastDebounce)) {
    resetListener();
  }
  if(checkDebounce(displayReading,displayCurState,displayLastState,displayLastDebounce)) {
    displayChangeListener();
  }

  resetLastState = resetReading;
  displayLastState = displayReading;

  //timeManager.manageTime();
  
  if(isDrinkOver()) {
    if((flowCount*MULTIPLIER)>MIN_ML_FOR_DRINK) {
      recordDrinkEnd();
    }
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
void displayChangeListener() {
  userCancelledCycleMode=true;
  changeDisplay();
}

/**
 * Change the display of the LCD
 */
void changeDisplay() {
  display.CycleCurrentValueToDisplay();
  printStatusReport();
}

/**
 * Automatically change the LCD display mode. 
 * Assumptions:
 *   The first time this is called, we will set a global variable to indicate we are in auto change mode.
 *   If the user hits the change button during this time, we will stop auto changing until the next drink
 *     though we still will change as a result of the button press.
 *   What that means is that upon completing a drink, you *have* to see different information. I think this is okay
 *     but if not, userCancelledCycleMode will need to become a 3-value enum/integer to allow for "cancelling" of a change
 *     during the autochangemode since right now we can't differentiate prior to this autoChangeDisplay call if the user expressly
 *     pressed the button as we just set it to false during the first call.
 */
void autoChangeDisplay() {
  if(!currentlyInDisplayCycle) {
    currentlyInDisplayCycle=true;
    currentCycleStep=0;
    userCancelledCycleMode=false; 

  } 
  else {
    if((currentCycleStep+1)==NUMBER_OF_DISPLAY_MODES_TO_CYCLE) {
      // We're done here, we want to turn off the currentlyInDisplayCycle for the next time
      currentlyInDisplayCycle=false;
    } 
  }
  
  if (!userCancelledCycleMode) {
      changeDisplay();
  }
  currentCycleStep++; // we just finished a step, increment;
}

/**
 * Called when the display cycle button is pressed.
 */

bool checkDebounce(int newReading, int& buttonState, int lastState, unsigned long& lastDebounce) {
  bool returnValue = false;
  
  if(newReading != lastState) {
    lastDebounce = millis(); //reset debouncing timer
  }
    
  unsigned long dif = millis() - lastDebounce;

  //wait until the buffer period has passed
  if(dif > DEBOUNCE_BUFFER) {
    //if the state has changed
    if(newReading != buttonState) {
      buttonState = newReading;

      //only return true if the button is depressed (and not lifted up)
      if(buttonState == LOW) {
        returnValue = true;
      }
    }
  }

  return(returnValue);
}

/****************************************************************/
/********************        Timing         *********************/
/****************************************************************/
/**
 * Method for getting the current time, intended to allow easy swapping of time management as needed
 */
unsigned long getCurrentTime() {
  return(millis());
}

/**
 * Called the first time the hall effect sensor has been triggered since the last drink session completed.
 */
void startDrink() {
  startTime=getCurrentTime();
}

/**
 * Called every time the hall effect sensor fires. Represents another ~2.65 ml have been drank.
 */
void drinkPulse() {
  endTime=getCurrentTime();
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
  
  mostRecentVolume=flowCount*MULTIPLIER;

  lifetime.addDrink(startTime,endTime,mostRecentVolume);
  tonight.addDrink(startTime,endTime,mostRecentVolume);
  
  setDrinkCompletionDuration();
  for (int i =0; i<=NUMBER_OF_DISPLAY_MODES_TO_CYCLE; i++) {
    Alarm.timerOnce((i+1)*SECONDS_BETWEEN_DISPLAY_CHANGE, autoChangeDisplay);
  }
  
  //@NOTE:: we'll want to tear this out once we properly define a session
  /*if (isNewDay()) {
    resetTonightRecord();
  }*/

  storeAllValues();
}

void resetListener() {
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
  return ((flowCount>0) && (endTime + MS_BETWEEN_DRINKS < millis()));
}

/**
 * Print out status of the device given the last drink that was completed.
 * 
 * @param storage boolean indicating where to read the data from
 */
void printStatusReport() {
  //display.DebugPrint(F("Memory Before: "));
  //outputFreeMemory();
  display.OutputData(lifetime,tonight,lifetime.lastDrink()); //mostRecentDrinkTime,mostRecentVolume);
  //display.DebugPrint(F("Memory After: "));
  //outputFreeMemory();
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

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__
 
int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}

/**
 * outputs a string telling the total free memory at the time the method was called (distance between heap and stack)
 * Used to track memory leaks
 */
void outputFreeMemory() {
  Serial.print(F("Memory: "));
  Serial.println(freeMemory());
}
