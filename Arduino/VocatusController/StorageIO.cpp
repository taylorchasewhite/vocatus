/**
 * StorageIO.cpp - Library for reading/writing to the Arduino UNO's EEPROM.
 * Intended to abstract the controller from the hardware addresses we need to code in
 * such that the controller just uses get/set commands.
 * Created by Taylor White, Stephen Lago, May 15, 2018
 * See https://www.arduino.cc/en/Hacking/LibraryTutorial for information on class conventions here.
 */
#ifndef StorageIO_h
#define StorageIO_h
#include "Arduino.h"
#include "Record.h"

class StorageIO
{
  public:
		StorageIO(StorageManager & copy);
		StorageIO();
		
		float readFloatData(int address);
		int readIntegerData(int address);

		Record& getRecord();
		void setRecord(Record& record);

		void storeAllValues(float drinkCount,float drinkTime, float drinkVolume);
		void storeAllValues(Record& lifetime, Record& tonight);
		void storeData(int address, float value);
		void storeData(int address, int value);
		void reset();

  private:
  	void clearEEPROM();
};
#endif