/*
 
  Guitar pitch detector that plays samples from an SD card, for Teensy Arduino.
     Choose samples for each note by uploading WAV files called "sound99.wav",
     where "99" is the midi note number. (eg. a sample on low E would be "sound50.wav")
     
  Uncomment lines 85 and 86 to send notes as MIDI over usb
  
  Use the Teensy Audio Board for 44.1K, 16 bit audio sample playback.

  Audio library by Paul Stoffregen http://www.pjrc.com/teensy/td_libs_Audio.html
  Tuner library: https://github.com/duff2013/AudioTuner, using the YIN algorithm:
  http://recherche.ircam.fr/equipes/pcm/cheveign/pss/2002_JASA_YIN.pdf
 
 */
#include <AudioTuner.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

AudioInputAnalog          input;        
AudioTuner                tuner;

AudioConnection patchCord1(input,  0, tuner, 0);

int base_a4 = 440;
int note=0;
int oldnote=0;
int val=0;
int oldval=0;

void setup() {
    AudioMemory(30);

    /*
     *  Initialize the yin algorithm's absolute
     *  threshold, .15 is good number.
     */
    tuner.initialize(.15);
    
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

          //uncomment to send MIDI notes over USB
          //usbMIDI.sendNoteOff(oldnote, 127, 1);
          //usbMIDI.sendNoteOn(note, 127, 1);
        }
    }

}
