//code from https://github.com/jdbugman/anduino-neopixel/blob/master/soundNeo/soundNeo.ino
#include <Adafruit_NeoPixel.h>

#define PIN 5

Adafruit_NeoPixel strip = Adafruit_NeoPixel(77, PIN, NEO_GRB + NEO_KHZ800); //first number controls the amount of pixels you have (add 4 so the drip falls off the edge)

int bright = 1;    //select 1 thru 10
int wait = 0;//20;    //speed of leds
int filterValue = 80; // MSGEQ7 always spits out a number around 60, so this filters those out
int global_brightness = 10; // Sets global brightness, i.e. 64 is 1/4 brightness.
int waveSize = 5; // size 4 uses 9 lights. One in the middle, 4 on each side.  (time 7 for 63 LEDs total)
//pixels used is (wave size * 2)+1)*7
int maxOverflow = 3;

//values for MSGEQ7 Chip
int analogPin = 0; // read from multiplexer using analog input 0
int strobePin = 4; // strobe is attached to digital pin 2
int resetPin = 5; // reset is attached to digital pin 3
int spectrumValue[7]; // to hold a2d values

//int j =0;

uint32_t beginTime = 0;
uint32_t endTime = 0;

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
 // Serial.println("MSGEQ7 test by J Skoba");

}
void loop() {  
  /*
  colorWipe(strip.Color(255, 0, 0), 50); // Red
  colorWipe(strip.Color(0, 255, 0), 50); // Green
  colorWipe(strip.Color(0, 0, 255), 50); // Blue 
  */
  
  //beginTime = millis();
  //while ( (millis()-beginTime) <= 10000)
  // sevenBands();
   
   
  beginTime = millis();
  while ( (millis()-beginTime) <= 10000)
   sevenWaves();
  
  /* 
  beginTime = millis();
  while ( (millis()-beginTime) <= 10000)
   vuMeter();
  */

} 

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.setBrightness(global_brightness);
      strip.show();
      delay(wait);
  }
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
  int overflow[77];
  
  
 for (q = 0; q<=77; q++)
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
 
       reading = round( spectrumValue[i]*5 / (600) );
        
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
          overflow[j] = j; 
        }
        
        for ( j = (middlePin + (waveSize + 1)); j <= ( middlePin + waveSize +diff); j++)
        {
          overflow[j] = j; 
        }
        
      }
      /*
        
      Serial.print(" Sepctrum: ");
      Serial.print(spectrumValue[i]);  
      Serial.print(" Pin: ");
      Serial.print(k);
      Serial.print(" Middle: ");
      Serial.print( middlePin);
      Serial.print(" Reading: ");
      Serial.print( reading);
      Serial.print(" REAL READING: ");
      Serial.print( spectrumValue[i]*5 / (1024)  );
      Serial.print(" Wave Size: ");
      Serial.print( waveSize);
      Serial.print(" Pinout Value: ");
      Serial.print( pinOutValue );
      Serial.print(" Place Value: ");
      Serial.print( placeInWave );
      Serial.println(); 
     
     */
     
     
     if(pinOutValue==1)
       pinOutValue = 255;
      
      // RGB for bands 1-7
      if(i==0)
      {
        r = 255; g = 0; b = 0;
      }
      if(i==1)
      {
        r = 255; g = 153; b= 0;
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
  
  Serial.println();
  for (q = 0; q<=77; q++)
  {
    if ( overflow[q]>1)
    {
     //strip.setPixelColor((q-1), 155, 43, 79); 
    } 
    overflow[q] = 0;
  }
  
  strip.setBrightness(global_brightness);
  strip.show();
  
}

void sevenBands(){
  int i;
  digitalWrite(resetPin, HIGH);
  digitalWrite(resetPin, LOW);

//grabs readings from chip
  for (i = 0; i < 7; i++)
  {
    digitalWrite(strobePin, LOW);
    delayMicroseconds(30); // to allow the output to settle
    spectrumValue[i] = analogRead(analogPin);
 
 
   
  if(spectrumValue[i] < filterValue)
     spectrumValue[i] = 0;
 
 //max reading is 1024, but color hex values only range 0-255
 //scale it down to stick within that range
 spectrumValue[i] = spectrumValue[i] / (1024/255);
 
 if(spectrumValue[i] > 255)
     spectrumValue[i] = 255;

    
 // comment out/remove the serial stuff to go faster
 // - its just here for show
 /*
 if (spectrumValue[i] < 10)
 {
 Serial.print(" ");
 Serial.print(spectrumValue[i]);
 }
 else if (spectrumValue[i] < 100 )
 {
 Serial.print(" ");
 Serial.print(spectrumValue[i]);
 }
 else
 {
 Serial.print(" ");
 Serial.print(spectrumValue[i]);
 }
 */  

    digitalWrite(strobePin, HIGH);
  }
 
 // Serial.println();
 
  
  
  strip.setPixelColor(0, spectrumValue[6], 0, 0);
  strip.setPixelColor(1, spectrumValue[5], 0, 0);
  strip.setPixelColor(2, spectrumValue[4], 0, 0);
  strip.setPixelColor(3, spectrumValue[3], 0, 0);
  strip.setPixelColor(4, spectrumValue[2], 0, 0);
  strip.setPixelColor(5, spectrumValue[1], 0, 0);
  strip.setPixelColor(6, spectrumValue[0], 0, 0);
  

  
  strip.setBrightness(global_brightness);
  strip.show();
}


