/*
  Wisp.h - Library for swirling Wisp animations
*/
#ifndef Wisp_h
#define Wisp_h

#include "Arduino.h"
#include <FastLED.h>

class Wisp
{
  public:
    Wisp(int pos, int trail, int starting_brightness, int hue);
    void set_pos(int pos);
    int get_pos();
    void set_trail(int trail);
    int get_trail();
    void set_brightness(int brightness);
    int get_brightness();
    void set_hue(int hue);
    int get_hue();
    void update(CRGB leds[], int brightness_step, int num_leds, int hue_step);
  private:
    int _brightness;
    int _trail;
    int _pos;
    int _trail_num;
    int _hue;
};

#endif