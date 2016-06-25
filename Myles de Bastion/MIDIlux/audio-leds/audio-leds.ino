  /*

 
 */
#include <AudioTuner.h>
#include <Audio.h>
#include "FastLED.h"

#define NUM_LEDS 300
#define DATA_PIN 6

CRGB leds[NUM_LEDS];

AudioInputAnalog          input;        
AudioTuner                tuner;

AudioConnection patchCord1(input,  0, tuner, 0);

int base_a4 = 440;
int note=0;
int oldnote=0;
int val=0;
int oldval=0;
int bright=0;

void setup() {
    AudioMemory(30);

    /*
     *  Initialize the yin algorithm's absolute
     *  threshold, .15 is good number.
     */
    tuner.initialize(.15);
    
    FastLED.addLeds<WS2811, DATA_PIN>(leds, NUM_LEDS);
}


void showLeds(int note) {
  //for(int j = 1; j<197; j++) {
    for(int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CHSV((note*2), 255, bright);
    }
    FastLED.show();
    delay(1);
  //}
}


void loop() {
    // read back fundamental frequency
    if (tuner.available()) {
        float freq = tuner.read();
        //float prob = tuner.probability();
        //Serial.printf("Note: %3.2f | Probability: %.2f\n", note, prob);
        oldnote=note;
        //frequency to midi conversion
        note = round(12*(log(freq/base_a4)/log(2))+69);
        if(oldnote!=note){
          Serial.println(note);
          bright=255;
          //uncomment to send MIDI notes over USB
          //usbMIDI.sendNoteOff(oldnote, 127, 1);
          //usbMIDI.sendNoteOn(note, 127, 1);
        }
    }
    showLeds(note);
    if(bright>0){bright--;}
}


