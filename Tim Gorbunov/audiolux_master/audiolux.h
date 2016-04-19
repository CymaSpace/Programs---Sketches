/*
  audiolux.h - Library 
  Created by Tim Gorbunov
  Released
*/
#ifndef audiolux_h
#define audiolux_h


#include "Arduino.h"
#include <FastLED.h>

class Audiolux
{
  public:
    Audiolux();
    void runs();
  private:
    CRGB leds[100];
    
};

#endif
