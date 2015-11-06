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
bool wait = false;
bool wait2 = false;


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
     done = radio.read(msg, 4);
     //Serial.print(msg[0]);
     //Serial.print(" ");
     //Serial.println(msg[1]);
     //Serial.println(r);
     
     
     if (msg[0] == 0 && msg[1] == 0){
         wait = true;}
     else if (msg[0] > 0 && wait){
         currentled = msg[0];
         r = msg[1];
         wait = !wait;
         wait2 = true;}
     else if (msg[0] > 1 && wait2){
         g = msg[0];
         b = msg[1];
         wait2 = !wait2;
     } 
     else if (msg[0] == 4){
       strip.show();
       //Serial.print("hello");
       }
     
       //delay(1);
      digitalWrite(LED1, HIGH);
      }
        strip.setPixelColor( currentled , r, g, b); 
       
      
     
     
     //delay(1);
   }
 /*else{Serial.println("No radio available");
     digitalWrite(LED1, LOW); 
     //strip.setPixelColor( 1, 0, 0, 0);
     }/*/
}
