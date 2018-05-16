
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

// Addresses
const int ADDR_BEER_COUNT             = 0;
const int ADDR_LIFETIME_VOLUME        = 1*sizeof(float);
const int ADDR_FASTEST_BEER           = 2*sizeof(float);

const int ADDR_TONIGHT_FASTEST_BEER   = 10*sizeof(float);
const int ADDR_TONIGHT_BEER_COUNT     = 11*sizeof(float);
const int ADDR_TONIGHT_VOLUME         = 12*sizeof(float);

StorageManager::StorageManager(int pin)
{
  pinMode(pin, OUTPUT);
  _pin = pin;
}

/*
  Get integer value held in permanent storage from EEPROM data.
  
  @param address integer value denoting where to read from
  @return the integer value stored in the desired address.
*/
int StorageManager::readIntegerData(int address) {
  int value;
  EEPROM.get(address, value);
  return value;
}

/*
  Get float value held in permanent storage from EEPROM data.
  
  @param address integer value denoting where to read from
  @return the float value stored in the desired address.
*/
float StorageManager::readFloatData(int address) {
  float value;
  EEPROM.get(address,value);
  /*Serial.print("Read ");
  Serial.print(value);
  Serial.print(" from ");
  Serial.println(address);*/
  return value;
}

/*
  Store integer value into permanent storage at EEPROM.
  NOTE: There are a limited number of times this can be called, 
    try to store to addresses as few times as possible.
  
  @param address integer value denoting where to store the data to
  @param value integer value you desire to store into EEPROM.
*/
void StorageManager::storeData(int address, int value) {
  float floatVal  = value;
  EEPROM.put(address,floatVal);
  debugPrint(floatVal);
  debugPrint(" stored at address ");
  debugPrintln(address);
}

/*
  Store float value into permanent storage at EEPROM.
  NOTE: There are a limited number of times this can be called, 
    try to store to addresses as few times as possible.
  
  @param address integer value denoting where to store the data to
  @param value float value you desire to store into EEPROM.
*/
void StorageManager::storeData(int address, float value) {
  EEPROM.put(address,value);
  debugPrint(value);
  debugPrint(" stored at address ");
  debugPrintln(address);
}
