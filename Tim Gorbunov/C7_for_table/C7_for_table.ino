#include <Adafruit_NeoPixel.h>
#define PIN 5
#define CNT_LIGHTS 112

Adafruit_NeoPixel strip = Adafruit_NeoPixel(CNT_LIGHTS, PIN, NEO_GRB + NEO_KHZ800); 
//fixed settings 
int analogPinL = 1; // read from multiplexer using analog input 0
int analogPinR = 0; // read from multiplexer using analog input 0
int strobePin = 4; // strobe is attached to digital pin 2
int resetPin = 5; // reset is attached to digital pin 3
int msgReadings[7] = {0,0,0,0,0,0,0};

int resolution = 100; //speed resulution
int stoptres = 5; // threshold for full stop
int moment = 10; //higher number means led will have more momentum
int momcount = 0;
float scale[7] = {1.6,1.5,1,1,1.2,1.5,1.8};
int speeds[7] = {0,0,0,0,0,0,0}; //counter for movement delay
int speedin[7] = {7,7,7,7,7,7,7}; //setspeed
int leddat[CNT_LIGHTS][3];
int ledw = CNT_LIGHTS/7;
int leds[7][4] = {{0, 0, 255, ledw*1}, 
                 {128, 0, 128, ledw*2}, 
                 {0, 255, 255, ledw*3}, 
                 {0, 255, 0, ledw*4}, 
                 {255, 255, 0, ledw*5}, 
                 {255, 128, 0, ledw*6}, 
                 {255, 0, 0, ledw*7}}; //grb + position


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
    getaudio();
    moveled(); //move led forward one
    //delay(100);
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
     if (speedin[n] <= resolution-stoptres){
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

void getaudio(){
  momcount++;   
  digitalWrite(resetPin, HIGH);
  digitalWrite(resetPin, LOW);
  for (int i = 0; i < 7; i++)
  {
    digitalWrite(strobePin, LOW);
     delayMicroseconds(30); // to allow the output to settle
    msgReadings[i] = analogRead(analogPinR);
    digitalWrite(strobePin, HIGH); 
  } 
  for (int i = 0; i < 7; i++)
  {
    int maps = map(msgReadings[i]*scale[i], 110, 1000, resolution, 1);
    maps = constrain(maps, 1, resolution);
    
    if (maps < speedin[i]){
      speedin[i]  = maps;
      if (speeds[i] >= speedin[i])
        speeds[i] = speedin[i]-1;
      
    }
    else if (maps > speedin[i]){
      if (momcount >= moment){
        speedin[i]+=1;
      }
    }
    /*
    /Serial.println(momcount);
    Serial.print(maps);
    Serial.print(":");
    Serial.print(speedin[i]);
    Serial.print(":");
    Serial.print(speeds[i]);
    Serial.print("  ");*/
  }
  if (momcount >= moment)
    momcount = 0;
  Serial.print(momcount);
 Serial.println();
}
