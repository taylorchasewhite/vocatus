import processing.serial.*;

  Serial myPort;        // The serial port
  boolean debugMode = true;  //set to true to see noisy output in the serial window
  
  int xPos = 1;         // current horizontal position of the graph
  float inByte = 0;
  
  int lifetimeTotalBeerCount = 0;
  int tonightTotalBeerCount = 0;
  
  int lifetimeFastestBeerTime = 0;
  int tonightFastestBeerTime = 0;
  int mostRecentBeerTime = 0;
  
  float lifetimeTotalVolume = 0.0;
  float tonightTotalVolume = 0.0; 
  float mostRecentVolume = 0.0; 


  void setup () {
    // set the window size:
    size(1200, 1400);
      
    println("Running...");
    
    // List all the available serial ports
    // if using Processing 2.1 or later, use Serial.printArray()
    if(debugMode) {
      println(Serial.list());
    }

    // I know that the last port in the serial list on my computer is always my
    // Arduino, so I open Serial.list()[2].
    // Open whatever port is the one you're using.
    myPort = new Serial(this, Serial.list()[2], 9600);

    // don't generate a serialEvent() unless you get a newline character:
    myPort.bufferUntil('\n');

    // set initial background:
    background(0);
  }
  
  //input container variables
  String inLifetimeTotalBeerCount;
  String inTonightTotalBeerCount;
  String inLifetimeFastestBeerTime;
  String inTonightFastestBeerTime;
  String inMostRecentBeerTime;
  String inLifetimeTotalVolume;
  String inTonightTotalVolume;
  String inMostRecentVolume;
  
  //com String delimiter
  String delim = ";";
  
  //UI variables
  int textSize = 45;
  
  int boxWidth = 420;
  int boxHeight = 150;
  int boxLabelX = 25;
  int boxLabelY = 50;
  int boxValueLineX = 25;
  int boxValueLineY = 120;
  int boxUnitX = 270;
  int boxUnitY = 120;

  int lifetimeTotalBeerCountX = 90;
  int lifetimeTotalBeerCountY = 100;

  int tonightTotalBeerCountX = lifetimeTotalBeerCountX;
  int tonightTotalBeerCountY = 300;
  
  int lifetimeFastestBeerTimeX = lifetimeTotalBeerCountX;
  int lifetimeFastestBeerTimeY = 500;
  
  int tonightFastestBeerTimeX = lifetimeTotalBeerCountX;
  int tonightFastestBeerTimeY = 700;
  
  int mostRecentBeerTimeX = lifetimeTotalBeerCountX;
  int mostRecentBeerTimeY = 700;
  
  int lifetimeTotalVolumeX = lifetimeTotalBeerCountX;
  int lifetimeTotalVolumeY = 900;
  
  int tonightTotalVolumeX = 600;
  int tonightTotalVolumeY = 100;
  
  int mostRecentVolumeX = 600;
  int mostRecentVolumeY = 300;

  void draw () {
    
    //lifetime count
    lifetimeTotalBeerCount = convertToInt(inLifetimeTotalBeerCount);
    drawStandardInfoBoxInt(lifetimeTotalBeerCountX,lifetimeTotalBeerCountY,"Lifetime:",lifetimeTotalBeerCount,"drinks");
    
    //tonight count
    tonightTotalBeerCount = convertToInt(inTonightTotalBeerCount);
    drawStandardInfoBoxInt(tonightTotalBeerCountX,tonightTotalBeerCountY,"Tonight:",tonightTotalBeerCount,"drinks");
    
    //lifetime record //TODO:: handle case where there is no record with an "N/A"
    lifetimeFastestBeerTime = convertToInt(inLifetimeFastestBeerTime);
    drawStandardInfoBoxInt(lifetimeFastestBeerTimeX,lifetimeFastestBeerTimeY,"All-Time Record:",lifetimeFastestBeerTime,"ms");
   
    //tonight record
    tonightFastestBeerTime = convertToInt(inTonightFastestBeerTime);
    drawStandardInfoBoxInt(tonightFastestBeerTimeX,tonightFastestBeerTimeY,"Tonight's Record:",tonightFastestBeerTime,"ms");
    
    //most recent speed
    mostRecentBeerTime = convertToInt(inMostRecentBeerTime);
    drawStandardInfoBoxInt(mostRecentBeerTimeX,mostRecentBeerTimeY,"Last drink:",mostRecentBeerTime,"ms");
    
    //lifetime volume
    //TODO
    
    //tonight volume
    //TODO
    
    //most recent volume
    //TODO
  }
  
    /*
   *  Draws a standard info box for an int value
   *  Assumes: black fill, white outline, green value, textSize 45 
   */
  void drawStandardInfoBoxInt(int xPos, int yPos, String labelText, int value, String unitText) {
    //draw box
    fill(0,0,0);
    textSize(45);
    stroke(255,255,255); //white
    rect(xPos,yPos,boxWidth,boxHeight);
    fill(255,255,255); //white
    text(labelText,xPos+boxLabelX,yPos+boxLabelY);
    fill(127,255,0); //lime green
    text(value,xPos+boxValueLineX,yPos+boxValueLineY);
    fill(255,255,255); //white
    text(unitText,xPos+boxUnitX,yPos+boxUnitY);
  }
  
  /*
   *  Draws a standard info box for a float value //<>//
   *  Assumes: black fill, white outline, green value, textSize 45 
   */
  void drawStandardInfoBoxFloat(int xPos, int yPos, String labelText, float value, String unitText) {
    //draw lastVolume
    fill(0,0,0);
    textSize(45);
    stroke(255,255,255); //white
    rect(xPos,yPos,boxWidth,boxHeight);
    fill(255,255,255); //white
    text(labelText,xPos+boxLabelX,yPos+boxLabelY);
    fill(127,255,0); //lime green
    text(value,xPos+boxValueLineX,yPos+boxValueLineY);
    fill(255,255,255); //white
    text(unitText,xPos+boxUnitX,yPos+boxUnitY);
  }

  /*
   * Converts the given string to an int
   */
  int convertToInt(String inValue) {
    int newValue = 0;
    
    if(inValue != null) {
      // trim off any whitespace:
      inValue = trim(inValue);
      // convert to an int
      newValue = int(inValue);
      //output debug message if enabled
      if(debugMode){ 
        print(newValue);
        print(delim);
        print(" ");
      }
    }
    
    return (newValue);
  }
  
  /*
   * Converts the given string to a float
   */
  float convertToFloat(String inValue) {
    float newValue = 0.0;
    
    if(inValue != null) {
      // trim off any whitespace:
      inValue = trim(inValue);
      // convert to a float
      newValue = float(inValue);
      //output debug message if enabled
      if(debugMode){ 
        print(newValue);
        print(delim);
        print(" ");
      }
    }
    
    return (newValue);
  }
  
  
  /*
   * Only print to the Serial Monitor if debug mode is enabled
   */
//TODO

  /*
   * Catch the com string and parse it into its component pieces
   */
  void serialEvent (Serial myPort) {
    // get the ASCII string:
    String inString = myPort.readStringUntil('\n');
    
    String[] inVars = split(inString,';');
    
    //assign variables
    inLifetimeTotalBeerCount = inVars[0];
    inTonightTotalBeerCount = inVars[1];
    inLifetimeFastestBeerTime = inVars[2];
    inTonightFastestBeerTime = inVars[3];
    inMostRecentBeerTime = inVars[4];
    inLifetimeTotalVolume = inVars[5];
    inTonightTotalVolume = inVars[6];
    inMostRecentVolume= inVars[7];
  }
