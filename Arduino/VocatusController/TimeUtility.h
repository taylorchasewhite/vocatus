/**
 * TimeUtility.h - Header file for displaying dateTimes.
 * Note that dateTime is not actually a type in this context but a language-agnostic term being used for the object being passed.
 * Technically the accepted object is a time_t struct based off of an unsigned long type.
 * 
 * Intended to be used by any function or class that needs to display dates.
 * 
 * Created by Taylor White, Stephen Lago, July 9, 2018
 * See https://www.arduino.cc/en/Hacking/LibraryTutorial for information on class conventions here.
 * See https://github.com/PaulStoffregen/Time for more information on accessing time on the Arduino.
 * See also https://www.pjrc.com/teensy/td_libs_Time.html
 */
#ifndef TimeUtility_h
#define TimeUtility_h

#include <TimeLib.h>
#include <CStringBuilder.h>

String _timeDisplayString(time_t dateTime);
String _dateTimeString(time_t dateTime);
String _dateString(time_t dateTime);
String _getDigits(int digits);

#endif
