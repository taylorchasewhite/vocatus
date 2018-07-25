/**
 * ControllerConstants.cpp - The sole purpose of this implementation is to initialize constants shared
 * across several classes. To use, simply include the declaration file of this .cpp in your consuming implementation classes.
 * 
 * Created by Taylor White, Stephen Lago, July 24, 2018
 * See https://www.arduino.cc/en/Hacking/LibraryTutorial for information on class conventions here.
 */
#include "ControllerConstants.h"

/****************************************************************/
/*******************        Constants        ********************/
/****************************************************************/
// Arduino constants 
const int BITS_PER_SECOND = 9600; // initialize serial communication at 9600 bits per second:

const int PIN_FLOW = 2;
const int PIN_RESET = 3;
const int PIN_CYCLE_DISPLAY = 4;

unsigned int MS_BETWEEN_DRINKS = 1000;
const unsigned long DEBOUNCE_BUFFER = 50; //time to wait for a "bounce", e.g. machine error reading a single press as multiple
const unsigned long SECONDS_IN_DAY = 86400;