#include <Adafruit_NeoPixel.h>
#include <avr/power.h>
#define actCNT_LIGHTS 1110 // the actual number of lights represented
#define CNT_LIGHTS 1110 //number of led desired
#define bottomlen 465
#define middlelen 412
#define toplen 233

Adafruit_NeoPixel bottoms = Adafruit_NeoPixel(bottomlen, 6, NEO_GRB + NEO_KHZ800); 
Adafruit_NeoPixel middles = Adafruit_NeoPixel(middlelen, 7, NEO_GRB + NEO_KHZ800); 
Adafruit_NeoPixel tops = Adafruit_NeoPixel(toplen, 8, NEO_GRB + NEO_KHZ800); 
//fixed settings 
int analogPinL = 1; // read from multiplexer using analog input 0
int analogPinR = 0; // read from multiplexer using analog input 0
int strobePin = 4; // strobe is attached to digital pin 2
int resetPin = 5; // reset is attached to digital pin 3
int msgReadings[7] = {0,0,0,0,0,0,0};

int resolution = 100; //speed resulution
int stoptres = 50; // threshold for full stop
int moment = 10; //higher number means led will have more momentum
int momcount = 0;
float scale[7] = {1.6,1.5,1,1,1.2,1.5,1.8};
int speeds[7] = {0,0,0,0,0,0,0}; //counter for movement delay
int speedin[7] = {7,7,7,7,7,7,7}; //setspeed

int leddatb[bottomlen][3]; //colors at each led
int leddatm[middlelen][3]; //colors at each led
int leddatt[toplen][3]; //colors at each led

int ledwb = bottomlen/7;
int ledwm = middlelen/7;
int ledwt = toplen/7;

int ledsb[7][4] = {{0, 0, 255, ledwb*1}, 
                 {128, 0, 128, ledwb*2}, 
                 {0, 255, 255, ledwb*3}, 
                 {0, 255, 0, ledwb*4}, 
                 {255, 255, 0, ledwb*5}, 
                 {255, 128, 0, ledwb*6}, 
                 {255, 0, 0, ledwb*7}}; //grb + position
                 
int ledsm[7][4] = {{0, 0, 255, ledwm*1}, 
                 {128, 0, 128, ledwm*2}, 
                 {0, 255, 255, ledwm*3}, 
                 {0, 255, 0, ledwm*4}, 
                 {255, 255, 0, ledwm*5}, 
                 {255, 128, 0, ledwm*6}, 
                 {255, 0, 0, ledwm*7}}; //grb + position                 

int ledst[7][4] = {{0, 0, 255, ledwt*1}, 
                 {128, 0, 128, ledwt*2}, 
                 {0, 255, 255, ledwt*3}, 
                 {0, 255, 0, ledwt*4}, 
                 {255, 255, 0, ledwt*5}, 
                 {255, 128, 0, ledwt*6}, 
                 {255, 0, 0, ledwt*7}}; //grb + position

void setup() {
  //Serial.begin(9600); // print to serial monitor
  bottoms.begin();
  middles.begin();
  tops.begin();
  bottoms.show();
  middles.show();
  tops.show();
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
    //Serial.print(speeds[0]);
    //Serial.print("  ");
    //Serial.println(speedin[0]);
}



