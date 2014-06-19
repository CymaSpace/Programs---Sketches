#include <Adafruit_NeoPixel.h>
#define PIN 6
#define CNT_LIGHTS 50

Adafruit_NeoPixel strip = Adafruit_NeoPixel(CNT_LIGHTS, PIN, NEO_GRB + NEO_KHZ800); 
//fixed settings 
int analogPinL = 1; // read from multiplexer using analog input 0
int analogPinR = 0; // read from multiplexer using analog input 0
int strobePin = 4; // strobe is attached to digital pin 2
int resetPin = 5; // reset is attached to digital pin 3
int spectrumValueL[6];
int spectrumValueR[6]; // to hold a2d values
int previousSpectrumValueL[6]; // to hold a2d values
int previousSpectrumValueR[6]; // to hold a2d values
int global_brightness = 40; // Sets global brightness, i.e. 64 is 1/4 brightness.
int useColor[2];
int num;
int prop[CNT_LIGHTS];
int prop_history[CNT_LIGHTS];
int refresh_counter = 0;

//lowest reading the MSGEQ7 should recognize 1-1000 range
int minFilter = 50;
//higher number refreshes slow - refreshed every nth interation
int refresh = 15;


 
void setup() 
{
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
}
 
void loop() 
{    
   runTime();
}
 
 
void runTime()
{
 
  digitalWrite(resetPin, HIGH);
  digitalWrite(resetPin, LOW);
  int changeL = 0;
  int changeR = 0;
  int changePinL = 0;
  int changePinR = 0;
  int k,i,r,g,b;
  int use_le = 0;
  int use_ls = ((CNT_LIGHTS/2)-1);
  int use_rs = ((CNT_LIGHTS/2)+0);
  int use_re = (CNT_LIGHTS-1);
  int spectrum_totalL = 0;
  int spectrum_totalR = 0;
  int spectrum_counts = 0;
  
 
 
  //get readings from chip
  for (i = 0; i < 7; i++)
  {
    spectrum_counts++;
    previousSpectrumValueL[i] = spectrumValueL[i];
    previousSpectrumValueR[i] = spectrumValueR[i];
    digitalWrite(strobePin, LOW);
    delayMicroseconds(30); // to allow the output to settle
    spectrumValueL[i] = analogRead(analogPinL);
    spectrumValueR[i] = analogRead(analogPinR);
    
    spectrum_totalL+= spectrumValueL[i];
    spectrum_totalR+= spectrumValueR[i];
    
    /*
    //figure out which pin changed the most since last reading
    if( abs(previousSpectrumValueL[i]-spectrumValueL[i]) > changeL)
    {
        changeL = abs(previousSpectrumValueL[i]-spectrumValueL[i]);
        if( spectrumValueL[i] > minFilter  )
          changePinL = (i+1);          
    }//if
   
    //figure out which pin changed the most since last reading
    if( abs(previousSpectrumValueR[i]-spectrumValueR[i]) > changeR)
    {
        changeR = abs(previousSpectrumValueR[i]-spectrumValueR[i]);
        if( spectrumValueR[i] > minFilter )
          changePinR = (i+1);          
    }//if  
    */
   
  }//for i
  
  changePinL = (spectrum_totalL);
  changePinR = (spectrum_totalR);
 
  
  refresh_counter++;
  if(refresh_counter>=refresh)
  {
    //reset the counter
    refresh_counter = 0;
         
    //save the history - RIGHT SIDE
    for (k = use_rs; k <= use_re; k++)
    {
      prop_history[k] = prop[k-1];
    }//for
     
    for (k = use_rs; k <= use_re; k++)
    {
      prop[k] = prop_history[k];
    }//for
    
    
    //save the history - LEFT SIDE
    for (k = use_ls; k >= use_le; k--)
    {
      prop_history[k] = prop[k+1];
    }//for
     
    for (k = use_ls; k >= use_le; k--)
    {
      prop[k] = prop_history[k];
    }//for
    
    //current;
    prop[use_rs] = changePinR;
    prop[use_ls] = changePinL;
            
    for (k = 0; k < CNT_LIGHTS; k++)
    {
      num = prop[k];
      //Serial.println(num);
      if(num>=0)
      {
        get_color();
        strip.setPixelColor( k, useColor[0], useColor[1], useColor[2]);
      }//if
    }//for
         
     
  }//if refresh  

  digitalWrite(strobePin, HIGH); 
  strip.setBrightness(global_brightness);
  strip.show();
}
 
void get_color()
{
  int r,g,b;
 
  // RGB for bands 1-7
    if(num<500)
    {
      r = 0; g = 0; b = 0;
    }
    if(num>500 && num <=750)
    {
      r = 255; g = 0; b = 0;
    }
    if(num>750 && num <=1000)
    {
      r = 255; g = 153; b= 0;
    }
    if(num>1000 && num <=1250)
    {
      r = 255; g = 255; b = 0;
    }
    if(num>1250 && num <=1500)
    {
      r = 0; g = 255; b = 0;
    }
    if(num>1500 && num <=1750)
    {
      r = 0; g = 255; b = 255;
    }
    if(num>1750 && num <=2000)
    {
      r = 0; g = 0 ; b = 255;
    }
    if(num>2000)
    {
      r = 255; g = 0; b = 153;
    }
   
    useColor[0] = r;
    useColor[1] = g;
    useColor[2] = b;
    
}



