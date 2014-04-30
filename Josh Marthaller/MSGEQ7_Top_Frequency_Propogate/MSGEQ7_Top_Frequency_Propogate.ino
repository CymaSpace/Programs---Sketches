//code from https://github.com/jdbugman/anduino-neopixel/blob/master/soundNeo/soundNeo.ino
#include <Adafruit_NeoPixel.h>
#define PIN 5
Adafruit_NeoPixel strip = Adafruit_NeoPixel(150, PIN, NEO_GRB + NEO_KHZ800); //first number controls the amount of pixels you have (add 4 so the drip falls off the edge)

//length of LED strip 
int led_strip_length = 150;    //select 1 thru 10

//half the srip length (length on each side) 
int strip_length = 75;
int prop_left[75];
int prop_left_his[75];
int prop_right[75];
int prop_right_his[75];

//if # is 10, it updates every 10th iteration, thus higher number makes
//refresh rate slower
int refresh = 10; //higher number refreshes slow
int refresh_counter = 0;

int readingsTotal = 800;
int readingsChanged = 100;

//was for the dial
int analogPinpot = 2;
int minFilter = 100;
 
//values for MSGEQ7 Chip
int analogPinL = 1; // read from multiplexer using analog input 0
int analogPinR = 0; // read from multiplexer using analog input 0
int strobePin = 4; // strobe is attached to digital pin 2
int resetPin = 5; // reset is attached to digital pin 3
int spectrumValueL[6];
int spectrumValueR[6]; // to hold a2d values
int previousSpectrumValueL[6]; // to hold a2d values
int previousSpectrumValueR[6]; // to hold a2d values
int global_brightness = 96; // Sets global brightness, i.e. 64 is 1/4 brightness.
int useColor[2];
int num;
int middle_pin = 25;
 
 
void setup() {
  Serial.begin(9600); // print to serial monitor
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  pinMode(analogPinL, INPUT);
  pinMode(analogPinR, INPUT);
  pinMode(strobePin, OUTPUT);
  pinMode(resetPin, OUTPUT);
  analogReference(DEFAULT);
  digitalWrite(resetPin, LOW);
  digitalWrite(strobePin, HIGH);
  
  pinMode(analogPinpot, INPUT);
}
 
void loop() {    
   sevenWaves();
}
 
 
void sevenWaves(){
 
  digitalWrite(resetPin, HIGH);
  digitalWrite(resetPin, LOW);
  int changeL = 0;
  int changeR = 0;
  int changePinL = 0;
  int changePinR = 0;
  int k,i,r,g,b;
  int pot;
  int tmp_adj = 0;
  
  
  
  //un comment below if the pentomiter is hooked up
  //these will override the default setting if used

  //--fetch pentimoter value 
  //pot = analogRead(analogPinpot);
  
  //Serial.println(pot);
  //--for the min amplitude to count as a reading
  //minFilter = round(pot * .4);
  
  //--refresh rate
  //refresh = round(pot * .05);
 
  //get readings from chip
  for (i = 0; i < 7; i++)
  {
    previousSpectrumValueL[i] = spectrumValueL[i];
    previousSpectrumValueR[i] = spectrumValueR[i];
    digitalWrite(strobePin, LOW);
    delayMicroseconds(30); // to allow the output to settle
    spectrumValueL[i] = analogRead(analogPinL);
    spectrumValueR[i] = analogRead(analogPinR);
 
   
   
    //figure out which pin changed the most since last reading
    if( abs(previousSpectrumValueL[i]-spectrumValueL[i]) > changeL)
    {
        changeL = abs(previousSpectrumValueL[i]-spectrumValueL[i]);
        if( spectrumValueL[i] > minFilter  )
          changePinL = (i+1);
          
        if( spectrumValueL[i] > 400 && i <=1)
          tmp_adj+= 23;
         
         readingsChanged++;
    }
   
    //figure out which pin changed the most since last reading
    if( abs(previousSpectrumValueR[i]-spectrumValueR[i]) > changeR)
    {
        changeR = abs(previousSpectrumValueR[i]-spectrumValueR[i]);
        if( spectrumValueR[i] > minFilter )
          changePinR = (i+1);
          
        if( spectrumValueR[i] > 400 && i <=1)
          tmp_adj+= 23;
         
         readingsChanged++;
    }
   
  }
 
  //refresh = round(readingsTotal/readingsChanged);
  
 
  
  if(tmp_adj < 0) { tmp_adj = 0; }
  if(tmp_adj > 200) { tmp_adj = 200; }

  
  
  refresh_counter++;
  if(refresh_counter>=(refresh - round(tmp_adj * .1)))
  //if(refresh_counter>=refresh )
  {
    

    refresh_counter = 0;
    //refresh =  round(5 * (readingsChanged / (readingsChanged * 7 * 2)));
    readingsChanged = 0;
    tmp_adj-= 1;
 
  //current;
  prop_right[0] = changePinR;
  //save the history
  for (k = 1; k <strip_length; k++)
  {
    prop_right_his[k] = prop_right[k-1];
  }
 
  for (k = 1; k <strip_length; k++)
  {
    prop_right[k] = prop_right_his[k];
  }
 
  for (k = 0; k <strip_length; k++)
  {
    num = prop_right[k];
   
    if(num>-1){
    get_color();
    strip.setPixelColor((strip_length-k-1), useColor[0], useColor[1], useColor[2]);
    }
  }
 
 
 
  //current;
  prop_left[0] = changePinL;
  //save the history
  for (k = 1; k <strip_length; k++)
  {
    prop_left_his[k] = prop_left[k-1];
  }
 
  for (k = 1; k <strip_length; k++)
  {
    prop_left[k] = prop_left_his[k];
  }
 
  for (k = 0; k <strip_length; k++)
  {
    num = prop_left[k];
    if(num>-1)
    {
    get_color();
    strip.setPixelColor((strip_length+k), useColor[0], useColor[1], useColor[2]);
    }
  }
 
  }
 

 
 
   //if(changePin > 0)
  // {
   digitalWrite(strobePin, HIGH);
  // }
 
 
 
  strip.setBrightness(global_brightness);
  strip.show();
 
}
 
void get_color()
{
  int r,g,b;
 
  // RGB for bands 1-7
    if(num==0)
    {
      r = 0; g = 0; b = 0;
    }
    if(num==1)
    {
      r = 255; g = 0; b = 0;
    }
    if(num==2)
    {
      r = 255; g = 153; b= 0;
    }
    if(num==3)
    {
      r = 255; g = 255; b = 0;
    }
    if(num==4)
    {
      r = 0; g = 255; b = 0;
    }
    if(num==5)
    {
      r = 0; g = 255; b = 255;
    }
    if(num==6)
    {
      r = 0; g = 0 ; b = 255;
    }
    if(num==7)
    {
      r = 255; g = 0; b = 153;
    }
   
    useColor[0] = r;
    useColor[1] = g;
    useColor[2] = b;
    
}
