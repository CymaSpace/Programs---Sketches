#include <Adafruit_NeoPixel.h>
#define STRAND1 2
#define STRAND2 3
#define STRAND3 4
#define STRAND4 5
#define STRAND5 6
#define STRAND6 7
#define STRAND7 8
#define CNT_LIGHTS 10

Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(CNT_LIGHTS, STRAND1, NEO_GRB + NEO_KHZ800); 
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(CNT_LIGHTS, STRAND2, NEO_GRB + NEO_KHZ800); 
Adafruit_NeoPixel strip3 = Adafruit_NeoPixel(CNT_LIGHTS, STRAND3, NEO_GRB + NEO_KHZ800); 
Adafruit_NeoPixel strip4 = Adafruit_NeoPixel(CNT_LIGHTS, STRAND4, NEO_GRB + NEO_KHZ800); 
Adafruit_NeoPixel strip5 = Adafruit_NeoPixel(CNT_LIGHTS, STRAND5, NEO_GRB + NEO_KHZ800); 
Adafruit_NeoPixel strip6 = Adafruit_NeoPixel(CNT_LIGHTS, STRAND6, NEO_GRB + NEO_KHZ800); 
Adafruit_NeoPixel strip7 = Adafruit_NeoPixel(CNT_LIGHTS, STRAND7, NEO_GRB + NEO_KHZ800); 


//fixed settings 
int analogPinL = 1; // read from multiplexer using analog input 0
int analogPinR = 0; // read from multiplexer using analog input 0
int strobePin = 4; // strobe is attached to digital pin 2
int resetPin = 5; // reset is attached to digital pin 3
int spectrumValueL[6];
int spectrumValueR[6]; // to hold a2d values
int previousSpectrumValueL[6]; // to hold a2d values
int previousSpectrumValueR[6]; // to hold a2d values
int global_brightness = 255; // Sets global brightness, i.e. 64 is 1/4 brightness.
int useColor[2];
int num;
float waveValue;
int prop[6][CNT_LIGHTS];
int prop_history[6][CNT_LIGHTS];
int refresh_counter = 0;



