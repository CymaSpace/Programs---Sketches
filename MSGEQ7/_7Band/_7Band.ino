//code from https://github.com/jdbugman/anduino-neopixel/blob/master/soundNeo/soundNeo.ino
#include <Adafruit_NeoPixel.h>

#define PIN 5

Adafruit_NeoPixel strip = Adafruit_NeoPixel(7, PIN, NEO_GRB + NEO_KHZ800); //first number controls the amount of pixels you have (add 4 so the drip falls off the edge)

int bright = 1;    //select 1 thru 10
int wait = 0;//20;    //speed of leds
int filterValue = 80; // MSGEQ7 always spits out a number around 60, so this filters those out
int global_brightness = 16; // Sets global brightness, i.e. 64 is 1/4 brightness.
int waveSize = 4; // size 4 uses 9 lights. One in the middle, 4 on each side.  (time 7 for 63 LEDs total)

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
  int middlePin;
  digitalWrite(resetPin, HIGH);
  digitalWrite(resetPin, LOW);
  
  //get readings from chip
  for (i = 0; i < 7; i++) 
  {
    
  //for testing
  if(i==3) {
  //  
    digitalWrite(strobePin, LOW);
    delayMicroseconds(30); // to allow the output to settle
    spectrumValue[i] = analogRead(analogPin);
    
    Serial.print("Sepctrum: ");
    Serial.print(spectrumValue[i]);
      
    if (i==0)
      middlePin = waveSize +1;
    else
      middlePin =  waveSize + 1 + (i * 1 + waveSize + waveSize);
    
    for (k = (middlePin - waveSize) -1; k < (middlePin + waveSize); k++)
    {
      int pinOutValue = 0;   
      int reading;
       reading  = (spectrumValue[i] / 1024) * waveSize;
      
      //if (spectrumValue[i] < filterValue)
       // reading = 0;
        
      int placeInWave = abs(k - middlePin);
    
      if(reading>=placeInWave)
        pinOutValue = 255;
        
      Serial.print("Pin: ");
      Serial.print(k);
      Serial.print(" Middle: ");
      Serial.print( middlePin);
      Serial.print(" Reading: ");
      Serial.print( reading);
      Serial.print(" Pinout Value: ");
      Serial.print( pinOutValue );
      Serial.print(" Place Value: ");
      Serial.print( placeInWave );
      Serial.println();  
      
      strip.setPixelColor(i, 0, 0, pinOutValue);    
        
    }
  
  //for testing
  }
  //
     
    digitalWrite(strobePin, HIGH);
  }
  
  Serial.println();
 
  
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

