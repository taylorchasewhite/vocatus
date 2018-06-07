/**
 * Beer.cpp - Represents a single beer drank by a Solo Cup Saver customer.
 * Intended to reduce duplication of variables in the controller as there are multiple instances of 
 * beers in the controller's runtime environment.
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

Beer::Beer() {
	_startTime=0;
	_endTime=0;
	_volume = 0;
}

Beer::Beer(int startTime,int endTime, int volume) {
	_endTime=endTime;
	_startTime=startTime;
}

/**
 * Get the last instant beer consumption was recorded
 * @TODO should this be a datetime?
 * @return int The last instant beer was consumed
 */
int Beer::endTime() { return _endTime; }

/**
 * Set the last instant this beer's volume was consumed. 
 * Setting this can happen repeatedly as the hall effect sensor measures input and outputs to the microcontroller.
 * If the last instant was significantly long in the past, the caller can determine this beer completed.
 * @TODO Should this be a datetime?
 */
void Beer::endTime(int endTime) { _endTime = endTime; }

/**
 * Get the first instant beer consumption was recorded.
 * @TODO should this be a datetime?
 * @return int The first instant beer was consumed
 */
int Beer::startTime() { return _startTime; }

/**
 * Set the first instant this beer's volume was consumed. This should be set in the constructor only.
 *
 * @TODO Should this be a datetime?
 */
void Beer::startTime(int startTime) { _startTime = startTime; }

/**
 * Get the rate at which this beer was consumed by the drinker.
 * @return int the measurement of how much beer (mL) was drank per second (s).
 */
int Beer::flowRate() {
	int flowRate=-1;
	flowRate=_volume/timeToFinish();

	return flowRate;
}
/**
 * How much time did this beer take to be finished?
 * @return int The amount of time from the first drop to the last drop of beer consumption
 */
int Beer::timeToFinish() {
	return _endTime-_startTime;
}
