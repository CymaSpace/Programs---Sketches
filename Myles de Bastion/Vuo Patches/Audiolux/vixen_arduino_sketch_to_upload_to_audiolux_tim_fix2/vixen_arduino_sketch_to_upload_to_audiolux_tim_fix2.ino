/*
 
Vixen Lights 3.x - Arduino Generic Serial for Addressable Pixels
 
Using this code is pretty straight forward, simply hookup your one wire (WS2811 or WS2812) data line to pin 6 of your Arduino
and upload this code.  Make sure you have properly installed the FastLED library from http://fastled.io Once you are done, simply
power your Pixel strips from an external power supply.  Next configure a Generic Serial Controller inside of Vixen Lights 3.x and
add 3 x pixels for the number of channels.  Configure the Generic Serial Controller to use 115200, 8, none, and 1.  Then create
your element and add "Multiple Items (1 x number of pixels).  Finally select your pixel elements and set them as RGB pixels before
patching them to the controler outputs.  You should now be ready to begin testing.
 
For a complete tutorial check out blog.huntgang.com
 
Created   November 8th, 2014
By        Richard Sloan - www.themindfactory.com
And       David Hunt - blog.huntgang.com
Version   1.4
 
*/
 
 
// You must download and install the library from http://fastled.io/
#include <FastLED.h>
 
// Sets the maximum number of LEDs that this code will handle to avoid running out of memory
#define NUM_LEDS 300
 
// Sets the pin which is used to connect to the LED pixel strip
#define DATA_PIN 6
 
CRGB leds[NUM_LEDS];
 
 
void setup() {
  // Define the speed of the serial port
  Serial.begin(115200);
}
 
void loop() {
  // Set some counter / temporary storage variables
  int cnt;
  unsigned int num_leds;
  unsigned int d1, d2, d3;
 
  // Begin an endless loop to receive and process serial data
  for(;;) {
    // Set a counter to 0.  This couter keeps track of the pixel colors received.
    cnt = 0;
    //Begin waiting for the header to be received on the serial bus
    //1st character
    while(!Serial.available());
      if(Serial.read() != '>') {
        continue;
        }
    //second character
    while(!Serial.available());
      if(Serial.read() != '>') {
        continue;
        }
    //get the first digit from the serial bus for the number of pixels to be used
    while(!Serial.available());
      d1 = Serial.read();
    //get the second digit from the serial bus for the number of pixels to be used
    while(!Serial.available());
      d2 = Serial.read();
    //get the third digit from the serial bus for the number of pixels to be used
    while(!Serial.available());
      d3 = Serial.read();
    //get the end of the header
    while(!Serial.available());
      if(Serial.read() != '<') {
        continue;
        }
    while(!Serial.available());
      if(Serial.read() != '<') {
        continue;
        }
    // calculate the number of pixels based on the characters provided in the header digits
    num_leds = (d1-'0')*100+(d2-'0')*10+(d3-'0');
    // ensure the number of pixels does not exceed the number allowed
    if(num_leds > NUM_LEDS) {
      continue;
      }
    // Let the FastLED library know how many pixels we will be addressing
    FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, num_leds);
    // Loop through each of the pixels and read the values for each color
    do {
      while(!Serial.available());
        leds[cnt].r = Serial.parseInt();
      while(!Serial.available());
        leds[cnt].g = Serial.parseInt();
      while(!Serial.available());
        leds[cnt++].b = Serial.parseInt();
      /*while(!Serial.available());
        boolean temp = Serial.parseInt();
        Serial.println("cont");//*/
      }
    while(--num_leds);
    // Tell the FastLED Library it is time to update the strip of pixels
    FastLED.show();
    // WOO HOO... We are all done and are ready to start over again!
    }
}
