#include <Adafruit_NeoPixel.h>
#define PIN 6
#define CNT_LIGHTS 97


Adafruit_NeoPixel strip = Adafruit_NeoPixel(CNT_LIGHTS, PIN, NEO_GRB + NEO_KHZ800);
//fixed settings
int analogPinL = 1; // read from multiplexer using analog input 0
int analogPinR = 0; // read from multiplexer using analog input 0
int strobePin = 12; // strobe is attached to digital pin 2
int resetPin = 13; // reset is attached to digital pin 3
int msgReadings[7] = {0, 0, 0, 0, 0, 0, 0};

int resolution = CNT_LIGHTS / 7+10; //speed resulution
int stoptres = 5; // threshold for full stop
int moment = 10; //higher number means led will have more momentum
int momcount = 0;
int speedcomp = 0; //for making snake shorter as time goes by
boolean flag1 =0;
float scale[7] = {1.6, 1.5, 1, 1, 1.2, 1.5, 1.8};
int speeds[7] = {0, 0, 0, 0, 0, 0, 0}; //counter for movement delay
int speedin[7] = {7, 7, 7, 7, 7, 7, 7}; //setspeed
int leddat[CNT_LIGHTS][3];
int ledw = CNT_LIGHTS / 7;
int widths[7] = {ledw, ledw, ledw, ledw, ledw, ledw, ledw}; 
int leds[7][4] = {{0, 0, 255, widths[0] * 1},
  {128, 0, 128, widths[1] * 2},
  {0, 255, 255, widths[2] * 3},
  {0, 255, 0, widths[3] * 4},
  {255, 255, 0, widths[4] * 5},
  {255, 128, 0, widths[5] * 6},
  {255, 0, 0, widths[6] * 7}
}; //grb + position


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
  Serial.print("WELCOME  ");
  Serial.println(ledw);
}

void loop() {
  getaudio();
  moveled(); //move led forward one
  //Serial.print(speedin[0]);
  //Serial.print("  ");
  //Serial.println(ledw-(ledw/(speedin[0])));
}



void moveled() {
  //leddat[10][2] = 255;
  //leddat[20][1] = 255;
  //________________________moving leds
  for (int h = 0; h < 7; h++) {                                         //7 bands
    widths[h] = constrain(ledw - speedin[h], 0, ledw);
    for (int i = leds[h][3]; i > leds[h][3] - widths[h]; i--) {              //width of led starting from led position
      
      for (int k = 0; k < 3; k++) {                                      //3 Colors
        if (i < 0) {                                                     //if going behind the strip
          if (leddat[CNT_LIGHTS + i][k] + leds[h][k] - ((leds[h][k] / widths[h]) * (leds[h][3] - i + 1)) >= 255) {
            leddat[CNT_LIGHTS + i][k] = 255;
          } else {
              leddat[CNT_LIGHTS + i][k] += leds[h][k] - ((leds[h][k] / widths[h]) * (leds[h][3] - i + 1));          //brighness diminish
          }
        } else {
          if (leddat[i][k] + leds[h][k] - ((leds[h][k] / widths[h]) * (leds[h][3] - i + 1)) >= 255) {
            leddat[i][k] = 255;
          }
          else {
              leddat[i][k] += leds[h][k] - ((leds[h][k] / widths[h]) * (leds[h][3] - i + 1));          //brighness diminish
          }
        }
      }
    }
  }


  /*
    //_________set the first to be strong
    for(int m=0; m<7; m++){
    for(int s=0; s<3; s++){
      leddat[leds[m][3]][s] = leds[m][s];
    }
    }
  */
  //________set positions with speed
  for (int n = 0; n < 7; n++) {
    if (speedin[n] <= resolution - stoptres) {
      if (leds[n][3] < CNT_LIGHTS - 1) {         //if before the end of the strip
        if (speeds[n] == speedin[n]) {
          leds[n][3] += 1;
          speeds[n] = 0;
        }
      } else {
        leds[n][3] = 1;                         //puts positon back to zero
        speeds[n] = 0;
      }
    }
  }

  //_______incriment speeds
  for (int p = 0; p < 7; p++) {
    speeds[p]++;
  }

  setled();
}


void setled() {
  for (int i = 0; i < CNT_LIGHTS; i++) {
    int r = leddat[i][0];
    int g = leddat[i][1];
    int b = leddat[i][2];
    strip.setPixelColor(i , r, g, b);
  }
  strip.show();
  clearleddat();
}


void clearleddat() {
  for (int i = 0; i < CNT_LIGHTS; i++) {
    leddat[i][0] = 0;
    leddat[i][1] = 0;
    leddat[i][2] = 0;
  }
}

void getaudio() {
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
    int maps = map(msgReadings[i] * scale[i], 110, 1000, resolution, 1);
    //Serial.println(msgReadings[3]*scale[3]);
    maps = constrain(maps, 1, resolution);

    if (maps < speedin[i]) {
      speedin[i]  = maps;
      if (speeds[i] >= speedin[i])
        speeds[i] = speedin[i] - 1;

    }
    else if (maps > speedin[i]) {
      if (momcount >= moment) {
        speedin[i] += 1;
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
  //Serial.print(momcount);
  //Serial.println();
}
