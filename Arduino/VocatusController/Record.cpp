/**
 * Record.cpp - Represents the metrics kept and saved in the Arduino
 * Intended to reduce duplication of variables in the controller as there are multiple instances of 
 * drinks in the controller's runtime environment.
 * Created by Taylor White, Stephen Lago, May 15, 2018
 * See https://www.arduino.cc/en/Hacking/LibraryTutorial for information on class conventions here.
 * http://www.cplusplus.com/doc/tutorial/templates/
 * 
 */
#include "Arduino.h"
#include "Record.h"
#include "Drink.h"

//@NOTE:: Not sold on variable naming convention; makes it confusing to parse the code
//examples: count(), startTime(), endTime()

enum Type {
	LifeTime,
	Tonight
} recordType;

/**
 * Initialize a new record with just count,volume and fastestTime.
 * @param count int the number of drinks this record should have
 */
Record::Record(int count, float volume, int fastestTime,int startTime) {
	this->count(count);
	this->addVolume(volume);
	_fastestDrink=*new Drink(fastestTime);
	//startTime(startTime);
}

/**
 * Initialize a new record that is a copy of the values in Record
 * @param copy this is the Record that we will copy from
 */
Record::Record(Record &copy) {
	this->addVolume(copy.volume());
	_fastestDrink=*new Drink(copy.fastestTime());
	this->count(copy.count());
}

/**
 * Default constructor, used to allocate space when defining members. 
 * Sets all values to null, 0, or other non-meaningful data.
 */
Record::Record() {
	_count=0;
	//Drink* _fastestDrink; // TODO, what to do?
    _endTime=0;
	_startTime=0;
	int _volume=0;
}

//Notes:
//change "Record" to "DrinkList" or "Session" 
//move calculation logic from controller to ^^

//Record::Record(enum type) {

//}

/**
 * Record the successful completion of another drink. Update the count indicating number of drinks drank,
 * the volume drank in this record's lifetime, and the evaluate if the new drink was the fastest
 * 
 * @param startTime int the first instant of drink consumption recorded
 * @param endTime int the last instant of drink consumption recorded
 * @param volume float the amount of recorded liquid in this drink
 */
void Record::addDrink(int startTime, int endTime, float volume) {
	this->addDrink(*new Drink(startTime,endTime,volume));
}

/**
 * Record the successful completion of another drink. Update the count indicating number of drinks drank,
 * the volume drank in this record's lifetime, and the evaluate if the new drink was the fastest.
 * 
 * @param drink Drink The drink that was just drank.
 */
void Record::addDrink(Drink& drink) {
	this->addCount();
	this->addVolume(drink.volume());
	this->evalAndUpdateFastestDrink(drink);
}

/**
 * Increase the number of drinks drank for this record by one
 */
void Record::addCount() {
	_count++;
}

/**
 * Return the number of drinks drank during this record's lifetime.
 */
int Record::count() {
	return _count;
}

/**
 * Will *set* the count for this record--be careful when using this, generally it should only be incremented
 * using addCount()
 */
void Record::count(int count) {
	_count=count;
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
 * Given a drink, evaluate if it is the fastest drink for this record and return back to the caller if so.
 * Also, save this drink as the new fastest time.
 * 
 * @param newDrink Drink the newly drank drink to be evaluated against the fastest drink.
 * @return bool if the new drink is the fastest drink, return true, else, return false.
 */
bool Record::evalAndUpdateFastestDrink(Drink& newDrink) {
	if (newDrink.startTime()-newDrink.endTime() < _fastestDrink.timeToFinish()) {
		_fastestDrink=newDrink;
		return true;	
	}
	return false;
}

/**
 * Return the completion duration of the fastest drink drank during this record's lifetime.
 * @return float The amount of time needed to completely finish the drink.
 * 
 */
float Record::fastestTime() { // return the completion duration of the fastest drink
	return _fastestDrink.timeToFinish();
} 			

/**
 * Return the fastest Drink drank during the course of this record.
 * @return Drink return the Drink that has the fastest time recorded
 */
Drink& Record::fastestDrink() { // Same as fastest time but returns the Drink record.
	return _fastestDrink;
} 		

/**
 * Get the start time that this record began tracking drink consumption. 
 * @TODO: Should this be a datetime?
 * @return int the integer value representing the start instant
 */
int Record::startTime() {
	return _startTime;
}

/**
 * Set the Record's start time to represent when it was initialized and began tracking drink consumption.
 * @TODO: Should this be a datetime?
 */
void Record::startTime(int startTime) {
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
