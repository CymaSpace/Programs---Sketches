#include <FastLED.h>
#define NUM_LEDS 60
#define DATA_PIN 6
CRGB leds[NUM_LEDS];

void setup() { 
   FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
}

void loop() {
    int val = analogRead(2);
    int numLedsToLight = map(val, 0, 1023, 0, NUM_LEDS);

    // First, clear the existing led values
    FastLED.clear();
    for(int led = 0; led < numLedsToLight; led++) { 
        leds[led] = CRGB::Blue; 
    }
    FastLED.show();
}


