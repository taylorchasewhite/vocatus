import processing.serial.*;

/****************************************************************/
/********************        Globals        *********************/
/****************************************************************/

  Serial myPort;        // The serial port
  boolean debugMode = false;  //set to true to see noisy output in the serial window
  boolean deviceAttached = true; //set this to false when testing code without a device attached to the computer (skips any messing around with Serial stuff)
  
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
  
  //com String DELIMiter
  static String DELIM = ";";
  
  //UI variables  
  color CHALKBOARD =color(0,150,136);
  color CHALKBOARD_SHADOW =color(47,79,79);
  
  static color BLACK = #000000;
  static color WHITE = #FFFFFF;
  static color LIME_GREEN = #7FFF00;
  static color YELLOW = #FFFF00;
  static color PURPLE = #FF00FF;
  static color textSize = 45;
  
  static int BOX_WIDTH = 420;
  static int BOX_HEIGHT = 50;
  static int BOX_LABEL_X = 25;
  static int BOX_LABEL_Y = 50;
  static int BOX_VALUE_X = 25;
  static int BOX_VALUE_Y = 120;
  static int BOX_UNIT_X = 270;
  static int BOX_UNIT_Y = BOX_VALUE_Y;
  static int LABEL_OFFSET=80;

  static int col1 = 50;
  static int colWidth = 510;
  static int row1 = 50;
  static int rowHeight = 140;
  
  static int LIFETIME_TOTAL_BEER_COUNT_COL = 0;
  static int LIFETIME_TOTAL_BEER_COUNT_ROW = 0;
  int lifetimeTotalBeerCountX;
  int lifetimeTotalBeerCountY;

  static int TONIGHT_TOTAL_BEER_COUNT_COL = 1;
  static int TONIGHT_TOTAL_BEER_COUNT_ROW = 0;
  int tonightTotalBeerCountX;
  int tonightTotalBeerCountY;
  
  static int LIFETIME_FASTEST_BEER_TIME_COL = 0;
  static int LIFETIME_FASTEST_BEER_TIME_ROW = 2;
  int lifetimeFastestBeerTimeX;
  int lifetimeFastestBeerTimeY;

  static int TONIGHT_FASTEST_BEER_TIME_COL = 0;
  static int TONIGHT_FASTEST_BEER_TIME_ROW = 3;
  int tonightFastestBeerTimeX;
  int tonightFastestBeerTimeY;
  
  static int MOST_RECENT_BEER_TIME_COL = 0;
  static int MOST_RECENT_BEER_TIME_ROW = 4;
  int mostRecentBeerTimeX;
  int mostRecentBeerTimeY;
  
  static int LIFETIME_TOTAL_VOLUME_COL = 1;
  static int LIFETIME_TOTAL_VOLUME_ROW = 2;
  int lifetimeTotalVolumeX;
  int lifetimeTotalVolumeY;
  
  static int TONIGHT_TOTAL_VOLUME_COL = 1;
  static int TONIGHT_TOTAL_VOLUME_ROW = 3;
  int tonightTotalVolumeX;
  int tonightTotalVolumeY;
  
  static int MOST_RECENT_VOLUME_COL = 1;
  static int MOST_RECENT_VOLUME_ROW = 4;
  int mostRecentVolumeX;
  int mostRecentVolumeY;
  
  //Skins and skin variables
  DisplaySkin mySkin; 
  
  DisplaySkin altitude;
  DisplaySkin verdant;
  DisplaySkin deepBlue;
  DisplaySkin lavender;
  DisplaySkin lowLight;  
  

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


    if(deviceAttached) {
      // I know that the last port in the serial list on my computer is always my
      // Arduino, so I open Serial.list()[2].
      // Open whatever port is the one you're using.
      myPort = new Serial(this, Serial.list()[2], 9600);
  
      // don't generate a serialEvent() unless you get a newline character:
      myPort.bufferUntil('\n');
    }

    //setup values
    initPosValues();
    initSkins();
    assignSkin();
    
    // Background Markup; draw this once for performance benefit
    drawBackgroundMarkup();
  }
  
  
  //Use a col/row system to init the x/y values, where the cols and rows are 0-indexed
  void initPosValues() {
    lifetimeTotalBeerCountX = colToX(LIFETIME_TOTAL_BEER_COUNT_COL);
    lifetimeTotalBeerCountY = rowToY(LIFETIME_TOTAL_BEER_COUNT_ROW);
    
    tonightTotalBeerCountX = colToX(TONIGHT_TOTAL_BEER_COUNT_COL);
    tonightTotalBeerCountY = rowToY(TONIGHT_TOTAL_BEER_COUNT_ROW);
    
    lifetimeFastestBeerTimeX = colToX(LIFETIME_FASTEST_BEER_TIME_COL);
    lifetimeFastestBeerTimeY = rowToY(LIFETIME_FASTEST_BEER_TIME_ROW);
    
    tonightFastestBeerTimeX = colToX(TONIGHT_FASTEST_BEER_TIME_COL);
    tonightFastestBeerTimeY = rowToY(TONIGHT_FASTEST_BEER_TIME_ROW);
    
    mostRecentBeerTimeX = colToX(MOST_RECENT_BEER_TIME_COL);
    mostRecentBeerTimeY = rowToY(MOST_RECENT_BEER_TIME_ROW);
    
    lifetimeTotalVolumeX = colToX(LIFETIME_TOTAL_VOLUME_COL);
    lifetimeTotalVolumeY = rowToY(LIFETIME_TOTAL_VOLUME_ROW);
    
    tonightTotalVolumeX = colToX(TONIGHT_TOTAL_VOLUME_COL);
    tonightTotalVolumeY = rowToY(TONIGHT_TOTAL_VOLUME_ROW);
    
    mostRecentVolumeX = colToX(MOST_RECENT_VOLUME_COL);
    mostRecentVolumeY = rowToY(MOST_RECENT_VOLUME_ROW);
    
  }
  
  //translate a col value to a corresponding x value
  int colToX(int colVal) {
    return(col1 + (colVal * colWidth)); 
  }
  
  //translate a row value to a corresponding y value
  int rowToY(int rowVal) {
    return(row1 + (rowVal * rowHeight)); 
  }
  
  void initSkins() {
    altitude = new DisplaySkin("Altitude",  color(238,247,255), color(0, 145, 234));
    deepBlue = new DisplaySkin("Deep-Blue", color(240,242,244), color(0, 57, 107));
    lavender = new DisplaySkin("Lavender",  color(244,244,251), color(156, 39, 176));
    verdant  = new DisplaySkin("Verdant",   color(242,250,248), color(14, 150, 152));
    lowLight = new DisplaySkin("Low-Light", color(211,213,218), color(0, 57, 107));
  }

  void assignSkin() {
    int random = floor(random(5)); // Cast to an int
    switch(random) {
      case 0:
        mySkin = altitude;
        break;
      case 1:
        mySkin = deepBlue;
        break;
      case 2:
        mySkin = lavender;
        break;
      case 3:
        mySkin = verdant;
        break;
      case 4:
        mySkin = lowLight;
        break;
    }
  }
  
  void drawBackgroundMarkup() {
    background(mySkin.backgroundColor);
    image(mySkin.backgroundImage,0,0,1024,768);
    image(mySkin.foregroundImage,426.5,238.5); // 298.5 is halfway down, icon is 171x171
    drawStatusBoardTitle();
    drawLabels();
  }
  
  void drawStatusBoardTitle() {
    PFont pencilPete;
    pencilPete = createFont("pencilPete FONT",64);
    textFont(pencilPete,32);
    fill(mySkin.fontColor);
    text("Solo Cup Saver", 426.5, 248);
    textFont(pencilPete,12);
  }
  
  void drawLabels() {
    drawLabel(lifetimeTotalBeerCountX,lifetimeTotalBeerCountY,"Lifetime:");
    drawLabel(tonightTotalBeerCountX,tonightTotalBeerCountY,"Tonight:");
    drawLabel(lifetimeFastestBeerTimeX,lifetimeFastestBeerTimeY,"All-Time Record:");
    drawLabel(tonightFastestBeerTimeX,tonightFastestBeerTimeY,"Tonight's Record:");
    drawLabel(mostRecentBeerTimeX,mostRecentBeerTimeY,"Last drink:"); 
    drawLabel(lifetimeTotalVolumeX,lifetimeTotalVolumeY,"All-time volume:");
    drawLabel(tonightTotalVolumeX,tonightTotalVolumeY,"Tonight's volume:");    
    drawLabel(mostRecentVolumeX,mostRecentVolumeY,"Last volume:");      
  }
  
  void updateValues() {
    //lifetime count
    lifetimeTotalBeerCount = convertToInt(inLifetimeTotalBeerCount);
    
    //tonight count
    tonightTotalBeerCount = convertToInt(inTonightTotalBeerCount);
    
    //lifetime record
    lifetimeFastestBeerTime = convertToInt(inLifetimeFastestBeerTime);
   
    //tonight record
    tonightFastestBeerTime = convertToInt(inTonightFastestBeerTime);
    
    //most recent speed
    mostRecentBeerTime = convertToInt(inMostRecentBeerTime);
    
    //lifetime volume
    lifetimeTotalVolume = convertToFloat(inLifetimeTotalVolume);
    
    //tonight volume
    tonightTotalVolume = convertToFloat(inTonightTotalVolume);
    
    //most recent volume
    mostRecentVolume = convertToFloat(inMostRecentVolume);
  }

