/**
 * StorageIO.cpp - Library for reading/writing to the Arduino UNO's EEPROM.
 * Intended to abstract the controller from the hardware addresses we need to code in
 * such that the controller just uses get/set commands.
 * Created by Taylor White, Stephen Lago, May 15, 2018
 * See https://www.arduino.cc/en/Hacking/LibraryTutorial for information on class conventions here.
 */

#include "Arduino.h"
#include "StorageIO.h"
#include "Record.h"
#include "EEPROM.h"

/**
 * Copy constructor, makes an exact copy in newly allocated memory
 */
StorageIO::StorageIO(StorageIO & copy) {
  // TODO: Write
}

/**
 * Default constructor, used to allocate space when defining members. 
 * Sets all values to null, 0, or other non-meaningful data.
 */
StorageIO::StorageIO() {
  
}

/**
 * Get float value held in permanent storage from EEPROM data.
 * 
 * @param address integer value denoting where to read from
 * @return the float value stored in the desired address
 */
float StorageIO::readFloatData(int address) {
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
int StorageIO::readIntegerData(int address) {
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
void StorageIO::storeData(int address, int value) {
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
void StorageIO::storeData(int address, float value) {
  EEPROM.put(address,value);
  //debugPrint(value);
  //debugPrint(" stored at address ");
  //debugPrintln(address);
}

/**
 * Reset all of the permanent storage in the Arduino. There's no going back once you do this.
 */ 
void StorageIO::reset() {
  _clearEEPROM();
}

/**
 * Only call this when resetting the device to factory settings!
 * Permanently erases all persistent data stored on the arduino or board.
 */
void StorageIO::_clearEEPROM() {
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    if(EEPROM.read(i) != 0) {                   //skip already "empty" addresses
    
      EEPROM.write(i, 0);                       //write 0 to address i
    }
  }
}
