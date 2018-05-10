import processing.serial.*;

  Serial myPort;        // The serial port
  boolean debugMode = true;  //set to true to see noisy output in the serial window
  
  int xPos = 1;         // current horizontal position of the graph
  float inByte = 0;
  int lifeCount = 0;
  int lifeRecord = 0;
  int curCount = 0;
  int curRecord = 0;
  int lastSpeed = 0;
  int sessionInProgress = 0;
  int lastVolume = 0; 

  void setup () {
    // set the window size:
    size(1200, 1400);
      
    println("Running...");
    
    // List all the available serial ports
    // if using Processing 2.1 or later, use Serial.printArray()
    if(debugMode) {
      println(Serial.list());
    }

    // I know that the first port in the serial list on my Mac is always my
    // Arduino, so I open Serial.list()[0].
    // Open whatever port is the one you're using.
    myPort = new Serial(this, Serial.list()[2], 19200);

    // don't generate a serialEvent() unless you get a newline character:
    myPort.bufferUntil('\n');

    // set initial background:
    background(0);
  }
  
  //input container variables
  String inLifeCount;
  String inLifeRecord;
  String inCurCount;
  String inCurRecord;
  String inLastSpeed;
  String inGraph;
  String inSessionInProgress;
  String inLastVolume; 
  
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

  int lifeCountX = 90;
  int lifeCountY = 100;

  int lifeRecordX = lifeCountX;
  int lifeRecordY = 300;
  
  int curCountX = lifeCountX;
  int curCountY = 500;
  
  int curRecordX = lifeCountX;
  int curRecordY = 700;
  
  int lastSpeedX = lifeCountX;
  int lastSpeedY = 900;
  
  int graphWidth = 1200-180;
  int graphHeight = boxHeight;
  int graphX = 100;
  int graphY = 1100;
  
  int sessionInProgressX = 600;
  int sessionInProgressY = 100;
  int statusX = 75;
  
  int lastVolumeX = 600;
  int lastVolumeY = 300;

  void draw () {
    
    //handle lifeCount
    if (inLifeCount != null) {
      // trim off any whitespace:
      inLifeCount = trim(inLifeCount);
      // convert to an int and map to the screen height:
      lifeCount = int(inLifeCount);
      if(debugMode){ 
        print(lifeCount);
        print("; ");
      }
    }
    
    //handle LifeRecord
    if (inLifeRecord != null) {
      // trim off any whitespace:
      inLifeRecord = trim(inLifeRecord);
      // convert to an int and map to the screen height:
      lifeRecord = int(inLifeRecord);
      if(debugMode){
        print(lifeRecord);
        print("; ");
      }
    }

    //handle curCount
    if (inCurCount != null) {
      // trim off any whitespace:
      inCurCount = trim(inCurCount);
      // convert to an int and map to the screen height:
      curCount = int(inCurCount);
      if(debugMode){
        print(curCount);
        print("; ");
      }
    }
    
    //handle curRecord
    if (inCurRecord != null) {
      // trim off any whitespace:
      inCurRecord = trim(inCurRecord);
      // convert to an int and map to the screen height:
      curRecord = int(inCurRecord);
      if(debugMode){
        print(curRecord);
        print("; ");
      }
    }

    //handle lastSpeed
    if (inLastSpeed != null) {
      // trim off any whitespace:
      inLastSpeed = trim(inLastSpeed);
      // convert to an int and map to the screen height:
      lastSpeed = int(inLastSpeed);
      if(debugMode){
        print(lastSpeed);
        print("; ");
      }
    }
    
    //handle sessionInProgress
    if (inSessionInProgress != null) {
      // trim off any whitespace:
      inSessionInProgress = trim(inSessionInProgress);
      // convert to an int and map to the screen height:
      sessionInProgress = int(inSessionInProgress);
      if(debugMode){
        print(sessionInProgress);
        print("; ");
      }
    }
    
    //handle lastVolume
    if (inLastVolume != null) {
      // trim off any whitespace:
      inLastVolume = trim(inLastVolume);
      // convert to an int and map to the screen height:
      lastVolume = int(inLastVolume);
      if(debugMode){
        print(lastVolume);
        print("; ");
      }
    }
    if(debugMode){println();}
    
    //draw lifeCount box
    fill(0,0,0);
    textSize(45);
    stroke(255,255,255); //white
    rect(lifeCountX,lifeCountY,boxWidth,boxHeight);
    fill(255,255,255); //white
    text("Lifetime:",lifeCountX+boxLabelX,lifeCountY+boxLabelY);
    fill(127,255,0); //lime green
    text(lifeCount,lifeCountX+boxValueLineX,lifeCountY+boxValueLineY);
    fill(255,255,255); //white
    text("drinks",lifeCountX+boxUnitX,lifeCountY+boxUnitY);
    
    //draw lifeRecord box
    fill(0,0,0);
    textSize(45);
    stroke(255,255,255); //white
    rect(lifeRecordX,lifeRecordY,boxWidth,boxHeight);
    fill(255,255,255); //white
    text("All-Time Record:",lifeRecordX+boxLabelX,lifeRecordY+boxLabelY);
    fill(127,255,0); //lime green
    if(lifeRecord < 0) {
      text("N/A",lifeRecordX+boxValueLineX,lifeRecordY+boxValueLineY);
    }else {
      text(lifeRecord,lifeRecordX+boxValueLineX,lifeRecordY+boxValueLineY);
    }
    fill(255,255,255); //white
    text("ms",lifeRecordX+boxUnitX,lifeRecordY+boxUnitY);
    
    //draw curCount box
    fill(0,0,0);
    textSize(45);
    stroke(255,255,255); //white
    rect(curCountX,curCountY,boxWidth,boxHeight);
    fill(255,255,255); //white
    text("Tonight:",curCountX+boxLabelX,curCountY+boxLabelY);
    fill(127,255,0); //lime green
    text(curCount,curCountX+boxValueLineX,curCountY+boxValueLineY);
    fill(255,255,255); //white
    text("drinks",curCountX+boxUnitX,curCountY+boxUnitY);
    
    //draw curRecord box
    fill(0,0,0);
    textSize(45);
    stroke(255,255,255); //white
    rect(curRecordX,curRecordY,boxWidth,boxHeight);
    fill(255,255,255); //white
    text("Tonight's Record:",curRecordX+boxLabelX,curRecordY+boxLabelY);
    fill(127,255,0); //lime green
    if(curRecord < 0) {
      text("N/A",curRecordX+boxValueLineX,curRecordY+boxValueLineY);
    }else {
      text(curRecord,curRecordX+boxValueLineX,curRecordY+boxValueLineY);
    }
    fill(255,255,255); //white
    text("ms",curRecordX+boxUnitX,curRecordY+boxUnitY);
    
    //draw lastSpeed box
    fill(0,0,0);
    textSize(45);
    stroke(255,255,255); //white
    rect(lastSpeedX,lastSpeedY,boxWidth,boxHeight);
    fill(255,255,255); //white
    text("Last Drink:",lastSpeedX+boxLabelX,lastSpeedY+boxLabelY);
    fill(127,255,0); //lime green
    if(lastSpeed < 0) {
      text("N/A",lastSpeedX+boxValueLineX,lastSpeedY+boxValueLineY);
    } else {
      text(lastSpeed,lastSpeedX+boxValueLineX,lastSpeedY+boxValueLineY);
    }
    fill(255,255,255); //white
    text("ms",lastSpeedX+boxUnitX,lastSpeedY+boxUnitY);
        
    //draw Session Status
    textSize(45);
    stroke(255,255,0); //yellow
    fill(0,0,0);
    rect(sessionInProgressX,sessionInProgressY,boxWidth,boxHeight);
    fill(255,255,255); //white
    text("Session Status:",sessionInProgressX+boxLabelX,sessionInProgressY+boxLabelY);
    if(sessionInProgress == 1) {  
      fill(127,255,0); //lime green
      text("Started",sessionInProgressX+statusX,sessionInProgressY+boxValueLineY);
    }else {
      fill(255,0,0); //red
      text("Not Started",sessionInProgressX+statusX,sessionInProgressY+boxValueLineY);
    }
    
    //draw lastVolume box
    fill(0,0,0);
    textSize(45);
    stroke(255,255,0); //yellow
    rect(lastVolumeX,lastVolumeY,boxWidth,boxHeight);
    fill(255,255,255); //white
    text("Last Volume:",lastVolumeX+boxLabelX,lastVolumeY+boxLabelY);
    fill(127,255,0); //lime green
    text(lastVolume,lastVolumeX+boxValueLineX,lastVolumeY+boxValueLineY);
    fill(255,255,255); //white
    text("mL",lastVolumeX+boxUnitX,lastVolumeY+boxUnitY);
  }

  void serialEvent (Serial myPort) {
    // get the ASCII string:
    String inString = myPort.readStringUntil('\n');
    //println(inString);
    
    String[] inVars = split(inString,';');
    
    //assign variables
    inLifeCount = inVars[0];
    inLifeRecord = inVars[1];
    inCurCount = inVars[2];
    inCurRecord = inVars[3];
    inLastSpeed = inVars[4];
    inGraph = inVars[5];
    inSessionInProgress = inVars[6];
    inLastVolume= inVars[7];
    
  }
