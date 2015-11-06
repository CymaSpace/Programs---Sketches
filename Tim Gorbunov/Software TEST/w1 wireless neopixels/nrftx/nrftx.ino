//light blue
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
int msg[2];
RF24 radio(9,10);
const uint64_t pipe = 0xE8E8F0F0E1LL;
int SW1 = 7;
int count = 0;
int mode = 0;
void setup(void){
 Serial.begin(115200);
 radio.begin();
 radio.openWritingPipe(pipe);}

void loop(void){
count++;
if (count >= 450){
  msg[0] = 4;
  count = 0;
  radio.write(msg, 4);
  mode+= 10;
}
if (msg[0] >= 3){
  msg[0] = 0;
  msg[1] = 0;
  radio.write(msg, 4);
}
if (digitalRead(SW1) == HIGH){
 msg[0] += 1;
 msg[1] = mode;
 //Serial.println(msg[3]);
 radio.write(msg, 4);
 }
 }