//lowest reading the MSGEQ7 should recognize 1-1000 range
int minFilter = 50;
//higher number refreshes slow - refreshed every nth interation
int refresh = 20;



 
void setup() 
{
  Serial.begin(9600); // print to serial monitor
  strip1.begin();
  strip1.show(); // Initialize all pixels to 'off'
  strip2.begin();
  strip2.show(); // Initialize all pixels to 'off'
  strip3.begin();
  strip3.show(); // Initialize all pixels to 'off'
  strip4.begin();
  strip4.show(); // Initialize all pixels to 'off'
  strip5.begin();
  strip5.show(); // Initialize all pixels to 'off'
  strip6.begin();
  strip6.show(); // Initialize all pixels to 'off'
  strip7.begin();
  strip7.show(); // Initialize all pixels to 'off'
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
  int k,i,r,g,b;
  int use_le = 0;
  int use_ls = ((CNT_LIGHTS/2)-1);
  int use_rs = ((CNT_LIGHTS/2)+0);
  int use_re = (CNT_LIGHTS-1);

  
 
 
  //get readings from chip
  for (i = 0; i < 7; i++)
  {
    previousSpectrumValueL[i] = spectrumValueL[i];
    previousSpectrumValueR[i] = spectrumValueR[i];
    digitalWrite(strobePin, LOW);
    delayMicroseconds(30); // to allow the output to settle
    spectrumValueL[i] = analogRead(analogPinL);
    spectrumValueR[i] = analogRead(analogPinR);
   
    digitalWrite(strobePin, HIGH); 
   
  }//for i
  
  
 
  refresh_counter++;
  if(refresh_counter>=refresh)
  {
    //reset the counter
    refresh_counter = 0;
    for (i = 0; i < 7; i++)
    {
        
             
        //save the history - RIGHT SIDE
        for (k = use_rs; k <= use_re; k++)
        {
          prop_history[i][k] = prop[i][k-1];
        }//for
         
        for (k = use_rs; k <= use_re; k++)
        {
          prop[i][k] = prop_history[i][k];
        }//for
        
        
        //save the history - LEFT SIDE
        for (k = use_ls; k >= use_le; k--)
        {
          prop_history[i][k] = prop[i][k+1];
        }//for
         
        for (k = use_ls; k >= use_le; k--)
        {
          prop[i][k] = prop_history[i][k];
        }//for
        
        //current;
        prop[i][use_rs] = spectrumValueR[i];
        prop[i][use_ls] = spectrumValueL[i];
                
        for (k = 0; k < CNT_LIGHTS; k++)
        {
          num = prop[i][k];
          //Serial.println(num);
          if(num>=0)
          {
            get_color();
            if(i==0) { strip1.setPixelColor( k, useColor[0], useColor[1], useColor[2]); }
            if(i==1) { strip2.setPixelColor( k, useColor[0], useColor[1], useColor[2]); }
            if(i==2) { strip3.setPixelColor( k, useColor[0], useColor[1], useColor[2]); }
            if(i==3) { strip4.setPixelColor( k, useColor[0], useColor[1], useColor[2]); }
            if(i==4) { strip5.setPixelColor( k, useColor[0], useColor[1], useColor[2]); }
            if(i==5) { strip6.setPixelColor( k, useColor[0], useColor[1], useColor[2]); }
            if(i==6) { strip7.setPixelColor( k, useColor[0], useColor[1], useColor[2]); }
          }//if
        }//for
        
    }//for i        
        
  }//if refresh  

  
  strip1.setBrightness(global_brightness);
  strip1.show();
  strip2.setBrightness(global_brightness);
  strip2.show();
  strip3.setBrightness(global_brightness);
  strip3.show();
  strip4.setBrightness(global_brightness);
  strip4.show();
  strip5.setBrightness(global_brightness);
  strip5.show();
  strip6.setBrightness(global_brightness);
  strip6.show();
  strip7.setBrightness(global_brightness);
  strip7.show();
}
 
void get_color()
{
  int r,g,b;
  //int waveValue;
  //Serial.println(num);
  //num=6000;
  
  if(num < 100)
  {
    r = 0; g = 0; b = 0;
    
    useColor[0] = g;
    useColor[1] = r;
    useColor[2] = b;
  }
  
  if(num>=100)
  {
    getWaveLength();
    getRGB();  
   
    
  }
    
}

void getWaveLength()
{
  float minVal = 100;
  float maxVal = 1000;
  float minWave = 350;
  float maxWave = 650;
  if(num>1000)
    maxVal = num;
    
  waveValue = ((num - minVal) / (maxVal-minVal) * (maxWave - minWave)) + minWave;
 // Serial.print(num);
  //Serial.println();
  //Serial.print(waveValue);
  //Serial.println();
}

void getRGB()
{
  float gamma = .8;
  float rz, gz, bz;
  int intsMax = 255;
  int r,g,b;
  
  if(waveValue >380 && waveValue <=439)
  {
    rz = (waveValue-440)/(440-380);
    gz = 0;
    bz = 1;
  }
  
  if(waveValue >=440 && waveValue <=489)
  {
    rz = 0;
    gz = (waveValue-440)/(490-440);
    bz = 1;
  }
  
  if(waveValue >=490 && waveValue <=509)
  {
    rz = 0;
    gz = 1;
    bz = (waveValue-510)/(510-490);
  }
  
  if(waveValue >=510 && waveValue <=579)
  {
    rz = (waveValue-510)/(580-510);
    gz = 1;
    bz = 0;
  }
  
  if(waveValue >=580 && waveValue <=644)
  {
    rz = 1;
    gz = (waveValue-645)/(645-580);
    bz = 0;
  }
  
  if(waveValue >=645 && waveValue <=780)
  {
    rz = 1;
    gz = 0;
    bz = 0;
  }
  
  r = rz * 255;
  b = bz * 255;
  g = gz * 255;
  
    useColor[0] = r;
    useColor[1] = g;
    useColor[2] = b;
  
  
  
}


