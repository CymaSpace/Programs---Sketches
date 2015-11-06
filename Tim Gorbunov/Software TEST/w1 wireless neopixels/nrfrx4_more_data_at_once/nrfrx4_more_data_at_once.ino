//dark blue
#include <SPI.h>
#include <Adafruit_NeoPixel.h>
#include "nRF24L01.h"
#include "RF24.h"
boolean start = true;

int currentled = 0;
int r = 0;
int g = 0;
int b = 0;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(150, 6, NEO_GRB + NEO_KHZ800); 
int msg[4];
RF24 radio(9,10);
const uint64_t pipe = 0xE8E8F0F0E1LL;
int LED1 = 3;
char leddat[255][3];

void setup(void){
 Serial.begin(115200);
 strip.begin();
 strip.show();
 radio.begin();
 
 radio.openReadingPipe(1,pipe);
 radio.startListening();
 pinMode(LED1, OUTPUT);
 pinMode(5, OUTPUT);
 digitalWrite(5, LOW);
 }

void loop(void){
 if (radio.available()){
   bool done = false;    
   while (!done){
     done = radio.read( &leddat, 32);
      }
      //Serial.println("radio");
      digitalWrite(LED1, HIGH);
      for (int i=0; i<254;i++){
      strip.setPixelColor( i, leddat[i][0], leddat[i][1], leddat[i][2]); 
      //Serial.print( i+1); 
      //Serial.print(leddat[i][0]); 
      //Serial.print(leddat[i][1]); 
      //Serial.print(leddat[i][2]); 
      }
      strip.show();
     //delay(1);
   }
 else{
     //Serial.println("No radio available");
     digitalWrite(LED1, LOW); 
     //strip.setPixelColor( 1, 0, 0, 0);
     }
}
