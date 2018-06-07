/**
 * Drink.cpp - Represents a single drink drank by a Solo Cup Saver customer.
 * Intended to reduce duplication of variables in the controller as there are multiple instances of 
 * drinks in the controller's runtime environment.
 * Created by Taylor White, Stephen Lago, May 15, 2018
 * See https://www.arduino.cc/en/Hacking/LibraryTutorial for information on class conventions here.
 * http://www.cplusplus.com/doc/tutorial/templates/
 * 
 */
#include "Arduino.h"
#include "StorageManager.h"

// Addresses
const int ADDR_BEER_COUNT             = 0;
const int ADDR_LIFETIME_VOLUME        = 1*sizeof(float);
const int ADDR_FASTEST_BEER           = 2*sizeof(float);

const int ADDR_TONIGHT_FASTEST_BEER   = 10*sizeof(float);
const int ADDR_TONIGHT_BEER_COUNT     = 11*sizeof(float);
const int ADDR_TONIGHT_VOLUME         = 12*sizeof(float);

Drink::Drink() {
	_startTime=0;
	_endTime=0;
	_volume = 0;
}

Drink::Drink(int startTime,int endTime, int volume) {
	_endTime=endTime;
	_startTime=startTime;
}

/**
 * Get the last instant drink consumption was recorded
 * @TODO should this be a datetime?
 * @return int The last instant drink was consumed
 */
int Drink::endTime() { return _endTime; }

/**
 * Set the last instant this drink's volume was consumed. 
 * Setting this can happen repeatedly as the hall effect sensor measures input and outputs to the microcontroller.
 * If the last instant was significantly long in the past, the caller can determine this drink completed.
 * @TODO Should this be a datetime?
 */
void Drink::endTime(int endTime) { _endTime = endTime; }

/**
 * Get the first instant drink consumption was recorded.
 * @TODO should this be a datetime?
 * @return int The first instant drink was consumed
 */
int Drink::startTime() { return _startTime; }

/**
 * Set the first instant this drink's volume was consumed. This should be set in the constructor only.
 *
 * @TODO Should this be a datetime?
 */
void Drink::startTime(int startTime) { _startTime = startTime; }

/**
 * Get the rate at which this drink was consumed by the drinker.
 * @return int the measurement of how much drink (mL) was drank per second (s).
 */
int Drink::flowRate() {
	int flowRate=-1;
	flowRate=_volume/timeToFinish();

	return flowRate;
}
/**
 * How much time did this drink take to be finished?
 * @return int The amount of time from the first drop to the last drop of drink consumption
 */
int Drink::timeToFinish() {
	return _endTime-_startTime;
}
