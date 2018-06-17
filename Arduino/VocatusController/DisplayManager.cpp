/**
 * DisplayManager.cpp - Library for sending data to the Arduino's LCD screen.
 * Intended to abstract the controller from the different displays that we might ultimately use.
 * Created by Taylor White, Stephen Lago, May 15, 2018
 * See https://www.arduino.cc/en/Hacking/LibraryTutorial for information on class conventions here.
 */
#include "Arduino.h"
#include "DisplayManager.h"
#include <LiquidCrystal.h>
#include <Time.h>

const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

DisplayManager::DisplayManager(OutputMode myOutputMode) {
  this->_changeOutputMode(myOutputMode);
  this->_currentValueToDisplay = LIFECOUNT;
}

DisplayManager::DisplayManager() {
  this->_changeOutputMode(DEBUG);
  this->_currentValueToDisplay = LIFECOUNT;
}

/**
 * centralized place for changing the output mode
 * updates the boolean state values to make sure they are always correct
 */
void DisplayManager::_changeOutputMode(OutputMode newOutputMode) {
  _outputMode = newOutputMode;
  _isDebugEnabled = ((newOutputMode&DEBUG) == DEBUG);
  _isStatusBoardEnabled = ((newOutputMode&STATUSBOARD) == STATUSBOARD);
  _isLcdEnabled = ((newOutputMode&LCD) == LCD);
  if(_isLcdEnabled){_initLcd();}
}

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
 * Send info to the appropriate output
 */
void DisplayManager::OutputData(Record lifetimeRecord, Record tonightRecord,int mostRecentDrinkTimeVar, float mostRecentVolumeVar)
{
  if(_isDebugEnabled) {
    _sendDebugReport(lifetimeRecord,tonightRecord,mostRecentDrinkTimeVar,mostRecentVolumeVar);
  }
  
  if(_isStatusBoardEnabled) { 
    _sendToStatusBoard(lifetimeRecord,tonightRecord,mostRecentDrinkTimeVar,mostRecentVolumeVar);
  }

  if(_isLcdEnabled) {
    _sendToLcd(lifetimeRecord,tonightRecord,mostRecentDrinkTimeVar);
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
void DisplayManager::_sendDebugReport(Record lifetimeRecord, Record tonightRecord,int mostRecentDrinkTimeVar, float mostRecentVolumeVar) {
  DebugPrintln(LIFECOUNT_LABEL + ": " + lifetimeRecord.count() + " " + _handleSingleCase(lifetimeRecord.count(),LIFECOUNT_UNIT_SINGLE,LIFECOUNT_UNIT));
  DebugPrintln(TONIGHTCOUNT_LABEL + ": " + tonightRecord.count() + " " + _handleSingleCase(tonightRecord.count(),TONIGHTCOUNT_UNIT_SINGLE,TONIGHTCOUNT_UNIT));
  DebugPrintln(LIFESPEED_LABEL + ": " + lifetimeRecord.fastestTime() + " " + LIFESPEED_UNIT);
  DebugPrintln(TONIGHTSPEED_LABEL + ": " + tonightRecord.fastestTime() + " " + TONIGHTSPEED_UNIT);
  DebugPrintln(LASTSPEED_LABEL + ": " + mostRecentDrinkTimeVar + " " + LASTSPEED_UNIT);
  DebugPrintln(LIFEVOLUME_LABEL + ": " + lifetimeRecord.volume() + " " + LIFEVOLUME_UNIT);
  DebugPrintln(TONIGHTVOLUME_LABEL + ": " + tonightRecord.volume() + " " + TONIGHTVOLUME_UNIT);
  DebugPrintln(LASTVOLUME_LABEL + ": " + mostRecentVolumeVar + " " + LASTVOLUME_UNIT);
}

/****************************************************************/
/*****************   Status Board Management   ******************/
/****************************************************************/

/**
 * Serial String to send to processing as output (e.g. to statusboard)
 * 
 * @return a semicolon delimited string containing the information to send as output
 */
String DisplayManager::_buildComString(Record lifetimeRecordVar, Record tonightRecordVar,int mostRecentDrinkTimeVar, float mostRecentVolumeVar)
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
  toSend += mostRecentDrinkTimeVar;
  toSend += delim;
  toSend += lifetimeRecordVar.volume();
  toSend += delim;
  toSend += tonightRecordVar.volume();
  toSend += delim;
  toSend += mostRecentVolumeVar;
  toSend += delim;  
  
  return (toSend);
}

void DisplayManager::_sendToStatusBoard(Record lifetimeRecordVar, Record tonightRecordVar,int mostRecentDrinkTimeVar, float mostRecentVolumeVar) {
  String comString = _buildComString(lifetimeRecordVar,tonightRecordVar,mostRecentDrinkTimeVar,mostRecentVolumeVar); 
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
void DisplayManager::_sendToLcd(Record lifetimeRecord, Record tonightRecord,int mostRecentDrinkTime)
{
  String toDisplayLabel = "";
  String toDisplayValue = "";
  String toDisplayUnit = "";

  //use the current mode to determine what to show on the LCD
  switch(_currentValueToDisplay) {
    case LIFECOUNT:
      toDisplayLabel = LIFECOUNT_LABEL;
      toDisplayValue = lifetimeRecord.count();
      toDisplayUnit  = _handleSingleCase(lifetimeRecord.count(),LIFECOUNT_UNIT_SINGLE,LIFECOUNT_UNIT); 
      break;
    case TONIGHTCOUNT:
      toDisplayLabel = TONIGHTCOUNT_LABEL;
      toDisplayValue = tonightRecord.count();
      toDisplayUnit  = _handleSingleCase(tonightRecord.count(),TONIGHTCOUNT_UNIT_SINGLE,TONIGHTCOUNT_UNIT);
      break;
    case LIFESPEED:
      toDisplayLabel = LIFESPEED_LABEL;
      toDisplayValue = lifetimeRecord.fastestTime();
      toDisplayUnit  = LIFESPEED_UNIT;
      break;
    case TONIGHTSPEED:
      toDisplayLabel = TONIGHTSPEED_LABEL;
      toDisplayValue = tonightRecord.fastestTime();
      toDisplayUnit  = TONIGHTSPEED_UNIT  ;
      break;
    case LASTSPEED:
      toDisplayLabel = LASTSPEED_LABEL;
      toDisplayValue = mostRecentDrinkTime;
      toDisplayUnit  = LASTSPEED_UNIT;
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

