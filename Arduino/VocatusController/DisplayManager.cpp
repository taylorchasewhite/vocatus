/**
 * DisplayManager.cpp - Library for sending data to the Arduino's LCD screen.
 * Intended to abstract the controller from the different displays that we might ultimately use.
 * Created by Taylor White, Stephen Lago, May 15, 2018
 * See https://www.arduino.cc/en/Hacking/LibraryTutorial for information on class conventions here.
 */
#include "Arduino.h"
#include "DisplayManager.h"

// Constants
//@TODO: all of this

DisplayManager::DisplayManager(int pin)
{
  pinMode(pin, OUTPUT);
  _pin = pin;
}

/**
 * Default constructor, used to allocate space when defining members. 
 * Sets all values to null, 0, or other non-meaningful data.
 */
DisplayManager::DisplayManager() {
	
}

