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
#include <Beer.h>
#include <DisplayManager.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <StorageManager.h>

/****************************************************************/
/********************        Globals        *********************/
/****************************************************************/
// Init & Constants
int bitsPerSecond;
int changeDisplayPin;
int flowPin;
int resetButtonInPin;
int resetButtonOutPin;
int modeCycleButtonInPin;

// Counts
int currCount;
int prevCount;

int mostRecentBeerTime;  // The time it took in ms to finish the last beer

float mostRecentVolume; //@NOTE:: why have this if we have a beer for it?
float multiplier;

double flowRate;
volatile int count;

// Timing
//@NOTE:: we aren't using these anywhere
Beer lifetimeBestBeer;
Beer tonightBestBeer; 
Beer lastBeer;

Record lifetime;
Record tonight;

unsigned long endTime;
unsigned long startTime;
int lastBeerDay;
int lastBeerHour;
int currentBeerDay;
int currentBeerHour;

const unsigned long SECONDS_IN_DAY = 86400;
unsigned long lastBeerCompletionInstant; //@NOTE:: why are all of these globals?
unsigned long currentBeerCompletionInstant;

// States
bool firstDropOfBeer; //@NOTE:: this does nothing; remove all references
bool startingUp;

//Flags
bool statusBoardEnabled; 
bool lcdModeEnabled;
bool debugModeOn; 

//LCD values
const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
enum DisplayMode {
  LIFECOUNT = 0, //explicitly set this guy to 0 to support cycling
  TONIGHTCOUNT,
  LIFESPEED,
  TONIGHTSPEED,
  LASTSPEED,
  ENDVALUE //this value should always be last to support cycling; add new values before this guy
} lcdDisplayMode;

// Display strings
DisplayManager display;

//@Note:: Do these need to be constants? we should either make our debug/LCD strings consistent or get rid of these
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
  pinMode(resetButtonOutPin, OUTPUT);
  pinMode(modeCycleButtonInPin, INPUT_PULLUP);

  if(lcdModeEnabled) {
    lcd.begin(16,2);
    lcd.print("Running...");
  }
  
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
  
  if (shouldPrintBeerTime()) {
    recordBeerEnd();
    printStatusReport(false);
    resetBeerSession();
  } 
  else if (prevCount!=currCount) {
    //debugPrintln(STR_PREV_COUNT + prevCount);
    //debugPrintln(STR_CURR_COUNT + currCount);
  }

  if (modeCycleButtonVal == LOW) {
    cycleMode();
    printStatusReport(true);
  } 
  
  if (resetButtonVal == LOW) {
    totalReset();
    printStatusReport(true);
  } 
  
}

/**
 * The method to run each time we recieve input from the device
 */
void Flow() {
  if (count==0) {
      beerStart();
      firstDropOfBeer=true;
  }
  count++;
  beerPulse();
  if(shouldPrint()){ debugPrintln(count); }
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
  resetButtonOutPin = 13;
  modeCycleButtonInPin = 4;
  bitsPerSecond = 9600;   // initialize serial communication at 9600 bits per second:
  multiplier = 3.05; //TCW: 2.647  Chode: 3.05

  currCount = 0;
  prevCount = -1;
  
  currCount=0;
  prevCount=-1;
  
  firstDropOfBeer=false;
  startingUp = false;
  lcdDisplayMode = LIFECOUNT;

  //initialize all tracking variables to 0 in case they are not read from storage

  tonight = new Record();
  lifetime = new Record();
  
  mostRecentBeerTime = 0;
  mostRecentVolume = 0.0;

  //set flags for initial desired state
  statusBoardEnabled = true; //set to true to have output sent via serial message to a statusboard (e.g. processing)
  debugModeOn = false; //set to true to enable noisy output (e.g. messages sent to Serial Monitor)
  lcdModeEnabled = false; //set to true to enable output to an LCD

  readFromStorage();
}

void initializeDisplay() {
  if (!startingUp) {
    debugPrintln("Welcome to the Red Solo Cup Saver!");
    debugPrintln("----------------------------------");
    printStatusReport(true);
    startingUp=true;
  }
}

/****************************************************************/
/********************        Timing         *********************/
/****************************************************************/
/**
 * Called the first time the hall effect sensor has been triggered since the last beer session completed.
 */
void beerStart() {
  startTime=millis();
}

/**
 * Called every time the hall effect sensor fires. Represents another ~2.65 ml have been drank.
 */
void beerPulse() {
  endTime=millis();
}

/**
 * Reset the start and end time for a beer that will be drank and judged.
 */
void resetTiming() {
  startTime=0;
  endTime=0;
}

