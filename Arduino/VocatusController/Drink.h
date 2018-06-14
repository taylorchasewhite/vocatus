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
<<<<<<< HEAD:Arduino/VocatusController/Beer.h
	Beer();
	Beer(int startTime, int endTime, float volume);
=======
  	Drink();
	Drink(Drink &copy);
	Drink(int duration);
	Drink(int startTime, int endTime, int volume);
>>>>>>> taylor-web-admin:Arduino/VocatusController/Drink.h
	
  	int endTime();
  	void endTime(int endTime);
	  
	int startTime();
	void startTime(int startTime);
<<<<<<< HEAD:Arduino/VocatusController/Beer.h

	float volume();
	void volume(float volume);
	void addVolume();
=======
>>>>>>> taylor-web-admin:Arduino/VocatusController/Drink.h
	
	int flowRate();
	bool isFinished();
	int timeToFinish();

	int volume();
	void volume(int volume);

  private:
    int _drinkDay;
	int _drinkHour;
	int _endTime;
	int _startTime;
	int _volume;
};
#endif
