import processing.serial.*;

/****************************************************************/
/********************        Globals        *********************/
/****************************************************************/

  Serial myPort;        // The serial port
  boolean debugMode = false;  //set to true to see noisy output in the serial window
  
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

  int col1 = 90;
  int colWidth = 510;
  int row1 = 100;
  int rowHeight = 200;
  
  int lifeTimeTotalBeerCountCol = 0;
  int lifetimeTotalBeerCountRow = 0;
  int lifetimeTotalBeerCountX;
  int lifetimeTotalBeerCountY;

  int tonightTotalBeerCountCol = 1;
  int tonightTotalBeerCountRow = 0;
  int tonightTotalBeerCountX;
  int tonightTotalBeerCountY;
  
  int lifetimeFastestBeerTimeCol = 0;
  int lifetimeFastestBeerTimeRow = 2;
  int lifetimeFastestBeerTimeX;
  int lifetimeFastestBeerTimeY;

  int tonightFastestBeerTimeCol = 0;
  int tonightFastestBeerTimeRow = 3;
  int tonightFastestBeerTimeX;
  int tonightFastestBeerTimeY;
  
  int mostRecentBeerTimeCol = 0;
  int mostRecentBeerTimeRow = 4;
  int mostRecentBeerTimeX;
  int mostRecentBeerTimeY;
  
  int lifetimeTotalVolumeCol = 1;
  int lifetimeTotalVolumeRow = 2;
  int lifetimeTotalVolumeX;
  int lifetimeTotalVolumeY;
  
  int tonightTotalVolumeCol = 1;
  int tonightTotalVolumeRow = 3;
  int tonightTotalVolumeX;
  int tonightTotalVolumeY;
  
  int mostRecentVolumeCol = 1;
  int mostRecentVolumeRow = 4;
  int mostRecentVolumeX;
  int mostRecentVolumeY;

/****************************************************************/
/********************         Setup         *********************/
/****************************************************************/
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

    //setup values
    initPosValues();

    // set initial background:
    background(0);
  }
  
  
  //Use a col/row system to init the x/y values, where the cols and rows are 0-indexed
  void initPosValues() {
    lifetimeTotalBeerCountX = colToX(lifeTimeTotalBeerCountCol);
    lifetimeTotalBeerCountY = rowToY(lifetimeTotalBeerCountRow);
    
    tonightTotalBeerCountX = colToX(tonightTotalBeerCountCol);
    tonightTotalBeerCountY = rowToY(tonightTotalBeerCountRow);
    
    lifetimeFastestBeerTimeX = colToX(lifetimeFastestBeerTimeCol);
    lifetimeFastestBeerTimeY = rowToY(lifetimeFastestBeerTimeRow);
    
    tonightFastestBeerTimeX = colToX(tonightFastestBeerTimeCol);
    tonightFastestBeerTimeY = rowToY(tonightFastestBeerTimeRow);
    
    mostRecentBeerTimeX = colToX(mostRecentBeerTimeCol);
    mostRecentBeerTimeY = rowToY(mostRecentBeerTimeRow);
    
    lifetimeTotalVolumeX = colToX(lifetimeTotalVolumeCol);
    lifetimeTotalVolumeY = rowToY(lifetimeTotalVolumeRow);
    
    tonightTotalVolumeX = colToX(tonightTotalVolumeCol);
    tonightTotalVolumeY = rowToY(tonightTotalVolumeRow);
    
    mostRecentVolumeX = colToX(mostRecentVolumeCol);
    mostRecentVolumeY = rowToY(mostRecentVolumeRow);
  }
  
  //trandslate a row value to a corresponding x value
  int colToX(int colVal) {
    return(col1 + (colVal * colWidth)); 
  }
  
  //trandslate a row value to a corresponding x value
  int rowToY(int rowVal) {
    return(row1 + (rowVal * rowHeight)); 
  }


/****************************************************************/
/********************         Draw          *********************/
/****************************************************************/
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
    lifetimeTotalVolume = convertToFloat(inLifetimeTotalVolume);
    drawStandardInfoBoxFloat(lifetimeTotalVolumeX,lifetimeTotalVolumeY,"All-time volume:",lifetimeTotalVolume,"mL");
    
    //tonight volume
    tonightTotalVolume = convertToFloat(inTonightTotalVolume);
    drawStandardInfoBoxFloat(tonightTotalVolumeX,tonightTotalVolumeY,"Tonight's volume:",tonightTotalVolume,"mL");
    
    //most recent volume
    mostRecentVolume = convertToFloat(inMostRecentVolume);
    drawStandardInfoBoxFloat(mostRecentVolumeX,mostRecentVolumeY,"Last volume:",mostRecentVolume,"mL");
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
  
  
/****************************************************************/
/********************     Input/Output      *********************/
/****************************************************************/
  
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