/**
 * Get the time it took to complete the most recent beer.
 * 
 * @return How long it took to complete the last beer.
 */
int getBeerCompletionDuration() {
  return mostRecentBeerTime;
}

void setBeerCompletionDuration(int startTime, int endTime) {
  mostRecentBeerTime = endTime-startTime;
  //@NOTE:: we should be using globals unless there's a reason to read from memory (globals can exist in the storage class, that's fine; but it looks like the plan is to have them read from memory every time)
  //@NOTE:: this method does not exist
  if ((mostRecentBeerTime < storage.getLifetimeFastestBeerTime()) || (storage.getLifetimeFastestBeerTime()<=0)) {
    lifetimeFastestBeerTime = mostRecentBeerTime;
    storage.setLifetimeFastestTime();
  }
}

/**
 * Denote the completion of a new beer at the current instant.
 */
void setBeerCompletionDateTime() {
  //currentBeerCompletionInstant=now(); @TODO: Now isn't working/validating
}

/**
 * Determines whether or not the completion of the last beer represents a new day.
 * 
 * @return Is the first beer of new day
 */
boolean isNewDay() {
  return (lastBeerCompletionInstant < (currentBeerCompletionInstant-SECONDS_IN_DAY));
}

/****************************************************************/
/***************** Statistics and State Change ******************/
/****************************************************************/
/**
 * Record that a beer has been completed, update any current records, storage and display to the LCD
 */
void recordBeerEnd() {
  mostRecentVolume=count*multiplier;

  lifetime.addBeer(startTime,endTime,mostRecentVolume);
  tonight.addBeer(startTime,endTime,mostRecentVolume);
  
  storeAllValues();
  
  setBeerCompletionDuration(startTime,endTime);
  
  setBeerCompletionDateTime(); // @NOTE:: This function does nothing

  //@NOTE:: we'll want to tear this out once we properly define a session
  if (isNewDay()) {
    resetTonightCounts();
  }
}

/**
 * Call if more than 12 hours have passed since the last time a beer was drank. 
 * This function will reset all of the relevant variables scoring beer statistics for a given night.
 */
void resetTonightCounts() {
  tonight = new Record();
}

/**
 * Resets all of the variables tied to a beer session.
 */
void resetBeerSession() {
  count=0;
  currCount=0;
  prevCount=0;
  resetTiming();
  firstDropOfBeer=true; // print it out
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
  
  mostRecentBeerTime = 0;
  mostRecentVolume = 0.0;

  storeAllValues();
}

/****************************************************************/
/********************        Printing       *********************/
/****************************************************************/

/**
 * Determines whether or not to print out the beer completion data. 
 */
boolean shouldPrintBeerTime() {
  return ((currCount>0) && (firstDropOfBeer) && (currCount==prevCount));
}

/**
 * Print out status of the device given the last beer that was completed.
 * 
 * @param storage boolean indicating where to read the data from
 */
void printStatusReport(bool readFromStorage) {
  if (readFromStorage) {
    debugPrintln(STR_LIFETIME_COUNT + storage.lifetimeCount());
    debugPrintln(STR_LIFETIME_VOLUME + storage.lifetimeVolume() + STR_LIFETIME_VOLUME_UNIT);
    debugPrintln(STR_FASTEST_TIME + storage.lifetimeFastestTime() + STR_BEER_TIMING_UNIT);
  }
  else {
    debugPrintln(STR_BEER_TIMING + getBeerCompletionDuration() + STR_BEER_TIMING_UNIT);
    debugPrintln(STR_LIFETIME_COUNT + lifetimeTotalBeerCount);
    debugPrintln(STR_LIFETIME_VOLUME + lifetimeTotalVolume + STR_LIFETIME_VOLUME_UNIT);
  }
  
  sendToStatusBoard();
  sendToLcd();
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
  lifetime = new Record();
  lifetime.count(storage.lifetimeCount());
  lifetime.volume(storage.lifetimeVolume());
  lifetime.time(storage.lifetimeFastestTime());
}

/**
 * Store all values we want to persist post power cut to EEPROM. 
 * This should happen ANYTIME data that needs to persist is created and/or updated.
 */
void storeAllValues() {
  storage.lifetimeCount(lifetimeTotalBeerCount);
  storage.lifetimeFastestTime(lifetimeFastestBeerTime);
  storage.lifetimeVolume(lifetimeTotalVolume);
}

/****************************************************************/
/********************   Serial Management   *********************/
/****************************************************************/
/**
 * Only print to the serial monitor if debug mode is turned on and if not using a status board
 */
boolean shouldPrint() {
  return(debugModeOn && !statusBoardEnabled); 
}

/**
 * Methods used to print debug messages
 * First checks whether it is appropriate to send text to the serial monitor
 */
