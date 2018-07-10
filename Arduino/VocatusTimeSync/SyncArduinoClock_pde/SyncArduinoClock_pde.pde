/**
 * SyncArduinoClock. 
 *
 * portIndex must be set to the port connected to the Arduino
 * 
 * The current time is sent in response to request message from Arduino 
 * or by clicking the display window 
 *
 * The time message is 11 ASCII text characters; a header (the letter 'T')
 * followed by the ten digit system time (unix time)
 */
 

import processing.serial.*;
import java.util.Date;
import java.util.Calendar;
import java.util.GregorianCalendar;

public static int portIndex;  // select the com port, 0 is the first port
public static final String TIME_HEADER = "T"; //header for arduino serial time message 
public static final char TIME_REQUEST = 7;  // ASCII bell character 
public static final char LF = 10;     // ASCII linefeed
public static final char CR = 13;     // ASCII linefeed

//Skins and skin variables
DisplaySkin mySkin; 

DisplaySkin altitude;
DisplaySkin verdant;
DisplaySkin deepBlue;
DisplaySkin lavender;
DisplaySkin lowLight;  

Serial myPort;     // Create object from Serial class

void setup() {  
  size(1024, 768);
  initSkins();
  assignSkin();
  
  // Background Markup; draw this once for performance benefit
  drawBackgroundMarkup();
  portIndex=Serial.list().length-2;
  println(Serial.list());
  println(" Connecting to -> " + Serial.list()[portIndex]);
  myPort = new Serial(this,Serial.list()[portIndex], 9600);
  println(getTimeNow());
}

void draw()
{
  if ( myPort.available() > 0) {  // If data is available,
    char val = char(myPort.read());         // read it and store it in val
    if(val == TIME_REQUEST){
       long t = getTimeNow();
       sendTimeMessage(TIME_HEADER, t);   
    }
    else
    { 
       if(val == LF)
           ; //igonore
       else if(val == CR)           
         println();
       else  
         print(val); // echo everying but time request
    }
  }  
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
  textFont(pencilPete,64);
  text("Click to send Time Sync", 250,448);
  
}
  
void mousePressed() {  
  sendTimeMessage( TIME_HEADER, getTimeNow()); 
}


void sendTimeMessage(String header, long time) {  
  String timeStr = String.valueOf(time);  
  myPort.write(header);  // send header and time to arduino
  myPort.write(timeStr); 
  myPort.write('\n');  
}

long getTimeNow(){
  // java time is in ms, we want secs    
  Date d = new Date();
  Calendar cal = new GregorianCalendar();
  long current = d.getTime()/1000;
  long timezone = cal.get(cal.ZONE_OFFSET)/1000;
  long daylight = cal.get(cal.DST_OFFSET)/1000;
  return current + timezone + daylight; 
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
    
    static final String IMAGE_PATH="../../VocatusStatusBoard/ZeroStateImages/";
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
