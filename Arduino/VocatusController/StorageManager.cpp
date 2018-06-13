
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
#include "Record.h"
#include "EEPROM.h"

// Addresses
const int ADDR_DRINK_COUNT             = 0;
const int ADDR_LIFETIME_VOLUME        = 1*sizeof(float);
const int ADDR_FASTEST_DRINK           = 2*sizeof(float);

const int ADDR_TONIGHT_FASTEST_DRINK   = 10*sizeof(float);
const int ADDR_TONIGHT_DRINK_COUNT     = 11*sizeof(float);
const int ADDR_TONIGHT_VOLUME         = 12*sizeof(float);

StorageManager::StorageManager(int pin)
{
  pinMode(pin, OUTPUT);
  _pin = pin;
}

/**
 * Copy constructor, makes an exact copy in newly allocated memory
 */
StorageManager::StorageManager(StorageManager & copy) {
  // TOOD: Write
}

/**
 * Default constructor, used to allocate space when defining members. 
 * Sets all values to null, 0, or other non-meaningful data.
 */
StorageManager::StorageManager() {
  _pin=-1;
}



//Getters and setters
float StorageManager::lifetimeCount() { 
  return readFloatData(ADDR_DRINK_COUNT); 
}
/**
 * Store the lifetime number of drinks drank during the lifetime of the machine.
 */
void StorageManager::lifetimeCount(float count) { 
  storeData(ADDR_DRINK_COUNT,count); 
}

/**
 * Get the fastest lifetime completion time for a drink
 */
int StorageManager::lifetimeFastestTime() { 
  return readFloatData(ADDR_FASTEST_DRINK); 
}
/**
 * Store the fastest drink by storing the fastest time
 * @param drinkTime the time in milliseconds that reflects the fastest time for the machine lifetime.
 */
void StorageManager::lifetimeFastestTime(float drinkTime) { 
  storeData(ADDR_FASTEST_DRINK,drinkTime); 
}
/**
 * Return the lifetime Record object that is stored in storage
 * @return Record the lifetime record object containing information on volume, speed, etc.
 */
Record* StorageManager::lifetimeRecord() {
  return new Record((int)lifetimeCount(),lifetimeVolume(),lifetimeFastestTime(),0); // TODO load startTime
}

float StorageManager::lifetimeVolume() { 
  return readFloatData(ADDR_LIFETIME_VOLUME); 
}

void StorageManager::lifetimeVolume(float volume) { 
  storeData(ADDR_LIFETIME_VOLUME,volume); 
}

/*
  Store all of the records we keep for the lifetime interval.
  @param drinkCount float the float value representing the number of drinks drank.
*/
void StorageManager::storeAllValues(float drinkCount,float drinkTime, float drinkVolume) {
  lifetimeCount(drinkCount);
  lifetimeFastestTime(drinkTime);
  lifetimeVolume(drinkVolume);
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
 * Get float value held in permanent storage from EEPROM data.
 * 
 * @param address integer value denoting where to read from
 * @return the float value stored in the desired address
 */
float StorageManager::readFloatData(int address) {
  float value;
  EEPROM.get(address,value);
  return value;
}

/**
 * Get integer value held in permanent storage from EEPROM data.
 * 
 * @param address integer value denoting where to read from
 * @return the integer value stored in the desired address.
 */
int StorageManager::readIntegerData(int address) {
  int value;
  EEPROM.get(address, value);
  return value;
}

/**
 * Store integer value into permanent storage at EEPROM.
 * NOTE: There are a limited number of times this can be called, 
 * try to store to addresses as few times as possible.
 * 
 * @param address integer value denoting where to store the data to
 * @param value integer value you desire to store into EEPROM
 */
void StorageManager::storeData(int address, int value) {
  float floatVal  = value;
  EEPROM.put(address,floatVal);
  //debugPrint(floatVal);
  //debugPrint(" stored at address ");
  //debugPrintln(address);
}

/**
 * Store float value into permanent storage at EEPROM.
 * NOTE: There are a limited number of times this can be called, 
 * try to store to addresses as few times as possible.
 * 
 * @param address integer value denoting where to store the data to
 * @param value float value you desire to store into EEPROM.
 */
void StorageManager::storeData(int address, float value) {
  EEPROM.put(address,value);
  //debugPrint(value);
  //debugPrint(" stored at address ");
  //debugPrintln(address);
}

/**
 * Reset all of the permanent storage in the Arduino. There's no going back once you do this.
 */ 
void StorageManager::reset() {
  clearEEPROM();
}

/**
 * Only call this when resetting the device to factory settings!
 * Permanently erases all persistent data stored on the arduino or board.
 */
void StorageManager::clearEEPROM() {
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    if(EEPROM.read(i) != 0) {                   //skip already "empty" addresses
    
      EEPROM.write(i, 0);                       //write 0 to address i
    }
  }
  //debugPrintln("EEPROM erased");
}