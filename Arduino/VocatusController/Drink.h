/**
 * Drink.h - Represents a single drink drank by a Solo Cup Saver customer.
 * Intended to reduce duplication of variables in the controller as there are multiple instances of 
 * drinks in the controller's runtime environment.
 * Created by Taylor White, Stephen Lago, May 15, 2018
 * See https://www.arduino.cc/en/Hacking/LibraryTutorial for information on class conventions here.
 */
#ifndef Drink_h
#define Drink_h
#include "Arduino.h"

class Drink
{
  public:
  	Drink();
	Drink(int startTime, int endTime, int volume);
	
  	int endTime();
  	void endTime(int endTime);
	  
	int startTime();
	void startTime(int startTime);
	
	int flowRate();
	int timeToFinish();

  private:
    int _drinkDay;
	int _drinkHour;
	int _endTime;
	int _startTime;
	int _volume;
};
#endif