/****************************************************************/
/********************    Dynamic Draw       *********************/
/****************************************************************/
  void draw () {
    updateValues();
    drawBoxes();
  }
  
  void drawBoxes() {
    drawValueBox(lifetimeTotalBeerCountX,lifetimeTotalBeerCountY,"drinks",lifetimeTotalBeerCount,WHITE,false);
    drawValueBox(tonightTotalBeerCountX,tonightTotalBeerCountY,"drinks",tonightTotalBeerCount,WHITE,false);
    drawValueBox(lifetimeFastestBeerTimeX,lifetimeFastestBeerTimeY,"ms",lifetimeFastestBeerTime,CHALKBOARD_SHADOW,true);
    drawValueBox(tonightFastestBeerTimeX,tonightFastestBeerTimeY,"ms",tonightFastestBeerTime,CHALKBOARD_SHADOW,true);
    drawValueBox(mostRecentBeerTimeX,mostRecentBeerTimeY,"ms",mostRecentBeerTime,CHALKBOARD_SHADOW,true); 
    drawValueBox(lifetimeTotalVolumeX,lifetimeTotalVolumeY,"mL",lifetimeTotalVolume,CHALKBOARD_SHADOW,false);
    drawValueBox(tonightTotalVolumeX,tonightTotalVolumeY,"mL",tonightTotalVolume,CHALKBOARD_SHADOW,false);    
    drawValueBox(mostRecentVolumeX,mostRecentVolumeY,"mL",mostRecentVolume,CHALKBOARD_SHADOW,false);  
  }
  
  void drawLabel(int xPos, int yPos, String labelText) {
    //draw text
    textSize(45);
    fill(mySkin.fontColor);
    text(labelText,xPos+BOX_LABEL_X,yPos+BOX_LABEL_Y);
  }
  
  /*
   *  Draws a value box for an int value
   *  Assumes: white text, textSize 45 
   */
  void drawValueBox(int xPos, int yPos, String unitText, int value, color borderColor, boolean mustBeNonZero) {
    drawBoxBackdrop(xPos,yPos,borderColor);
    
    if (mustBeNonZero) {      
      text(getFormattedRecordValue(value),xPos+BOX_VALUE_X,yPos+BOX_VALUE_Y);
      if (value > 0) {
        text(unitText,xPos+BOX_UNIT_X,yPos+BOX_UNIT_Y);
      }
    }
    else {
      text(value,xPos+BOX_VALUE_X,yPos+BOX_VALUE_Y);
      text(unitText,xPos+BOX_UNIT_X,yPos+BOX_UNIT_Y);
    }
  }
  
  /*
   *  Draws a value box for an float value
   *  Assumes: white text, textSize 45 
   */
  void drawValueBox(int xPos, int yPos, String unitText, float value, color borderColor, boolean mustBeNonZero) {
    drawBoxBackdrop(xPos,yPos,borderColor);
    
    if (mustBeNonZero) {
      text(getFormattedRecordValue(value),xPos+BOX_VALUE_X,yPos+BOX_VALUE_Y);
      if (value >0) {
        text(unitText,xPos+BOX_UNIT_X,yPos+BOX_UNIT_Y);
      }
    }
    else {
      text(value,xPos+BOX_VALUE_X,yPos+BOX_VALUE_Y);
      text(unitText,xPos+BOX_UNIT_X,yPos+BOX_UNIT_Y);
    }
  }
  
  /*
   * Helper function, draws the backdrop for the value box.
   * Assumes that this is called right before drawing the values, as it sets up the text size and color.
   */
  void drawBoxBackdrop(int xPos, int yPos, color borderColor) {
    fill(CHALKBOARD);
    strokeWeight(6);
    stroke(borderColor,5); 
    rect(xPos,yPos+LABEL_OFFSET,BOX_WIDTH,BOX_HEIGHT);  
    
    //draw text
    textSize(45);
    fill(WHITE); 
    
  } //<>//

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
      debugPrint(DELIM);
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
      debugPrint(DELIM);
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
  
