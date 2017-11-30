#include <Adafruit_NeoPixel.h> // include the Adfruit Neopixel library
  
#define PIN_NEOPIXELS 6 // connect Neopixel strip to this pin
#define PIN_STROBE 12 // spectrum shield strobe pin
#define PIN_RESET 13 // spectrum shield reset pin
#define PIN_SOUND 0 //analog input pin spectrum shield - using only one/right channel
#define NUMLEDS 113 // LEDs in Neopixel strip
  
#define MINPOWER 128 // minimum amplitude value to be considered
#define MAXPOWER 1023 // max amplitude value
  
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMLEDS, PIN_NEOPIXELS, NEO_GRB + NEO_KHZ800);
  
int soundvalue[7]; //band array to store amplitutde value for each band
  
void setup() 
{
 strip.begin(); //initialize neopixels
 strip.setBrightness(35);
 strip.show();
 //initialize spectrum shield graphic equalizer
 pinMode(PIN_RESET, OUTPUT); // reset
 pinMode(PIN_STROBE, OUTPUT); // strobe
 digitalWrite(PIN_RESET,LOW); // reset low
 digitalWrite(PIN_STROBE,HIGH); //pin 5 is RESET on the shield
}
  
void loop() {
 
 readMSGEQ7();
 int bColor = colorValue((soundvalue[0]+soundvalue[1])/2);
 int gColor = colorValue((soundvalue[2]+soundvalue[3]+soundvalue[4])/3);
 int rColor = colorValue((soundvalue[5]+soundvalue[6])/2);
 byte red = rColor; //convert to 0-255 byte value for Neopixel
 byte blue = bColor;
 byte green = gColor;
  
 for (int i = 0; i < NUMLEDS; i=i++) //show strip
 {
 if(i<=240) {strip.setPixelColor(i, (i<=rColor) ? red : 0,(i<=gColor) ? green : 0,(i<=bColor) ? blue : 0);}
 else {strip.setPixelColor(i, (i<=rColor) ? 255 : 0,(i<=gColor) ? 255 : 0,(i<=bColor) ? 255 : 0);} 
 } //you might have to play with this based on your setup. The if statement is to make the top LEDs show brighter
 strip.show();
 delay(5);
}
  
int colorValue(int powerValue) // calculate a a RGB color value from the sound bands
{
 return map(powerValue, MINPOWER, MAXPOWER, 0, NUMLEDS);
}
  
// Read the Analog sound data from 7 bands
void readMSGEQ7()
{
 digitalWrite(PIN_RESET, HIGH); //reset the data
 digitalWrite(PIN_RESET, LOW);
 for(int band=0; band < 7; band++) { //loop thru all 7 bands
 digitalWrite(PIN_STROBE,LOW); // go to the next band 
 delayMicroseconds(10); //gather some data
 soundvalue[band] = analogRead(PIN_SOUND); // store left band reading
 digitalWrite(PIN_STROBE,HIGH); // reset the strobe pin
 }
}
