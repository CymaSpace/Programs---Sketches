/*
  Morse.h - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/
#ifndef audiolux_h
#define audiolux_h

#include "Arduino.h"
#include <FastLED.h>
#include "EEPROM.h"

#define ANALOG_PIN_L 1 // Left audio channel
#define ANALOG_PIN_R 0 // Right audio channel
#define REFRESH_POT_PIN 2 // Left pot
#define SENSITIVITY_POT_PIN 3 // Right pot
#define STOMP_PIN 5 // The pin connected to the stomp button

/* Sensitivity variables, refresh variables, and start/end points */
#define REFRESH_DIVISOR 80. // Higher = range of refresh values is lower
#define SENSITIVITY_DIVISOR 100. // Higher = range of sensitivity values on pot is lower
#define MAX_AMPLITUDE 4700 // Maximum possible amplitude value
#define MAX_AMPLITUDE_MULTIPLIER 380
#define MIN_AMPLITUDE 545 // Lowest possible amplitude value (Higher number causes there to be more blank LED's)
#define MIN_AMPLITUDE_MULTIPLIER 200
#define SENSITIVITY_MULTIPLIER 200 // Higher = range of sensitivity values on pot is lower


class Audiolux
{
  public:
    Audiolux(uint8_t DATA_PIN, uint8_t NUM_LEDS);
    void dot();
    void dash();
    uint8_t
      STROBE_PIN = 12,
      RESET_PIN = 13;
      
  private:
    uint8_t 
      _DATA_PIN;
    uint16_t 
      _NUM_LEDS,
      _LEFT_START_POINT,
      _LEFT_END_POINT,
      _RIGHT_START_POINT,
      _RIGHT_END_POINT,
      _LED_STACK_SIZE;
    
};

#endif