void moveled(){
  //leddat[10][2] = 255;
//leddat[20][1] = 255;
  //________________________moving leds bottom
  for (int h =0; h<1; h++){
    for (int i=ledsb[h][3]; i>ledsb[h][3]-ledwb; i--){ //from current position to length of tail
     for (int k=0; k<3; k++){ //for each of the three colors
       if (i<0)//if going behind the strip
         if (leddatb[bottomlen+i][k] + ledsb[h][k] -((ledsb[h][k]/ledwb)*(ledsb[h][3]-i+1)) >=255)//if the color is full
            leddatb[bottomlen+i][k] = 255;
         else
            leddatb[bottomlen+i][k] += ledsb[h][k] -((ledsb[h][k]/ledwb)*(ledsb[h][3]-i+1));
       else
           if (leddatb[i][k] + ledsb[h][k] -((ledsb[h][k]/ledwb)*(ledsb[h][3]-i+1)) >=255) //if the color is full
             leddatb[i][k] = 255;
           else
             leddatb[i][k] += ledsb[h][k] -((ledsb[h][k]/ledwb)*(ledsb[h][3]-i+1));
     }
   }
  }
  //________________________moving leds middle
  for (int h =3; h<4; h++){
    for (int i=ledsm[h][3]; i>ledsm[h][3]-ledwm; i--){ //from current position to length of tail
     for (int k=0; k<3; k++){ //for each of the three colors
       if (i<0)//if going behind the strip
         if (leddatm[middlelen+i][k] + ledsm[h][k] -((ledsm[h][k]/ledwm)*(ledsm[h][3]-i+1)) >=255)//if the color is full
            leddatm[middlelen+i][k] = 255;
         else
            leddatm[middlelen+i][k] += ledsm[h][k] -((ledsm[h][k]/ledwm)*(ledsm[h][3]-i+1));
       else
           if (leddatm[i][k] + ledsm[h][k] -((ledsm[h][k]/ledwm)*(ledsm[h][3]-i+1)) >=255) //if the color is full
             leddatm[i][k] = 255;
           else
             leddatm[i][k] += ledsm[h][k] -((ledsm[h][k]/ledwm)*(ledsm[h][3]-i+1));
     }
   }
  }
  //________________________moving leds top
  for (int h =6; h<7; h++){
    for (int i=ledst[h][3]; i>ledst[h][3]-ledwt; i--){ //from current position to length of tail
     for (int k=0; k<3; k++){ //for each of the three colors
       if (i<0)//if going behind the strip
         if (leddatt[toplen+i][k] + ledst[h][k] -((ledst[h][k]/ledwt)*(ledst[h][3]-i+1)) >=255)//if the color is full
            leddatt[toplen+i][k] = 255;
         else
            leddatt[toplen+i][k] += ledst[h][k] -((ledst[h][k]/ledwt)*(ledst[h][3]-i+1));
       else
           if (leddatt[i][k] + ledst[h][k] -((ledst[h][k]/ledwt)*(ledst[h][3]-i+1)) >=255) //if the color is full
             leddatt[i][k] = 255;
           else
             leddatt[i][k] += ledst[h][k] -((ledst[h][k]/ledwt)*(ledst[h][3]-i+1));
     }
   }
  }
  

 
 
 //_________set the first to be strong
 for(int m=0; m<7; m++){
   for(int s=0; s<3; s++){
     leddatb[ledsb[m][3]][s] = ledsb[m][s];
     leddatm[ledsm[m][3]][s] = ledsm[m][s];
     leddatt[ledst[m][3]][s] = ledst[m][s];
   }
 }
 
 //________set positions with speed
   for(int n=0; n<7; n++){
     if (speedin[n] < stoptres){
       //BOTTOM_____________________
       if (speeds[n] == speedin[n]){ //is counter up
         if (ledsb[n][3]<bottomlen-1){//if not at then end of strip
             ledsb[n][3]+=1;
             //speeds[n] = 0;
         } else{
            ledsb[n][3]=1;
            //speeds[n] = 0;
         }
         //MIDDLE_____________________
         if (ledsm[n][3]<middlelen-1){ //if not at then end of strip
             ledsm[n][3]+=1;
         } else{
            ledsm[n][3]=1;
         }
         //TOP_________________________
         if (ledst[n][3]<toplen-1){//if not at then end of strip
             ledst[n][3]+=1;
         } else{
            ledst[n][3]=1;
         }
         speeds[n] = 0;
       }
     }else{
    for(int p=0; p<7; p++){
      speeds[p] = 0;
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
  for (int i=0; i<bottomlen; i++){
    int r = leddatb[i][0];
    int g = leddatb[i][1];
    int b = leddatb[i][2];
    if (i != 0)
      bottoms.setPixelColor(i, r, g, b);
  }
  bottoms.show();
  for (int i=0; i<middlelen; i++){
    int r = leddatm[i][0];
    int g = leddatm[i][1];
    int b = leddatm[i][2];
    if (i != 0)
      middles.setPixelColor(i, r, g, b);
  }
  middles.show();
  for (int i=0; i<toplen; i++){
    int r = leddatt[i][0];
    int g = leddatt[i][1];
    int b = leddatt[i][2];
    if (i != 0)
      tops.setPixelColor(i, r, g, b);
  }
  tops.show();
  clearleddat();
}


void clearleddat(){
  for (int i=0; i<bottomlen; i++){
    leddatb[i][0] = 0;
    leddatb[i][1] = 0;
    leddatb[i][2] = 0;
  }
  for (int i=0; i<middlelen; i++){
    leddatm[i][0] = 0;
    leddatm[i][1] = 0;
    leddatm[i][2] = 0;
  }
  for (int i=0; i<toplen; i++){
    leddatt[i][0] = 0;
    leddatt[i][1] = 0;
    leddatt[i][2] = 0;
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
  
}
