
/*
 * StorageManager.cpp - Library for reading/writing to the Arduino UNO's EEPROM.
 * Intended to abstract the controller from the hardware addresses we need to code in
 * such that the controller just uses get/set commands.
 * Created by Taylor White, Stephen Lago, May 15, 2018
 * See https://www.arduino.cc/en/Hacking/LibraryTutorial for information on class conventions here.
 * https://stackoverflow.com/questions/2043493/where-to-declare-define-class-scope-constants-in-c
 * 
*/
#include "Arduino.h"
#include "StorageManager.h"
#include "StorageIO.h"
#include "Record.h"
#include "EEPROM.h"
#include <TimeLib.h>

// Addresses
const int ADDR_DRINK_COUNT              = 0;
const int ADDR_LIFETIME_VOLUME          = 1*sizeof(float);
const int ADDR_FASTEST_DRINK            = 2*sizeof(float);
const int ADDR_LIFETIME_START_TIME      = 3*sizeof(float);

const int ADDR_TONIGHT_DRINK_COUNT      = 10*sizeof(float);
const int ADDR_TONIGHT_FASTEST_DRINK    = 11*sizeof(float);
const int ADDR_TONIGHT_VOLUME           = 12*sizeof(float);
const int ADDR_TONIGHT_START_TIME       = 13*sizeof(float);

/**
 * Copy constructor, makes an exact copy in newly allocated memory
 */
StorageManager::StorageManager(StorageManager & copy) {
  // TODO: Write
  _io =* new StorageIO();
}

/**
 * Default constructor, used to allocate space when defining members. 
 * Sets all values to null, 0, or other non-meaningful data.
 */
StorageManager::StorageManager() {
  _io =* new StorageIO();
}


/*
  Store all of the records we keep for the lifetime interval.
  @param drinkCount float the value representing the number of drinks drank.
  @param fastestDrinkTime float the value representing the amount of time needed to record the fastest drink time
  @param drinkVolume float the amount of volume drank during the lifetime record
*/
void StorageManager::storeLifetimeValues(float drinkCount,float fastestDrinkTime, float drinkVolume) {
  lifetimeCount(drinkCount);
  lifetimeFastestTime(fastestDrinkTime);
  lifetimeVolume(drinkVolume);
}

/*
  Store all of the records we keep for the lifetime interval.
  @param drinkCount float the value representing the number of drinks drank.
  @param fastestDrinkTime float the value representing the amount of time needed to record the fastest drink time
  @param drinkVolume float the amount of volume drank during the lifetime record
*/
void StorageManager::storeTonightValues(float drinkCount,float drinkTime, float drinkVolume) {
  tonightCount(drinkCount);
  tonightFastestTime(drinkTime);
  tonightVolume(drinkVolume);
}

/*
  Store all of the records we keep for the lifetime interval.
  @param lifetime Record the record representing the lifetime values
  @param tonight Record the record representing the tonight values
*/
void StorageManager::storeAllValues(Record& lifetime, Record& tonight) {
  lifetimeCount(lifetime.count());
  lifetimeFastestTime(lifetime.fastestTime());
  lifetimeVolume(lifetime.volume());
  tonightCount(tonight.count());
  tonightFastestTime(tonight.fastestTime());
  tonightVolume(tonight.volume());
}

//Getters and setters
float StorageManager::lifetimeCount() { 
  return _io.readFloatData(ADDR_DRINK_COUNT); 
}
/**
 * Store the lifetime number of drinks drank during the lifetime of the machine.
 */
void StorageManager::lifetimeCount(float count) { 
  _io.storeData(ADDR_DRINK_COUNT,count); 
}

/**
 * Get the fastest lifetime completion time for a drink
 */
int StorageManager::lifetimeFastestTime() { 
  return _io.readFloatData(ADDR_FASTEST_DRINK); 
}
/**
 * Store the fastest drink by storing the fastest time
 * @param drinkTime the time in milliseconds that reflects the fastest time for the machine lifetime.
 */
void StorageManager::lifetimeFastestTime(float drinkTime) { 
  _io.storeData(ADDR_FASTEST_DRINK,drinkTime); 
}
/**
 * Return the lifetime Record object that is stored in storage
 * @return Record the lifetime record object containing information on volume, speed, etc.
 */
Record& StorageManager::lifetimeRecord() {
  return *new Record((int)lifetimeCount(),lifetimeVolume(),lifetimeFastestTime(),0); // TODO load startTime
}

float StorageManager::lifetimeVolume() { 
  return _io.readFloatData(ADDR_LIFETIME_VOLUME); 
}

void StorageManager::lifetimeVolume(float volume) { 
  _io.storeData(ADDR_LIFETIME_VOLUME,volume); 
}

/**
 * Get the start time for the tonight record
 * @return time_t the start instant for when the record started.
 */
time_t StorageManager::lifetimeStartTime() {
  return _io.readFloatData(ADDR_TONIGHT_START_TIME);
  
}

/**
 * Store the start time for the tonight record
 * @param startTime time_t the start instant for when this record started.
 */
void StorageManager::lifetimeStartTime(time_t startTime) {
  _io.storeData(ADDR_TONIGHT_START_TIME,startTime);
}

/**************
 * Tonight
 **************/
float StorageManager::tonightCount() { 
  return _io.readFloatData(ADDR_TONIGHT_DRINK_COUNT); 
}
/**
 * Store the lifetime number of drinks drank during the lifetime of the machine.
 */
void StorageManager::tonightCount(float count) { 
  _io.storeData(ADDR_TONIGHT_DRINK_COUNT,count); 
}

/**
 * Get the fastest lifetime completion time for a drink
 */
int StorageManager::tonightFastestTime() { 
  return _io.readFloatData(ADDR_TONIGHT_FASTEST_DRINK); 
}
/**
 * Store the fastest drink by storing the fastest time
 * @param drinkTime the time in milliseconds that reflects the fastest time for the machine lifetime.
 */
void StorageManager::tonightFastestTime(float drinkTime) { 
  _io.storeData(ADDR_TONIGHT_FASTEST_DRINK,drinkTime); 
}
/**
 * Return the lifetime Record object that is stored in storage
 * @return Record the lifetime record object containing information on volume, speed, etc.
 */
Record& StorageManager::tonightRecord() {
  return *new Record((int)tonightCount(),tonightVolume(),tonightFastestTime(),0); // TODO load startTime
}

/**
 * Return the volume from memory that represents the amount of liquid drank tonight.
 * @return float the amount of liquid consumed tonight
 */
float StorageManager::tonightVolume() { 
  return _io.readFloatData(ADDR_TONIGHT_VOLUME); 
}

/**
 * Set in memory the amount of liquid drank tonight
 */
void StorageManager::tonightVolume(float volume) { 
  _io.storeData(ADDR_TONIGHT_VOLUME,volume); 
}

/**
 * Get the start time for the tonight record
 * @return time_t the start instant for when the record started.
 */
time_t StorageManager::tonightStartTime() {
  return _io.readFloatData(ADDR_TONIGHT_START_TIME);
  
}

/**
 * Store the start time for the tonight record
 * @param startTime time_t the start instant for when this record started.
 */
void StorageManager::tonightStartTime(time_t startTime) {
  _io.storeData(ADDR_TONIGHT_START_TIME,startTime);
}

/**
 * Reset all of the permanent storage in the Arduino. There's no going back once you do this.
 */ 
void StorageManager::reset() {
  _io.reset();
}
