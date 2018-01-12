// Lucas Morgan - www.enviral-design.com //

#define USE_OCTOWS2811
#include<OctoWS2811.h>
#include<FastLED.h>

// ------------ Change these as neccesary -----------//
#define NUM_LEDS_PER_STRIP 288
#define NUM_STRIPS 8

CRGB leds[NUM_STRIPS * NUM_LEDS_PER_STRIP];
const int numOfBytes = NUM_LEDS_PER_STRIP * NUM_STRIPS * 3;
const int numLeds = NUM_LEDS_PER_STRIP * NUM_STRIPS;
char inputBuffer[numOfBytes];

// ------------------- Setup -------------------- //
void setup() {
  LEDS.addLeds<OCTOWS2811>(leds, NUM_LEDS_PER_STRIP);
  LEDS.setBrightness(255);
  delay(500);
  Serial.begin(115200);
  Serial.setTimeout(500);
  LEDS.show();
}

// ------------------- Main Loop -------------------- //
void loop() {
  if(Serial.available() > 0) {
    Serial.readBytes(inputBuffer, numOfBytes);
  }
      for (int j = 0; j < numLeds; j++) {
        leds[j] = CRGB(inputBuffer[(j*3)],inputBuffer[(j*3)+1],inputBuffer[(j*3)+2]);
      }
      LEDS.show();
}
