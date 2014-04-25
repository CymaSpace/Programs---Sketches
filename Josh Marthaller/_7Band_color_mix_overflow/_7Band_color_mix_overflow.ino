//code from https://github.com/jdbugman/anduino-neopixel/blob/master/soundNeo/soundNeo.ino
#include <Adafruit_NeoPixel.h>
#define PIN 5
Adafruit_NeoPixel strip = Adafruit_NeoPixel(147, PIN, NEO_GRB + NEO_KHZ800); //first number controls the amount of pixels you have (add 4 so the drip falls off the edge)

int bright = 1;    //select 1 thru 10
int wait = 0;//20;    //speed of leds
int filterValue = 115; // MSGEQ7 always spits out a number around 60, so this filters those out
int global_brightness = 96; // Sets global brightness, i.e. 64 is 1/4 brightness.
int waveSize = 10; // size 4 uses 9 lights. One in the middle, 4 on each side.  (time 7 for 63 LEDs total)  //pixels used is (wave size * 2)+1)*7
int maxOverflow = 0;
int amplify = 0; //  1 - 30 amplifies the spread of the lights on each spectrum  //amplify needs to be 25 or so to get an overflow

//values for MSGEQ7 Chip
int analogPin = 0; // read from multiplexer using analog input 0
int strobePin = 4; // strobe is attached to digital pin 2
int resetPin = 5; // reset is attached to digital pin 3
int spectrumValue[7]; // to hold a2d values


void setup() {
  Serial.begin(9600); // print to serial monitor
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  pinMode(analogPin, INPUT);
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

  int i;
  int k;
  int tmpVal;
  int r;
  int g;
  int b;
  int j;
  int x;
  int q;
  int middlePin;
  digitalWrite(resetPin, HIGH);
  digitalWrite(resetPin, LOW);
  int overflow[151];
  
  
 for (q = 0; q<=150; q++)
 {
    overflow[q]= 0;
 }
 
  //get readings from chip
  for (i = 0; i < 7; i++) 
  {
    
    
  //for testing
  //if(i==3) {
  //  
    digitalWrite(strobePin, LOW);
    delayMicroseconds(30); // to allow the output to settle
    spectrumValue[i] = analogRead(analogPin);
   
    if (i==0)
      middlePin = waveSize +1;
    else
      middlePin =  waveSize + 1 + (  (1 + waveSize * 2) * i);
    
    for (k = (middlePin - waveSize); k <= (middlePin + waveSize); k++)
    {
      int pinOutValue = 0;   
      int reading;
       
       
       reading = round( spectrumValue[i]*(waveSize+maxOverflow) / ( 1000 - (25 * amplify)) );
        
      int placeInWave = abs(k - middlePin);
    
      if( reading >= placeInWave)
        pinOutValue = 1;
        
      if( spectrumValue[i] < filterValue)
        pinOutValue = 0;
      
       
      //build the overflow
      
      if( reading > waveSize)
      {
        int diff = round(abs(waveSize-reading));
     
        if(diff>maxOverflow)
          diff = maxOverflow;
        
        for ( j = (middlePin - (waveSize +diff)); j <( middlePin - waveSize); j++)
        {
          if(j>=0 && j<=150){
            overflow[j] = 1; 
            //Serial.print("J in range: "); Serial.print(j); Serial.println();
          }
          else
          {
            //Serial.print("J not in range: "); Serial.print(j); Serial.println();
          }
          
        }
        
        for ( j = (middlePin + (waveSize + 1)); j <= ( middlePin + waveSize +diff); j++)
        {
          if(j>=0 && j<=150)
          {
            overflow[j] = 1; 
          }
        }
        
      }
      
  
     if(pinOutValue==1)
       pinOutValue = 255;
      
      // RGB for bands 1-7
      if(i==0)
      {
        r = 255; g = 0; b = 0;
      }
      if(i==1)
      {
        r = 220; g = 75; b= 0;
      }
      if(i==2)
      {
        r = 255; g = 255; b = 0;
      }
      if(i==3)
      {
        r = 0; g = 255; b = 0;
      }
      if(i==4)
      {
        r = 0; g = 255; b = 255;
      }
      if(i==5)
      {
        r = 0; g = 0 ; b = 255;
      }
      if(i==6)
      {
        r = 255; g = 0; b = 153;
      }
      
      if (pinOutValue == 0)
      {
        r = 0; g = 0; b = 0;
      }
      strip.setPixelColor(k-1, r, g, b); 
    
    }
  
  //for testing
  //}
  //
     
    digitalWrite(strobePin, HIGH);
  }
  
  //Serial.println();
  
  for (q = 0; q<=150; q++)
  {
     //Serial.print("OVerflow at: "); Serial.print(q); 
     //Serial.print(" is: "); Serial.print( overflow[q] ); 
     //Serial.println(); 
    
    if ( overflow[q]==1)
    {
      strip.setPixelColor((q-1), 255, 0, 0);
    
      //Serial.print("OVerflow at: "); Serial.print(q); 
      //Serial.println(); 
    } 
     overflow[q] = 0;
  }
  
  
  strip.setBrightness(global_brightness);
  strip.show();
  
}