/****************************************************************/
/********************      Skin Class       *********************/
/****************************************************************/
  class DisplaySkin {
    String themeName;
    PImage backgroundImage;
    PImage foregroundImage;
    int backgroundColor;
    int fontColor; 
    
    static final String IMAGE_PATH="./ZeroStateImages/";
    static final String IMAGE_SUFFIX = "_Zero-State";
    static final String ICON_SUFFIX = "-Icon";
    static final String IMAGE_TYPE = ".png";
   
    /*
     * Constructor for DisplaySkin
     * Image urls are created automatically based off of the theme name 
     * 
     * @param inThemeName: the name of the theme
     * @param inBackgroundColor: the background color
     * @param inFontColor: the font color
     */
    DisplaySkin(String inThemeName, int inBackgroundColor, int inFontColor) {
      themeName = inThemeName;
      String bgURL = IMAGE_PATH+themeName+IMAGE_SUFFIX+IMAGE_TYPE;
      backgroundImage = loadImage(bgURL);
      String fgURL = IMAGE_PATH+themeName+IMAGE_SUFFIX+ICON_SUFFIX+IMAGE_TYPE;
      foregroundImage = loadImage(fgURL);
      backgroundColor = inBackgroundColor;
      fontColor = inFontColor;
    }
  }
  
  /****************************************************************/
  /*********************      Utility       ***********************/
  /****************************************************************/
   /*
    * Provide a nicely formatted display value for the records stored in the arduino.
    * Any integer passed in with a value of 0 or less is interpreted as no stored record.
    * 
    * @param value: the record value we may want to format (if a reserved/special value)
    * @return: Returns a formatted record value
    */  
  String getFormattedRecordValue(int value) {
    if (value <=0) {
      return "No record on file!";
    }
    
    return str(value); 
}

   /*
    * Provide a nicely formatted display value for the records stored in the arduino.
    * Any integer passed in with a value of 0 or less is interpreted as no stored record.
    * 
    * @param value: the record value we may want to format (if a reserved/special value)
    * @return: Returns a formatted record value
    */  
  String getFormattedRecordValue(float value) {
    if (value <=0) {
      return "No record on file!";
    }
    
    return str(value); 
}
