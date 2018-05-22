/**
 * Beer.h - Represents a single beer drank by a Solo Cup Saver customer.
 * Intended to reduce duplication of variables in the controller as there are multiple instances of 
 * beers in the controller's runtime environment.
 * Created by Taylor White, Stephen Lago, May 15, 2018
 * See https://www.arduino.cc/en/Hacking/LibraryTutorial for information on class conventions here.
 */
#ifndef Beer_h
#define Beer_h
#include "Arduino.h"

class Beer
{
  public:
  	Beer(int pin);
	
  	int endTime();
  	void endTime(int endTime);
	  
	int startTime();
	void startTime(startTime);
	
	int flowRate();
	int timeToFinish();

  private:
    int _beerDay;
	int _beerHour;
	int _endTime;
	int _startTime;
	int _totalVolume;
};
#endif