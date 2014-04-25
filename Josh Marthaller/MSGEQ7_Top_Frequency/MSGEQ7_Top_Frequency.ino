//code from https://github.com/jdbugman/anduino-neopixel/blob/master/soundNeo/soundNeo.ino
#include <Adafruit_NeoPixel.h>
#define PIN 5
Adafruit_NeoPixel strip = Adafruit_NeoPixel(147, PIN, NEO_GRB + NEO_KHZ800); //first number controls the amount of pixels you have (add 4 so the drip falls off the edge)

int led_strip_length = 150;    //select 1 thru 10

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
}

void loop() {    
   sevenWaves();
} 


void sevenWaves(){

  digitalWrite(resetPin, HIGH);
  digitalWrite(resetPin, LOW);
  int changeL, changeR = 0;
  int changePinL, changePinR = 0;
  int k,i,r,g,b;
 
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
        if( spectrumValueL[i] > 100 && changeL > 100 && i !=0)
          changePinL = i;
    }
    
    //figure out which pin changed the most since last reading
    if( abs(previousSpectrumValueR[i]-spectrumValueR[i]) > changeR)
    {
        changeR = abs(previousSpectrumValueR[i]-spectrumValueR[i]);
        if( spectrumValueR[i] > 100 && changeR > 100 && i !=0)
          changePinR = i;
    }
    
  }
  
   num = changePinR;
   get_color();
    
    if(changePinR > 0)
    {
      for (k = 0; k <=75; k++)
      {
        strip.setPixelColor(k, useColor[0], useColor[1], useColor[2]);
      } 
    }
    
   num = changePinL;
   get_color();
    
    if(changePinL > 0)
    {
      for (k = 76; k <=150; k++)
      {
        strip.setPixelColor(k, useColor[0], useColor[1], useColor[2]);
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
      r = 255; g = 0; b = 0;
    }
    if(num==1)
    {
      r = 255; g = 153; b= 0;
    }
    if(num==2)
    {
      r = 255; g = 255; b = 0;
    }
    if(num==3)
    {
      r = 0; g = 255; b = 0;
    }
    if(num==4)
    {
      r = 0; g = 255; b = 255;
    }
    if(num==5)
    {
      r = 0; g = 0 ; b = 255;
    }
    if(num==6)
    {
      r = 255; g = 0; b = 153;
    }
    
    useColor[0] = r;
    useColor[1] = g;
    useColor[2] = b;
}


