/**
 * DisplayManager.h - Library for sending data to the Arduino's LCD screen.
 * Intended to abstract the controller from the different displays that we might ultimately use.
 * Created by Taylor White, Stephen Lago, May 15, 2018
 * See https://www.arduino.cc/en/Hacking/LibraryTutorial for information on class conventions here.
 */
#ifndef DisplayManager_h
#define DisplayManager_h
#include "Arduino.h"

class DisplayManager
{
  public:
    DisplayManager(int pin);
    DisplayManager();
  private:
    int _pin;
};

enum DisplayMode {
  LIFECOUNT = 0, //explicitly set this guy to 0 to support cycling
  TONIGHTCOUNT,
  LIFESPEED,
  TONIGHTSPEED,
  LASTSPEED,
  ENDVALUE //this value should always be last to support cycling; add new values before this guy
}

#endif
