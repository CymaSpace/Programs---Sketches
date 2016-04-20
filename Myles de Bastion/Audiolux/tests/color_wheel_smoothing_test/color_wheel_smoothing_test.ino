#include <FastLED.h>
#define NUM_LEDS 111
#define DATA_PIN 6
#define LEFT_POT_PIN 2 // Left pot controls brightness
#define RIGHT_POT_PIN 3 // Right pot controls color

CRGB leds[NUM_LEDS];

// Code for Smoothing Pot Analog Input
const int numReadings = 10;

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int potControlHueAverage = 0;   // the average

void setup() { 
    //Serial.begin(9600);      // open the serial port at 9600 bps: 
    
    // initialize all the readings to 0:
    for (int thisReading = 0; thisReading < numReadings; thisReading++) {
        readings[thisReading] = 0;
    }
    
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
}
   
void loop() { 
        int potControlBrightness = analogRead(2);
        int potControlHue = analogRead(3);
        
        // Code for Analog Pot smoothing
        // subtract the last reading:
        total = total - readings[readIndex];
        // read from the pot:
        readings[readIndex] = potControlHue;
        // add the reading to the total:
        total = total + readings[readIndex];
        // advance to the next position in the array:
        readIndex = readIndex + 1;

        // if we're at the end of the array...
        if (readIndex >= numReadings) {
        // ...wrap around to the beginning:
        readIndex = 0;
        }

        // calculate the average:
        potControlHueAverage = total / numReadings;
        // send it to the computer as ASCII digits
        //Serial.println(potControlHueAverage);
        delay(1);        // delay in between reads for stability

        //int numLedsToLight = map(brightness, 1023, 0, 0, NUM_LEDS); // map number of LEDs to left pot
        int brightness = map(potControlBrightness, 1023, 0, 0, 255); // map brightness to left pot
        int hue = map(potControlHueAverage, 0, 1023, 0, 255); // map hue to right pot
        //Serial.println(potControlBrightness);
        
        // First, clear the existing led values
        FastLED.clear();
        for(int led = 0; led < NUM_LEDS; led++) { 
            leds[led] = CHSV(hue, 255, brightness); 
        }
        FastLED.show();
}
