#include "audiolux.h"

//Audiolux "outputname"(PIN, NUM_LED)
Audiolux al1(6, 100);//initialize new audiolux output on this pin

void setup() {
}

void loop() {
  al1.dot();
}
