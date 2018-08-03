
/**
 * VocatusManager.h - The object to manage the state of the program
 * Intended to abstract the controller from as much as possible
 * 
 * Created by Taylor White, Stephen Lago, July 17, 2018
 * See https://www.arduino.cc/en/Hacking/LibraryTutorial for information on class conventions here.
 */
#ifndef VocatusManager_h
#define VocatusManager_h
#include "Record.h"
#include "ControllerConstants.h"
#include "DisplayManager.h"
#include "StorageManager.h"
#include "TimeManager.h"

/****************************************************************/
/********************        Globals        *********************/
/****************************************************************/

class VocatusManager
{
  public:
	
	VocatusManager();

	// Setup
	void initGlobals();

	// Interrupts & Button Presses
	void Flow();
	void displayChangeListener();
	bool checkDebounce(int , int& , int , unsigned long& );

	// Timing
	unsigned long getCurrentTime();
	void startDrink();
	void drinkPulse();
	void setDrinkCompletionDuration();
	boolean isNewDay();

	// Statistics and State Change
	void recordDrinkEnd();
	void resetListener();
	void reset();
	void resetCurrentDrink();
	void resetTonightRecord();
	void resetLifetimeRecord();
	
	// Storage
	void readFromStorage();
	void storeAllValues();
	
	//
	boolean isDrinkOver();
	void printStatusReport();
	
	// Timing
	Record lifetime;
	Record tonight;

	// I/O Controls
	DisplayManager display;
	StorageManager storage;
	TimeManager timeManager;

	// Counts
	volatile int flowCount; //@NOTE:: I don't think this needs to be volatile. It's only needed if the variable could be changed from outside of the code (e.g another class)
	float multiplier;
	float mostRecentVolume;

	unsigned long endTime;
	unsigned long startTime;
	int mostRecentDrinkTime;  // The time it took in ms to finish the last drink

	unsigned long lastDrinkCompletionInstant; //@NOTE:: why are all of these globals?
	unsigned long currentDrinkCompletionInstant;


	unsigned long resetLastDebounce;
	unsigned long displayLastDebounce;

	//states
	int resetLastState;
	int resetCurState;
	int displayLastState;
	int displayCurState;

  private:

};

void outputFreeMemory();
int freeMemory();

#endif
