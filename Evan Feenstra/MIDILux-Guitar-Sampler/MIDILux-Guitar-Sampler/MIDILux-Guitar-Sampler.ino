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

AudioInputI2S             input;        
AudioOutputI2S            dac;
AudioTuner                tuner;
AudioSynthWaveformSine    sine;
AudioMixer4               mixer;
AudioPlaySdWav           playSd;

AudioConnection patchCord1(input,  0, mixer, 0);
AudioConnection patchCord2(mixer, 0, tuner, 0);
AudioConnection patchCord3(playSd, 0, dac, 0);

AudioControlSGTL5000     sgtl5000_1;

int base_a4 = 440;
int note=0;
int oldnote=0;
int val=0;
int oldval=0;

void setup() {
    AudioMemory(30);

    sgtl5000_1.enable();
    sgtl5000_1.volume(0.5);
    /*
     *  Initialize the yin algorithm's absolute
     *  threshold, .15 is good number.
     */
    tuner.initialize(.15);
    
    sine.frequency(30.87);
    sine.amplitude(1);

    SPI.setMOSI(7);
    SPI.setSCK(14);
    if (!(SD.begin(10))) {
      // stop here, but print a message repetitively
      while (1) {
        //Serial.println("Unable to access the SD card");
        delay(5);
      }
    }
    
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
          String string  = "sound";
          String string2 = string + note;
          String string3 = string2 + ".wav";
          //string = string + ".wav";
          char filename[12];
          string3.toCharArray(filename, 12);
          Serial.println(filename);
          playSd.play(filename);
          
          //uncomment to send MIDI notes over USB
          //usbMIDI.sendNoteOff(oldnote, 127, 1);
          //usbMIDI.sendNoteOn(note, 127, 1);
        }
    }

    //input gain pot
    float vol = analogRead(15);
    oldval=val;
    val = vol / 64;
    if(val!=oldval){
      sgtl5000_1.lineInLevel(val);
      Serial.println(val);
    }
}
