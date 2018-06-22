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
#include <Time.h>

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

//constant string values for display
const String LIFECOUNT_LABEL          = "Lifetime";
const String LIFECOUNT_UNIT           = "drinks";
const String LIFECOUNT_UNIT_SINGLE    = "drink";
const String TONIGHTCOUNT_LABEL       = "Tonight";
const String TONIGHTCOUNT_UNIT        = "drinks";
const String TONIGHTCOUNT_UNIT_SINGLE = "drink";
const String LIFESPEED_LABEL          = "All-Time Record";
const String LIFESPEED_UNIT           = "ms";
const String TONIGHTSPEED_LABEL       = "Tonight's Record";
const String TONIGHTSPEED_UNIT        = "ms";
const String LASTSPEED_LABEL          = "Last Drink";
const String LASTSPEED_UNIT           = "ms";
const String LIFEVOLUME_LABEL         = "All-Time Volume";
const String LIFEVOLUME_UNIT          = "mL";
const String TONIGHTVOLUME_LABEL      = "Tonight's Volume";
const String TONIGHTVOLUME_UNIT       = "mL";
const String LASTVOLUME_LABEL         = "Last Volume";
const String LASTVOLUME_UNIT          = "mL";


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

    void OutputData(Record&, Record&, int, float);
    void CycleCurrentValueToDisplay();
  private:
    OutputMode _outputMode;
    CurrentValueToDisplay _currentValueToDisplay;
    
    bool _isDebugEnabled;
    bool _isLcdEnabled;
    bool _isStatusBoardEnabled;

    void _initDebug();
    bool _shouldDebug();
    String _handleSingleCase(float,String,String);
    String _handleSingleCase(int,String,String);
    String _createDebugString(String, int, String);
    String _createDebugString(String, float, String);
    void _sendDebugReport(Record&, Record&, int, float);
    void _changeOutputMode(OutputMode);
    String _buildComString(Record&, Record&, int, float);
    void _sendToStatusBoard(Record&, Record&, int, float);
    void _initLcd();
    void _sendToLcd(Record&, Record&, int);
};

#endif
