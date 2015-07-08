#include <Adafruit_NeoPixel.h>
#include "EEPROM.h"
#define PIN 6
#define CNT_LIGHTS 51

// Instantiate Neopixel Strip
Adafruit_NeoPixel strip = Adafruit_NeoPixel(CNT_LIGHTS, PIN, NEO_GRB + NEO_KHZ800); 

/* === Fixed settings === */

// Set pin values
int analogPinL = 1; // read from multiplexer using analog input 0
int analogPinR = 0; // read from multiplexer using analog input 0
int analogPinpot = 2;// dial for speed
int analogpotrange = 3;// dial for range
int stomp = 5;
int strobePin = 12; // strobe is attached to digital pin 2
int resetPin = 13; // reset is attached to digital pin 3

// Instantiate spectrum data arrays and variables
int spectrumValueL[7];
int spectrumValueR[7]; // to hold a2d values
int previousSpectrumValueL[7]; // to hold a2d values
int previousSpectrumValueR[7]; // to hold a2d values
int minFilter = 70; //lowest reading the MSGEQ7 should recognize 1-1000 range
float waveValue;

// Color and brightness
int useColor[3]; // Instantiate color array
int colorState = 0;
float use_brightness = 0;
int global_brightness = 255; // Sets global brightness, i.e. 64 is 1/4 brightness.
int prop[CNT_LIGHTS];
int prop_history[CNT_LIGHTS];
float pot_range = 0.0;

// Variable to test whether monomode is active
boolean monomode = 0;

//higher number refreshes slow - refreshed every nth interation
int refresh = 10;
int refresh_counter = 0;
int use_refresh = 0;
int tmp_refresh_adj = 0;
 
void setup() 
{  
  Serial.begin(9600); // print to serial monitor
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  // Make sure EEPROM is within the accepted range (0-1)
  if (EEPROM.read(1) > 1){EEPROM.write(1,0);} // Clear EEPROM?

  // Set pin modes
  pinMode(analogPinL, INPUT);
  pinMode(analogPinR, INPUT);
  pinMode(stomp, INPUT);
  pinMode(analogPinR, INPUT);
  pinMode(strobePin, OUTPUT);
  pinMode(resetPin, OUTPUT);

  digitalWrite(resetPin, LOW);
  digitalWrite(strobePin, HIGH); 

  // Check whether the stomp button is pressed and set the EEPROM
  if (digitalRead(stomp) == HIGH){
    if (EEPROM.read(1) == 0){
      EEPROM.write(1,1);
    }else if (EEPROM.read(1) == 1) {
      EEPROM.write(1,0);
    }  
  }

  // Set monomode based on the stomp button state
  monomode = EEPROM.read(1);
}
 
void loop() 
{  
  // If stomp button is being pressed, alter color state for monomode
  if (digitalRead(stomp) == HIGH){
    delay(100);
    if(colorState < 2){colorState++;}
    else if(colorState == 2){colorState=0;}
  }

  // Reset the MSGEQ7 chip
  digitalWrite(resetPin, HIGH);
  digitalWrite(resetPin, LOW);

  int changePinL = 0;
  int changePinR = 0;
  int k,i; // Looping variables
  float pot_value = 0.0;
  
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
    
    if( spectrumValueL[i] > 400 && i <=1)
          tmp_refresh_adj+= 23;
    if( spectrumValueR[i] > 400 && i <=1)
          tmp_refresh_adj+= 23;
    
    digitalWrite(strobePin, HIGH); 
   
  }//for i

  if (monomode == 1){
    changePinL = spectrum_totalR;
  } else if (monomode == 0){
    changePinL = spectrum_totalL;
  }
  changePinR = spectrum_totalR;
 
  use_refresh = refresh;
  use_brightness = global_brightness;
  
  //for use of dial
  pot_value = analogRead(analogPinpot);
  pot_range = analogRead(analogpotrange);
  if(pot_value < 1000) {
    if (pot_value > 20){
      use_refresh = pot_value / 50;
    } else {
     use_refresh = 0;
    }
  }

  if(tmp_refresh_adj < 0) { tmp_refresh_adj = 0; }
  if(tmp_refresh_adj > 200) { tmp_refresh_adj = 200; }
 
  refresh_counter++;

  if(refresh_counter>=(use_refresh - round(tmp_refresh_adj * .1))) {
    //reset the counter
    refresh_counter = 0;
    tmp_refresh_adj-= 1;
         
    //save the history - RIGHT SIDE
    for (k = use_rs; k <= use_re; k++) {
      prop_history[k] = prop[k-1];
    }//for
     
    for (k = use_rs; k <= use_re; k++) {
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
            
    for (k = 0; k < CNT_LIGHTS; k++) {
      int num;
      num = prop[k];
      if(num >= 0) {
        getColor(num);
        strip.setPixelColor( k, useColor[0], useColor[1], useColor[2]);
      }//if
    }//for
         
     
  }//if refresh  

  strip.show();
}
 
void getColor(int num) 
{
  int r,g,b;
  
  if(num < 500) {
    
    if(colorState == 0) {

      useColor[0] = 0;
      useColor[1] = 0;
      useColor[2] = 0;

    }

  } else {
    getRGB(getWaveLength(num));  
  }
    
}

float getWaveLength(int num)
{
  float minVal = 500;
  float maxVal = 4700;
  float minWave = 350;
  float maxWave = 650;
  maxVal = pot_range * 5;
  minVal = pot_range / 2;
  if(num>maxVal)
    maxVal = num;
    
  return ((num - minVal) / (maxVal-minVal) * (maxWave - minWave)) + minWave;

}

void getRGB(float waveValue)
{
  float rz = 0, gz = 0, bz = 0;
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
  
   if(colorState == 0){
    useColor[0] = g;
    useColor[1] = r;
    useColor[2] = b;
   }
   if(colorState == 1){
    useColor[0] = r;
    useColor[1] = g;
    useColor[2] = b;
   }
   if(colorState == 2){
    useColor[0] = b;
    useColor[1] = g;
    useColor[2] = r;
   } 
  
}


