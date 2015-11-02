//This sketch listens to FFT analysis from the Teensy Audio Shield and displays 
//it on three RGB LED strips to make a simple equalizer. For Teensy 3.1, use the 
//latest (beta) version on FastLED.

#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include "FastLED.h"

#define NUM_LEDS 180
#define DATA_PIN 5

CRGB leds[NUM_LEDS];

int incoming = 0;

const int myInput = AUDIO_INPUT_LINEIN;
//const int myInput = AUDIO_INPUT_MIC;

// Create the Audio components.  These should be created in the
// order data flows, inputs/sources -> processing -> outputs
//
AudioInputI2S       audioInput;         // audio shield: mic or line-in
AudioAnalyzeFFT256  myFFT(20);
AudioOutputI2S      audioOutput;        // audio shield: headphones & line-out

// Create Audio connections between the components
//
AudioConnection c1(audioInput, 0, audioOutput, 0);
AudioConnection c2(audioInput, 0, myFFT, 0);
AudioConnection c3(audioInput, 1, audioOutput, 1);

// Create an object to control the audio shield.
// 
AudioControlSGTL5000 audioShield;


void setup() {
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(12);

  // Enable the audio shield and set the output volume.
  audioShield.enable();
  audioShield.inputSelect(myInput);
  audioShield.volume(60);
  
  FastLED.addLeds<WS2811, DATA_PIN >(leds, NUM_LEDS);
}

void loop() {
  if (myFFT.available()) {
    // each time new FFT data is available
    // print it all to the Arduino Serial Monitor
    Serial.print("FFT: ");
    for (int i=0; i<60; i++) {
      incoming = myFFT.output[i];
      if (incoming > 1) {
        leds[i] = CHSV(incoming, 255, 255);
        if (incoming > 5) {
        leds[119-i] = CHSV(incoming, 255, 255);
          if (incoming > 10) {
            leds[i+120] = CHSV(incoming, 255, 255);
          }
        }
      }
      else if (incoming < 1) {
          leds[i] = CRGB(0, 0, 0);
          leds[119-i] = CHSV(200, 0, 0);
          leds[i+120] = CHSV(200, 0, 0);
      }
      
      
      Serial.print(myFFT.output[i]);
      Serial.print(",");
    }
    Serial.println();
    FastLED.show();
  delay(10);
  }
  
}



