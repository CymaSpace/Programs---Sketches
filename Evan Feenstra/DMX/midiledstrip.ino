#include <Adafruit_NeoPixel.h>

#define PIN 3

Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);


void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  
  usbMIDI.setHandleNoteOn(OnNoteOn);   //FOR OUTGOING DATA
}

void loop() {
  
  usbMIDI.read(); // USB MIDI receive


}

void OnNoteOn(byte channel, byte note, byte velocity) {
  
  int led;
  led = note - 25;
  
  int blue;
  blue = velocity;
  
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, 0, 0, 0);
      strip.show();
  }
  
   
  strip.setPixelColor(led, 255, 0, blue);
  strip.show();
  
}
