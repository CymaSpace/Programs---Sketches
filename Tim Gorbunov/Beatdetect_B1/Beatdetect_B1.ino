#include <Adafruit_NeoPixel.h>
#define PIN 6
#define CNT_LIGHTS 30

Adafruit_NeoPixel strip = Adafruit_NeoPixel(CNT_LIGHTS, PIN, NEO_GRB + NEO_KHZ800); 
//fixed settings 
int analogPinL = 1; // read from multiplexer using analog input 0
int analogPinR = 0; // read from multiplexer using analog input 0
int strobePin = 12; // strobe is attached to digital pin 2
int resetPin = 13; // reset is attached to digital pin 3
int led1p = 1;
int led1w = 4;
int led1c[3] = {255, 255, 0};
int led2p = 10;
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
    blinkled(-1); //move led forward one
    delay(400);
}



void blinkled(int o){
  int u;//led1
  for (int i = led1p; i <= led1p + led1w; i++ ){
    int u = i;
    if (i < 0)
      u = CNT_LIGHTS - i;
    else if (i > CNT_LIGHTS)
      u = i - CNT_LIGHTS;
    if (i == led1p)
      strip.setPixelColor(u , 0, 0, 0);
    else if (i == led1p + led1w)
      strip.setPixelColor(u , 0, 0, 0);
    else  
      strip.setPixelColor(u , Wheel(100));
    Serial.println(u);
    if (u <=10 && u >=5){
      strip.setPixelColor(5 , Wheel(50+(u*5)));
    } else {
     strip.setPixelColor(5 , 2,2,2);
    } 
  }
    strip.show();
    
  if (led1p < 1){
    led1p = CNT_LIGHTS -1;
  }
  else if (led1p > CNT_LIGHTS){
    led1p = 1;
  }else {
    led1p = led1p + o;
  }
  
}

uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
