/**
 * DisplayManager.cpp - Library for sending data to the Arduino's LCD screen.
 * Intended to abstract the controller from the different displays that we might ultimately use.
 * Created by Taylor White, Stephen Lago, May 15, 2018
 * See https://www.arduino.cc/en/Hacking/LibraryTutorial for information on class conventions here.
 */
#include "Arduino.h"
#include "DisplayManager.h"
#include <LiquidCrystal.h>
#include <TimeLib.h>

const String GREETINGS[] = {
  "Aye, drinking time, is it?",
  "Shiver me timbers! You again!",
  "Welcome aboard, mate!",
  "Avast! It's good to see ye!"
};

const int GREETINGS_LENGTH=4;

const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

/****************************************************************/
/***********************   Constructors   ***********************/
/****************************************************************/
DisplayManager::DisplayManager(OutputMode myOutputMode) {
  this->_changeOutputMode(myOutputMode);
  this->_currentValueToDisplay = LIFECOUNT;
}

DisplayManager::DisplayManager() {
  this->_changeOutputMode(DEBUG);
  this->_currentValueToDisplay = LIFECOUNT;
}


/****************************************************************/
/********************   General Functions   *********************/
/****************************************************************/
/**
 * centralized place for changing the output mode
 * updates the boolean state values to make sure they are always correct
 */
void DisplayManager::_changeOutputMode(OutputMode newOutputMode) {
  //set new modes
  _outputMode = newOutputMode;
  _isDebugEnabled = ((newOutputMode&DEBUG) == DEBUG);
  _isStatusBoardEnabled = ((newOutputMode&STATUSBOARD) == STATUSBOARD);
  _isLcdEnabled = ((newOutputMode&LCD) == LCD);

  //run any init code
  if(_isLcdEnabled){_initLcd();}
  if(_isDebugEnabled){_initDebug();}
}

/**
 * Methods used for making sure that display appropriately handles the "1" case
 * E.g. it says "1 drink" instead of "1 drinks"
 */
String DisplayManager::_handleSingleCase(float value,String stringIfSingle, String stringIfNotSingle) { 
  return (_handleSingleCase((int)value, stringIfSingle, stringIfNotSingle));
}
String DisplayManager::_handleSingleCase(int value,String stringIfSingle,String stringIfNotSingle) {
  if(value==1)
  {
    return(stringIfSingle);
  }
  return(stringIfNotSingle);
}

/**
 * Setup code to run when initializing the display manager
 */
void DisplayManager::_initDebug() {
  DebugPrintln(_welcomeText());
}

/**
 * Display a cute greeting to the user and/or welcome them traditionally.
 */
String DisplayManager::_welcomeText() {
  String returnString = _randomGreeting();
  returnString +=F("\nWelcome to Vocatus!\n");
  returnString+=F("----------------------------------");
  return returnString;
}

/**
 * In the pre-programmed greetings, return one of them.
 */
String DisplayManager::_randomGreeting() {
  return GREETINGS[random(GREETINGS_LENGTH-1)]; 
}

/**
 * Send info to the appropriate output
 */
void DisplayManager::OutputData(Record& lifetimeRecord, Record& tonightRecord,Drink& lastDrink)
{
  if(_isDebugEnabled) {
    _sendDebugReport(lifetimeRecord,tonightRecord,lastDrink);
  }
  
  if(_isStatusBoardEnabled) { 
    _sendToStatusBoard(lifetimeRecord,tonightRecord,lastDrink);
  }

  if(_isLcdEnabled) {
    _sendToLcd(lifetimeRecord,tonightRecord,lastDrink);
  }
}

/****************************************************************/
/*********************   Debug Management   *********************/
/****************************************************************/
/**
 * Only print to the serial monitor if debug mode is turned on and if not using a status board
 */
bool DisplayManager::_shouldDebug() {
  return(_isDebugEnabled && !_isStatusBoardEnabled);
}

/**
 * Methods used to print debug messages
 * First checks whether it is appropriate to send text to the serial monitor
 */
