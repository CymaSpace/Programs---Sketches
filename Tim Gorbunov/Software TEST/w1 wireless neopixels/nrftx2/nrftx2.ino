//light blue
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
int msg[2];
RF24 radio(9,10);
const uint64_t pipe = 0xE8E8F0F0E1LL;

const int leds = 150;
int strip[leds][3];

void setup(void){
 Serial.begin(115200);
 radio.begin();
 radio.openWritingPipe(pipe);}

void loop(void){
  
 for (int i=1; i<leds+1; i++){
 stripSet(i, 255, 1,3);
 }
 stripShow();
 for (int i=1; i<leds+1; i++){
 stripSet(i, 1, 255,3);
 }
 stripShow();
}

void stripSet(int led, int r, int g, int b){
  strip[led-1][0] = r;
  strip[led-1][1] = g;
  strip[led-1][2] = b;
}
void stripShow(){
  msg[0] = 4;
  radio.write(msg, 4);
  for (int i=0; i<leds; i++){
    msg[0] = 0;
    msg[1] = 0;
    radio.write(msg, 4);
    for (int k=1; k<4; k++){
      msg[0] = k;
      msg[1] = strip[i][k-1];
      radio.write(msg, 4);
    }
  }
}
