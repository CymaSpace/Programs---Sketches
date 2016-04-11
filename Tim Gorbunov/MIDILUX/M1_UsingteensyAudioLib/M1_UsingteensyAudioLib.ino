#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

     
AudioAnalyzeNoteFrequency notefreq;
AudioInputAnalog         adc(A3); 
AudioConnection          patchCord1(adc, notefreq);
// GUItool: end automatically generated code

void setup() {
  AudioMemory(30);
    /*
     *  Initialize the yin algorithm's absolute
     *  threshold, this is good number.
     */
    notefreq.begin(.15);
}

void loop() {
  // read back fundamental frequency
    if (notefreq.available()) {
        float note = notefreq.read();
        float prob = notefreq.probability();
        Serial.printf("Note: %3.2f | Probability: %.2f\n", note, prob);
    }
}
