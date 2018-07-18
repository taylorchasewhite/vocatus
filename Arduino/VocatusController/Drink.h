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
#include "TimeUtility.h"
  
class Drink
{
  public:
  	Drink();
  	Drink(Drink &copy);
  	Drink(int duration);
  	Drink(int startTime, int endTime, float volume);

    ~Drink();

    void Reset();
	
  	int endTime();
  	void endTime(int endTime);
    String endTimeString();
	  
	  int startTime();
	  void startTime(int startTime);
    String startTimeString(); //TODO Remove -- cannot use millis for this
	  
	  int flowRate();
	  bool isFinished();
	  int timeToFinish();
	  
	  float volume();
	  void volume(float volume);

  private:
    time_t _endInstant;
    int _endTime;
    int _startTime;
    int _volume;
};
#endif
