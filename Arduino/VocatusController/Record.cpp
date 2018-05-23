/**
 * Record.cpp - Represents the metrics kept and saved in the Arduino
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

enum Type {
	LifeTime,
	Tonight
} recordType;

Record::Record(enum type) {

}

/**
 * Record the successful completion of another beer. Update the count indicating number of beers drank,
 * the volume drank in this record's lifetime, and the evaluate if the new beer was the fastest
 * 
 * @param startTime int the first instant of beer consumption recorded
 * @param endTime int the last instant of beer consumption recorded
 * @param volume float the amount of recorded liquid in this beer
 */
void Record::addBeer(int startTime, int endTime, float volume) {
	this.addBeer(new Beer(startTime,endTime,volume));
}

/**
 * Record the successful completion of another beer. Update the count indicating number of beers drank,
 * the volume drank in this record's lifetime, and the evaluate if the new beer was the fastest.
 * 
 * @param beer Beer The beer that was just drank.
 */
void Record::addBeer(Beer beer) {
	addCount();
	addVolume(beer.volume()):
	evalAndUpdateFastestBeer();
}

/**
 * Increase the number of beers drank for this record by one
 */
void Record::addCount() {
	_count++;
}

/**
 * Return the number of beers drank during this record's lifetime.
 */
int Record::count() {
	return _count;
}

/**
 * Will *set* the count for this record--be careful when using this, generally it should only be incremented
 * using addCount()
 */
void Record::count(int count) {
	_coount=count;
}

/**
 * Return the time this record became inactive. If this record is a lifetime record, this should never be set.
 * If this record is a nightly record, it will be set whenever the night has been determined to be complete.
 * TODO:Should this return a datetime?
 * @return int the end time
 */
int Record::endTime() {
	return _endTime;
}

/**
 * Set an end time for this record. This will essentially discontinue this record's life.
 */
void Record::endTime(int endTime) {
	_endTime=endTime;
}

/**
 * Given a beer, evaluate if it is the fastest beer for this record and return back to the caller if so.
 * Also, save this beer as the new fastest time.
 * 
 * @param newBeer Beer the newly drank beer to be evaluated against the fastest beer.
 * @return bool if the new beer is the fastest beer, return true, else, return false.
 */
bool Record::evalAndUpdateFastestBeer(Beer newBeer) {
	if (newBeer.startTime()-newBeer.endTime() < _fastestBeer.timeToFinish()) {
		_fastestBeer=newBeer;
		return true;	
	}
	return false;
}

/**
 * Return the completion duration of the fastest beer drank during this record's lifetime.
 * @return float The amount of time needed to completely finish the beer.
 * 
 */
float Record::fastestTime() { // return the completion duration of the fastest beer
	return _fastestBeer.timeToFinish();
} 			

/**
 * Return the fastest Beer drank during the course of this record.
 * @return Beer return the Beer that has the fastest time recorded
 */
Beer Record::fastestBeer() { // Same as fastest time but returns the Beer record.
	return _fastestBeer;
} 		

/**
 * Get the start time that this record began tracking beer consumption. 
 * @TODO: Should this be a datetime?
 * @return int the integer value representing the start instant
 */
int Record::startTime() {
	return _startTime;
}

/**
 * Set the Record's start time to represent when it was initialized and began tracking beer consumption.
 * @TODO: Should this be a datetime?
 */
void Record::startTime(startTime) {
	_startTime=startTime;
}
	
/**
 * Return what the average flow rate has been over the course of this record.
 * This should be the calculation of:
 * 	= Total volume consumed/# of minutes spent consuming.
 * @TODO: Implement
*/ 
int Record::flowRate() {

}

/*
	Get back the total volume of liquid consumed during this record's lifetime.
*/
float Record::volume() {
	return _volume;
}

/*
	Given a float volume value, add it to this record's total volume.
*/
void Record::addVolume(float volume) {
	_volume += volume;
}
