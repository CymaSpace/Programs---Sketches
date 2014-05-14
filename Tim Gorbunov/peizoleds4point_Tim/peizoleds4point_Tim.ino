/* progmram by Tim Gorbunov which moves a led dot left and right
based on piezo pressure sensor, the speed of the strip is set by 
the delays variable
strip length is set to whatever you like.
sensor inputs are set in the loop function
*/


#include <Adafruit_NeoPixel.h>
int stripl = 150; //strip length
Adafruit_NeoPixel strip = Adafruit_NeoPixel(stripl, 6, NEO_GRB + NEO_KHZ800);
int halfstrip = stripl / 2;
int qstrip = halfstrip / 2;
int estrip = qstrip / 2;
int change = 0;
int temp = 0;
int lastchange = 0;
int s1, s2, s3, s4;
int bigest1;
int bigest2;
int delays = 7; //speed of moving led
int sensitivity = 15; // sensitivity... 


void shows(){
  if (s1 > s2 && s1 > s3 && s1 > s4){bigest1 = 1;}
  else if (s2 > s1 && s2 > s3 && s2 > s4){bigest1 = 2;}
  else if (s3 > s1 && s3 > s2 && s3 > s4){bigest1 = 3;}
  else if (s4 > s1 && s4 > s2 && s4 > s3){bigest1 = 4;}
  
      lastchange = temp;
      
      
    switch (bigest1) {
    case 1:
      if (lastchange < halfstrip){
        change = estrip - s1;
      } else {
        change = stripl - qstrip + s1;
      }
      break;
    case 2:
      if (lastchange < halfstrip + qstrip && lastchange > qstrip){
        change = qstrip + estrip - s1;
      } else {
        change = estrip + s1;
      }
      break;
    case 3:
      if (lastchange < stripl && lastchange > halfstrip){
        change = halfstrip + estrip - s1;
      } else {
        change = qstrip + estrip + s1;
      }
      break;
    case 4:
      //do something when var equals 2
      break;
    default: 
      // if nothing else matches, do the default
      // default is optional
      break;
  }

    
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

void setup() {

  Serial.begin(9600);
   strip.begin();
    for(uint16_t i=0; i<stripl; i++) {
    if (i < estrip || i > stripl - estrip){//region 1
     strip.setPixelColor(i, 255, 0, 0);
    }else if(i < halfstrip - estrip && i >  qstrip - estrip){//region 2
      strip.setPixelColor(i, 0 , 255, 0);
    }else if(i < halfstrip + estrip && i > halfstrip - estrip){//region 3
      strip.setPixelColor(i, 0 , 0, 255);
    }else if(i < halfstrip + qstrip + estrip && i > halfstrip + estrip){//region 4
      strip.setPixelColor(i, 255 , 255, 255);
    }
    
   }
  
  
  strip.show(); 
  delay(5000);
}


void loop() {
  s1 = analogRead(A0);
  s2 = analogRead(A1);
  s3 = analogRead(A2);
  s4 = analogRead(A3);
   s1 = map(s1, 0, 1023, 0, estrip);
   s2 = map(s2, 0, 1023, 0, estrip);
   s3 = map(s3, 0, 1023, 0, estrip);
   s4 = map(s4, 0, 1023, 0, estrip);
   Serial.print("s1=");
   Serial.print(s1);
   Serial.print("  s2=");
   Serial.print(s2);
   Serial.print("  s3=");
   Serial.print(s3);
   Serial.print("  s4=");
   Serial.println(s4);
   
   if (s1 > sensitivity || s2 > sensitivity || s3 > sensitivity || s4 > sensitivity){ 
   shows();
   Serial.println("#############################");
   }
}

