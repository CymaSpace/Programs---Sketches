//light blue
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
int msg[2];
RF24 radio(9,10);
const uint64_t pipe = 0xE8E8F0F0E1LL;

const int leds = 150;
int strip[leds][3];
char leddat[255][3];

//A_t duino1; 


void setup(void){
 Serial.begin(115200);
 radio.begin();
 radio.openWritingPipe(pipe);}

void loop(void){
  
 for (int i=1; i<leds; i++){
 stripSet(i, 255, 8,3);
 }
 stripShow();
 //delay(100);
for (int i=1; i<leds; i++){
 stripSet(i, 0, 255,3);
 }
 stripShow();
 //delay(100);
}

void stripSet(char led, char r, char g, char b){
  leddat[led-1][0] = r;
  leddat[led-1][1] = g;
  leddat[led-1][2] = b;
  
}
void stripShow(){
  radio.write(&leddat, 32);
  Serial.println(sizeof(leddat));
  //delay(15);
}
