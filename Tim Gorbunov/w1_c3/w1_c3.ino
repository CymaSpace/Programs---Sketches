//light blue
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
int msg[2];
RF24 radio(9,10);
const uint64_t pipe = 0xE8E8F0F0E1LL;

#define CNT_LIGHTS 100
int strip[CNT_LIGHTS][3];


int speeds[7] = {0,0,0,0,0,0,0};
int speedin[7] = {0,0,0,0,0,0,0};
int leddat[CNT_LIGHTS][3];
int ledw = CNT_LIGHTS/7;
int leds[7][4] = {{0, 0, 255, ledw*1}, 
                 {0, 255, 0, ledw*2}, 
                 {0, 255, 50, ledw*3}, 
                 {255, 0, 0, ledw*4}, 
                 {255, 0, 255, ledw*5}, 
                 {255, 255, 0, ledw*6}, 
                 {255, 255, 255, ledw*7}}; //grb + position
                 
                 
void setup(void){
 Serial.begin(115200);
 radio.begin();
 radio.openWritingPipe(pipe);}

void loop(void){
  speedin[0] = 5;
      speedin[1] = 4;
      speedin[2] = 3;
      speedin[3] = 7;
      speedin[4] = 8;
      speedin[5] = 9;
      speedin[6] = 3;
    
    moveled(); //move led forward one
}

void moveled(){
  //leddat[10][2] = 255;
//leddat[20][1] = 255;
  //________________________moving leds
  for (int h =0; h<7; h++){
    for (int i=leds[h][3]; i>leds[h][3]-ledw; i--){
     for (int k=0; k<3; k++){
       if (i<0)//if going behind the strip
         if (leddat[CNT_LIGHTS+i][k] + leds[h][k] -((leds[h][k]/ledw)*(leds[h][3]-i+1)) >=255)
            leddat[CNT_LIGHTS+i][k] = 255;
         else
            leddat[CNT_LIGHTS+i][k] += leds[h][k] -((leds[h][k]/ledw)*(leds[h][3]-i+1));
       else
           if (leddat[i][k] + leds[h][k] -((leds[h][k]/ledw)*(leds[h][3]-i+1)) >=255)
             leddat[i][k] = 255;
           else
             leddat[i][k] += leds[h][k] -((leds[h][k]/ledw)*(leds[h][3]-i+1));
     }
   }
  }
  
 
 
 //_________set the first to be strong
 for(int m=0; m<7; m++){
   for(int s=0; s<3; s++){
     leddat[leds[m][3]][s] = leds[m][s];
   }
 }
 
 //________set positions with speed
   for(int n=0; n<7; n++){
     if (leds[n][3]<CNT_LIGHTS-1){
       if (speeds[n] == speedin[n]){
         leds[n][3]+=1;
         speeds[n] = 0;
       }
     } else{
        leds[n][3]=1;
        speeds[n] = 0;
     }
  }

//_______incriment speeds
 for(int p=0; p<7; p++){
  speeds[p]++;
 }

  setled();
}


void setled(){
  for (int i=0; i<CNT_LIGHTS; i++){
    int r = leddat[i][0];
    int g = leddat[i][1];
    int b = leddat[i][2];
    stripSet(i , r, g, b);
  }
  stripShow();
  clearleddat();
}


void clearleddat(){
  for (int i=0; i<CNT_LIGHTS; i++){
    leddat[i][0] = 0;
    leddat[i][1] = 0;
    leddat[i][2] = 0;
  }
}


void stripSet(int led, int r, int g, int b){
  strip[led-1][0] = r;
  strip[led-1][1] = g;
  strip[led-1][2] = b;
}
void stripShow(){
  msg[0] = 4;
  radio.write(msg, 4);
  for (int i=0; i<CNT_LIGHTS; i++){
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