void debugPrint(String debugText) { if(shouldPrint()){ Serial.print(debugText); }}
void debugPrintln(String debugText) { if(shouldPrint()){ Serial.println(debugText); }}
void debugPrint(int debugText) { if(shouldPrint()){ Serial.print(debugText); }}
void debugPrintln(int debugText) { if(shouldPrint()){ Serial.println(debugText); }}
void debugPrint(long debugText) { if(shouldPrint()){ Serial.print(debugText); }}
void debugPrintln(long debugText) { if(shouldPrint()){ Serial.println(debugText); }}
void debugPrint(unsigned long debugText) { if(shouldPrint()){ Serial.print(debugText); }}
void debugPrintln(unsigned long debugText) { if(shouldPrint()){ Serial.println(debugText); }}
void debugPrint(float debugText) { if(shouldPrint()){ Serial.print(debugText); }}
void debugPrintln(float debugText) { if(shouldPrint()){ Serial.println(debugText); }}
void debugPrint(double debugText) { if(shouldPrint()){ Serial.print(debugText); }}
void debugPrintln(double debugText) { if(shouldPrint()){ Serial.println(debugText); }}

/**
 * Serial String to send to processing as output (e.g. to statusboard)
 * 
 * @return a semicolon delimited string containing the information to send as output
 */
String buildComString(Record lifetimeRecord, Record tonightRecord,int mostRecentBeerTimeVar, float mostRecentVolumeVar)
{
  String toSend = "";
  String delim = ";";
  
  toSend += lifetimeRecord.count();
  toSend += delim;
  toSend += tonightRecord.count();
  toSend += delim;
  toSend += lifetimeRecord.fastestTime();
  toSend += delim;
  toSend += tonightRecord.fastestTime();
  toSend += delim;
  toSend += mostRecentBeerTimeVar;
  toSend += delim;
  toSend += lifetimeRecord.volume();
  toSend += delim;
  toSend += lifetimeRecord.volume();
  toSend += delim;
  toSend += mostRecentVolumeVar;
  toSend += delim;  
  
  return (toSend);
}

/**
 * Send info to the status board
 */
void sendToStatusBoard()
{
  if(statusBoardEnabled) { 
    String comString = buildComString(lifetime,tonight,mostRecentBeerTime,mostRecentVolume); 
    Serial.println(comString);  
  }
}

/****************************************************************/
/********************     LCD Management    *********************/
/****************************************************************/
/**
 * Only print to the lcd display if lcd mode is turned on
 */
bool shouldSendToLcd() {
  return(lcdModeEnabled);
}


/**
 * Cycle through to the next display mode according to the order in the enum
 */
void cycleMode() {
  int intValue = (int) lcdDisplayMode;
  intValue++; //increment by one
  if(intValue == ENDVALUE) {
    intValue = 0; //close the cycle
  }
  lcdDisplayMode = (DisplayMode) intValue;
}


/**
 * Send the relevant info for display on the LCD, based on the current lcdDisplayMode
 */
void sendToLcd()
{
  if(!shouldSendToLcd()) { return; } //short circuit this if we shouldn't be doing anything

  String toDisplayLabel = "";
  String toDisplayValue = "";
  String toDisplayUnit = "";

  //use the current mode to determine what to show on the LCD
  switch(lcdDisplayMode) {
    case LIFECOUNT:
      toDisplayLabel = "Lifetime:";
      toDisplayValue = lifetime.count();
      toDisplayUnit = "beers"; //TODO:: handle 1 drink
    case TONIGHTCOUNT:
      toDisplayLabel = "Tonight:";
      toDisplayValue = tonight.count();
      toDisplayUnit = "beers"; //TODO:: handle 1 drink
    case LIFESPEED:
      toDisplayLabel = "All-Time Record:";
      toDisplayValue = lifetime.fastestTime();
      toDisplayUnit = "ms";
    case TONIGHTSPEED:
      toDisplayLabel = "Tonight's Record";
      toDisplayValue = tonight.fastestTime();
      toDisplayUnit = "ms";
    case LASTSPEED:
      toDisplayLabel = "Last Drink";
      toDisplayValue = mostRecentBeerTime;
      toDisplayUnit = "ms";
    default:  //might as well have a saftey case //TODO:: do we want to remove this in the final product for less noisy errors?
      toDisplayLabel = "ERROR:";
      toDisplayValue = "BAD ENUM VALUE"; 
  }

  //print the first line
  lcd.setCursor(0,0); 
  lcd.print(toDisplayLabel);

  //print the second line
  lcd.setCursor(0,1); 
  lcd.print(toDisplayValue + " " + toDisplayUnit);
}

