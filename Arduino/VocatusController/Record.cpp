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

void Record::addBeer(int startTime, int endTime, float volume) {
	addCount();
	addVolume(volume):
	if (startTime-endTime < _fastestBeer.timeToFinish()) {
		
	}
}

void Record::addCount() {
	_count++;
}

int Record::count() {
	return _count;
}

void Record::count(int count) {
	_coount=count;
}
	
int Record::endTime() {
	return _endTime;
}
void Record::endTime(int endTime) {
	_endTime=endTime;
}

float Record::fastestTime() { // return the completion duration of the fastest beer
	return _fastestBeer.timeToFinish();
} 			

Beer Record::fastestBeer() { // Same as fastest time but returns the Beer record.
	return _fastestBeer;
} 		
	  
int Record::startTime() {
	return _startTime;
}
void Record::startTime(startTime) {
	_startTime=startTime;
}
	
/*
	Return what the average flow rate has been over the course of this record.
	This should be the calculation of:
		= Total volume consumed/# of minutes spent consuming.
	TODO: Implement
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
	Given a float volume value, add it to this record's total volume
*/
void Record::addVolume(float volume) {
	_volume += volume;
}
