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
 */
TimeManager::TimeManager(TimeManager & copy) {
  // TODO: Write
  _initialize();
}

/**
 * Default constructor, used to allocate space when defining members. 
 * Sets all values to null, 0, or other non-meaningful data.
 */
TimeManager::TimeManager() {
  _initialize();
}

/**
 * Initialize the TimeManager by setting up the sync function
 */
void TimeManager::_initialize() {
	setSyncProvider(_requestSync);  //set function to call when sync required	
	Serial.println("Waiting for sync message");
}

void TimeManager::manageTime() {
  if (Serial.available()) {
    _processSyncMessage();
  }
  if (timeStatus()!= timeNotSet) {
    digitalClockDisplay();  
  }
  if (timeStatus() == timeSet) {
    digitalWrite(13, HIGH); // LED on if synced
  } else {
    digitalWrite(13, LOW);  // LED off if needs refresh
  }
}

void TimeManager::digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  _printDigits(minute());
  _printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
}

void TimeManager::_printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}


void TimeManager::_processSyncMessage() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 757036800; // Dec 28, 1993

  if(Serial.find(TIME_HEADER)) {
     pctime = Serial.parseInt();
     if( pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
       setTime(pctime); // Sync Arduino clock to the time received on the serial port
     }
  }
}

time_t _requestSync()
{
  Serial.write(TIME_REQUEST);  
  return 0; // the time will be sent later in response to serial mesg
}