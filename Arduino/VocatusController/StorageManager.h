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

class StorageManager
{
  public:
    StorageManager(int pin);

	float readFloatData(int address);
	int readIntegerData(int address);

	float getLifetimeCount();
	void setLifetimeCount(float count);

	int getLifetimeFastestTime();
	void setLifetimeFastestTime(float beerTime);

	Record getLifetimeRecord();

	float getLifetimeVolume();
	void storeLifetimeTotalVolume(float volume);

	void storeAllValues(float beerCount,float beerTime, float beerVolume);
	void storeData(int address, float value);
	void storeData(int address, int value);
	void reset();

  private:
    int _pin;
		void clearEEPROM();
};
#endif