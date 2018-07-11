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
  this->_initialize(10);
}

/**
 * Constructor that takes a variable number of seconds to
 * alternate the frequency of the sync request
 * @param int seconds the number of seconds between sync requests
 */
TimeManager::TimeManager(int seconds) {
  this->_initialize(seconds);
}

/**
 * Default constructor, defaults to ten seconds between sync requests
 */
TimeManager::TimeManager() {
	//this->_initialize(10);
}

/**
 * Destructor that deletes each property for the TimeManager object and ends syncing with the time provider
 */
TimeManager::~TimeManager() {
  
}

/**
 * Initialize the TimeManager by setting up the sync function
 */
void TimeManager::_initialize(int seconds) {
  while (!Serial) ; // Needed for Leonardo only
	setSyncProvider(_requestSync);  //set function to call when sync required	
  setTime(1531274012); // TODO: Remove when using RTC
	//setSyncInterval(seconds);
	pinMode(13, OUTPUT);
  
  // TODO: Uncomment when using with RTC
	/*Serial.print("Waiting for sync message.");
	Serial.print(" Requesting sync every ");
	Serial.print(seconds);
	Serial.print(" second(s).");*/
}

/**
 * Process Sync Message
 */
void TimeManager::manageTime() {
  if (Serial.available()) {
    //_processSyncMessage();
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
  Serial.print(month());
  Serial.print("/");
  Serial.print(day());
  Serial.print("/");
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


void _processSyncMessage() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 757036800; // Dec 28, 1993 - Jessie's birthday
  if(Serial.find(TIME_HEADER)) {
     pctime = Serial.parseInt();
     if( pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
       setTime(pctime); // Sync Arduino clock to the time received on the serial port
	   //Serial.println(_dateString((time_t)pctime));
     }
  }
}

time_t _requestSync()
{
  Serial.write(TIME_REQUEST);  
  return 0; // the time will be sent later in response to serial mesg
}