void DisplayManager::DebugPrint(String debugText) { if(_shouldDebug()){ Serial.print(debugText); }}
void DisplayManager::DebugPrintln(String debugText) { if(_shouldDebug()){ Serial.println(debugText); }}
void DisplayManager::DebugPrint(int debugText) { if(_shouldDebug()){ Serial.print(debugText); }}
void DisplayManager::DebugPrintln(int debugText) { if(_shouldDebug()){ Serial.println(debugText); }}
void DisplayManager::DebugPrint(long debugText) { if(_shouldDebug()){ Serial.print(debugText); }}
void DisplayManager::DebugPrintln(long debugText) { if(_shouldDebug()){ Serial.println(debugText); }}
void DisplayManager::DebugPrint(unsigned long debugText) { if(_shouldDebug()){ Serial.print(debugText); }}
void DisplayManager::DebugPrintln(unsigned long debugText) { if(_shouldDebug()){ Serial.println(debugText); }}
void DisplayManager::DebugPrint(float debugText) { if(_shouldDebug()){ Serial.print(debugText); }}
void DisplayManager::DebugPrintln(float debugText) { if(_shouldDebug()){ Serial.println(debugText); }}
void DisplayManager::DebugPrint(double debugText) { if(_shouldDebug()){ Serial.print(debugText); }}
void DisplayManager::DebugPrintln(double debugText) { if(_shouldDebug()){ Serial.println(debugText); }}

/**
 * Output a standard debug message to the console with the current state of variables
 */
void DisplayManager::_sendDebugReport(Record& lifetimeRecord, Record& tonightRecord,Drink& lastDrink) {
  //top separator
  DebugPrintln(_seperator());
  DebugPrintln("");

  //Lifetime Header
  DebugPrint("Lifetime - "); DebugPrintln(lifetimeRecord.startTimeString());
  DebugPrintln("----------");

  //Lifetime Info
  DebugPrint(lifetimeRecord.count()); DebugPrintln(" " + _handleSingleCase(lifetimeRecord.count(),"drink","drinks"));
  DebugPrint(lifetimeRecord.volume()); DebugPrintln(" total mL");
  DebugPrint("All-Time Record: "); DebugPrint(lifetimeRecord.fastestTime()); DebugPrintln(" ms");
  DebugPrintln(F(""));

  //Tonight Header
  DebugPrint("Tonight - "); DebugPrintln(tonightRecord.startTimeString());
  DebugPrintln("----------");

  //Tonight Info
  DebugPrint("Tonight: "); DebugPrint(tonightRecord.count()); DebugPrintln(" " + _handleSingleCase(tonightRecord.count(),"drink","drinks"));
  DebugPrint(tonightRecord.volume()); DebugPrintln(F(" total mL"));
  DebugPrint("Tonight's Record: "); DebugPrint(tonightRecord.fastestTime()); DebugPrintln(" ms");
  DebugPrintln(F(""));

  //Last drink header
  DebugPrint("Last Drink - "); DebugPrintln(lastDrink.endTimeString());
  DebugPrintln("----------");

  //Last drink info
  DebugPrint("Last Volume: "); DebugPrint(lastDrink.volume()); DebugPrintln(" mL");
  DebugPrint("Last Drink: "); DebugPrint(lastDrink.timeToFinish()); DebugPrintln(" ms");

  //bottom separator 
  DebugPrintln("");
  DebugPrintln(_seperator());
}

/**
* Method to output the debug info to the monitor directly (without using DebugPrint)
* Probably no longer needed, but useful in case there's another memory leak problem
* If we need to free up memory in the code, this whole method can be deleted
*/
void DisplayManager::_sendSerialDebugReport(Record& lifetimeRecord, Record& tonightRecord,Drink& lastDrink) {
  Serial.println("");
  Serial.println(F("-------------------------------------------------"));
  Serial.println("");
  
  Serial.print(F("Lifetime"));
  Serial.print(F(" - "));
  Serial.println(lifetimeRecord.startTimeString());
  Serial.println(F("----------"));
  
  Serial.print(lifetimeRecord.count());
  Serial.print(F(" "));
  Serial.println(_handleSingleCase(lifetimeRecord.count(),"drink","drinks"));

  Serial.print(lifetimeRecord.volume());
  Serial.print(F(" "));
  Serial.print(F("total"));
  Serial.print(F(" "));
  Serial.println(F("mL"));

  Serial.print(lifetimeRecord.fastestTime());
  Serial.print(F(" "));
  Serial.println(F("ms"));

  Serial.println(F(""));

  Serial.print(F("Tonight"));
  Serial.print(F(" - "));
  Serial.println(tonightRecord.startTimeString());
  Serial.println(F("----------"));
  
  Serial.print(tonightRecord.count());
  Serial.print(F(" "));
  Serial.println(_handleSingleCase(tonightRecord.count(),"drink","drinks"));

  Serial.print(tonightRecord.volume());
  Serial.print(F(" "));
  Serial.print(F("total"));
  Serial.print(F(" "));
  Serial.println(F("mL"));
  
  Serial.print(tonightRecord.fastestTime());
  Serial.print(F(" "));
  Serial.println(F("ms"));
  Serial.println(F(""));

  Serial.print(F("Last Drink - "));
  Serial.println(lastDrink.endTimeString());
  Serial.println(F("----------"));

  Serial.print(lastDrink.volume());
  Serial.print(F(" "));
  Serial.println(F("mL"));

  Serial.print(lastDrink.timeToFinish());
  Serial.print(F(" "));
  Serial.println(F("ms"));
  
  Serial.println();
}

