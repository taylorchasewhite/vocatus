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
#include "Beer.h"

Beer::Beer() {
	_startTime=0;
	_endTime=0;
	_totalVolume=0;
}

Beer::Beer(int startTime, int endTime, float volume) {
	_startTime = startTime;
	_endTime = endTime;
	_totalVolume = volume;
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
 * 
 * @param endTime int the endTime i(n seconds since 1970) that the last drop of beer was drank for this beer.
 * @TODO Should this be a datetime?
 */
void Beer::endTime(int endTime) { _endTime = endTime; }

/**
 * Determines whether or not this beer has been finished. If another drop is recorded by the controller, this function should
 * be used to determine if the additional volume should be attributed to this beer or a new beer.
 * 
 * @return bool Returns if this beer has an endTime set, which is being used to attribute whether the beer is finished or not.
 */
bool Beer::isFinished() {
	return _endTime>0;
}

/**
 * Get the rate at which this beer was consumed by the drinker.
 * @return int the measurement of how much beer (mL) was drank per second (s).
 */
int Beer::flowRate() {
	int flowRate=-1;
	flowRate=_totalVolume/timeToFinish();

	return flowRate;
}

/**
 * Get the first instant beer consumption was recorded.
 * @TODO should this be a datetime?
 * @return int The first instant beer was consumed
 */
int Beer::startTime() { return _startTime; }

/**
 * Set the first instant this beer's volume was consumed.
 * @param startTime int the start time in seconds since 1970 representing when the first drop of beer happened.
 * 
 * @TODO Should this be a datetime?
 */
void Beer::startTime(int startTime) { _startTime = startTime; }

/**
 * How much time did this beer take to be finished?
 * @return int The amount of time from the first drop to the last drop of beer consumption
 */
int Beer::timeToFinish() { return _endTime-_startTime; }

/**
 * Gets the amount of liquid (in mL) consumed for this beer.
 * @return float the quantity of liquid consumed
 */
float Beer::volume() { return _volume; }

/**
 * Add the specified amount of beer to this beer's total volume
 * @param volume float the amount of volume that's been consumed.
float addVolume(float volume) {
	_volume +=volume;
}

/**
 * Set the volume consumed for this beer.
 *
 */
void Beer:volume(float volume) {
	_volume = volume;
}