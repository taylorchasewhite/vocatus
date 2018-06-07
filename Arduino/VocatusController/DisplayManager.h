/**
 * DisplayManager.h - Library for sending data to the Arduino's LCD screen.
 * Intended to abstract the controller from the different displays that we might ultimately use.
 * Created by Taylor White, Stephen Lago, May 15, 2018
 * See https://www.arduino.cc/en/Hacking/LibraryTutorial for information on class conventions here.
 */
#ifndef DisplayManager_h
#define DisplayManager_h
#include "Arduino.h"

typedef enum OutputMode {
  DEBUG       = 0x0001,  //noisy debug info output to the console
  STATUSBOARD = 0x0002,  //pretty output to a Processing-based status board (via Serial Monitor)
  LCD         = 0x0004  //prototype LCD output (single line)
};

typedef enum CurrentValueToDisplay {
  LIFECOUNT = 0, //explicitly set this guy to 0 to support cycling
  TONIGHTCOUNT,
  LIFESPEED,
  TONIGHTSPEED,
  LASTSPEED,
  ENDVALUE //this value should always be last to support cycling; add new values before this guy
};

class DisplayManager
{
  public:
    DisplayManager();
    DisplayManager(OutputMode);
    
    void DebugPrint(String);
    void DebugPrintln(String);
    void DebugPrint(int);
    void DebugPrintln(int);
    void DebugPrint(long);
    void DebugPrintln(long);
    void DebugPrint(unsigned long);
    void DebugPrintln(unsigned long);
    void DebugPrint(float);
    void DebugPrintln(float);
    void DebugPrint(double);
    void DebugPrintln(double);
  private:
    OutputMode _outputMode;
    CurrentValueToDisplay _currentValueToDisplay;
    
    bool _isDebugEnabled;
    bool _isLcdEnabled;
    bool _isStatusBoardEnabled;

    bool _shouldDebug();
    void _changeOutputMode(OutputMode);
};

#endif
