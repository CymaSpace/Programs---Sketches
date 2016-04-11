/*
  Morse.cpp - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/

#include "Arduino.h"
#include "audiolux.h"
#include <FastLED.h>
#include "EEPROM.h"

Audiolux::Audiolux(uint8_t pin, uint8_t NUMLEDS) {
  pinMode(pin, OUTPUT);
  _DATA_PIN = pin;
  _NUM_LEDS = NUMLEDS;
  _LEFT_START_POINT = ((_NUM_LEDS / 2)); // Starting LED for left side
  _LEFT_END_POINT = 1; // Generally the end of the left side is the first LED
  _RIGHT_START_POINT = ((_NUM_LEDS / 2) + 1); // Starting LED for the right side
  _RIGHT_END_POINT = (_NUM_LEDS - 1); // Generally the end of the right side is the last LED
  _LED_STACK_SIZE = (_NUM_LEDS / 2); // How many LED's in each stack
}

void Audiolux::dot() {
  digitalWrite(_DATA_PIN, HIGH);
  delay(250);
  digitalWrite(_DATA_PIN, LOW);
  delay(250);  
}

void Audiolux::dash() {
  digitalWrite(_DATA_PIN, HIGH);
  delay(1000);
  digitalWrite(_DATA_PIN, LOW);
  delay(250);
}
