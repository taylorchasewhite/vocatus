/**
 * DisplayManager.h - Library for sending data to the Arduino's LCD screen.
 * Intended to abstract the controller from the different displays that we might ultimately use.
 * Created by Taylor White, Stephen Lago, May 15, 2018
 * See https://www.arduino.cc/en/Hacking/LibraryTutorial for information on class conventions here.
 */
#ifndef DisplayManager_h
#define DisplayManager_h
#include "Arduino.h"
#include "Record.h"

enum OutputMode {
  DEBUG       = 0x0001,  //noisy debug info output to the console
  STATUSBOARD = 0x0002,  //pretty output to a Processing-based status board (via Serial Monitor)
  LCD         = 0x0004  //prototype LCD output (single line)
};

enum CurrentValueToDisplay {
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

    void OutputData(Record, Record, int, float);
    void CycleCurrentValueToDisplay();
  private:
    OutputMode _outputMode;
    CurrentValueToDisplay _currentValueToDisplay;
    
    bool _isDebugEnabled;
    bool _isLcdEnabled;
    bool _isStatusBoardEnabled;

    bool _shouldDebug();
    void _changeOutputMode(OutputMode);
    String _buildComString(Record, Record, int, float);
    void _sendToStatusBoard(Record, Record, int, float);
    void _initLcd();
    void _sendToLcd(Record, Record, int);

    //@Note:: Do these need to be constants? we should either make our debug/LCD strings consistent or get rid of these
    const String STR_BEER_TIMING          = "Time: ";
    const String STR_BEER_TIMING_UNIT     = "ms";
    const String STR_PREV_COUNT           = "Prev: ";
    const String STR_CURR_COUNT           = "Curr: ";
    const String STR_FASTEST_TIME         = "Fastest time: ";
    const String STR_LIFETIME_COUNT       = "Total drinks: ";
    const String STR_LIFETIME_VOLUME      = "Total volume: ";
    const String STR_LIFETIME_VOLUME_UNIT = " ml";
    const String STR_TONIGHT              = " tonight";
};

#endif
