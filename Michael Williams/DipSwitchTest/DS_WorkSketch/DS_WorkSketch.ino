#include "FastLED.h"

// Assign dip switches
#define DIP_9 2
#define DIP_8 3
#define DIP_7 4
#define DIP_6 7
#define DIP_5 8
#define DIP_4 9
#define DIP_3 10
#define DIP_2 11
#define DIP_1 A4

#define DATA_PIN 6

int checkDIP(){
  int dipValue = 0;
  if (digitalRead(DIP_1) == HIGH){
    dipValue ++;
  }
  if (digitalRead(DIP_2) == HIGH){
    dipValue += 2;
  }
  if (digitalRead(DIP_3) == HIGH){
    dipValue += 4;
  }
  if (digitalRead(DIP_4) == HIGH){
    dipValue += 8;
  }
  if (digitalRead(DIP_5) == HIGH){
    dipValue += 16;
  }
  if (digitalRead(DIP_6) == HIGH){
    dipValue += 32;
  }
  if (digitalRead(DIP_7) == HIGH){
    dipValue += 64;
  }
  if (digitalRead(DIP_8) == HIGH){
    dipValue += 128;
  }
  if (digitalRead(DIP_9) == HIGH){
    dipValue += 264;
  }
  return dipValue;
}

int NUM_LEDS = checkDIP(); // How many leds in your strip?

// Define the array of leds
CRGB leds[NUM_LEDS];

void setup() {
  
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

}

void loop() {
FastLED.clear();
  
int g = checkDIP();
  for (int j = 0; j < g; j++){
    leds[j] = CRGB::Blue;
  }
FastLED.show();
  
}
