/**
 * ControllerConstants.h - The sole purpose of this class is to declare constants shared
 * across several classes. To use, simply include this header file in your consuming implementation classes.
 * 
 * Created by Taylor White, Stephen Lago, July 24, 2018
 * See https://www.arduino.cc/en/Hacking/LibraryTutorial for information on class conventions here.
 */
#ifndef ControllerConstants_h
#define ControllerConstants_h

/****************************************************************/
/*******************        Constants        ********************/
/****************************************************************/
// Arduino constants 
extern const int BITS_PER_SECOND; // initialize serial communication at 9600 bits per second:

extern const int PIN_FLOW;
extern const int PIN_RESET;
extern const int PIN_CYCLE_DISPLAY;

extern unsigned int MS_BETWEEN_DRINKS;
extern const unsigned long DEBOUNCE_BUFFER; //time to wait for a "bounce", e.g. machine error reading a single press as multiple
extern const unsigned long SECONDS_IN_DAY;

#endif

