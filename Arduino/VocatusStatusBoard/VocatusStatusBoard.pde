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
  int black = #000000;
  int white = #FFFFFF;
  int limeGreen = #7FFF00;
  int yellow = #FFFF00;
  int purple = #FF00FF;
  int textSize = 45;
  
  int boxWidth = 420;
  int boxHeight = 150;
  int boxLabelX = 25;
  int boxLabelY = 50;
  int boxValueLineX = 25;
  int boxValueLineY = 120;
  int boxUnitX = 270;
  int boxUnitY = 120;

  int col1 = 50;
  int colWidth = 510;
  int row1 = 50;
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
    size(1024, 768);
      
    println("Running...");
    
    // List all the available serial ports
    // if using Processing 2.1 or later, use Serial.debugPrintArray()
    debugPrintln(Serial.list());

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
  
  //trandslate a col value to a corresponding x value
  int colToX(int colVal) {
    return(col1 + (colVal * colWidth)); 
  }
  
  //trandslate a row value to a corresponding y value
  int rowToY(int rowVal) {
    return(row1 + (rowVal * rowHeight)); 
  }


/****************************************************************/
/********************         Draw          *********************/
/****************************************************************/
  void draw () {
    
    //lifetime count
    lifetimeTotalBeerCount = convertToInt(inLifetimeTotalBeerCount);
    drawStandardInfoBox(lifetimeTotalBeerCountX,lifetimeTotalBeerCountY,"Lifetime:",lifetimeTotalBeerCount,"drinks",white);
    
    //tonight count
    tonightTotalBeerCount = convertToInt(inTonightTotalBeerCount);
    drawStandardInfoBox(tonightTotalBeerCountX,tonightTotalBeerCountY,"Tonight:",tonightTotalBeerCount,"drinks",white);
    
    //lifetime record //TODO:: handle case where there is no record with an "N/A"
    lifetimeFastestBeerTime = convertToInt(inLifetimeFastestBeerTime);
    drawStandardInfoBox(lifetimeFastestBeerTimeX,lifetimeFastestBeerTimeY,"All-Time Record:",lifetimeFastestBeerTime,"ms",yellow);
   
    //tonight record
    tonightFastestBeerTime = convertToInt(inTonightFastestBeerTime);
    drawStandardInfoBox(tonightFastestBeerTimeX,tonightFastestBeerTimeY,"Tonight's Record:",tonightFastestBeerTime,"ms",yellow);
    
    //most recent speed
    mostRecentBeerTime = convertToInt(inMostRecentBeerTime);
    drawStandardInfoBox(mostRecentBeerTimeX,mostRecentBeerTimeY,"Last drink:",mostRecentBeerTime,"ms",yellow);
    
    //lifetime volume
    lifetimeTotalVolume = convertToFloat(inLifetimeTotalVolume);
    drawStandardInfoBox(lifetimeTotalVolumeX,lifetimeTotalVolumeY,"All-time volume:",lifetimeTotalVolume,"mL",purple);
    
    //tonight volume
    tonightTotalVolume = convertToFloat(inTonightTotalVolume);
    drawStandardInfoBox(tonightTotalVolumeX,tonightTotalVolumeY,"Tonight's volume:",tonightTotalVolume,"mL",purple);
    
    //most recent volume
    mostRecentVolume = convertToFloat(inMostRecentVolume);
    drawStandardInfoBox(mostRecentVolumeX,mostRecentVolumeY,"Last volume:",mostRecentVolume,"mL",purple);
  }
  
    /*
   *  Draws a standard info box for an int value
   *  Assumes: black fill, white outline, green value, textSize 45 
   */
  void drawStandardInfoBox(int xPos, int yPos, String labelText, int value, String unitText, int borderColor) {
    //draw box
    fill(black);
    textSize(45);
    stroke(borderColor); 
    rect(xPos,yPos,boxWidth,boxHeight);
    //draw text
    fill(white); 
    text(labelText,xPos+boxLabelX,yPos+boxLabelY);
    fill(limeGreen); 
    text(value,xPos+boxValueLineX,yPos+boxValueLineY);
    fill(white); 
    text(unitText,xPos+boxUnitX,yPos+boxUnitY);
  }
  
  /*
   *  Draws a standard info box for a float value //<>//
   *  Assumes: black fill, white outline, green value, textSize 45 
   */
  void drawStandardInfoBox(int xPos, int yPos, String labelText, float value, String unitText, int borderColor) {
    //draw lastVolume
    fill(black);
    textSize(45);
    stroke(borderColor); 
    rect(xPos,yPos,boxWidth,boxHeight);
    //draw text
    fill(white); 
    text(labelText,xPos+boxLabelX,yPos+boxLabelY);
    fill(limeGreen);
    text(value,xPos+boxValueLineX,yPos+boxValueLineY);
    fill(white);
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
      debugPrint(newValue);
      debugPrint(delim);
      debugPrint(" ");
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
      debugPrint(newValue);
      debugPrint(delim);
      debugPrint(" ");
    }
    
    return (newValue);
  }
  
  
/****************************************************************/
/********************     Input/Output      *********************/
/****************************************************************/
  
  /*
   * Only debugPrint to the Serial Monitor if debug mode is enabled
   */
   boolean shouldPrint() {
     return(debugMode);
   }
   
   /*
   * Method used to print debug messages
   * First checks whether it is appropriate to send text to the serial monitor
   */
   void debugPrint(String debugVal) { if(shouldPrint()) { print(debugVal); }}
   void debugPrintln(String debugVal) { if(shouldPrint()) { println(debugVal); }}
   void debugPrint(String[] debugVal) { if(shouldPrint()) { print(debugVal); }}
   void debugPrintln(String[] debugVal) { if(shouldPrint()) { println(debugVal); }}
   void debugPrint(int debugVal) { if(shouldPrint()) { print(debugVal); }}
   void debugPrintln(int debugVal) { if(shouldPrint()) { println(debugVal); }}
   void debugPrint(long debugVal) { if(shouldPrint()) { print(debugVal); }}
   void debugPrintln(long debugVal) { if(shouldPrint()) { println(debugVal); }}
   void debugPrint(float debugVal) { if(shouldPrint()) { print(debugVal); }}
   void debugPrintln(float debugVal) { if(shouldPrint()) { println(debugVal); }}
   void debugPrint(double debugVal) { if(shouldPrint()) { print(debugVal); }}
   void debugPrintln(double debugVal) { if(shouldPrint()) { println(debugVal); }}

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
