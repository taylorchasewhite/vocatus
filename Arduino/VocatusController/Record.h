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
#include <TimeLib.h>

class Record
{
  public:
  	//Record(Type type);
	  Record(int count, float volume, int fastestTime,time_t startTime);
	  Record(Record &copy);
	  Record();

    ~Record();

    void Reset();

	  void addDrink(int startTime, int endTime, float volume);
	  void addDrink(Drink& drink);
	  void addCount();

	  int count();
	  void count(int count);
	
  	time_t endTime();
  	void endTime(time_t endTime);

	  bool evalAndUpdateFastestDrink(Drink& drink);

	  float fastestTime(); 			// return the completion duration of the fastest drink
	  Drink& fastestDrink(); 			// Same as fastest time but returns the Drink record.

    Drink& lastDrink();
	  
	  time_t startTime();
	  String startTimeString();
	  void startTime(time_t startTime);
	
	  int flowRate();

	  float volume();					// what's the total volume consumed for this record's lifetime
	  void addVolume(float volume);

  private:
	  int _count;
	  Drink _fastestDrink;
    Drink _lastDrink;
    time_t _endTime;
	  time_t _startTime;
	  float _volume;
};

String _dateString(time_t dateString);
#endif
