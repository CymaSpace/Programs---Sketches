#include "FastLED.h"

#define NUM_LEDS 2
CRGB leds[1];
void setup() {
  FastLED.addLeds<NEOPIXEL, 6>(leds, NUM_LEDS);
  FastLED.show();
}
void loop() {
  if(Serial.available()){
    Serial.readBytes( (char*)leds, NUM_LEDS * 3);
  }
  FastLED.show();
}
