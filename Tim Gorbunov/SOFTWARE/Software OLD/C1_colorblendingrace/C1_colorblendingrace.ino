#include <Adafruit_NeoPixel.h>
#define PIN 6
#define CNT_LIGHTS 120

Adafruit_NeoPixel strip = Adafruit_NeoPixel(CNT_LIGHTS, PIN, NEO_GRB + NEO_KHZ800); 
//fixed settings 
int analogPinL = 1; // read from multiplexer using analog input 0
int analogPinR = 0; // read from multiplexer using analog input 0
int strobePin = 12; // strobe is attached to digital pin 2
int resetPin = 13; // reset is attached to digital pin 3

int speed1 = 0; //counters for speed of section
int speed2 = 0;
int speed3 = 0;

int leddat[CNT_LIGHTS][3];
int ledw = CNT_LIGHTS/3;
int led1[4] = {0, 255, 0, CNT_LIGHTS/3*1}; //grb + position
int led2[4] = {0, 0, 255, CNT_LIGHTS/3*2}; //grb + position
int led3[4] = {255, 0, 0, CNT_LIGHTS/3*3}; //grb + position



void setup() {
  Serial.begin(9600); // print to serial monitor
  strip.begin();
  strip.show();
  pinMode(analogPinL, INPUT);
  pinMode(analogPinR, INPUT);
  pinMode(strobePin, OUTPUT);
  pinMode(resetPin, OUTPUT);
  digitalWrite(resetPin, LOW);
  digitalWrite(strobePin, HIGH);
}

void loop() {
    moveled(5,4,6); //move led forward one
   
    delay(1);
}



void moveled(int o, int tw, int th){
  leddat[10][2] = 255;
  leddat[20][1] = 255;
  //________________________moving led 1
  for (int i=led1[3]; i>led1[3]-ledw; i--){
   for (int k=0; k<3; k++){
     if (i<0)
       if (leddat[CNT_LIGHTS+i][k] + led1[k] -((led1[k]/ledw)*(led1[3]-i+1)) >=255)
          leddat[CNT_LIGHTS+i][k] = 255;
       else
          leddat[CNT_LIGHTS+i][k] += led1[k] -((led1[k]/ledw)*(led1[3]-i+1));
     else
       if (led1[3]-i == 0)
         leddat[i][k] = led1[k]; //first one must be strong
       else
         if (leddat[i][k] + led1[k] -((led1[k]/ledw)*(led1[3]-i+1)) >=255)
           leddat[i][k] = 255;
         else
           leddat[i][k] += led1[k] -((led1[k]/ledw)*(led1[3]-i+1));
   }
 }
   //________________________moving led 2
  for (int i=led2[3]; i>led2[3]-ledw; i--){
   for (int k=0; k<3; k++){
     if (i<0)
       if (leddat[CNT_LIGHTS+i][k] + led2[k] -((led2[k]/ledw)*(led2[3]-i+1)) >=255)
          leddat[CNT_LIGHTS+i][k] = 255;
       else
          leddat[CNT_LIGHTS+i][k] += led2[k] -((led2[k]/ledw)*(led2[3]-i+1));
     else
       if (led2[3]-i == 0)
         leddat[i][k] = led2[k]; //first one must be strong
       else
         if (leddat[i][k] + led2[k] -((led2[k]/ledw)*(led2[3]-i+1)) >=255)
           leddat[i][k] = 255;
         else
           leddat[i][k] += led2[k] -((led2[k]/ledw)*(led2[3]-i+1));
   }
 }
 

   //________________________moving led 3
  for (int i=led3[3]; i>led3[3]-ledw; i--){
   for (int k=0; k<3; k++){
     if (i<0)
       if (leddat[CNT_LIGHTS+i][k] + led3[k] -((led3[k]/ledw)*(led3[3]-i+1)) >=255)
          leddat[CNT_LIGHTS+i][k] = 255;
       else
          leddat[CNT_LIGHTS+i][k] += led3[k] -((led3[k]/ledw)*(led3[3]-i+1));
     else
       if (led3[3]-i == 0)
         leddat[i][k] = led3[k]; //first one must be strong
       else
         if (leddat[i][k] + led3[k] -((led3[k]/ledw)*(led3[3]-i+1)) >=255)
           leddat[i][k] = 255;
         else
           leddat[i][k] += led3[k] -((led3[k]/ledw)*(led3[3]-i+1));
   }
 }
 
 //_________set the first to be strong
 for(int s=0; s<3; s++){
   leddat[led1[3]][s] = led1[s];
   leddat[led2[3]][s] = led2[s];
   leddat[led3[3]][s] = led3[s];
 }
 
 
 //________set positions with speed
 if (led1[3]<CNT_LIGHTS-1){
   if (speed1 == o){
     led1[3]+=1;
     speed1 = 0;
   }
 }
  else{
    led1[3]=1;
    speed1 = 0;
  }
 if (led2[3]<CNT_LIGHTS-1){
   if (speed2 == tw){
     led2[3]+=1;
     speed2 = 0;
   }
 }
  else{
    led2[3]=1; 
    speed2 = 0; 
  }
 if (led3[3]<CNT_LIGHTS-1){
   if (speed3 == th){
     led3[3]+=1;
     speed3 = 0;
   }
 }
  else{
    led3[3]=1;
    speed3 = 0;
  } 
    
  speed1++;
  speed2++;
  speed3++;
  setled();
}


void setled(){
  for (int i=0; i<CNT_LIGHTS; i++){
    int r = leddat[i][0];
    int g = leddat[i][1];
    int b = leddat[i][2];
    strip.setPixelColor(i , r, g, b);
  }
  strip.show();
  clearleddat();
}


void clearleddat(){
  for (int i=0; i<CNT_LIGHTS; i++){
    leddat[i][0] = 0;
    leddat[i][1] = 0;
    leddat[i][2] = 0;
  }
}
