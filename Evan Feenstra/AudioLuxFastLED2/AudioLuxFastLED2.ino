#include "FastLED.h"
#include "EEPROM.h"
#define PIN 6
#define CNT_LIGHTS 113
#define ROWS 7
#define COLUMNS 16
#define COPYS 1

CRGB leds[CNT_LIGHTS];

int fadeL[ROWS];
int pixelL[COLUMNS];
int colorOffset = 255/ROWS; //color change from row to row. More rows=less change


int refresh_counter = 0;
int analogPinL = 1; // read from multiplexer using analog input 0
int analogPinR = 0; // read from multiplexer using analog input 0
int strobePin = 12; // strobe is attached to digital pin 2
int resetPin = 13; // reset is attached to digital pin 3
int spectrumValueL[7];
int spectrumValueR[7]; // to hold a2d values
int oldSpectrumValueL[7];
int oldSpectrumValueR[7]; // to hold a2d values
int changeL[7];
int changeR[7];
int analogPinpot = 2;// dial for speed
int analogpotrange = 3;// dial for range
float pot_range = 0.0;
float pot_value = 0.0;
boolean monomode = 0;
int stomp = 5;
boolean firstt = true;

void setup() 
{  
  FastLED.addLeds<NEOPIXEL, PIN>(leds, CNT_LIGHTS);
  Serial.begin(9600); // print to serial monitor
  //strip.begin();
  FastLED.show(); // Initialize all pixels to 'off'
  if (EEPROM.read(1) > 1){EEPROM.write(1,0);}
  pinMode(analogPinL, INPUT);
  pinMode(analogPinR, INPUT);
  pinMode(stomp, INPUT);
  pinMode(analogPinR, INPUT);
  pinMode(strobePin, OUTPUT);
  pinMode(resetPin, OUTPUT);
  analogReference(DEFAULT);
  digitalWrite(resetPin, LOW);
  digitalWrite(strobePin, HIGH); 
   Serial.println(EEPROM.read(1));
  if (digitalRead(stomp) == HIGH){
    if (EEPROM.read(1) == 0){
      EEPROM.write(1,1);
    }else if (EEPROM.read(1) == 1) {
      EEPROM.write(1,0);
    }  
  }
  
  monomode = EEPROM.read(1);
  Serial.println(EEPROM.read(1));
  Serial.println(monomode);
  for(int q=0; q<ROWS; q++){
    fadeL[q]=0;
  }
  for(int r=0; r<COLUMNS; r++) {
    pixelL[r]=0;
  }
  Serial.println("colorOffset: ");
  Serial.print(colorOffset);
}

void loop() 
{  
    if (digitalRead(stomp) == HIGH || firstt){
      firstt = false;
      delay(200);
      for (int o=0; o<CNT_LIGHTS; o++)
        leds[o].setRGB(0,0,0);
      
      FastLED.show();
      do {
      delay(2);
      }while(digitalRead(stomp) == LOW);
      delay(200);
    }
    //Serial.println(colorOffset);
   runTime();
   //EEPROM.write(1, monomode);
}


void runTime()
{
  digitalWrite(resetPin, HIGH);
  digitalWrite(resetPin, LOW);
  int spectrum_totalL = 0;
  int spectrum_totalR = 0;

  for (int i = 0; i < 7; i++)
  {
    digitalWrite(strobePin, LOW);
    delayMicroseconds(30); // to allow the output to settle
    //Serial.print(i);
    //Serial.print("  ");
    oldSpectrumValueL[i] = spectrumValueL[i];
    //oldSpectrumValueR[i] = spectrumValueR[i];
    spectrumValueL[i] = analogRead(analogPinR); //left pin blown SWITCH
    //spectrumValueR[i] = analogRead(analogPinR);
    changeL[i]=spectrumValueL[i]-oldSpectrumValueL[i];
    //changeR[i]=spectrumValueR[i]-oldSpectrumValueR[i];

    spectrum_totalL+= spectrumValueL[i];
    //spectrum_totalR+= spectrumValueR[i];
    /*Serial.print(i);
    Serial.print(",");
    Serial.print(spectrumValueL[i]);
    Serial.print("  ");*/
    
    digitalWrite(strobePin, HIGH);
   
  }//for i

  //Serial.println("  ");


  pot_value = analogRead(analogPinpot);
  pot_range = analogRead(analogpotrange);
  float _gain = (pot_range/-512.0)+2;
  byte thresh = pot_value/25;
  //Serial.println(thresh);
   
   for (int k = 0; k < ROWS; k++) { //columns
      byte odd=k%2;
      //int input=changeL[(k-0.1)/COPYS]*_gain;
      //int newK=(k-0.1)/COPYS;
      if((changeL[k]*_gain>thresh) && (fadeL[k]<201)){fadeL[k]=250;}
      for(int j=0; j<COLUMNS; j++) { 
        byte jrev;
        if(odd==1){jrev=(COLUMNS-1)-j;}
        else{jrev=j;}
        rows(j,k,jrev,odd); 
      }//rows     
      
      
      if(fadeL[k]>0){fadeL[k]=fadeL[k]-2;}
    }//for

    FastLED.show();
    //Serial.println(_gain);
    //Serial.println(thresh);
}

void rows(int j, int k, byte jrev, byte odd) {
  byte g = jrev*16; //fade gradient
  int grad = fadeL[k] - g; //int
  if( grad < 0 ) { grad=0; }
  //byte upcount=(COLUMNS-1)-fadeL[k]/16;
  //byte c=0; //color shift
  byte brightness = grad;
  /*if( jrev==upcount) {
    c=25+(25*odd);
    brightness=90; 
  }*/
  /*if(upcount>14){brightness=0;}*/
  leds[k*COLUMNS+j+1]=CHSV(120 + k*colorOffset+(jrev*5)-20,187,brightness );
}
