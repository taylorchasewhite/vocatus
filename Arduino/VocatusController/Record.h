/**
 * Record.h - Represents a single beer drank by a Solo Cup Saver customer.
 * Intended to reduce duplication of variables in the controller as there are multiple instances of 
 * beers in the controller's runtime environment.
 * Created by Taylor White, Stephen Lago, May 15, 2018
 * See https://www.arduino.cc/en/Hacking/LibraryTutorial for information on class conventions here.
 */
#ifndef Record_h
#define Record_h
#include "Arduino.h"
#include "Beer.h"

class Record
{
  public:
  	Record(RecordType type);
	
  	int endTime();
  	void endTime(int endTime);
	  
	int startTime();
	void startTime(startTime);
	
	int flowRate();
	int completionDuration();

	float totalVolume();			// what's the total volume consumed for this record's lifetime
	float fastestTime(); 			// return the completion duration of the fastest beer
	Beer fastestBeer(); 			// Same as fastest time but returns the Beer record.

  private:
    int _volume;
	int _count;
	Beer _fastestBeer;
	int _startTime;
	int _totalVolume;
};
#endif