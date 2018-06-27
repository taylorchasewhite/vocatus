/**
 * Record.h - Represents a single drink drank by a Solo Cup Saver customer.
 * Intended to reduce duplication of variables in the controller as there are multiple instances of 
 * drinks in the controller's runtime environment.
 * Created by Taylor White, Stephen Lago, May 15, 2018
 * See https://www.arduino.cc/en/Hacking/LibraryTutorial for information on class conventions here.
 */
#ifndef Record_h
#define Record_h
#include "Arduino.h"
#include "Drink.h"

class Record
{
  public:
  	//Record(Type type);
	Record(int count, float volume, int fastestTime,int startTime);
	Record(Record &copy);
	Record();

	void addDrink(int startTime, int endTime, float volume);
	void addDrink(Drink& drink);
	void addCount();

	int count();
	void count(int count);
	
  	int endTime();
  	void endTime(int endTime);

	bool evalAndUpdateFastestDrink(Drink& drink);

	float fastestTime(); 			// return the completion duration of the fastest drink
	Drink& fastestDrink(); 			// Same as fastest time but returns the Drink record.
	  
	int startTime();
	void startTime(int startTime);
	
	int flowRate();

	float volume();					// what's the total volume consumed for this record's lifetime
	void addVolume(float volume);

  private:
	int _count;
	Drink _fastestDrink;
  int _endTime;
	int _startTime;
	float _volume;
};
#endif
