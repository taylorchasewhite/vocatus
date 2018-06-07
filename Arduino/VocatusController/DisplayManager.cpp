/**
 * DisplayManager.cpp - Library for sending data to the Arduino's LCD screen.
 * Intended to abstract the controller from the different displays that we might ultimately use.
 * Created by Taylor White, Stephen Lago, May 15, 2018
 * See https://www.arduino.cc/en/Hacking/LibraryTutorial for information on class conventions here.
 */
#include "Arduino.h"
#include "DisplayManager.h"


DisplayManager::DisplayManager(OutputMode myOutputMode) {
  _changeOutputMode(myOutputMode);
}

DisplayManager::DisplayManager() {
  DisplayManager(DEBUG);
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
}

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
