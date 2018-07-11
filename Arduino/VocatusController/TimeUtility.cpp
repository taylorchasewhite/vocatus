/**
 * TimeUtility.h - Implementation file for displaying dateTimes.
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
#include "TimeUtility.h"
/**
 * Given a time_t struct, return a long formatted date
 */
String _dateString(time_t dateTime) {
  String returnString = String(monthStr(month(dateTime)));
  returnString+=String(" ");
  returnString+=String(day(dateTime));
  returnString+=String(", ");
  returnString+=String(year(dateTime));
  return returnString;
}

/**
 * Utility function for time display. Prints a preceding colon and leading 0.
 * @return String returns a human readable time string
 */
String _getDigits(int digits) {
  String returnString=":";
  
  if(digits < 10) {
    returnString+="0";
  }
  returnString+=digits;
  return returnString;
}

/**
 * Get a string in the format of "Month dd, yyyy hh:mm:ss"
 * @return String the long-formatted string of a time_t object
 */
String _dateTimeString(time_t dateTime) {
  String returnString;
  returnString += _dateString(dateTime);
  returnString += " (";
  returnString += _timeDisplayString(dateTime);
  returnString += ")";

  return returnString;
}

/**
 * Get a human-readable string in the format of "hh:mm:tt"
 * @return String the display string representing a time
 */
String _timeDisplayString(time_t dateTime) {
  String returnString = String(hour(dateTime));
  returnString += String(_getDigits(minute(dateTime)));
  returnString += String(_getDigits(second(dateTime)));

  return returnString;
}
