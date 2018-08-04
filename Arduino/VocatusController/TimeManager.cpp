/**
 * TimeManager.cpp - Library for getting/setting/syncing time.
 * Intended to abstract the controller from the hardware needed to sync the time
 * such that the controller can just ask for the time once this object has been instantiated.
 * Created by Taylor White, Stephen Lago, July 9, 2018
 * See https://www.arduino.cc/en/Hacking/LibraryTutorial for information on class conventions here.
 * See https://github.com/PaulStoffregen/Time for more information on accessing time on the Arduino.
 * See also https://www.pjrc.com/teensy/td_libs_Time.html
 */
#include "Arduino.h"
#include "TimeManager.h"

#define TIME_HEADER  "T"   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 


/**
 * Copy constructor, makes an exact copy in newly allocated memory
 * @param copy TimeManager the TimeManager we are copying from
 */
TimeManager::TimeManager(TimeManager & copy) {
  // TODO: Write
  this->_initialize();
}

/**
 * Default constructor, does nothing so as not to use memory
 */
TimeManager::TimeManager() {
   this->_initialize();
}

/**
 * Destructor that deletes each property for the TimeManager object and ends syncing with the time provider
 */
TimeManager::~TimeManager() {
  
}

/**
 * Initialize the TimeManager by setting up the sync function
 */
void TimeManager::_initialize() {
  while (!Serial) ; // wait until Arduino Serial Monitor opens
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  
  if(timeStatus()!= timeSet) {
     Serial.println("Unable to sync with the RTC");
  }
  else {
     Serial.println("RTC has set the system time");  
     this->digitalClockDisplay();
  }
}

/**
 * Process Sync Message
 */
void TimeManager::manageTime() {
  if (timeStatus() == timeSet) {
    digitalWrite(13, HIGH); // LED on if synced
    digitalClockDisplay();
  } 
  else {
    digitalWrite(13, LOW);  // LED off if needs refresh
    Serial.println("The time has not been set.  Please run the Time");
    Serial.println("TimeRTCSet example, or DS1307RTC SetTime example.");
    Serial.println();
  }
}

void TimeManager::digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(_timeDisplayString(now()));
  Serial.print(" ");
  Serial.print(month());
  Serial.print("/");
  Serial.print(day());
  Serial.print("/");
  Serial.print(year()); 
  Serial.println(); 
}

/**
 * Outputs the human readable time display to the console.
 */
void TimeManager::_printDigits(int digits){
  Serial.print(_getDigits(digits));
}


