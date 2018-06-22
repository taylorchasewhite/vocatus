/**
 * StorageManager.h - Library for reading/writing to the Arduino UNO's EEPROM.
 * Intended to abstract the controller from the hardware addresses we need to code in
 * such that the controller just uses get/set commands.
 * Created by Taylor White, Stephen Lago, May 15, 2018
 * See https://www.arduino.cc/en/Hacking/LibraryTutorial for information on class conventions here.
 */
#ifndef StorageManager_h
#define StorageManager_h
#include "Arduino.h"
#include "StorageIO.h"
#include "Record.h"

class StorageManager
{
  public:
		StorageManager(StorageManager & copy);
		StorageManager();
		
		float readFloatData(int address);
		int readIntegerData(int address);
		
		float lifetimeCount();
		void lifetimeCount(float count);

		int lifetimeFastestTime();
		void lifetimeFastestTime(float drinkTime);

		Record& lifetimeRecord();

		float lifetimeVolume();
		void lifetimeVolume(float volume);

		float tonightCount();
		void tonightCount(float count);

		int tonightFastestTime();
		void tonightFastestTime(float drinkTime);

		Record& tonightRecord();

		float tonightVolume();
		void tonightVolume(float volume);

		void storeAllValues(float drinkCount,float drinkTime, float drinkVolume);
		void storeAllValues(Record& lifetime, Record& tonight);
		void storeData(int address, float value);
		void storeData(int address, int value);
		void reset();

  private:
    int _pin;
		void clearEEPROM();
};
#endif