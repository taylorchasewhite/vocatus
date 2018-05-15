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
  
  //com String delimiter
  String delim = ";";
  
  //UI variables
  int BLACK = #000000;
  int WHITE = #FFFFFF;
  int LIME_GREEN = #7FFF00;
  int YELLOW = #FFFF00;
  int PURPLE = #FF00FF;
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
  int rowHeight = 180;
  
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
  
  //Skins and skin variables
  DisplaySkin mySkin; 
  
  DisplaySkin altitude;
  DisplaySkin verdant;
  DisplaySkin deepBlue;
  DisplaySkin lavender;
  DisplaySkin lowLight;  
  
  static final String IMAGE_PATH="./ZeroStateImages/";
  static final String IMAGE_SUFFIX = "_Zero-State";
  static final String ICON_SUFFIX = "-Icon";
  static final String IMAGE_TYPE = ".png";

/****************************************************************/
/********************         Setup         *********************/
/****************************************************************/
  void setup () {
    // set the window size:
    size(1024, 768); //TODO:: window size to small; cutting off the "most recent" info
      
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
  }
  
  void drawStatusBoardTitle() {
    PFont pencilPete;
    pencilPete = createFont("pencilPete FONT",64);
    textFont(pencilPete,32);
    fill(mySkin.fontColor);
    text("Solo Cup Saver", 426.5, 248);
    textFont(pencilPete,12);
    //text("©", 475, 235); //TODO:: Remove this comment?
  }
  
  //TODO these outline colors don't work with all themes
  void drawBoxes() {
    drawLabelBox(lifetimeTotalBeerCountX,lifetimeTotalBeerCountY,"Lifetime:","drinks",WHITE);
    drawLabelBox(tonightTotalBeerCountX,tonightTotalBeerCountY,"Tonight:","drinks",WHITE);
    drawLabelBox(lifetimeFastestBeerTimeX,lifetimeFastestBeerTimeY,"All-Time Record:","ms",YELLOW);
    drawLabelBox(tonightFastestBeerTimeX,tonightFastestBeerTimeY,"Tonight's Record:","ms",YELLOW);
    drawLabelBox(mostRecentBeerTimeX,mostRecentBeerTimeY,"Last drink:","ms",YELLOW); 
    drawLabelBox(lifetimeTotalVolumeX,lifetimeTotalVolumeY,"All-time volume:","mL",PURPLE);
    drawLabelBox(tonightTotalVolumeX,tonightTotalVolumeY,"Tonight's volume:","mL",PURPLE);    
    drawLabelBox(mostRecentVolumeX,mostRecentVolumeY,"Last volume:","mL",PURPLE);  
  }


/****************************************************************/
/********************    Dynamic Draw       *********************/
/****************************************************************/
  void draw () {

    //draw the boxes/background each time //TODO:: we could still gain performance here if we wanted to by only overwriting the value portion of the display, but I think we're hitting diminishing returns at that point
    drawBoxes();
    
    //lifetime count
    lifetimeTotalBeerCount = convertToInt(inLifetimeTotalBeerCount);
    drawStandardInfoBox(lifetimeTotalBeerCountX,lifetimeTotalBeerCountY,lifetimeTotalBeerCount);
    
    //tonight count
    tonightTotalBeerCount = convertToInt(inTonightTotalBeerCount);
    drawStandardInfoBox(tonightTotalBeerCountX,tonightTotalBeerCountY,tonightTotalBeerCount);
    
    //lifetime record //TODO:: handle case where there is no record with an "N/A"
    lifetimeFastestBeerTime = convertToInt(inLifetimeFastestBeerTime);
    drawStandardInfoBox(lifetimeFastestBeerTimeX,lifetimeFastestBeerTimeY,lifetimeFastestBeerTime);
   
    //tonight record
    tonightFastestBeerTime = convertToInt(inTonightFastestBeerTime);
    drawStandardInfoBox(tonightFastestBeerTimeX,tonightFastestBeerTimeY,tonightFastestBeerTime);
    
    //most recent speed
    mostRecentBeerTime = convertToInt(inMostRecentBeerTime);
    drawStandardInfoBox(mostRecentBeerTimeX,mostRecentBeerTimeY,mostRecentBeerTime);
    
    //lifetime volume
    lifetimeTotalVolume = convertToFloat(inLifetimeTotalVolume);
    drawStandardInfoBox(lifetimeTotalVolumeX,lifetimeTotalVolumeY,lifetimeTotalVolume);
    
    //tonight volume
    tonightTotalVolume = convertToFloat(inTonightTotalVolume);
    drawStandardInfoBox(tonightTotalVolumeX,tonightTotalVolumeY,tonightTotalVolume);
    
    //most recent volume
    mostRecentVolume = convertToFloat(inMostRecentVolume);
    drawStandardInfoBox(mostRecentVolumeX,mostRecentVolumeY, mostRecentVolume);
  }
 
  void drawLabelBox(int xPos, int yPos, String labelText, String unitText, int borderColor) {
    fill(getChalkboardColor(),120);
    stroke(borderColor); 
    rect(xPos,yPos,boxWidth,boxHeight);
    
    //draw text
    textSize(45);
    fill(mySkin.fontColor);
    text(labelText,xPos+boxLabelX,yPos+boxLabelY);
    fill(WHITE); 
    text(unitText,xPos+boxUnitX,yPos+boxUnitY);
  }
  
  /*
   *  Draws a standard info box for an int value
   *  Assumes: black fill, white outline, green value, textSize 45 
   */
  void drawStandardInfoBox(int xPos, int yPos, int value) {

    fill(WHITE); 
    text(value,xPos+boxValueLineX,yPos+boxValueLineY);
  }
  
  /*
   *  Draws a standard info box for a float value //<>//
   *  Assumes: black fill, white outline, green value, textSize 45 
   */
  void drawStandardInfoBox(int xPos, int yPos, float value) {
    fill(WHITE);
    text(value,xPos+boxValueLineX,yPos+boxValueLineY);
  }
  
  //TODO:: Why is this here? Should this be a global variable with the other colors?
  color getChalkboardColor() {
    return color(47,79,79);
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
  
/****************************************************************/
/********************      Skin Class       *********************/
/****************************************************************/
  class DisplaySkin {
    String themeName;
    PImage backgroundImage;
    PImage foregroundImage;
    int backgroundColor;
    int fontColor; 
    
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
