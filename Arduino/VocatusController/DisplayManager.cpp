/**
 * DisplayManager.cpp - Library for sending data to the Arduino's LCD screen.
 * Intended to abstract the controller from the different displays that we might ultimately use.
 * Created by Taylor White, Stephen Lago, May 15, 2018
 * See https://www.arduino.cc/en/Hacking/LibraryTutorial for information on class conventions here.
 */
#include "Arduino.h"
#include "DisplayManager.h"

// Constants

DisplayManager::DisplayManager(int pin)
{
  _outputMode
}

<<<<<<< HEAD
/**
 * Default constructor, used to allocate space when defining members. 
 * Sets all values to null, 0, or other non-meaningful data.
 */
DisplayManager::DisplayManager() {
	
}

=======
void DisplayManager::dot()
{
  digitalWrite(_pin, HIGH);
  delay(250);
  digitalWrite(_pin, LOW);
  delay(250);  
}

void DisplayManager::dash()
{
  digitalWrite(_pin, HIGH);
  delay(1000);
  digitalWrite(_pin, LOW);
  delay(250);
}
>>>>>>> abf1569348b82bcf62b14234c744f7dc36a96d6d
