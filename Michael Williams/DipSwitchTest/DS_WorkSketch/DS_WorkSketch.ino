#include "FastLED.h"
#include "EEPROM.h"

#define STOMP_PIN 5 // The pin connected to the stomp button

// Assign dip switches
#define DIP_9 2
#define DIP_8 3
#define DIP_7 4
#define DIP_6 7
#define DIP_5 8
#define DIP_4 9
#define DIP_3 10
#define DIP_2 11
#define DIP_1 A4

#define DATA_PIN 6

int checkDIP(){
  int dipValue = 0;
  if (digitalRead(DIP_1) == HIGH){
    dipValue ++;
  }
  if (digitalRead(DIP_2) == HIGH){
    dipValue += 2;
  }
  if (digitalRead(DIP_3) == HIGH){
    dipValue += 4;
  }
  if (digitalRead(DIP_4) == HIGH){
    dipValue += 8;
  }
  if (digitalRead(DIP_5) == HIGH){
    dipValue += 16;
  }
  if (digitalRead(DIP_6) == HIGH){
    dipValue += 32;
  }
  if (digitalRead(DIP_7) == HIGH){
    dipValue += 64;
  }
  if (digitalRead(DIP_8) == HIGH){
    dipValue += 128;
  }
  if (digitalRead(DIP_9) == HIGH){
    dipValue += 264;
  }
  return dipValue;
}

  CRGB leds[ 500 ];

void setup() {
  int NUM_LEDS = 0;
  NUM_LEDS = checkDIP(); // How many leds in your strip?
  // Define the array of leds
  
  
  // Clear any old values on EEPROM
  if (EEPROM.read(1) > 1){EEPROM.write(1,0);} // Clear EEPROM

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

}

int x = 1;

void loop() {
int g =checkDIP();
  for (int j = 0; j < g; j++){
    leds[j] = CRGB::Blue;
  }
FastLED.show();

/*
  if (digitalRead(STOMP_PIN) == HIGH){
    delay(200);
    if(digitalRead(STOMP_PIN) == LOW){
      if(digitalRead(DIP_1) == HIGH){
        leds[x] = CRGB::Red;    
      }else{
        leds[x] = CRGB::Green;
      }
    FastLED.show();
    x++;
    }
  }

  if (x>NUM_LEDS){
    x=0;
  }
*/
  
}