String DisplayManager::_seperator() {
  return F("-------------------------------------------------");
}

/****************************************************************/
/*****************   Status Board Management   ******************/
/****************************************************************/

/**
 * Serial String to send to processing as output (e.g. to statusboard)
 * 
 * @return a semicolon delimited string containing the information to send as output
 */
String DisplayManager::_buildComString(Record& lifetimeRecordVar, Record& tonightRecordVar,Drink& lastDrink)
{
  String toSend = "";
  String delim = ";";
  
  toSend += lifetimeRecordVar.count();
  toSend += delim;
  toSend += tonightRecordVar.count();
  toSend += delim;
  toSend += lifetimeRecordVar.fastestTime();
  toSend += delim;
  toSend += tonightRecordVar.fastestTime();
  toSend += delim;
  toSend += lastDrink.timeToFinish();
  toSend += delim;
  toSend += lifetimeRecordVar.volume();
  toSend += delim;
  toSend += tonightRecordVar.volume();
  toSend += delim;
  toSend += lastDrink.volume();
  toSend += delim;  
  
  return (toSend);
}


void DisplayManager::_sendToStatusBoard(Record& lifetimeRecordVar, Record& tonightRecordVar, Drink& lastDrink) {
  String comString = _buildComString(lifetimeRecordVar,tonightRecordVar,lastDrink); 
  Serial.println(comString);  
}

/****************************************************************/
/********************     LCD Management    *********************/
/****************************************************************/
/**
 * Initialize the lcd components
 */
void DisplayManager::_initLcd() {
  lcd.begin(16,2);
  lcd.print("Running...");

  _currentValueToDisplay = LIFECOUNT;
}

/**
 * Cycle through to the next display mode according to the order in the enum
 */
void DisplayManager::CycleCurrentValueToDisplay() {
  int intValue = (int) _currentValueToDisplay;
  intValue++; //increment by one
  if((intValue >= ENDVALUE) || (intValue < 0)) {
    intValue = 0; //close the cycle
  }
  _currentValueToDisplay = (CurrentValueToDisplay) intValue;
}

/**
 * Send the relevant info for display on the LCD, based on the current lcdDisplayMode
 */
void DisplayManager::_sendToLcd(Record& lifetimeRecord, Record& tonightRecord, Drink& lastDrink)
{
  String toDisplayLabel = "";
  String toDisplayValue = "";
  String toDisplayUnit = "";

  //use the current mode to determine what to show on the LCD
  switch(_currentValueToDisplay) {
    case LIFECOUNT:
      toDisplayLabel = "Lifetime";
      toDisplayValue = lifetimeRecord.count();
      toDisplayUnit  = _handleSingleCase(lifetimeRecord.count(),"drink","drinks"); 
      break;
    case TONIGHTCOUNT:
      toDisplayLabel = "Tonight";
      toDisplayValue = tonightRecord.count();
      toDisplayUnit  = _handleSingleCase(tonightRecord.count(),"drink","drinks");
      break;
    case LIFESPEED:
      toDisplayLabel = "All-Time Record";
      toDisplayValue = lifetimeRecord.fastestTime();
      toDisplayUnit  = "ms";
      break;
    case TONIGHTSPEED:
      toDisplayLabel = "Tonight's Record";
      toDisplayValue = tonightRecord.fastestTime();
      toDisplayUnit  = "ms"  ;
      break;
    case LASTSPEED:
      toDisplayLabel = "Last Drink";
      toDisplayValue = lastDrink.timeToFinish();
      toDisplayUnit  = "ms";
      break;
    default:  //might as well have a saftey case //TODO:: do we want to remove this in the final product for less noisy errors?
      toDisplayLabel = "ERROR:";
      toDisplayValue = "BAD ENUM VALUE " + _currentValueToDisplay; 
      break;
  }

  //erase previous screen
  lcd.clear();

  //print the first line
  lcd.setCursor(0,0); 
  lcd.print(toDisplayLabel + ":");

  //print the second line
  lcd.setCursor(0,1); 
  lcd.print(toDisplayValue + " " + toDisplayUnit);
}
