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
int delays = 7; //speed of moving led
int sensitivity = 15; // sensitivity... 
int keepgoing = 0; // 0= false 1= true for jumping led to other side

void shows(){
   if (s1 != s2){

    
    
    lastchange = temp;
     change = s1-s2; // s1 and s2 do not change if you loop through this function
     change = change + halfstrip ; 
    Serial.print(change);
    Serial.print("    ");
    Serial.println(lastchange);
   /* Serial.print("  s1=");
    Serial.print(s1);
    Serial.print("  s2=");
    Serial.print(s2);
    Serial.print("  s1-s2=");
    Serial.println(s1-s2);
   */ 
    
   if (lastchange > change){ // checks which way to run the led
    
    if (lastchange - change > halfstrip){ // for the "text wrap" function to jump the led to other side)
      change = lastchange + 1;
      if (change > stripl - 1){
        change = change - stripl;
      }
    }else {
    change = lastchange - 1; 
    }
   } else if (lastchange < change){
        
         if (change - lastchange > halfstrip){ // for the "text wrap" function to jump the led to other side)
            change = lastchange - 1;
           if (change < 0){
            lastchange = stripl;
            change = change = lastchange - 1;
            }
        }else {
        change = lastchange + 1; 
        }
   }
   else {
   loop(); 
   }
   
  
 
   for(uint16_t i=0; i<stripl; i++) {
     if (i == change){//if i is at the vlue of the pressure
      strip.setPixelColor(i, 200, 0, 0);
      temp = i;
     } else{
     strip.setPixelColor(i, 0, 0, 0);
     }
   }
  
  
    strip.show();
     delay(delays);
 if (change != lastchange){
 
   shows(); // if you havent reached the set value keep going
 }
  delay (6);
   }
   
}

void setup() {

  Serial.begin(9600);
   strip.begin();
    for(uint16_t i=0; i<stripl; i++) {
    if (i< 75){
     strip.setPixelColor(i, 0, 0, 255);
    }else{
      strip.setPixelColor(i, 0 , 255, 255);
    }
   }
  
  
  strip.show(); 
  delay(5000);
}


void loop() {
  s1 = analogRead(A0);
  s2 = analogRead(A1);
   s1 = map(s1, 0, 1023, 0, halfstrip -1);
   s2 = map(s2, 0, 1023, 0, halfstrip);
   Serial.print("s1=");
   Serial.print(s1);
   Serial.print("  s2=");
   Serial.print(s2);
   Serial.print("  s1-s2=");
   Serial.println(s1-s2);
   
   if (s1 > sensitivity || s2 > sensitivity){ 
   shows();
   Serial.println("#############################");
   }
}

