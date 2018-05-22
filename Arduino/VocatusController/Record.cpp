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

int Beer::endTime() { return _endTime; }
void Beer::endTime(int endTime) { _endTime = endTime; }
	
int Beer::startTime() { return _startTime; }
void Beer::startTime(startTime) { _startTime = startTime; }

int Beer::flowRate() {
	int flowRate=-1;
	flowRate=_totalVolume/completionDuration();

	return flowRate;
}
int Beer::completionDuration() {

}
