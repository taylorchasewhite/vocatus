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
#include <LiquidCrystal.h>

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

int lifetimeTotalBeerCount;
int tonightTotalBeerCount;

int lifetimeFastestBeerTime;
int tonightFastestBeerTime;
int mostRecentBeerTime;  // The time it took in ms to finish the last beer

float lifetimeTotalVolume;
float tonightTotalVolume;
float mostRecentVolume;
float multiplier;

double flowRate;
volatile int count;

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
  ENDVALUE //this calue should always be last to support cycling; add new values before this guy
} lcdDisplayMode;

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

  //if the mode cycle button is pressed
  if (modeCycleButtonVal == LOW) {
    cycleMode();
    printStatusReport(true);
  } 

  //if the reset button is pressed
  if (resetButtonVal == LOW) {
    totalReset();
    printStatusReport(true);
  } 
}

/*
 * The method to run each time we recieve input from the device
 */
void Flow() {
  if (count==0) {
      beerStart();
  }
  count++;
  beerPulse();
  debugPrintln(count);
}

/****************************************************************/
/********************      Initialize       *********************/
/****************************************************************/
/*
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
  
  startingUp = false;
  lcdDisplayMode = LIFECOUNT;

  //initialize all tracking variables to 0 in case they are not read from storage
  lifetimeTotalBeerCount = 0;
  tonightTotalBeerCount = 0;

  lifetimeFastestBeerTime = 0;
  tonightFastestBeerTime = 0;
  mostRecentBeerTime = 0;

  lifetimeTotalVolume = 0.0;
  tonightTotalVolume = 0.0;
  mostRecentVolume = 0.0;

  //set flags for initial desired state
  statusBoardEnabled = true; //set to true to have output sent via serial message to a statusboard (e.g. processing)
  debugModeOn = false; //set to true to enable noisy output (e.g. messages sent to Serial Monitor)
  lcdModeEnabled = true; //set to true to enable output to an LCD

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
/*
  Called the first time the hall effect sensor has been triggered since the last beer session completed.
*/

void beerStart() {
  startTime=millis();
}

/*
  Called every time the hall effect sensor fires. Represents another ~2.65 ml have been drank.
*/
void beerPulse() {
  endTime=millis();
}

/*
  Reset the start and end time for a beer that will be drank and judged.
*/
void resetTiming() {
  startTime=0;
  endTime=0;
}

/*
  Get the time it took to complete the most recent beer.
 
  @return How long it took to complete the last beer.
*/
int getBeerCompletionDuration() {
  return mostRecentBeerTime;
}

void setBeerCompletionDuration(int startTime, int endTime) {
  mostRecentBeerTime = endTime-startTime;
  //TODO:: we should be using globals unless there's a reason to read from memory
  if ((mostRecentBeerTime < readLifetimeFastestBeerTime()) || (readLifetimeFastestBeerTime()<=0)) {
    lifetimeFastestBeerTime = mostRecentBeerTime;
    storeLifetimeFastestBeerTime();
  }
}

/*
  Denote the completion of a new beer at the current instant.
*/
void setBeerCompletionDateTime() {
  //currentBeerCompletionInstant=now(); @TODO: Now isn't working/validating
}

/*
  Determines whether or not the completion of the last beer represents a new day.
  
  @return Is the first beer of new day
*/
boolean isNewDay() {
  return (lastBeerCompletionInstant < (currentBeerCompletionInstant-SECONDS_IN_DAY));
}

/****************************************************************/
/***************** Statistics and State Change ******************/
/****************************************************************/
void recordBeerEnd() {
  lifetimeTotalBeerCount++;
  tonightTotalBeerCount++;

  mostRecentVolume=count*multiplier;
  tonightTotalVolume+=mostRecentVolume;
  lifetimeTotalVolume+=mostRecentVolume;
  
  storeAllValues();
  
  setBeerCompletionDuration(startTime,endTime);
  
  setBeerCompletionDateTime(); // @TODO: This function does nothing
  
  if (isNewDay()) {
    resetTonightCounts();
  }
}

