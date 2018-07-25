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

#include "VocatusManager.h"
#include <TimeLib.h>

VocatusManager manager;

/**
 * Standard Arduino setup code, run before starting the loop when code is first initialized
 */
void setup() {
  Serial.begin(BITS_PER_SECOND);
  manager.initGlobals();
  manager.readFromStorage();

  
  pinMode(PIN_FLOW, INPUT);           
  pinMode(PIN_RESET, INPUT_PULLUP);
  pinMode(PIN_CYCLE_DISPLAY, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(PIN_FLOW), Flow, RISING);

  manager.printStatusReport();
}

/****************************************************************/
/********************         Core          *********************/
/****************************************************************/
void loop() {
  //handle button presses
  int resetReading = digitalRead(PIN_RESET);
  int displayReading = digitalRead(PIN_CYCLE_DISPLAY);

  if(manager.checkDebounce(resetReading,manager.resetCurState,manager.resetLastState,manager.resetLastDebounce)) {
    manager.resetListener();
  }
  if(manager.checkDebounce(displayReading,manager.displayCurState,manager.displayLastState,manager.displayLastDebounce)) {
    manager.displayChangeListener();
  }

  manager.resetLastState = resetReading;
  manager.displayLastState = displayReading;

  //timeManager.manageTime();
  
  if(manager.isDrinkOver()) {
    manager.recordDrinkEnd();
    manager.printStatusReport();
    manager.resetCurrentDrink();
  }
}

/**
 * Called when liquid flows through the flow meter.
 */
void Flow() {
  manager.Flow();
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

