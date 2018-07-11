/**
 * TimeManager.h - Header file for getting/setting/syncing time.
 * Intended to abstract the controller from the hardware needed to sync the time
 * such that the controller can just ask for the time once this object has been instantiated.
 * Created by Taylor White, Stephen Lago, July 9, 2018
 * See https://www.arduino.cc/en/Hacking/LibraryTutorial for information on class conventions here.
 * See https://github.com/PaulStoffregen/Time for more information on accessing time on the Arduino.
 * See also https://www.pjrc.com/teensy/td_libs_Time.html
 */
#ifndef TimeManager_h
#define TimeManager_h
#include <TimeLib.h>
#include "Arduino.h"
#include "Record.h"

class TimeManager
{
  public:
  	TimeManager(TimeManager & copy);
	  TimeManager();
    ~TimeManager();
	  TimeManager(int seconds);
	  void manageTime();
	  void digitalClockDisplay();
  private:
	  void _initialize(int seconds);
	  void _printDigits(int digits);
};

String _timeDisplayString(time_t dateTime);
String _dateTimeString(time_t dateTime);
String _dateString(time_t dateTime);
String _getDigits(int digits);
void _processSyncMessage();
time_t _requestSync();
#endif
