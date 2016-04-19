#include "audiolux.h"
#include <FastLED.h>

Audiolux al1;

void setup() {
}
void loop() {
  al1.runs();
}





















//////////////////////////////////////////////////////////////////////
//IDEAL PROGRAM LAYOUT
////////////////////////////////////////////////////////////////////
/*

#include "audiolux.h"

Audiolux show();

show.setAudio(MSGEQ7, STROBE_PIN, RESSET, PIN);
show.setAnimation(PIN, A4, SHOWSIZE);
show.setLED(NUM_DIMENTIONS, NUM_PIXELS);

void setup() {
}

void loop() {
  show.updateAnimation();
  show.display();
}



//*/
