
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

// Addresses
const int ADDR_DRINK_COUNT             = 0;
const int ADDR_LIFETIME_VOLUME        = 1*sizeof(float);
const int ADDR_FASTEST_DRINK           = 2*sizeof(float);

const int ADDR_TONIGHT_DRINK_COUNT     = 10*sizeof(float);
const int ADDR_TONIGHT_FASTEST_DRINK   = 11*sizeof(float);
const int ADDR_TONIGHT_VOLUME         = 12*sizeof(float);

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
  @param drinkCount float the float value representing the number of drinks drank.
*/
void StorageManager::storeAllValues(float drinkCount,float drinkTime, float drinkVolume) {
  lifetimeCount(drinkCount);
  lifetimeFastestTime(drinkTime);
  lifetimeVolume(drinkVolume);
  tonightCount();
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

float StorageManager::tonightVolume() { 
  return _io.readFloatData(ADDR_TONIGHT_VOLUME); 
}

void StorageManager::tonightVolume(float volume) { 
  _io.storeData(ADDR_TONIGHT_VOLUME,volume); 
}

/**
 * Store all of the records we keep for the tonight interval.
 * @param drinkCount float the float value representing the number of drinks drank.
 * @param drinkTime float the float value representing the number of drinks drank.
 * @param drinkVolume float the total number milliters drank
 */
/*void StorageManager::setTonightValues(float drinkCount,float drinkTime, float drinkVolume) {
  //@TODO this doesn't do anything right now, we should eventually have the storage manager or an abstraction layer handle the logic for tonight vs. lifetime.
  lifetimeCount(drinkCount);
  lifetimeFastestTime(drinkTime);
  lifetimeVolume(drinkVolume);

}*/

/**
 * Reset all of the permanent storage in the Arduino. There's no going back once you do this.
 */ 
void StorageManager::reset() {
  _io.reset();
}