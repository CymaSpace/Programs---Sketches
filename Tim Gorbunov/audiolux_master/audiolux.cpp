/*
  Morse.cpp - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/

#include "Arduino.h"
#include "audiolux.h"
#include <FastLED.h>

Audiolux::Audiolux()
{
  FastLED.addLeds<NEOPIXEL, 6>(leds, 100);
}

void Audiolux::runs(){
  leds[2] = CRGB::Red;
  FastLED.show();
}