/*
  Call if more than 12 hours have passed since the last time a beer was drank. 
  This function will reset all of the relevant variables scoring beer statistics for a given night.
*/
void resetTonightCounts() {
  tonightTotalBeerCount=0;
  tonightTotalVolume=0;
  tonightFastestBeerTime=0;
}

/*
  Resets all of the variables tied to a beer session.
*/
void resetBeerSession() {
  count=0;
  currCount=0;
  prevCount=0;
  resetTiming();
}

/*
 * Completely reset all tracked values, including tonight and lifetime
 */
void totalReset() {
  resetBeerSession();

  lifetimeTotalBeerCount = 0;
  tonightTotalBeerCount = 0;

  lifetimeFastestBeerTime = 0;
  tonightFastestBeerTime = 0;
  mostRecentBeerTime = 0;

  lifetimeTotalVolume = 0.0;
  tonightTotalVolume = 0.0;
  mostRecentVolume = 0.0;

  storeAllValues();
}

/****************************************************************/
/********************        Printing       *********************/
/****************************************************************/

/*
  Determines whether or not to print out the beer completion data. 
*/
boolean shouldPrintBeerTime() {
  return ((currCount>0) && (currCount==prevCount));
}

/*
  Print out status of the device given the last beer that was completed.
  
  @param storage boolean indicating where to read the data from
*/
void printStatusReport(bool readFromStorage) {
  if (readFromStorage) {
    debugPrintln(STR_LIFETIME_COUNT + readLifetimeTotalBeerCount());
    debugPrintln(STR_LIFETIME_VOLUME + readLifetimeTotalVolume() + STR_LIFETIME_VOLUME_UNIT);
    debugPrintln(STR_FASTEST_TIME + readLifetimeFastestBeerTime() + STR_BEER_TIMING_UNIT);
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
/*
  Get integer value held in permanent storage from EEPROM data.
  
  @param address integer value denoting where to read from
  @return the integer value stored in the desired address.
*/
int readIntegerData(int address) {
  int value;
  EEPROM.get(address, value);
  return value;
}

/*
  Get float value held in permanent storage from EEPROM data.
  
  @param address integer value denoting where to read from
  @return the float value stored in the desired address.
*/
float readFloatData(int address) {
  float value;
  EEPROM.get(address,value);
  /*Serial.print("Read ");
  Serial.print(value);
  Serial.print(" from ");
  Serial.println(address);*/
  return value;
}

/*
  Store integer value into permanent storage at EEPROM.
  NOTE: There are a limited number of times this can be called, 
    try to store to addresses as few times as possible.
  
  @param address integer value denoting where to store the data to
  @param value integer value you desire to store into EEPROM.
*/
void storeData(int address, int value) {
  float floatVal  = value;
  EEPROM.put(address,floatVal);
  debugPrint(floatVal);
  debugPrint(" stored at address ");
  debugPrintln(address);
}

/*
  Store float value into permanent storage at EEPROM.
  NOTE: There are a limited number of times this can be called, 
    try to store to addresses as few times as possible.
  
  @param address integer value denoting where to store the data to
  @param value float value you desire to store into EEPROM.
*/
void storeData(int address, float value) {
  EEPROM.put(address,value);
  debugPrint(value);
  debugPrint(" stored at address ");
  debugPrintln(address);
}

//Getters and setters
float readLifetimeTotalBeerCount() { 
  return readFloatData(ADDR_BEER_COUNT); 
}
void storeLifetimeTotalBeerCount() { 
  storeData(ADDR_BEER_COUNT,lifetimeTotalBeerCount); 
}

int readLifetimeFastestBeerTime() { 
  return readFloatData(ADDR_FASTEST_BEER); 
}
void storeLifetimeFastestBeerTime() { 
  storeData(ADDR_FASTEST_BEER,lifetimeFastestBeerTime); 
}

float readLifetimeTotalVolume() { 
  return readFloatData(ADDR_LIFETIME_VOLUME); 
}

void storeLifetimeTotalVolume() { 
  storeData(ADDR_LIFETIME_VOLUME,lifetimeTotalVolume); 
}


void readFromStorage() {
  lifetimeTotalBeerCount = readLifetimeTotalBeerCount();
  lifetimeFastestBeerTime = readLifetimeFastestBeerTime();
  lifetimeTotalVolume=readLifetimeTotalVolume();
}

void storeAllValues() {
  storeLifetimeTotalBeerCount();
  storeLifetimeFastestBeerTime();
  storeLifetimeTotalVolume();
}

/*
  Only call this when resetting the device to factory settings!
  Permanently erases all persistent data stored on the arduino or board.
*/
void clearEEPROM() {
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    if(EEPROM.read(i) != 0) {                   //skip already "empty" addresses
    
      EEPROM.write(i, 0);                       //write 0 to address i
    }
  }
  debugPrintln("EEPROM erased");
}

/****************************************************************/
/********************   Serial Management   *********************/
/****************************************************************/
/*
 * Only print to the serial monitor if debug mode is turned on and if not using a status board
 */
boolean shouldPrint() {
  return(debugModeOn && !statusBoardEnabled); 
}

/*
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

/*
 * Serial String to send to processing as output (e.g. to statusboard)
 * 
 * @return a semicolon delimited string containing the information to send as output
 */
String buildComString(int lifetimeTotalBeerCountVar,int tonightTotalBeerCountVar,int lifetimeFastestBeerTimeVar,int tonightFastestBeerTimeVar,int mostRecentBeerTimeVar, float lifetimeTotalVolumeVar, float tonightTotalVolumVar, float mostRecentVolumeVar)
{
  String toSend = "";
  String delim = ";";
  
  toSend += lifetimeTotalBeerCountVar;
  toSend += delim;
  toSend += tonightTotalBeerCountVar;
  toSend += delim;
  toSend += lifetimeFastestBeerTimeVar;
  toSend += delim;
  toSend += tonightFastestBeerTimeVar;
  toSend += delim;
  toSend += mostRecentBeerTimeVar;
  toSend += delim;
  toSend += lifetimeTotalVolumeVar;
  toSend += delim;
  toSend += tonightTotalVolumVar;
  toSend += delim;
  toSend += mostRecentVolumeVar;
  toSend += delim;  
  
  return (toSend);
}

/*
 * Send info to the status board
 */
void sendToStatusBoard()
{
  if(statusBoardEnabled) { 
    String comString = buildComString(lifetimeTotalBeerCount,tonightTotalBeerCount,lifetimeFastestBeerTime,tonightFastestBeerTime,mostRecentBeerTime,lifetimeTotalVolume,tonightTotalVolume,mostRecentVolume); 
    //TODO modify this new string builder based on TCW's variables
    Serial.println(comString);  
  }
}

/****************************************************************/
/********************     LCD Management    *********************/
/****************************************************************/
/*
 * Only print to the lcd display if lcd mode is turned on
 */
bool shouldSendToLcd() {
  return(lcdModeEnabled);
}


/*
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


/*
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
      toDisplayValue = lifetimeTotalBeerCount;
      toDisplayUnit = "beers"; //TODO:: handle 1 drink
    case TONIGHTCOUNT:
      toDisplayLabel = "Tonight:";
      toDisplayValue = tonightTotalBeerCount;
      toDisplayUnit = "beers"; //TODO:: handle 1 drink
    case LIFESPEED:
      toDisplayLabel = "All-Time Record:";
      toDisplayValue = lifetimeFastestBeerTime;
      toDisplayUnit = "ms";
    case TONIGHTSPEED:
      toDisplayLabel = "Tonight's Record";
      toDisplayValue = tonightFastestBeerTime;
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