void vuMeter(){
  int i;
  int j;
  int loopEnd;
  int numPixels = strip.numPixels();
  double pixRatio = (float)numPixels / 1;
  double channelRatio = pixRatio / 100;
  float weight =1;

  digitalWrite(resetPin, HIGH);
  digitalWrite(resetPin, LOW);

//grabs readings from chip
  for (i = 0; i < 7; i++)
  {
    digitalWrite(strobePin, LOW);
//    delayMicroseconds(30); // to allow the output to settle
    spectrumValue[i] = analogRead(analogPin);

    digitalWrite(strobePin, HIGH);

    //weight = 1;
    if (i < 3)
     weight = 1.5;
    else 
      weight = 1; 
      //makes 10 bit value 8 bit
    spectrumValue[i] /= 4;
    spectrumValue[i] = (int)( (double)spectrumValue[i] * (channelRatio * weight) );

    if(spectrumValue[i] > 255)
     spectrumValue[i] = 255;

   /*if (spectrumValue[i] >= 40)
    spectrumValue[i] = 0;*/
  }


  
 for (i = 0;i<spectrumValue[0] && i< numPixels; i++)
  strip.setPixelColor(i, 255, 0, 0);

 loopEnd = i + spectrumValue[1];
 for (;i<loopEnd && i< numPixels; i++)
  strip.setPixelColor(i,0, 255, 0);

 loopEnd = i + spectrumValue[2];
 for (;i<loopEnd && i< numPixels; i++)
  strip.setPixelColor(i,0,0, 255);

 loopEnd = i + spectrumValue[3];
 for (;i<loopEnd && i< numPixels; i++)
  strip.setPixelColor(i,255, 0,255);

 loopEnd = i + spectrumValue[4];
 for (;i<loopEnd && i< numPixels; i++)
  strip.setPixelColor(i,255, 255, 0);

 loopEnd = i + spectrumValue[5];
 for (;i<loopEnd && i< numPixels; i++)
  strip.setPixelColor(i,255, 128, 0);

 loopEnd = i + spectrumValue[6];
 for (;i<loopEnd && i< numPixels; i++)
  strip.setPixelColor(i,255,255,255);
  
   
  for(;i<numPixels; i++)
  strip.setPixelColor(i,0,0,0);
  
  for (i = numPixels;i>numPixels-spectrumValue[0] && i>0; i--)
  strip.setPixelColor(i, 255, 0, 0);

 loopEnd = i - spectrumValue[1];
 for (;i>loopEnd && i>0; i--)
  strip.setPixelColor(i,0, 255, 0);

 loopEnd = i - spectrumValue[2];
 for (;i>loopEnd && i>0; i--)
  strip.setPixelColor(i,0,0, 255);

 loopEnd = i - spectrumValue[3];
 for (;i>loopEnd && i>0; i--)
  strip.setPixelColor(i,255, 0,255);

 loopEnd = i - spectrumValue[4];
 for (;i>loopEnd && i>0; i--)
  strip.setPixelColor(i,255, 255, 0);

 loopEnd = i - spectrumValue[5];
 for (;i>loopEnd && i>0; i--)
  strip.setPixelColor(i,255, 128, 0);

 loopEnd = i - spectrumValue[6];
 for (;i>loopEnd && i>0; i--)
  strip.setPixelColor(i,255,255,255);
  strip.setBrightness(global_brightness);
  strip.show();

}

void musicRainbowLoop_1(){
 
 int i = 0, j= 0;
  
  digitalWrite(resetPin, HIGH);
  digitalWrite(resetPin, LOW);

//grabs readings from chip
  for (i = 0; i < 7; i++)
  {
    digitalWrite(strobePin, LOW);
//    delayMicroseconds(30); // to allow the output to settle
    spectrumValue[i] = analogRead(analogPin);

    digitalWrite(strobePin, HIGH);
    
    //makes 10 bit value 8 bit
    spectrumValue[i] /= 4;
    
    if(spectrumValue[i] > 255)
     spectrumValue[i] = 255;
   
   for (j=0; j<27; j++)
    { 
      if (spectrumValue[i] <= 55)
       spectrumValue[i] = 0;
    if (i == 0)
     nextLed(spectrumValue[i], 0,0, wait);
    else if (i == 1)
     nextLed(0, spectrumValue[i], 0, wait);
    else if (i == 2)
     nextLed(0,0, spectrumValue[i], wait);
    else if (i == 3)
     nextLed(spectrumValue[i], 0, spectrumValue[i], wait);
    else if (i == 4)
     nextLed(0, spectrumValue[i], spectrumValue[i], wait);
    else if (i == 5)
     nextLed(spectrumValue[i], spectrumValue[i],0, wait);
    else 
     nextLed(spectrumValue[i], spectrumValue[i], spectrumValue[i], wait);
    }
    delay(2);
   }
}


void nextLed(int r, int g, int b, int wait){
static int currentLed = 0;


strip.setPixelColor(currentLed, r, g, b);
delay(wait);

//if (currentLed % 4 == 0)
 // strip.show();

currentLed++;
// currentLed += 4;

if (currentLed >= strip.numPixels() ){
  currentLed = 0;
  strip.show();
}
  
}

