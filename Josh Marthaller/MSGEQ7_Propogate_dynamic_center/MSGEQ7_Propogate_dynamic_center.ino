//code from https://github.com/jdbugman/anduino-neopixel/blob/master/soundNeo/soundNeo.ino
#include <Adafruit_NeoPixel.h>
#define PIN 5
Adafruit_NeoPixel strip = Adafruit_NeoPixel(150, PIN, NEO_GRB + NEO_KHZ800); //first number controls the amount of pixels you have (add 4 so the drip falls off the edge)

Adafruit_NeoPixel::setPixelColorB( uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint16_t brightness) {
  setPixelColor(n, (brightness*r/255) , (brightness*g/255), (brightness*b/255));
}
//length of LED strip 
int led_strip_length = 150;

//half the srip length (length on each side) 
int prop[150];
int prop_his[150];

//if # is 10, it updates every 10th iteration, thus higher number makes
//refresh rate slower
int refresh = 9; //higher number refreshes slow
int refresh_counter = 0;

//where to start / end the sides
float left_start = 75;
float left_end = 1;
float right_start = 76;
float right_end = 150;

//pin for pint dial
int analogPinpot = 2;

//lowest reading the MSGEQ7 should recognize
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
   runTime();
}
 
 
void runTime(){
 
  digitalWrite(resetPin, HIGH);
  digitalWrite(resetPin, LOW);
  int changeL = 0;
  int changeR = 0;
  int changePinL = 0;
  int changePinR = 0;
  int k,i,r,g,b;
  float pot;
  int use_ls, use_le, use_rs, use_re;
  
  
  
  //un comment below if the pentomiter is hooked up
  //these will override the default setting if used

  //--fetch pentimoter value 
  pot = analogRead(analogPinpot);
  Serial.println(pot);
  
  if (pot >=400 && pot <= 600)
  {
    use_ls = left_start;
    use_le = left_end;
    use_rs = right_start;
    use_re = right_end;
  }
  else
  {
   
   if(pot<400)
   {
    use_ls = round( (pot/400) * left_start);
    use_rs = use_ls+1;
    
   } 
   if(pot>600)
   {
    use_ls = round( ((pot-600)/400) * left_start) + left_start;
    use_rs = use_ls+1;
    
   } 
   
   if(use_ls<1)
   {
     use_ls = 1;
     use_rs = 2;
   }
   if(use_ls> led_strip_length)
   {
     use_ls = led_strip_length -1;
     use_rs = led_strip_length;
   }
    
  }
  //Serial.println(pot/400);
  //Serial.println(use_rs);
  //Serial.println(use_ls);
   
  
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
          
    

    }
   
    //figure out which pin changed the most since last reading
    if( abs(previousSpectrumValueR[i]-spectrumValueR[i]) > changeR)
    {
        changeR = abs(previousSpectrumValueR[i]-spectrumValueR[i]);
        if( spectrumValueR[i] > minFilter )
          changePinR = (i+1);
          
          
    }
   
  }
 

  
  refresh_counter++;
  if(refresh_counter>=refresh )
  {
    
    refresh_counter = 0;

 
  //current;
  prop[(use_rs-1)] = changePinR;
  prop[(use_ls-1)] = changePinL;
  
  //save the history - RIGHT SIDE
  for (k = (use_rs-0); k <= (use_re-1); k++)
  {
    prop_his[k] = prop[k-1];
  }
 
  for (k = (use_rs-0); k <= (use_re-1); k++)
  {
    prop[k] = prop_his[k];
  }
  
  
  //save the history - LEFT SIDE
  for (k = (use_ls-0); k >= (use_le-1); k--)
  {
    prop_his[k] = prop[k+1];
  }
 
  for (k = (use_ls-0); k >= (use_le-1); k--)
  {
    prop[k] = prop_his[k];
  }
  
  
  
 
  for (k = 0; k <= led_strip_length; k++)
  {
    num = prop[k];
   
    if(num>-1){
    get_color();
    strip.setPixelColor( k, useColor[0], useColor[1], useColor[2]);
    }
  }
 
 
 /*
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
  */
 
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
