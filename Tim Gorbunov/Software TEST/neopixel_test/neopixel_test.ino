#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel strip = Adafruit_NeoPixel(150, 6, NEO_GRB + NEO_KHZ800); 


void setup() {
  strip.begin();
  strip.show();
}

// the loop function runs over and over again forever
void loop() {
   for (int i=1;i<151;i++){
  strip.setPixelColor( i , 0, 200, 0); 
   }
   delay(500);
   strip.show();
 for (int i=1;i<151;i++){
  strip.setPixelColor( i , 200, 0, 0); 
    
 }
 delay(500);
 strip.show();
}
