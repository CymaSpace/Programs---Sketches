#include <FastLED.h>
#include "EEPROM.h"

#define NUM_LEDS 251 // Number of LED's in the strip
#define ROWS 10 //# of leds in each row (in vertical pos)
#define COLUMNS 25 //# of leds in each column

/* Output pin definitions */
#define DATA_PIN 6 // Data out
#define ANALOG_PIN_L 1 // Left audio channel
#define ANALOG_PIN_R 0 // Right audio channel
#define REFRESH_POT_PIN 2 // Left pot
#define SENSITIVITY_POT_PIN 3 // Right pot
#define STOMP_PIN 5 // The pin connected to the STOMP_PIN button
#define STROBE_PIN 12 // Strobe pin 
#define RESET_PIN 13 // Reset Pin

CRGB leds[NUM_LEDS];

int fadeL[ROWS] = {};
int pixelL[COLUMNS] = {};
int colorOffset = 255/ROWS; //color change from row to row. More rows=less change
int refresh_counter = 0;
int spectrumValueL[7];
int spectrumValueR[7]; // to hold a2d values
int oldSpectrumValueL[7];
int oldSpectrumValueR[7]; // to hold a2d values
int changeL[7];
int changeR[7];
float pot_range = 0.0;
float pot_value = 0.0;
boolean monomode = 0;

void setup() 
{
  /* Initialize FastLED */
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  Serial.begin(9600); // print to serial monitor
  FastLED.show(); // Initialize all pixels to 'off'

  /* Check EEPROM for monomode */
  if (EEPROM.read(1) > 1){EEPROM.write(1,0);}
  if (digitalRead(STOMP_PIN) == HIGH){
    if (EEPROM.read(1) == 0){
      EEPROM.write(1,1);
    }else if (EEPROM.read(1) == 1) {
      EEPROM.write(1,0);
    }  
  }
  monomode = EEPROM.read(1);

  /* Set pinmodes */
  pinMode(ANALOG_PIN_L, INPUT);
  pinMode(ANALOG_PIN_R, INPUT);
  pinMode(STOMP_PIN, INPUT);
  pinMode(ANALOG_PIN_R, INPUT);
  pinMode(STROBE_PIN, OUTPUT);
  pinMode(RESET_PIN, OUTPUT);
  digitalWrite(RESET_PIN, LOW);
  digitalWrite(STROBE_PIN, HIGH); 

}

void loop() 
{  
    if (stomp_pressed()){
      delay(200);
      toggle_LEDS();
    }

   digitalWrite(RESET_PIN, HIGH);
   digitalWrite(RESET_PIN, LOW);
   int spectrum_totalL = 0;
   int spectrum_totalR = 0;

   for (int i = 0; i < 7; i++)
   {
     digitalWrite(STROBE_PIN, LOW);
     delayMicroseconds(30); // to allow the output to settle

     oldSpectrumValueL[i] = spectrumValueL[i];
     spectrumValueL[i] = analogRead(ANALOG_PIN_R); //left pin blown SWITCH
     changeL[i]=spectrumValueL[i]-oldSpectrumValueL[i];

     spectrum_totalL+= spectrumValueL[i];
     
     digitalWrite(STROBE_PIN, HIGH);
    
   }//for i

   pot_value = analogRead(REFRESH_POT_PIN);
   pot_range = analogRead(SENSITIVITY_POT_PIN);
   float _gain = (pot_range/-512.0)+2;
   byte thresh = pot_value/25;

    for (int l = 0; l<7; l++) {
     if((changeL[l]*_gain>thresh) && (fadeL[l]<201)){fadeL[l]=250;}
     if(fadeL[l]>0){fadeL[l]=fadeL[l]-5;}
    }
    
    for (int k = 0; k < ROWS; k++) { //columns
       byte odd=k%2;
       for(int j=0; j<COLUMNS; j++) { 
         byte jrev;
         if(odd==1){jrev=(COLUMNS-1)-j;}
         else{jrev=j;}
         rows(j,k,jrev,odd);
       }//rows
       
     }//for

     FastLED.show();
}

void rows(int j, int k, byte jrev, byte odd) {
  int color;
  int grad;
  
  if(jrev<4){color=36;grad = fadeL[0];}
  else if(jrev<7){color=72;grad = fadeL[1];}
  else if(jrev<11){color=108;grad = fadeL[2];}
  else if(jrev<14){color=140;grad = fadeL[3];}
  else if(jrev<18){color=172;grad = fadeL[4];}
  else if(jrev<22){color=204;grad = fadeL[5];}
  else if(jrev<26){color=240;grad = fadeL[6];}
  
  if( grad < 0 ) { grad=0; }
  byte brightness = grad;
  leds[k*COLUMNS+j+1]=CHSV(99-color-k*2,187,brightness );
}

// Check if stomp button is being pressed
int stomp_pressed() {
  if (digitalRead(STOMP_PIN) == HIGH){
    return 1;
  } else {
    return 0;
  }
}

void toggle_LEDS() {
  for (int o=0; o<NUM_LEDS; o++)
    leds[o].setRGB(0,0,0);
  
  FastLED.show();
  do {
  delay(2);
  }while(digitalRead(STOMP_PIN) == LOW);
  delay(200);
}
