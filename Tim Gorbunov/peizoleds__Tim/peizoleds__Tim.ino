#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel strip = Adafruit_NeoPixel(20, 2, NEO_GRB + NEO_KHZ800);

void setup() {

  Serial.begin(9600);
   strip.begin();
  strip.show(); 
}


void loop() {
  int s1 = analogRead(A0);
  int s2 = analogRead(A1);
   s1 = map(s1, 0, 1023, 0, 30);
   s2 = map(s2, 0, 1023, 0, 30);
   int change = 0;
   if (s1 == s2){
    change == 0 
   }else {
    change = s1-s2; 
     
   }
   change = change + 30;
  int  half =   strip.numPixels()/2;
   for(uint16_t i=0; i<strip.numPixels(); i++) {
     if (i == change){
      strip.setPixelColor(i, 200, 0, 0);
     } else{
     strip.setPixelColor(i, 0, 0, 0);
     }
      strip.show();
      delay(2);
  }
 

}

