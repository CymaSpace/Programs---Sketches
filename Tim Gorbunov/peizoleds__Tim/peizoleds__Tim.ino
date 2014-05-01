/* progmram by Tim Gorbunov which moves a led dot left and right
based on piezo pressure sensor, the speed of the strip is set by 
the delays variable
strip length is set to whatever you like.
sensor inputs are set in the loop function
*/


#include <Adafruit_NeoPixel.h>
int stripl = 150; //strip length
Adafruit_NeoPixel strip = Adafruit_NeoPixel(stripl, 6, NEO_GRB + NEO_KHZ800);
int halfstrip = stripl / 2 ;
int change = 0;
int temp = 0;
int lastchange = 0;
int s1;
int s2;
int delays = 2; //speed of moving led
void shows(){
   if (s1 != s2){

     
 
    lastchange = temp;
     change = s1-s2; 
     change = change + halfstrip ; 
   if (lastchange > change){
    change = lastchange - 1; 
   } else if (lastchange < change){
     
    change = lastchange + 1; 
    
   }
   else {
   loop(); 
   }
   
  
 
   for(uint16_t i=0; i<stripl; i++) {
     if (i == change){
      strip.setPixelColor(i, 200, 0, 0);
      temp = i;
     } else{
     strip.setPixelColor(i, 0, 0, 0);
     }
   }
  
  
    strip.show();
     delay(delays);
 if (change != lastchange){
 
   shows(); 
 }
   }
}

void setup() {

  Serial.begin(9600);
   strip.begin();
  strip.show(); 
}


void loop() {
  s1 = analogRead(A0);
  s2 = analogRead(A1);
   s1 = map(s1, 0, 1023, 0, halfstrip -1);
   s2 = map(s2, 0, 1023, 0, halfstrip);
   shows();
}

