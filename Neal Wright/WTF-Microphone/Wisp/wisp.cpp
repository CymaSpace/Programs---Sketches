#include "Arduino.h"
#include <FastLED.h>
#include "Wisp.h"

Wisp::Wisp(int pos, int trail, int starting_brightness, int hue) {
  _pos = pos;
  _trail = trail;
  _brightness = starting_brightness;
  _hue = hue;
}

void Wisp::set_pos(int pos) {
  _pos = pos;
}

int Wisp::get_pos() {
  return _pos;
}

void Wisp::set_trail(int trail) {
  _trail = trail;
}

int Wisp::get_trail() {
  return _trail;
}

void Wisp::set_brightness(int brightness) {
  _brightness = brightness;
}

int Wisp::get_brightness() {
  return _brightness;
}

void Wisp::set_hue(int hue) {
  _hue = hue;
}

int Wisp::get_hue() {
  return _hue;
}

void Wisp::update(CRGB leds[], int brightness_step, int num_leds, int hue_step) {
  _trail_num = 0;
  for(int i = _pos; _trail_num < _trail; i--) {

    int new_hue = _hue - ((_trail_num + 1) * hue_step);
    int new_brightness = _brightness - ((_trail_num + 1) * brightness_step);
    if(new_hue < 0) new_hue += 255;

    if(i < 0) {
      leds[i + num_leds] = CHSV(new_hue, 255, new_brightness);
    } else {
      leds[i] = CHSV(new_hue, 255, new_brightness);
    }
    _trail_num++;
  }
  _pos++;
}