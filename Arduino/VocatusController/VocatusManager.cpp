/*
 * VocatusManager.cpp - The object to manage the state of the program
 * Intended to abstract the controller from as much as possible
 * 
 * Created by Taylor White, Stephen Lago, July 17, 2018
 * See https://www.arduino.cc/en/Hacking/LibraryTutorial for information on class conventions here.
*/
#include "VocatusManager.h"

/****************************************************************/
/*******************      Constructors       ********************/
/****************************************************************/

/**
 * Default constructor, used to allocate space when defining members. 
 * Sets all values to null, 0, or other non-meaningful data.
 */
VocatusManager::VocatusManager() {
}

/****************************************************************/
/********************      Initialize       *********************/
/****************************************************************/

/**
 * Set starting values for globals
 */
void VocatusManager::initGlobals() {
  outputFreeMemory();
  
  multiplier = 3.05; //TCW: 2.647  Chode: 3.05

  flowCount = 0;

  display = *new DisplayManager(DEBUG); //set it to whatever mode(s) you want: DEBUG|STATUSBOARD|LCD
  storage = *new StorageManager();
  
  //timeManager = *new TimeManager(10); // TODO Allow different initialization modes like DisplayManager
  
  //initialize all tracking variables to 0 in case they are not read from storage
  readFromStorage();

  //start the button states at their expected starting values
  resetLastState = HIGH;
  displayLastState = HIGH;
  resetCurState = HIGH;
  displayCurState = HIGH;
  resetLastDebounce = 0;
  displayLastDebounce = 0;
  
}


/****************************************************************/
/**********        Button Presses & Interrupts         **********/
/****************************************************************/
/**
 * Called when liquid flows through the flow meter.
 */
void VocatusManager::Flow() {
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
void VocatusManager::displayChangeListener() {
  display.CycleCurrentValueToDisplay();
  printStatusReport();
}

/**
 * Called when the display cycle button is pressed.
 */
bool VocatusManager::checkDebounce(int newReading, int& buttonState, int lastState, unsigned long& lastDebounce) {
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
unsigned long VocatusManager::getCurrentTime() {
  return(millis());
}

/**
 * Called the first time the hall effect sensor has been triggered since the last drink session completed.
 */
void VocatusManager::startDrink() {
  startTime=getCurrentTime();
}

/**
 * Called every time the hall effect sensor fires. Represents another ~2.65 ml have been drank.
 */
void VocatusManager::drinkPulse() {
  endTime=getCurrentTime();
}

/**
 * Use the timing globals to record the duration for the latest drink
 */
void VocatusManager::setDrinkCompletionDuration() {
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
boolean VocatusManager::isNewDay() {
  return (lastDrinkCompletionInstant < (currentDrinkCompletionInstant-SECONDS_IN_DAY));
}


/****************************************************************/
/***************** Statistics and State Change ******************/
/****************************************************************/
/**
 * Record that a drink has been completed, update any current records, storage and display to the LCD
 */
void VocatusManager::recordDrinkEnd() {
  
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

void VocatusManager::resetListener() {
    reset();
    printStatusReport();
}

/**
 * Completely reset all tracked values, including tonight and lifetime
 */
void VocatusManager::reset() {
  resetCurrentDrink();
  resetTonightRecord();
  resetLifetimeRecord();

  storeAllValues();
}

/**
 * Resets all of the variables tied to the most recent drink
 */
void VocatusManager::resetCurrentDrink() {
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
void VocatusManager::resetTonightRecord() {
  tonight.Reset();
}

/**
 * Call if a total reset is needed. 
 * This function will reset all of the relevant variables scoring drink statistics for the device lifetime.
 */
void VocatusManager::resetLifetimeRecord() {
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
boolean VocatusManager::isDrinkOver() {
  return ((flowCount>0) && (endTime + MS_BETWEEN_DRINKS < millis()));
}

/**
 * Print out status of the device given the last drink that was completed.
 * 
 * @param storage boolean indicating where to read the data from
 */
void VocatusManager::printStatusReport() {
  display.OutputData(lifetime,tonight,lifetime.lastDrink()); //mostRecentDrinkTime,mostRecentVolume);
}

/****************************************************************/
/********************        Storage        *********************/
/****************************************************************/
/**
 * Get all of the values we want at startup held in permanent storage from EEPROM data.
 * 
 * @return the integer value stored in the desired address.
 */
void VocatusManager::readFromStorage() {
  lifetime = storage.lifetimeRecord();
  Serial.println("read from storage");
  Serial.println("");
  Serial.print("Lifetime");
  Serial.println(SECTION_SEPARATOR);
  
  Serial.print(lifetime.count());
  Serial.print(F(" (loaded)"));
  Serial.print(storage.lifetimeCount());
  Serial.print(" (storage)");
  Serial.println(" drinks");

  Serial.print(lifetime.volume());
  Serial.print(F(" "));
  Serial.print("total");
  Serial.print(F(" "));
  Serial.println("mL");

  Serial.print(lifetime.fastestTime());
  Serial.print(F(" "));
  Serial.println("seconds");

  tonight = storage.tonightRecord();
}

/**
 * Store all values we want to persist post power cut to EEPROM. 
 * This should happen ANYTIME data that needs to persist is created and/or updated.
 */
void VocatusManager::storeAllValues() {
  Serial.println("storing");
  storage.storeAllValues(lifetime,tonight);
  readFromStorage();
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
